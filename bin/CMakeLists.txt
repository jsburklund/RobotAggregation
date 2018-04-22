add_executable(evolve evolve.cpp)
target_link_libraries(evolve mpga argos3core_simulator)

add_executable(evaluate evaluate.cpp)
target_link_libraries(evaluate segregation_loop_function argos3core_simulator)
