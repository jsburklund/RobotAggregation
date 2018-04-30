#include <stack>

#include "cluster_metric.h"

size_t find_largest_component(int **A, size_t n) {
  // use BFS to find components and count their size
  unsigned int largest_componenet_size = 0;
  bool visited[n] = {false};
  for (size_t i = 0; i < n; ++i) {
    auto r = i;
    if (!visited[r]) {
      unsigned int component_size = 0;
      std::stack<int> stack;
      stack.push(r);
      while (!stack.empty()) {
        auto s = stack.top();
        stack.pop();
        if (!visited[s]) {
          component_size++;
          visited[s] = true;
          for (size_t j = 0; j < n; j++) {
            if (A[s][j] == 1) {
              stack.push(j);
            }
          }
        }
      }

      if (component_size > largest_componenet_size) {
        largest_componenet_size = component_size;
      }
    }
  }

  return largest_componenet_size;
}

Real ClusterMetricLoopFunction::CostAtStep(unsigned long step, GroupMap groups) {
  Real cost = 0;
  for (auto &kv : groups) {
    const auto group_id = kv.first;
    const auto robots = kv.second;

    int **A = static_cast<int **>(malloc(robots.size() * sizeof(int *)));
    for (size_t i = 0; i < robots.size(); ++i) {
      A[i] = static_cast<int *>(calloc(robots.size(), sizeof(int)));
    }

    for (size_t i = 0; i < robots.size(); ++i) {
      A[i][i] = 0;
      for (size_t j = i + 1; j < robots.size(); ++j) {
        auto i_position = robots[i]->GetEmbodiedEntity().GetOriginAnchor().Position;
        auto j_position = robots[j]->GetEmbodiedEntity().GetOriginAnchor().Position;
        const auto distance = (i_position - j_position).Length();
        // we some distance threshold for what is a "cluster"
        constexpr double ROBOT_RADIUS = 0.085036758f;
        const auto a = (distance < 2 * ROBOT_RADIUS + .05) ? 1 : 0;
        A[i][j] = a;
        A[j][i] = a;
      }
    }

    auto largest_component_size = find_largest_component(A, robots.size());

    for (size_t i = 0; i < robots.size(); ++i) {
      free(A[i]);
    }
    free(A);

    cost += -static_cast<Real>(largest_component_size) / robots.size();
  }

  return cost / groups.size();
}

REGISTER_LOOP_FUNCTIONS(ClusterMetricLoopFunction, "cluster_metric")
