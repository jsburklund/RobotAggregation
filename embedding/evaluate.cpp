#include <algorithm>
#include <iostream>
#include <functional>
#include <fstream>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/utility/logging/argos_log.h>
#include <loop_functions/segregation_loop_function.h>

#include "args.h"

int main(int argc, const char **argv) {
  args::ArgumentParser parser("main");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::Positional<std::string>
      argos_filename_flag(parser, "argos_filename", "argos config file", args::Options::Required);
  args::Positional<std::string>
      params_filename_flag(parser, "params_filename", ".dat file of controller parameters", args::Options::Required);
  args::ValueFlag<unsigned int> trials_flag(parser, "trials", "number of trails", {'t', "trials"}, 10);
  args::Flag verbose_flag(parser, "verbose", "print the cost at each time step", {'v', "verbose"});

  try {
    parser.ParseCLI(argc, argv);
  }
  catch (args::Help &e) {
    std::cout << parser;
    return 0;
  }
  catch (args::RequiredError &e) {
    std::cout << parser;
    return 0;
  }

  argos::CSimulator &simulator = argos::CSimulator::GetInstance();

  try {
    /* Set the .argos configuration file
     * This is a relative path which assumed that you launch the executable
     * from argos3-examples (as said also in the README) */
    simulator.SetExperimentFileName(args::get(argos_filename_flag));
    /* Load it to configure ARGoS */
    simulator.LoadExperiment();
    argos::LOG.Flush();
    argos::LOGERR.Flush();
  }
  catch (argos::CARGoSException &ex) {
    argos::LOGERR << ex.what() << std::endl;
  }

  /* Get a reference to the loop functions */
  auto &loop_function = dynamic_cast<SegregationLoopFunction &>(simulator.GetLoopFunctions());
  loop_function.LoadFromFile(args::get(params_filename_flag));

  unsigned int N = args::get(trials_flag);
  Real sum = 0;
  for (unsigned int i = 0u; i < N; ++i) {
    simulator.Reset();
    loop_function.Reset();
    simulator.Execute();
    Real cost = loop_function.Cost();
    sum += cost;
    if (args::get(verbose_flag)) {
    } else {
      printf("trial: %u, cost: %.0f\r\n", i, cost);
    }
  }
  auto mean =  sum / N;
  printf("mean: %.4e\r\n", mean);
}
