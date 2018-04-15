#include <argos3/core/utility/logging/argos_log.h>
#include "SegregationController.h"

void SegregationController::ControlStep() {
  auto sensor_state = GetKinSensorVal();
  LOGERR << static_cast<unsigned int>(sensor_state) << "\n";
  switch (sensor_state) {
    case SensorState::KIN: {
      m_pcWheels->SetLinearVelocity(SCALE * m_params[0], SCALE * m_params[1]);
      break;
    }
    case SensorState::NOTHING: {
      m_pcWheels->SetLinearVelocity(SCALE * m_params[2], SCALE * m_params[3]);
      break;
    }
    case SensorState::NONKIN: {
      m_pcWheels->SetLinearVelocity(SCALE * m_params[4], SCALE * m_params[5]);
      break;
    }
  }
}

REGISTER_CONTROLLER(SegregationController, "footbot_segregation_controller")
