#include <argos3/core/utility/logging/argos_log.h>
#include "segregation_footbot_controller.h"

#include <cstdio>

SegregationFootbotController::SegregationFootbotController()
    : m_params{0, 0, 0, 0},
      m_pcWheels(nullptr),
      m_pcRABAct(nullptr),
      m_pcRABSens(nullptr),
      m_pcLEDs(nullptr),
      m_class(0ul) {
}

void SegregationFootbotController::Init(TConfigurationNode &t_node) {
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
    LoadFromFile(params_filename);
  }

  Reset();
}

void SegregationFootbotController::Reset() {
  // not this only supports 256 classs
  m_pcRABAct->SetData(0, (uint8_t) m_class);
}

void SegregationFootbotController::LoadFromFile(const std::string &params_filename) {
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
                             << " were expected");
  }

  // create weights vector and load it from file
  for (size_t i = 0; i < params_length; ++i) {
    if (!(cIn >> m_params[i])) {
      THROW_ARGOSEXCEPTION("Cannot read data from file '" << params_filename << "'");
    }
  }
}

SegregationFootbotController::SensorState SegregationFootbotController::GetKinSensorVal() {
  const CCI_RangeAndBearingSensor::TReadings &tMsgs = m_pcRABSens->GetReadings();

  auto sens_state = SensorState::NOTHING;
  if (!tMsgs.empty()) {
    Real closest_range = INFINITY;

    for (const auto &tMsg : tMsgs) {
      Real bearing = tMsg.HorizontalBearing.GetValue();
      Real range = tMsg.Range;
      if (bearing < kCAM_VIEW_ANG && bearing > -kCAM_VIEW_ANG) {
        if (range < closest_range) {
          closest_range = range;
          uint8_t robot_class = tMsg.Data[0];
          sens_state = (m_class == robot_class) ? SensorState::KIN : SensorState::NONKIN;
        }
      }
    }
  }

  for (auto i = 10; i < 14; ++i) {
    UInt32 led_id = static_cast<UInt32>(i % 12);
    switch (sens_state) {
      case SensorState::KIN: m_pcLEDs->SetSingleColor(led_id, CColor::BLUE);
        break;
      case SensorState::NONKIN: m_pcLEDs->SetSingleColor(led_id, CColor::RED);
        break;
      case SensorState::NOTHING: m_pcLEDs->SetSingleColor(led_id, CColor::WHITE);
        break;
    }
  }

  return sens_state;
}

void SegregationFootbotController::ControlStep() {

  switch (m_class) {
    case 0: m_pcLEDs->SetAllColors(CColor::GREEN);
      break;
    case 1: m_pcLEDs->SetAllColors(CColor::RED);
      break;
    case 2: m_pcLEDs->SetAllColors(CColor::BLUE);
      break;
    case 3: m_pcLEDs->SetAllColors(CColor::WHITE);
      break;
    default: m_pcLEDs->SetAllColors(CColor::BLACK);
      break;
  }

  auto sensor_state = GetKinSensorVal();
  switch (sensor_state) {
    case SensorState::NOTHING: {
      m_pcWheels->SetLinearVelocity(SCALE * m_params[0], SCALE * m_params[1]);
      break;
    }
    case SensorState::KIN: {
      m_pcWheels->SetLinearVelocity(SCALE * m_params[2], SCALE * m_params[3]);
      break;
    }
    case SensorState::NONKIN: {
      m_pcWheels->SetLinearVelocity(SCALE * m_params[4], SCALE * m_params[5]);
      break;
    }
  }
}

void SegregationFootbotController::SetParameters(const size_t num_params, const Real *params) {
  if (num_params != m_params.size()) {
    THROW_ARGOSEXCEPTION("Number of parameter mismatch: was passed"
                             << num_params
                             << " parameters, while "
                             << m_params.size()
                             << " were expected");
  }

  for (size_t i = 0; i < num_params; ++i) {
    m_params[i] = params[i];
  }
}

void SegregationFootbotController::SetClassId(unsigned long class_id) {
  m_class = class_id;
  m_pcRABAct->SetData(0, (uint8_t) m_class);
}

REGISTER_CONTROLLER(SegregationFootbotController, "footbot_segregation_controller")
