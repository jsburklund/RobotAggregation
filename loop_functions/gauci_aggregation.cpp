#include <numeric>
#include <iterator>

#include "gauci_aggregation.h"

Real GauciLoopFunction::CostAtStep(unsigned long step, GroupMap /*groups*/) {
  // Equation (1) from Self-organized aggregation without computation
  CSpace::TMapPerType &robot_map = GetSpace().GetEntitiesByType("foot-bot");

  auto accum_position = [](CVector3 sum, const CSpace::TMapPerType::value_type &p) {
    auto robot = any_cast<CFootBotEntity *>(p.second);
    auto robot_position = robot->GetEmbodiedEntity().GetOriginAnchor().Position;
    return sum + robot_position;
  };

  auto centroid =
      std::accumulate(std::begin(robot_map), std::end(robot_map), CVector3::ZERO, accum_position) / robot_map.size();

  auto accum_cost = [centroid](double cost, const CSpace::TMapPerType::value_type &p) {
    auto robot = any_cast<CFootBotEntity *>(p.second);
    auto robot_position = robot->GetEmbodiedEntity().GetOriginAnchor().Position;
    auto c = (robot_position - centroid).SquareLength();
    return cost + c;
  };

  auto cost = std::accumulate(std::begin(robot_map), std::end(robot_map), 0.0, accum_cost);
  constexpr double ROBOT_RADIUS = 0.17;
  cost *= 1 / (4 * std::pow(ROBOT_RADIUS, 2));

  return cost;
}

REGISTER_LOOP_FUNCTIONS(GauciLoopFunction, "gauci_aggregation")
