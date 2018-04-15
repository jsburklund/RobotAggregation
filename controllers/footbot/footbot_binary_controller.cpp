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


  // Finish setup
  Reset();
  Reset();
}

void CFootBotBinaryController::Reset() {
  argos::LOG <<"Called Reset\n";
  m_pcRABAct->Reset();
  my_id = my_id+1000;
  m_pcRABAct->SetData(0, (uint8_t) my_id);
  m_pcRABAct->SetData(1, (uint8_t) (((uint16_t) my_id)>>8));
  my_id = my_id-1000;
}

void CFootBotBinaryController::ControlStep() {
  unsigned int I; // TODO: figure this out from other robots or mock sensor

  GetKinSensorVal();

  m_pcWheels->SetLinearVelocity(-7, -10);
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

unsigned int CFootBotBinaryController::GetKinSensorVal() {
  // Get the other robots range and bearing
  const CCI_RangeAndBearingSensor::TReadings& tMsgs = m_pcRABSens->GetReadings();
  // Look at all robots that have sent a message
  if(! tMsgs.empty()) {
    if (my_id == 0) {
      argos::LOG<<"Num neighbors: "<<tMsgs.size()<<"\n";
    }
    for (size_t i=0; i<tMsgs.size(); i++) {
      Real range  = tMsgs[i].Range;
      uint16_t rid =  ((uint16_t) tMsgs[i].Data[1])<<8 | ((uint16_t) tMsgs[i].Data[0]);
      Real bearing = tMsgs[i].HorizontalBearing.GetValue();
      if (my_id == 0) {
        argos::LOG << "RID ["<<rid<<"]: dist "<<range<<"ang "<<bearing<<"\n";
      }
    }
  }

  return 0;
}

REGISTER_CONTROLLER(CFootBotBinaryController, "footbot_binary_controller")
