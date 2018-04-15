#include "AggregationController.h"

/**
 * In this controller we ignore the "non kin" parameter because everyone is kin!
 */
void AggregationController::ControlStep() {
  auto sensor_state = GetKinSensorVal();
  switch (sensor_state) {
    case SensorState::KIN: {
      m_pcWheels->SetLinearVelocity(SCALE * m_params[0], SCALE * m_params[1]);
      break;
    }
    default : {
      m_pcWheels->SetLinearVelocity(SCALE * m_params[2], SCALE * m_params[3]);
      break;
    }
  }
}

REGISTER_CONTROLLER(AggregationController, "footbot_aggregation_controller")
