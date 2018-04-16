#pragma once

#include <array>

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_leds_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_sensor.h>

using namespace argos;

class SegregationFootbotController : public CCI_Controller {

 public:

  enum class SensorState : unsigned int {
    NOTHING = 0,
    KIN = 1,
    NONKIN = 2
  };

  static const size_t GENOME_SIZE = 6;
  constexpr static auto SCALE = 20.0;

  SegregationFootbotController();

  void Init(TConfigurationNode &t_node) override;

  void Reset() override;

  void ControlStep() override;

  void LoadFromFile(const std::string &params_filename);

  void SetParameters(size_t num_params, const Real *params);

  void SetGroup(unsigned long group);

  SensorState GetKinSensorVal();

  std::array<Real, GENOME_SIZE> m_params;
  CCI_DifferentialSteeringActuator *m_pcWheels;
  CCI_RangeAndBearingActuator *m_pcRABAct;
  CCI_RangeAndBearingSensor *m_pcRABSens;
  CCI_LEDsActuator *m_pcLEDs;

  Real kCAM_VIEW_ANG = 15 * 3.141593 / 180.0;

 private:
  unsigned long m_group = 0;
};

