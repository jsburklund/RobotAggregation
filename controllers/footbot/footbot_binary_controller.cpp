#include <argos3/core/utility/logging/argos_log.h>
#include "footbot_binary_controller.h"

#include <cstdio>

static CRange<Real> WHEEL_ACTUATION_RANGE(-5.0, 5.0);

CFootBotBinaryController::CFootBotBinaryController()
    : m_pcWheels(nullptr),
      m_params{0, 0, 0, 0},
      m_pcRABAct(nullptr),
      m_pcRABSens(nullptr),
      m_pcLEDs(nullptr),
      my_id(0ul),
      my_group(0ul) {
}

void CFootBotBinaryController::Init(TConfigurationNode &t_node) {
  try {
    m_pcWheels = GetActuator<CCI_DifferentialSteeringActuator>("differential_steering");
    m_pcRABAct = GetActuator<CCI_RangeAndBearingActuator>("range_and_bearing");
    m_pcRABSens = GetSensor<CCI_RangeAndBearingSensor>("range_and_bearing");
    m_pcLEDs = GetActuator<CCI_LEDsActuator>("leds");

  }
  catch (CARGoSException &ex) {
    THROW_ARGOSEXCEPTION_NESTED("Error initializing sensors/actuators", ex);
  }

  std::string params_filename;
  GetNodeAttributeOrDefault(t_node, "parameter_file", params_filename, std::string());

  if (!params_filename.empty()) {
    std::ifstream cIn(params_filename.c_str());
    if (!cIn) {
      THROW_ARGOSEXCEPTION("Cannot open parameter file '" << params_filename << "' for reading");
    }

    // first parameter is the number of real-valued weights
    UInt32 params_length = 0;
    if (!(cIn >> params_length)) {
      THROW_ARGOSEXCEPTION("Cannot read data from file '" << params_filename << "'");
    }

    // check consistency between parameter file and xml declaration
    if (params_length != m_params.size()) {
      THROW_ARGOSEXCEPTION("Number of parameter mismatch: '"
                               << params_filename
                               << "' contains "
                               << params_length
                               << " parameters, while "
                               << m_params.size()
                               << " were expected from the XML configuration file");
    }

    // create weights vector and load it from file
    for (size_t i = 0; i < params_length; ++i) {
      if (!(cIn >> m_params[i])) {
        THROW_ARGOSEXCEPTION("Cannot read data from file '" << params_filename << "'");
      }
    }
  }

  // Parse the ID number
  const std::string my_idstr = GetId();
  size_t pos;
  my_id = std::stoul(my_idstr, &pos);

  // Set group based on ID modulo some number
  // TODO: make this modulo a parameter
  my_group = my_id % 2;

  switch (my_group) {
    case 0: m_pcLEDs->SetAllColors(CColor::BLUE);
      break;
    case 1: m_pcLEDs->SetAllColors(CColor::GREEN);
      break;
    case 2: m_pcLEDs->SetAllColors(CColor::PURPLE);
      break;
    case 3: m_pcLEDs->SetAllColors(CColor::YELLOW);
      break;
    case 5: m_pcLEDs->SetAllColors(CColor::ORANGE);
      break;
    default: m_pcLEDs->SetAllColors(CColor::RED);
      break;
  }

  Reset();
}

void CFootBotBinaryController::Reset() {
  // Setup the robot and group id data to send
  m_pcRABAct->SetData(0, (uint8_t) my_group);  //TODO Only supports 256 groups for now
}

void CFootBotBinaryController::ControlStep() {
  bool sens_state = GetKinSensorVal();

  if (sens_state) {
    // Saw a robot of the same type
    m_pcWheels->SetLinearVelocity(20 * m_params[0], 20 * m_params[1]);
  } else {
    // Saw nothing or a robot of different type
    m_pcWheels->SetLinearVelocity(20 * m_params[2], 20 * m_params[3]);
  }
}

void CFootBotBinaryController::SetParameters(const size_t num_params, const Real *params) {
  if (num_params != m_params.size()) {
    THROW_ARGOSEXCEPTION("Number of parameter mismatch: passed "
                             << num_params
                             << " parameters, while GENE_SIZE is "
                             << m_params.size());
  }

  for (size_t i = 0; i < num_params; ++i) {
    LOG << params[i] << ", ";
    m_params[i] = params[i];
  }
  LOG << "\n";

}

/* Simulate a kin/nothing/non-kin type sensor with limited viewing range
 * Returns true if a robot in the same group is the closest thing within
 * the viewing angle.
 */
bool CFootBotBinaryController::GetKinSensorVal() {
  // Get the other robots range and bearing
  const CCI_RangeAndBearingSensor::TReadings &tMsgs = m_pcRABSens->GetReadings();
  // Look at all robots that have sent a message
  if (!tMsgs.empty()) {
    Real closest_range = INFINITY;  // Closest robot detected so far
    bool sens_state = false;        // Assume that a robot is in view of the camera

    // Parse all of the other robots detected
    for (const auto &tMsg : tMsgs) {
      // Check if the neighbor is in view of the camera
      Real bearing = tMsg.HorizontalBearing.GetValue();
      Real range = tMsg.Range;
      if (bearing < kCAM_VIEW_ANG && bearing > -kCAM_VIEW_ANG) {
        //Robot is in view of the camera, check to see if it is the closest
        if (range < closest_range) {
          closest_range = range;
          uint8_t robot_group = tMsg.Data[0];
          sens_state = (my_group == robot_group);
        }
      }
    }
    return sens_state;
  }

  return false;
}

REGISTER_CONTROLLER(CFootBotBinaryController, "footbot_binary_controller")
