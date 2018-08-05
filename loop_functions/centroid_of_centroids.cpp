#include <numeric>
#include <iterator>

#include <loop_functions/centroid_of_centroids.h>
#include <loop_functions/segregation_loop_function.h>

namespace argos {

Real centroid_of_centroids(GroupPosMap groups) {
  constexpr double ROBOT_RADIUS = 0.085036758f;
  auto accum_position = [](CVector3 sum, const auto &pos) {
    auto robot_position = pos;
    return sum + robot_position;
  };

  auto group_aggregation_cost = 0.0;
  std::vector<CVector3> centroids;
  for (const auto &group : groups) {
    auto poses = group.second;

    auto centroid =
        std::accumulate(std::begin(poses), std::end(poses), CVector3::ZERO, accum_position) / poses.size();

    centroids.emplace_back(centroid);

    auto accum_cost = [centroid](double cost, const auto &pos) {
      auto robot_position = pos;
      auto c = (robot_position - centroid).SquareLength();
      return cost + c;
    };

    group_aggregation_cost = std::accumulate(std::begin(poses), std::end(poses), 0.0, accum_cost);
    group_aggregation_cost *= 1 / (4 * std::pow(ROBOT_RADIUS, 2));

  }

  auto centroid_of_centroids =
      std::accumulate(std::begin(centroids), std::end(centroids), CVector3::ZERO) / groups.size();
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

}
