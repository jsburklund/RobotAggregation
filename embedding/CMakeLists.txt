add_executable(mpga_aggregation main.cpp)
target_link_libraries(mpga_aggregation
                      footbot_binary
                      aggregation_loop_functions
                      argos3core_simulator)

add_executable(photoaxis_main photoaxis_main.cpp)
target_link_libraries(photoaxis_main
                      footbot_nn
                      mpga_phototaxis_loop_functions
                      argos3core_simulator)
