#pragma once

#include <unordered_map>

#include <argos3/core/utility/math/rng.h>
#include <argos3/core/simulator/loop_functions.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>

#include <loop_functions/segregation_loop_function.h>
#include <controllers/footbot/segregation_footbot_controller.h>
#include <bits/unordered_map.h>

using namespace argos;

class SegregationLoopFunction : public CLoopFunctions {

 public:

  using GroupMap = std::unordered_map<unsigned long, std::vector<CFootBotEntity *>>;
  using GroupPosMap = std::unordered_map<unsigned long, std::vector<CVector3>>;

  static constexpr auto XML_CONTROLLER_ID = "fb_segregation";

  SegregationLoopFunction();

  void Init(TConfigurationNode &t_node) override;

  void Reset() override;

  /* Configures the robot controller from the genome */
  void ConfigureFromGenome(const Real *pf_genome);

  void LoadFromFile(const std::string &params_filename);

  void LoadParameters(const size_t n_params, const Real *params);

  /**
   * Executes user-defined logic right after a control step is executed.
   */
  void PostStep() override;

  virtual Real CostAtStep(unsigned long step, GroupMap map) = 0;

  /* Calculates the performance of the robot in a trial */
  Real Cost();

  GroupMap classes;
  std::vector<GroupPosMap> classes_over_time;

 protected:
  std::unordered_map<std::string, unsigned long> id_string_class_map;

 private:

  void PlaceSingle(const CVector2 &center, unsigned long class_id, UInt32 id);

  void PlaceLine(const CVector2 &center, UInt32 un_robots, Real distance, UInt32 un_id_start);

  void PlaceCluster(const CVector2 &center, UInt32 un_robots, Real density, UInt32 un_id_start);

  struct RobotAndInitialPose {
    CFootBotEntity *robot;
    CVector3 position;
    CQuaternion orientation;
  };

  CRandom::CRNG *m_rng;
  std::vector<SegregationFootbotController *> m_controllers;
  std::vector<RobotAndInitialPose> m_robots;
  Real m_cost = 0;
  unsigned long m_step = 0;

  UInt8 n_classes;
};
