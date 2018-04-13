#include <argos3/core/utility/logging/argos_log.h>
#include "footbot_binary_controller.h"

static CRange<Real> WHEEL_ACTUATION_RANGE(-5.0f, 5.0f);

CFootBotBinaryController::CFootBotBinaryController() : m_pcWheels(nullptr), m_params{0, 0, 0, 0} {
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
  m_pcWheels->SetLinearVelocity(-0.7, -1.0);
  return;
  switch (I) {
    case 0: {
      m_pcWheels->SetLinearVelocity(m_params[0], m_params[1]);
      break;
    }
    case 1: {
      m_pcWheels->SetLinearVelocity(m_params[2], m_params[3]);
      break;
    }
    default: {
      THROW_ARGOSEXCEPTION("Invalid sensor state. Must be 0 or 1.");
    }
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
    m_params[i] = params[i];
  }
}

REGISTER_CONTROLLER(CFootBotBinaryController, "footbot_binary_controller")
