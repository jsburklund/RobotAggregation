#include <stack>

#include "cluster_metric.h"

Real ClusterMetricLoopFunction::CostAtStep(unsigned long step, GroupMap groups) {
  for (auto &kv : groups) {
    const auto group_id = kv.first;
    const auto robots = kv.second;

    int A[robots.size()][robots.size()];
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

    // use BFS to find components and count their size
    unsigned int largest_componenet_size = 0;
    bool visited[robots.size()] = {false};
    for (size_t i = 0; i < robots.size(); ++i) {
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
            for (size_t j = 0; j < robots.size(); j++) {
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

//    std::cout << group_id << ":\n";
//    for (size_t i = 0 ; i < robots.size(); ++i) {
//      for (size_t j = 0 ; j < robots.size(); ++j) {
//        std::cout << A[i][j] << " ";
//      }
//      std::cout << std::endl;
//    }
  }

  return 0;
}

REGISTER_LOOP_FUNCTIONS(ClusterMetricLoopFunction, "cluster_metric")
