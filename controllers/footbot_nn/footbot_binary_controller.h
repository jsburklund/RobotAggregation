#pragma once

#include <array>

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_proximity_sensor.h>

using namespace argos;

class CFootBotBinaryController : public CCI_Controller {

 public:

  CFootBotBinaryController();

  void Init(TConfigurationNode &t_node) override;

  void ControlStep() override;

  void SetParameters(size_t i, const Real *pDouble);

 private:

  CCI_DifferentialSteeringActuator *m_pcWheels;
  std::array<Real, 4> params;

};

