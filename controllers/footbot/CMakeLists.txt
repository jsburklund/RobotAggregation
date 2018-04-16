add_library(footbot_segregation_controller SHARED segregation_footbot_controller.cpp)
target_link_libraries(footbot_segregation_controller
                      argos3core_simulator
                      argos3plugin_simulator_footbot
                      argos3plugin_simulator_genericrobot)
