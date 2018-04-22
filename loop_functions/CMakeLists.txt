add_library(segregation_loop_function SHARED segregation_loop_function.cpp)
target_link_libraries(segregation_loop_function
                      footbot_segregation_controller
                      argos3core_simulator
                      argos3plugin_simulator_dynamics2d
                      argos3plugin_simulator_entities
                      argos3plugin_simulator_footbot
                      argos3plugin_simulator_genericrobot
                      argos3plugin_simulator_media
                      rt)

add_library(mpga SHARED mpga.cpp)
target_link_libraries(mpga argos3core_simulator segregation_loop_function)

add_library(gauci_segregation_loop_function SHARED gauci_aggregation.cpp)
target_link_libraries(gauci_segregation_loop_function segregation_loop_function)

add_library(n_class_segregation_loop_function SHARED n_class.cpp)
target_link_libraries(n_class_segregation_loop_function segregation_loop_function)

add_library(cluster_metric_segregation_loop_function SHARED cluster_metric.cpp)
target_link_libraries(cluster_metric_segregation_loop_function segregation_loop_function)
