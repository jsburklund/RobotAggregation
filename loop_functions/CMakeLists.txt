set(FREE_FUNCS centroid_of_centroids.cpp cluster_metric.cpp)

add_library(segregation_loop_function SHARED segregation_loop_function.cpp ${FREE_FUNCS})
target_link_libraries(segregation_loop_function
        footbot_segregation_controller
        argos3core_simulator
        argos3plugin_simulator_dynamics2d
        argos3plugin_simulator_entities
        argos3plugin_simulator_footbot
        argos3plugin_simulator_genericrobot
        argos3plugin_simulator_media
        rt)

