#include "AggregationController.h"

/**
 * In this controller we ignore the "non kin" parameter because everyone is kin!
 */
void AggregationController::ControlStep() {
  auto sensor_state = GetKinSensorVal();
  constexpr auto scale = 20.0;
  switch (sensor_state) {
    case SensorState::KIN: {
      m_pcWheels->SetLinearVelocity(scale * m_params[0], scale * m_params[1]);
      break;
    }
    default : {
      m_pcWheels->SetLinearVelocity(scale * m_params[2], scale * m_params[3]);
      break;
    }
  }
}

REGISTER_CONTROLLER(AggregationController, "footbot_aggregation_controller")
