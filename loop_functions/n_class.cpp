#include <numeric>
#include <iterator>

#include "n_class.h"

Real NClass::CostAtStep(unsigned long step, GroupMap groups) {
  constexpr double ROBOT_RADIUS = 0.085036758f;
  auto accum_position = [](CVector3 sum, const auto &robot) {
    auto robot_position = robot->GetEmbodiedEntity().GetOriginAnchor().Position;
    return sum + robot_position;
  };

  auto group_aggregation_cost = 0.0;
  std::vector<CVector3> centroids;
  for (const auto &group : groups) {
    auto robots = group.second;

    auto centroid =
        std::accumulate(std::begin(robots), std::end(robots), CVector3::ZERO, accum_position) / robots.size();

    centroids.emplace_back(centroid);

    auto accum_cost = [centroid](double cost, const auto &robot) {
      auto robot_position = robot->GetEmbodiedEntity().GetOriginAnchor().Position;
      auto c = (robot_position - centroid).SquareLength();
      return cost + c;
    };

    group_aggregation_cost = std::accumulate(std::begin(robots), std::end(robots), 0.0, accum_cost);
    group_aggregation_cost *= 1 / (4 * std::pow(ROBOT_RADIUS, 2));

  }

  auto centroid_of_centroids = std::accumulate(std::begin(centroids), std::end(centroids), CVector3::ZERO);
  auto accum_centroid_cost = [centroid_of_centroids](double cost, const CVector3 p) {
    auto c = (p - centroid_of_centroids).SquareLength();
    return cost + c;
  };
  auto centroid_dispersion_cost =
      std::accumulate(std::begin(centroids), std::end(centroids), 0.0, accum_centroid_cost);
  centroid_dispersion_cost *= 1 / (4 * std::pow(ROBOT_RADIUS, 2));

  // centroid dispersion cost should be high
  // so we subtract it to make the overall cost lower when centroids are dispersed.
  return group_aggregation_cost - centroid_dispersion_cost;
}

REGISTER_LOOP_FUNCTIONS(NClass, "n_class")
