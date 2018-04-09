#include "footbot_binary_controller.h"

static CRange<Real> WHEEL_ACTUATION_RANGE(-5.0f, 5.0f);

CFootBotBinaryController::CFootBotBinaryController() : m_pcWheels(nullptr), params{0, 0, 0, 0} {
}

void CFootBotBinaryController::Init(TConfigurationNode &t_node) {
  try {
    m_pcWheels = GetActuator<CCI_DifferentialSteeringActuator>("differential_steering");
  }
  catch (CARGoSException &ex) {
    THROW_ARGOSEXCEPTION_NESTED("Error initializing sensors/actuators", ex);
  }
}

void CFootBotBinaryController::ControlStep() {
  unsigned int I; // TODO: figure this out from other robots or mock sensor
  switch (I) {
    case 0: {
      m_pcWheels->SetLinearVelocity(params[0], params[1]);
      break;
    }
    case 1: {
      m_pcWheels->SetLinearVelocity(params[2], params[3]);
      break;
    }
    default: {
      THROW_ARGOSEXCEPTION("Invalid sensor state. Must be 0 or 1.");
    }
  }
}

void CFootBotBinaryController::SetParameters(const size_t i, const Real *pDouble) {
  if (i != 4) {
    THROW_ARGOSEXCEPTION("Number of parameter mismatch: '"
                             << "passed "
                             << i
                             << " parameters, while "
                             << 4
                             << " were expected from the XML configuration file");
  }

}

REGISTER_CONTROLLER(CFootBotBinaryController, "footbot_binary_controller")
