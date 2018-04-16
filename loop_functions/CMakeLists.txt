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

add_library(gauci_segregation_loop_function SHARED gauci_segregation_loop_function.cpp)
target_link_libraries(gauci_segregation_loop_function segregation_loop_function)
