#include <numeric>
#include <iterator>

#include "aggregation_loop_functions.h"

CMPGAAggregationLoopFunctions::CMPGAAggregationLoopFunctions() :
    m_vecInitSetup(5),
    m_pcFootBot(nullptr),
    m_pcController(nullptr),
    m_pcRNG(nullptr) {}

void CMPGAAggregationLoopFunctions::Init(TConfigurationNode &t_node) {
  m_pcRNG = CRandom::CreateRNG("argos");

  m_pcFootBot = new CFootBotEntity(
      "fb",    // entity id
      "fbbinary"    // controller id as set in the XML
  );
  AddEntity(*m_pcFootBot);
  m_pcController = &dynamic_cast<CFootBotBinaryController &>(m_pcFootBot->GetControllableEntity().GetController());

  /*
   * Create the initial setup for each trial
   * The robot is placed 4.5 meters away from the light
   * (which is in the origin) at angles
   * { PI/12, 2*PI/12, 3*PI/12, 4*PI/12, 5*PI/12 }
   * wrt to the world reference.
   * Also, the rotation of the robot is chosen at random
   * from a uniform distribution.
   */
  CRadians cOrient;
  for (size_t i = 0; i < 5; ++i) {
    /* Set position */
    m_vecInitSetup[i].Position.FromSphericalCoords(
        4.5f,                                          // distance from origin
        CRadians::PI_OVER_TWO,                         // angle with Z axis
        static_cast<Real>(i + 1) * CRadians::PI / 12.0f // rotation around Z
    );
    /* Set orientation */
    cOrient = m_pcRNG->Uniform(CRadians::UNSIGNED_RANGE);
    m_vecInitSetup[i].Orientation.FromEulerAngles(
        cOrient,        // rotation around Z
        CRadians::ZERO, // rotation around Y
        CRadians::ZERO  // rotation around X
    );
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
  /*
   * Move robot to the initial position corresponding to the current trial
   */
  if (!MoveEntity(
      m_pcFootBot->GetEmbodiedEntity(),             // move the body of the robot
      m_vecInitSetup[GetTrial()].Position,    // to this position
      m_vecInitSetup[GetTrial()].Orientation, // with this orientation
      false                                         // this is not a check, leave the robot there
  )) {
    LOGERR << "Can't move robot in <"
           << m_vecInitSetup[GetTrial()].Position
           << ">, <"
           << m_vecInitSetup[GetTrial()].Orientation
           << ">"
           << std::endl;
  }
}

void CMPGAAggregationLoopFunctions::ConfigureFromGenome(const Real *pf_genome) {
  /* Set the Binary parameters */
  m_pcController->SetParameters(GENOME_SIZE, pf_genome);
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
    return cost + (robot_position - centroid).SquareLength();
  };

  auto cost = std::accumulate(std::begin(tFBMap), std::end(tFBMap), 0, accum_cost) / tFBMap.size();
  constexpr double ROBOT_RADIUS = 0.17;
  cost *= 1 / (4 * std::pow(ROBOT_RADIUS, 2));
}

REGISTER_LOOP_FUNCTIONS(CMPGAAggregationLoopFunctions, "aggregation_loop_functions")
