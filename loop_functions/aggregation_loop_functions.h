#pragma once

#include <controllers/footbot/GenericFootbotController.h>

#include <argos3/core/utility/math/rng.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>

#include <loop_functions/mpga_loop_functions.h>

using namespace argos;

class CMPGAAggregationLoopFunctions : public CMPGALoopFunctions {

 public:

  static constexpr auto XML_CONTROLLER_ID = "fb_aggregation";

  CMPGAAggregationLoopFunctions();

  void Init(TConfigurationNode &t_node) override;

  void Reset() override;

  /* Configures the robot controller from the genome */
  void ConfigureFromGenome(const Real *pf_genome) override;

  /* Calculates the performance of the robot in a trial */
  Real Score() override;

 private:

  void PlaceLine(const CVector2 &c_center, UInt32 un_robots, Real f_distance, UInt32 un_id_start);

  void PlaceCluster(const CVector2 &c_center, UInt32 un_robots, Real f_density, UInt32 un_id_start);

  struct RobotAndInitialPose {
    CFootBotEntity *robot;
    CVector3 position;
    CQuaternion orientation;
  };

  CRandom::CRNG *m_rng;
  std::vector<GenericFootbotController *> m_controllers;
  std::vector<RobotAndInitialPose> m_robots;
};

