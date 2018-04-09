#include "footbot_binary_controller.h"

static CRange<Real> WHEEL_ACTUATION_RANGE(-5.0f, 5.0f);

CFootBotBinaryController::CFootBotBinaryController() {
}

CFootBotBinaryController::~CFootBotBinaryController() {
}

void CFootBotBinaryController::Init(TConfigurationNode &t_node) {
  try {
    m_pcWheels = GetActuator<CCI_DifferentialSteeringActuator>("differential_steering");
    m_pcProximity = GetSensor<CCI_FootBotProximitySensor>("footbot_proximity");
  }
  catch (CARGoSException &ex) {
    THROW_ARGOSEXCEPTION_NESTED("Error initializing sensors/actuators", ex);
  }
}

void CFootBotBinaryController::ControlStep() {
  const CCI_FootBotProximitySensor::TReadings &tProx = m_pcProximity->GetReadings();
  m_pcWheels->SetLinearVelocity(m_fLeftSpeed, m_fRightSpeed);
}

void CFootBotBinaryController::Reset() {
}

void CFootBotBinaryController::Destroy() {
}

REGISTER_CONTROLLER(CFootBotBinaryController, "footbot_binary_controller")
