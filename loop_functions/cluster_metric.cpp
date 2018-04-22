#include "cluster_metric.h"
#include "eigen3/Eigen/Eigen"

Real ClusterMetricLoopFunction::CostAtStep(unsigned long step, GroupMap groups) {
  // FOR EACH GROUP:
  //   compute the adjacency matrix and degree matrix
  //   compute the lapacian
  //   find the eigenvalues of the laplacian
  //   find the number of robots in the largest cluster using the largest eigenvalue
  //   divide this by the number of robots of that group present

  for (auto &kv : groups) {
    const auto group_id = kv.first;
    const auto robots = kv.second;

    Eigen::MatrixXd A(robots.size(), robots.size());
    Eigen::MatrixXd D(robots.size(), robots.size());
    D.setZero();
    for (size_t i = 0 ; i < robots.size(); ++i) {
      for (size_t j = i + 1 ; j < robots.size(); ++j) {
        auto i_position = robots[i]->GetEmbodiedEntity().GetOriginAnchor().Position;
        auto j_position = robots[j]->GetEmbodiedEntity().GetOriginAnchor().Position;
        const auto distance = (i_position - j_position).SquareLength();
        // we some distance threshold for what is a "cluster"
        constexpr double ROBOT_RADIUS = 0.17;
        const auto a = (distance < 2 * ROBOT_RADIUS) ? 1 : 0;
        A(i, j) = a;
        A(j, i) = a;
        D(i, i) += 1;
      }
    }

    const auto L = D - A;

    std::cout << A << std::endl;
    std::cout << D << std::endl;
  }

  return 0;
}

REGISTER_LOOP_FUNCTIONS(ClusterMetricLoopFunction, "cluster_metric")
