#include <numeric>
#include <iterator>

#include "aggregation_loop_functions.h"

CMPGAAggregationLoopFunctions::CMPGAAggregationLoopFunctions() : m_rng(nullptr) {}

void CMPGAAggregationLoopFunctions::Init(TConfigurationNode &t_node) {
  m_rng = CRandom::CreateRNG("argos");

  try {
    /*
     * Parse the configuration file
     */
    UInt32 unPlacedRobots = 0;
    /* Go through the nodes */
    TConfigurationNodeIterator itDistr;
    for (itDistr = itDistr.begin(&t_node);
         itDistr != itDistr.end();
         ++itDistr) {
      /* Make sure a known distribution was passed */
      if (itDistr->Value() != "line" &&
          itDistr->Value() != "cluster" &&
          itDistr->Value() != "scalefree") {
        THROW_ARGOSEXCEPTION("Unknown topology \"" << itDistr->Value() << "\"");
      }
      /* Get current node */
      TConfigurationNode &tDistr = *itDistr;
      /* Parse common attributes */
      /* Distribution center */
      CVector2 cCenter;
      GetNodeAttribute(tDistr, "LineEnd", cCenter);
      /* Number of robots to place */
      UInt32 unRobots;
      GetNodeAttribute(tDistr, "robot_num", unRobots);
      /* Parse distribution-specific attributes and place robots */
      if (itDistr->Value() == "line") {
        /* Distance between the robots */
        Real fDistance;
        GetNodeAttribute(tDistr, "robot_distance", fDistance);
        /* Place robots */
        PlaceLine(cCenter, unRobots, fDistance, unPlacedRobots);
      } else if (itDistr->Value() == "cluster") {
        /* Density of the robots */
        Real fDensity;
        GetNodeAttribute(tDistr, "robot_density", fDensity);
        /* Place robots */
        PlaceCluster(cCenter, unRobots, fDensity, unPlacedRobots);
      }
      /* Update robot count */
      unPlacedRobots += unRobots;
    }
  }
  catch (CARGoSException &ex) {
    THROW_ARGOSEXCEPTION_NESTED("Error initializing the loop functions", ex);
  }

  /*
   * Process trial information, if any
   */
  try {
    UInt32 unTrial;
    GetNodeAttribute(t_node, "trial", unTrial);
    SetTrial(unTrial);
    Reset();
  }
  catch (CARGoSException &ex) {}
}

void CMPGAAggregationLoopFunctions::Reset() {
  for (const auto &robot: m_robots) {
    auto entity = robot->GetEmbodiedEntity();
    CVector3 position;
    CQuaternion orientation;
    auto success = MoveEntity(entity, position, orientation, false);
    if (!success) {
      LOGERR << "Can't move robot " << entity.GetId() << "\n";
    }
  }
}

void CMPGAAggregationLoopFunctions::PlaceLine(const CVector2 &c_center,
                                              UInt32 n_robots,
                                              Real f_distance,
                                              UInt32 un_id_start) {
  try {
    CFootBotEntity *pcFB;
    std::ostringstream cFBId;
    /* For each robot */
    for (size_t i = 0; i < n_robots; ++i) {
      /* Make the id */
      cFBId.str("");
      cFBId << "fb" << (i + un_id_start);
      /* Create the robot in the origin and add it to ARGoS space */
      pcFB = new CFootBotEntity(cFBId.str(), XML_CONTROLLER_ID, CVector3(i + c_center.GetX(), i + c_center.GetY(), 0));
      AddEntity(*pcFB);
    }
  }
  catch (CARGoSException &ex) {
    THROW_ARGOSEXCEPTION_NESTED("While placing robots in a line", ex);
  }
}

/****************************************/
/****************************************/

void CMPGAAggregationLoopFunctions::PlaceCluster(const CVector2 &c_center,
                                                 UInt32 n_robots,
                                                 Real f_density,
                                                 UInt32 un_id_start) {
  try {
    /* Calculate side of the region in which the robots are scattered */
    CRange<Real> cAreaRange(-4, 4);
    /* Place robots */
    std::ostringstream cFBId;
    CVector3 cFBPos;
    CQuaternion cFBRot;

    /* Create a RNG (it is automatically disposed of by ARGoS) */

    /* For each robot */
    for (size_t i = 0; i < n_robots; ++i) {
      /* Make the id */
      cFBId.str("");
      cFBId << "fb" << (i + un_id_start);

      /* Create the robot in the origin and add it to ARGoS space */
      auto pcFB = new CFootBotEntity(cFBId.str(), XML_CONTROLLER_ID);
      AddEntity(*pcFB);

      /* Try to place it in the arena */
      auto unTrials = 0;
      bool bDone;
      do {
        /* Choose a random position */
        ++unTrials;
        cFBPos.Set(m_rng->Uniform(cAreaRange) + c_center.GetX(), m_rng->Uniform(cAreaRange) + c_center.GetY(), 0.0f);
        cFBRot.FromAngleAxis(m_rng->Uniform(CRadians::UNSIGNED_RANGE), CVector3::Z);
        bDone = MoveEntity(pcFB->GetEmbodiedEntity(), cFBPos, cFBRot);
      } while (!bDone && unTrials <= 20);
      if (!bDone) {
        THROW_ARGOSEXCEPTION("Can't place " << cFBId.str());
      }
    }
  }
  catch (CARGoSException &ex) {
    THROW_ARGOSEXCEPTION_NESTED("While placing robots in a cluster", ex);
  }
}

void CMPGAAggregationLoopFunctions::ConfigureFromGenome(const Real *genome) {
  /* Set the Binary parameters */
  for (const auto &robot_controller: m_controllers) {
    robot_controller->SetParameters(CFootBotBinaryController::GENOME_SIZE, genome);
  }
}

Real CMPGAAggregationLoopFunctions::Score() {
  CSpace::TMapPerType &tFBMap = GetSpace().GetEntitiesByType("foot-bot");

  auto accum_position = [](CVector3 sum, const CSpace::TMapPerType::value_type &p) {
    CFootBotEntity *pcFB = any_cast<CFootBotEntity *>(p.second);
    auto robot_position = pcFB->GetEmbodiedEntity().GetOriginAnchor().Position;
    return sum + robot_position;
  };

  auto centroid = std::accumulate(std::begin(tFBMap), std::end(tFBMap), CVector3::ZERO, accum_position) / tFBMap.size();

  auto accum_cost = [centroid](double cost, const CSpace::TMapPerType::value_type &p) {
    CFootBotEntity *pcFB = any_cast<CFootBotEntity *>(p.second);
    auto robot_position = pcFB->GetEmbodiedEntity().GetOriginAnchor().Position;
    auto c = (robot_position - centroid).SquareLength();
    return cost + c;
  };

  auto cost = std::accumulate(std::begin(tFBMap), std::end(tFBMap), 0.0, accum_cost);
  constexpr double ROBOT_RADIUS = 0.17;
  cost *= 1 / (4 * std::pow(ROBOT_RADIUS, 2));

  return cost;
}

REGISTER_LOOP_FUNCTIONS(CMPGAAggregationLoopFunctions, "aggregation_loop_functions")
