add_executable(evolve evolve.cpp)
target_link_libraries(evolve mpga
                      gauci_segregation_loop_function
                      n_class_segregation_loop_function
                      argos3core_simulator)

add_executable(evaluate evaluate.cpp)
target_link_libraries(evaluate segregation_loop_function
                      gauci_segregation_loop_function
                      n_class_segregation_loop_function
                      argos3core_simulator)
