#include "segregation_loop_function.h"

SegregationLoopFunction::SegregationLoopFunction() : m_rng(nullptr) {}

void SegregationLoopFunction::Init(TConfigurationNode &t_node) {
  try {
    UInt32 seed;
    GetNodeAttribute(t_node, "random_seed", seed);
    CRandom::CreateCategory("aggregation_loop_function", seed);
    m_rng = CRandom::CreateRNG("aggregation_loop_function");

    UInt32 unPlacedRobots = 0;

    /* Go through the nodes */
    TConfigurationNodeIterator itDistr;
    for (itDistr = itDistr.begin(&t_node); itDistr != itDistr.end(); ++itDistr) {

      /* Make sure a known distribution was passed */
      if (itDistr->Value() != "line" && itDistr->Value() != "cluster") {
        THROW_ARGOSEXCEPTION("Unknown topology \"" << itDistr->Value() << "\"");
      }

      /* Get current node */
      TConfigurationNode &tDistr = *itDistr;

      /* Parse common attributes */
      /* Distribution center */
      CVector2 cCenter;
      GetNodeAttribute(tDistr, "center", cCenter);

      /* Number of robots to place */
      UInt32 unRobots;
      GetNodeAttribute(tDistr, "robot_num", unRobots);
      /* Parse distribution-specific attributes and place robots */
      if (itDistr->Value() == "line") {
        /* Distance between the robots */
        Real fDistance;
        GetNodeAttribute(tDistr, "robot_distance", fDistance);
        PlaceLine(cCenter, unRobots, fDistance, unPlacedRobots);

      } else if (itDistr->Value() == "cluster") {
        /* Density of the robots */
        Real fDensity;
        GetNodeAttribute(tDistr, "robot_density", fDensity);
        PlaceCluster(cCenter, unRobots, fDensity, unPlacedRobots);
      }

      /* Update robot count */
      unPlacedRobots += unRobots;
    }
  }
  catch (CARGoSException &ex) {
    THROW_ARGOSEXCEPTION_NESTED("Error initializing the loop functions", ex);
  }

  Reset();
}

void SegregationLoopFunction::Reset() {
  m_step = 0;
  m_cost = 0;

  for (const auto &robot_and_initial_pose: m_robots) {
    auto &entity = robot_and_initial_pose.robot->GetEmbodiedEntity();

    bool success;
    auto attempts = 0;
    do {
      CVector3 position_noise{m_rng->Gaussian(0.05, 0.0), m_rng->Gaussian(0.05, 0.0), 0};
      auto position = position_noise + robot_and_initial_pose.position;
      success = MoveEntity(entity, position, robot_and_initial_pose.orientation, false);
      ++attempts;
    } while (!success and attempts < 10);
  }
}

void SegregationLoopFunction::PlaceLine(const CVector2 &c_center,
                                        UInt32 n_robots,
                                        Real f_distance,
                                        UInt32 un_id_start) {
  try {
    std::ostringstream cFBId;
    int j = -static_cast<int>(n_robots / 2);
    for (size_t i = 0; i < n_robots; ++i, ++j) {
      cFBId.str("");
      cFBId << (i + un_id_start);

      /* Create the robot in the origin and add it to ARGoS space */
      auto position = CVector3{f_distance * j + c_center.GetX(), f_distance * j + c_center.GetY(), 0};
      CQuaternion orientation = CQuaternion{m_rng->Uniform(CRadians::UNSIGNED_RANGE), CVector3::Z};
      auto robot = new CFootBotEntity(cFBId.str(), XML_CONTROLLER_ID, position, orientation);
      AddEntity(*robot);
      auto &generic_controller = robot->GetControllableEntity().GetController();
      auto controller = &dynamic_cast<SegregationFootbotController &>(generic_controller);
      m_controllers.emplace_back(controller);
      m_robots.emplace_back(RobotAndInitialPose{robot, position, orientation});
    }
  }
  catch (CARGoSException &ex) {
    THROW_ARGOSEXCEPTION_NESTED("While placing robots in a line", ex);
  }
}

void SegregationLoopFunction::PlaceCluster(const CVector2 &c_center,
                                           UInt32 n_robots,
                                           Real f_density,
                                           UInt32 un_id_start) {
  try {
    Real fHalfSide = Sqrt((M_PI * Square(0.085036758f) * n_robots) / f_density) / 2.0f;
    CRange<Real> cAreaRange(-fHalfSide, fHalfSide);

    std::ostringstream cFBId;

    for (size_t i = 0; i < n_robots; ++i) {
      cFBId.str("");
      cFBId << (i + un_id_start);

      /* Create the robot in the origin and add it to ARGoS space */
      auto robot = new CFootBotEntity(cFBId.str(), XML_CONTROLLER_ID);
      AddEntity(*robot);
      auto &generic_controller = robot->GetControllableEntity().GetController();
      auto controller = &dynamic_cast<SegregationFootbotController &>(generic_controller);
      m_controllers.emplace_back(controller);

      /* Try to place it in the arena */
      auto attempts = 0;
      bool bDone;
      CVector3 position;
      CQuaternion orientation;
      do {
        /* Choose a random position and orientation */
        ++attempts;
        position =
            CVector3(m_rng->Uniform(cAreaRange) + c_center.GetX(),
                     m_rng->Uniform(cAreaRange) + c_center.GetY(),
                     0.0f);
        orientation = CQuaternion{m_rng->Uniform(CRadians::UNSIGNED_RANGE), CVector3::Z};
        bDone = MoveEntity(robot->GetEmbodiedEntity(), position, orientation);
      } while (!bDone && attempts <= 10);
      if (!bDone) {
        THROW_ARGOSEXCEPTION("Can't place " << cFBId.str());
      }

      m_robots.emplace_back(RobotAndInitialPose{robot, position, orientation});
    }
  }
  catch (CARGoSException &ex) {
    THROW_ARGOSEXCEPTION_NESTED("While placing robots in a cluster", ex);
  }
}

void SegregationLoopFunction::ConfigureFromGenome(const Real *genome) {
  for (const auto &robot_controller: m_controllers) {
    robot_controller->SetParameters(SegregationFootbotController::GENOME_SIZE, genome);
  }
}

void SegregationLoopFunction::LoadFromFile(const std::string &params_filename) {
  for (const auto &robot_controller: m_controllers) {
    robot_controller->LoadFromFile(params_filename);
  }
}

Real SegregationLoopFunction::Score() {
  return m_cost;
}

void SegregationLoopFunction::PostStep() {
  m_cost += CostAtStep(m_step) * m_step;
  ++m_step;
}

