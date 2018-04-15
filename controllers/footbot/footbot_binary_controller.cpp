#include <argos3/core/utility/logging/argos_log.h>
#include "footbot_binary_controller.h"

#include <cstdio>

static CRange<Real> WHEEL_ACTUATION_RANGE(-5.0f, 5.0f);

CFootBotBinaryController::CFootBotBinaryController() : m_pcWheels(nullptr), m_params{0, 0, 0, 0} {//}, m_pcRABAct(NULL), m_pcRABSens(NULL) {
}

void CFootBotBinaryController::Init(TConfigurationNode &t_node) {
  try {
    m_pcWheels = GetActuator<CCI_DifferentialSteeringActuator>("differential_steering");
    m_pcRABAct = GetActuator<CCI_RangeAndBearingActuator  >("range_and_bearing" );
    m_pcRABSens = GetSensor <CCI_RangeAndBearingSensor >("range_and_bearing" );
  }
  catch (CARGoSException &ex) {
    THROW_ARGOSEXCEPTION_NESTED("Error initializing sensors/actuators", ex);
  }

  // Parse the ID number
  std::string my_idstr = GetId();
  std::sscanf(my_idstr.c_str(), "fb%d", &my_id);

  // Set the group
  my_group = 0;  // Currently all robots are in the same group
                 // TODO add some parameter for this...

  // Finish setup
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
    m_pcWheels->SetLinearVelocity(m_params[0], m_params[1]);
  } else {
    // Saw nothing or a robot of different type
    m_pcWheels->SetLinearVelocity(m_params[2], m_params[3]);
  }
  return;
}

void CFootBotBinaryController::SetParameters(const size_t num_params, const Real *params) {
  if (num_params != m_params.size()) {
    THROW_ARGOSEXCEPTION("Number of parameter mismatch: passed "
                             << num_params
                             << " parameters, while GENE_SIZE is "
                             << m_params.size());
  }

  for (size_t i = 0; i < num_params; ++i) {
    m_params[i] = params[i];
  }

}

/* Simulate a kin/nothing/non-kin type sensor with limited viewing range
 * Returns true if a robot in the same group is the closest thing within
 * the viewing angle.
 */
bool CFootBotBinaryController::GetKinSensorVal() {
  // Get the other robots range and bearing
  const CCI_RangeAndBearingSensor::TReadings& tMsgs = m_pcRABSens->GetReadings();
  // Look at all robots that have sent a message
  if(! tMsgs.empty()) {
    Real closest_range = INFINITY;  // Closest robot detected so far
    bool sens_state = false;        // Assume that a robot is in view of the camera

    // Parse all of the other robots detected
    for (size_t i=0; i<tMsgs.size(); i++) {
      // Check if the neighbor is in view of the camera
      Real bearing = tMsgs[i].HorizontalBearing.GetValue();
      Real range  = tMsgs[i].Range;
      if (bearing < kCAM_VIEW_ANG && bearing > -kCAM_VIEW_ANG) {
        //Robot is in view of the camera, check to see if it is the closest
        if (range < closest_range) {
          closest_range = range;
          uint8_t robot_group = tMsgs[i].Data[0];
          sens_state = (my_group == robot_group);
        }
      }
    }
    return sens_state;
  }

  return false;
}

REGISTER_CONTROLLER(CFootBotBinaryController, "footbot_binary_controller")
