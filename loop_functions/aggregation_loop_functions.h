#pragma once

#include <controllers/footbot/footbot_binary_controller.h>

#include <argos3/core/utility/math/rng.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>

#include <loop_functions/mpga_loop_functions.h>

using namespace argos;

class CMPGAAggregationLoopFunctions : public CMPGALoopFunctions {

 public:

  static const size_t GENOME_SIZE = 6;

  static const UInt32 NUM_TRIALS = 5;

  CMPGAAggregationLoopFunctions();

  void Init(TConfigurationNode &t_node) override;

  void Reset() override;

  /* Configures the robot controller from the genome */
  void ConfigureFromGenome(const Real *pf_genome) override;

  /* Calculates the performance of the robot in a trial */
  Real Score() override;

 private:

   void PlaceLine(const CVector2& c_center,
                  UInt32 un_robots,
                  Real f_distance,
                  UInt32 un_id_start);

   void PlaceCluster(const CVector2& c_center,
                     UInt32 un_robots,
                     Real f_density,
                     UInt32 un_id_start);

   void PlaceScaleFree(const CVector2& c_center,
                       UInt32 un_robots,
                       Real f_range,
                       UInt32 un_id_start);

  struct SInitSetup {
    CVector3 Position;
    CQuaternion Orientation;
  };

  std::vector<SInitSetup> m_vecInitSetup;
  CFootBotEntity *m_pcFootBot;
  CFootBotBinaryController *m_pcController;
  CRandom::CRNG *m_pcRNG;
};

