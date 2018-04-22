#include "cluster_metric.h"

Real ClusterMetricLoopFunction::CostAtStep(unsigned long step, GroupMap groups) {
  // FOR EACH GROUP:
  //   compute the adjacency matrix and degree matrix
  //   compute the lapacian
  //   find the eigenvalues of the laplacian
  //   find the number of robots in the largest cluster using the largest eigenvalue
  //   divide this by the number of robots of that group present

  for (auto &kv : groups) {
    auto group_id = kv.first;
    auto robots = kv.second;
  }

  return 0;
}

REGISTER_LOOP_FUNCTIONS(ClusterMetricLoopFunction, "cluster_metric")
