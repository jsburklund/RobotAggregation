#pragma once

#include <array>

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_proximity_sensor.h>

using namespace argos;

class CFootBotBinaryController : public CCI_Controller {

 public:

  static const size_t GENOME_SIZE = 6;

  CFootBotBinaryController();

  void Init(TConfigurationNode &t_node) override;

  void ControlStep() override;

  void SetParameters(size_t num_params, const Real *params);

 private:

  CCI_DifferentialSteeringActuator *m_pcWheels;
  std::array<Real, GENOME_SIZE> m_params;

};

