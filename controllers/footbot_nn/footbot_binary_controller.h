#pragma once

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_proximity_sensor.h>

using namespace argos;

class CFootBotBinaryController : public CCI_Controller {

 public:

  CFootBotBinaryController();

  virtual ~CFootBotBinaryController();

  void Init(TConfigurationNode &t_node);

  void ControlStep();

  void Reset();

  void Destroy();

 private:

  CCI_DifferentialSteeringActuator *m_pcWheels;
  CCI_FootBotProximitySensor *m_pcProximity;
  Real m_fLeftSpeed, m_fRightSpeed;

};

