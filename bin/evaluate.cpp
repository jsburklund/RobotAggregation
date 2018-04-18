#include <algorithm>
#include <iostream>
#include <functional>
#include <fstream>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/utility/logging/argos_log.h>
#include <loop_functions/segregation_loop_function.h>

#include "args.h"
#include "json.hpp"

int main(int argc, const char **argv) {
  args::ArgumentParser parser("main");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::Positional<std::string>
      argos_filename_flag(parser, "argos_filename", "argos config file", args::Options::Required);
  args::Positional<std::string>
      params_filename_flag(parser, "params_filename", ".dat file of controller parameters", args::Options::Required);
  args::ValueFlag<unsigned int> trials_flag(parser, "trials", "number of trails", {'t', "trials"}, 10);
  args::Flag generate_poses_flag(parser, "generate_poses", "generate a file of robots poses", {'p', "poses"}, false);

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

  std::ofstream dev_null("/dev/null");
  LOG.DisableColoredOutput();
//  LOG.GetStream().rdbuf(dev_null.rdbuf());

  argos::CSimulator &simulator = argos::CSimulator::GetInstance();
  auto &argos_filename = args::get(argos_filename_flag);
  auto generate_poses = args::get(generate_poses_flag);

  try {
    /* Set the .argos configuration file
     * This is a relative path which assumed that you launch the executable
     * from argos3-examples (as said also in the README) */
    simulator.SetExperimentFileName(argos_filename);
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
  auto &params = args::get(params_filename_flag);
  loop_function.LoadFromFile(params);

  nlohmann::json j;
  std::ofstream of;
  std::stringstream ss;
  std::replace(argos_filename.begin(), argos_filename.end(), '/', '-');
  std::replace(params.begin(), params.end(), '/', '-');
  ss << params << "_" << argos_filename << "_robot_poses.json";
  of.open(ss.str());

  std::cout << ss.str() << "\n";

  if (!of.good()) {
    std::cout << strerror(errno) << "\n";
    std::cout << ss.str() << "\n";
    return -1;
  }

  unsigned int N = args::get(trials_flag);
  for (unsigned int i = 0u; i < N; ++i) {
    simulator.Reset();
    loop_function.Reset();
    simulator.Execute();
    Real cost = loop_function.Cost();
    nlohmann::json trial_j;
    for (const auto &class_at_time_t : loop_function.classes_over_time) {
      // print all the X, Y positions of the robots at each time step
      nlohmann::json j_t;
      for (const auto &p : class_at_time_t) {
        auto class_id = p.first;
        auto poses = p.second;
        nlohmann::json j_class;
        for (const auto &pose : poses) {
          j_class.push_back({pose.GetX(), pose.GetY()});
        }
        j_t.push_back(j_class);
      }
      trial_j.push_back(j_t);
    }
    j.push_back(trial_j);
    if (!generate_poses) {
      std::cout << "trial: " << i << " cost: " << cost << "\n";
    }
  }

  if (generate_poses) {
    of << j.dump(2) << std::endl;
  }
}
