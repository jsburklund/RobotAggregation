#pragma once

#include <array>

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_proximity_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_sensor.h>

using namespace argos;

class CFootBotBinaryController : public CCI_Controller {

 public:

  static const size_t GENOME_SIZE = 6;

  CFootBotBinaryController();

  void Init(TConfigurationNode &t_node) override;

  void Reset();

  void ControlStep() override;

  void SetParameters(size_t num_params, const Real *params);

  bool GetKinSensorVal();

 private:

  CCI_DifferentialSteeringActuator *m_pcWheels;
  std::array<Real, GENOME_SIZE> m_params;
  /* Pointer to the range-and-bearing actuator */
  CCI_RangeAndBearingActuator* m_pcRABAct;
  /* Pointer to the range-and-bearing sensor */
  CCI_RangeAndBearingSensor* m_pcRABSens;
  // Robot Id number 0-N
  unsigned int my_id;
  unsigned int my_group;
  Real kCAM_VIEW_ANG = 15 * 3.141593/180.0;
};

