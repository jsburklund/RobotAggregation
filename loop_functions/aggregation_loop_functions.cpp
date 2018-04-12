#include <numeric>
#include <iterator>

#include "aggregation_loop_functions.h"

static const Real        FB_AREA          = ARGOS_PI * Square(0.085036758f);
static const std::string FB_CONTROLLER    = "fb_binary";
static const UInt32      MAX_PLACE_TRIALS = 20;

CMPGAAggregationLoopFunctions::CMPGAAggregationLoopFunctions() :
    m_vecInitSetup(5),
    m_pcFootBot(nullptr),
    m_pcController(nullptr),
    m_pcRNG(nullptr) {}

void CMPGAAggregationLoopFunctions::Init(TConfigurationNode &t_node) {
  m_pcRNG = CRandom::CreateRNG("argos");

  m_pcFootBot = new CFootBotEntity(
      "fb",    // entity id
      "fb_binary"    // controller id as set in the XML
  );
  AddEntity(*m_pcFootBot);
  m_pcController = &dynamic_cast<CFootBotBinaryController &>(m_pcFootBot->GetControllableEntity().GetController());

  try {
      /*
       * Parse the configuration file
       */
      UInt32 unPlacedRobots = 0;
      /* Go through the nodes */
      TConfigurationNodeIterator itDistr;
      for(itDistr = itDistr.begin(&t_node);
          itDistr != itDistr.end();
          ++itDistr) {
         /* Make sure a known distribution was passed */
         if(itDistr->Value() != "line" &&
            itDistr->Value() != "cluster" &&
            itDistr->Value() != "scalefree") {
            THROW_ARGOSEXCEPTION("Unknown topology \"" << itDistr->Value() << "\"");
         }
         /* Get current node */
         TConfigurationNode& tDistr = *itDistr;
         /* Parse common attributes */
         /* Distribution center */
         CVector2 cCenter;
         GetNodeAttribute(tDistr, "LineEnd", cCenter);
         /* Number of robots to place */
         UInt32 unRobots;
         GetNodeAttribute(tDistr, "robot_num", unRobots);
         /* Parse distribution-specific attributes and place robots */
         if(itDistr->Value() == "line") {
            /* Distance between the robots */
            Real fDistance;
            GetNodeAttribute(tDistr, "robot_distance", fDistance);
            /* Place robots */
            PlaceLine(cCenter, unRobots, fDistance, unPlacedRobots);
         }
         else if(itDistr->Value() == "cluster") {
            /* Density of the robots */
            Real fDensity;
            GetNodeAttribute(tDistr, "robot_density", fDensity);
            /* Place robots */
            PlaceCluster(cCenter, unRobots, fDensity, unPlacedRobots);
         }
         else /* (itDistr->Value() == "scalefree") */ {
            /* Range around each robot */
            Real fRange;
            GetNodeAttribute(tDistr, "robot_range", fRange);
            /* Place robots */
            PlaceScaleFree(cCenter, unRobots, fRange, unPlacedRobots);
         }
         /* Update robot count */
         unPlacedRobots += unRobots;
      }
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("Error initializing the loop functions", ex);
   }

  /*
   * Create the initial setup for each trial
   * The robot is placed 4.5 meters away from the light
   * (which is in the origin) at angles
   * { PI/12, 2*PI/12, 3*PI/12, 4*PI/12, 5*PI/12 }
   * wrt to the world reference.
   * Also, the rotation of the robot is chosen at random
   * from a uniform distribution.
   */
  // CRadians cOrient;
  // for (size_t i = 0; i < 5; ++i) {
  //   /* Set position */
  //   m_vecInitSetup[i].Position.FromSphericalCoords(
  //       4.5f,                                          // distance from origin
  //       CRadians::PI_OVER_TWO,                         // angle with Z axis
  //       static_cast<Real>(i + 1) * CRadians::PI / 12.0f // rotation around Z
  //   );
  //   /* Set orientation */
  //   cOrient = m_pcRNG->Uniform(CRadians::UNSIGNED_RANGE);
  //   m_vecInitSetup[i].Orientation.FromEulerAngles(
  //       cOrient,        // rotation around Z
  //       CRadians::ZERO, // rotation around Y
  //       CRadians::ZERO  // rotation around X
  //   );
  // }



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

void CMPGAAggregationLoopFunctions::PlaceLine(const CVector2& c_center,
                                                  UInt32 un_robots,
                                                  Real f_distance,
                                                  UInt32 un_id_start) {
   try {
      CFootBotEntity* pcFB;
      std::ostringstream cFBId;
      /* For each robot */
      for(size_t i = 0; i < un_robots; ++i) {
         /* Make the id */
         cFBId.str("");
         cFBId << "fb" << (i + un_id_start);
         /* Create the robot in the origin and add it to ARGoS space */
         pcFB = new CFootBotEntity(
            cFBId.str(),
            FB_CONTROLLER,
            CVector3(i + c_center.GetX(), i + c_center.GetY(), 0));
         AddEntity(*pcFB);
      }
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("While placing robots in a line", ex);
   }
}

/****************************************/
/****************************************/

void CMPGAAggregationLoopFunctions::PlaceCluster(const CVector2& c_center,
                                                     UInt32 un_robots,
                                                     Real f_density,
                                                     UInt32 un_id_start) {
   try {
      /* Calculate side of the region in which the robots are scattered */
      Real fHalfSide = Sqrt((FB_AREA * un_robots) / f_density) / 2.0f;
      CRange<Real> cAreaRange(-fHalfSide, fHalfSide);
      /* Place robots */
      UInt32 unTrials;
      CFootBotEntity* pcFB;
      std::ostringstream cFBId;
      CVector3 cFBPos;
      CQuaternion cFBRot;
      /* Create a RNG (it is automatically disposed of by ARGoS) */
      CRandom::CRNG* pcRNG = CRandom::CreateRNG("argos");
      /* For each robot */
      for(size_t i = 0; i < un_robots; ++i) {
         /* Make the id */
         cFBId.str("");
         cFBId << "fb" << (i + un_id_start);
         /* Create the robot in the origin and add it to ARGoS space */
         pcFB = new CFootBotEntity(
            cFBId.str(),
            FB_CONTROLLER);
         AddEntity(*pcFB);
         /* Try to place it in the arena */
         unTrials = 0;
         bool bDone;
         do {
            /* Choose a random position */
            ++unTrials;
            cFBPos.Set(pcRNG->Uniform(cAreaRange) + c_center.GetX(),
                       pcRNG->Uniform(cAreaRange) + c_center.GetY(),
                       0.0f);
            cFBRot.FromAngleAxis(pcRNG->Uniform(CRadians::UNSIGNED_RANGE),
                                 CVector3::Z);
            bDone = MoveEntity(pcFB->GetEmbodiedEntity(), cFBPos, cFBRot);
         } while(!bDone && unTrials <= MAX_PLACE_TRIALS);
         if(!bDone) {
            THROW_ARGOSEXCEPTION("Can't place " << cFBId.str());
         }
      }
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("While placing robots in a cluster", ex);
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
