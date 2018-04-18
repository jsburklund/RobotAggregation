#include <numeric>
#include <iterator>

#include "n_class_loop_function.h"

Real NClassLoopFunction::CostAtStep(unsigned long step, GroupMap groups) {
  auto accum_position = [](CVector3 sum, const auto &robot) {
    auto robot_position = robot->GetEmbodiedEntity().GetOriginAnchor().Position;
    return sum + robot_position;
  };

  auto cost = 0.0;
  // calculate the cluster metric for each group
  std::vector<CVector3> centroids;
  for (const auto &group : groups) {
    auto robots = group.second;

    auto centroid =
        std::accumulate(std::begin(robots), std::end(robots), CVector3::ZERO, accum_position)
            / robots.size();

    centroids.emplace_back(centroid);

    auto accum_cost = [centroid](double cost, const auto &robot) {
      auto robot_position = robot->GetEmbodiedEntity().GetOriginAnchor().Position;
      auto c = (robot_position - centroid).SquareLength();
      return cost + c;
    };

    cost = std::accumulate(std::begin(robots), std::end(robots), 0.0, accum_cost);
    constexpr double ROBOT_RADIUS = 0.17;
    cost *= 1 / (4 * std::pow(ROBOT_RADIUS, 2));

  }

  auto centroid_of_centroids = std::accumulate(std::begin(centroids), std::end(centroids), CVector3::ZERO);
  auto accum_centroid_cost = [centroid_of_centroids](double cost, const CVector3 p) {
    auto c = (p - centroid_of_centroids).SquareLength();
    return cost + c;
  };
  auto centroid_dispersion_cost =
      std::accumulate(std::begin(centroids), std::end(centroids), 0.0, accum_centroid_cost);
  constexpr double ROBOT_RADIUS = 0.17;
  centroid_dispersion_cost *= 1 / (4 * std::pow(ROBOT_RADIUS, 2));

  // centroid dispersion cost should be high
  // so we subtract it to make the overall cost lower when centroids are dispersed.
  return cost - centroid_dispersion_cost;
}

REGISTER_LOOP_FUNCTIONS(NClassLoopFunction, "n_class_segregation_loop_function")
