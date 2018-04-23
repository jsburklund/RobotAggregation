#pragma once

#include <array>

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_leds_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_sensor.h>

using namespace argos;

template<typename T>
constexpr T deg2rad(T deg) noexcept {
  return deg * M_PI / 180.0;
}

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

  void SetClassId(unsigned long class_id);

  SensorState GetKinSensorVal();

  std::array<Real, GENOME_SIZE> m_params;
  CCI_DifferentialSteeringActuator *m_pcWheels;
  CCI_RangeAndBearingActuator *m_pcRABAct;
  CCI_RangeAndBearingSensor *m_pcRABSens;
  CCI_LEDsActuator *m_pcLEDs;

  static constexpr Real kCAM_VIEW_ANG = deg2rad(15.0);

 private:
  unsigned long m_class = 0;
  float sensor_length_cm = INFINITY;
  bool viz = false;
};

