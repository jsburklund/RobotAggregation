#include <numeric>
#include <iterator>

#include "convex_hull.h"

Real ConvexHull::CostAtStep(unsigned long step, GroupMap groups) {
  auto accum_position = [](CVector3 sum, const auto &robot) {
    auto robot_position = robot->GetEmbodiedEntity().GetOriginAnchor().Position;
    return sum + robot_position;
  };

  return 0;
}

REGISTER_LOOP_FUNCTIONS(ConvexHull, "convex_hull")
