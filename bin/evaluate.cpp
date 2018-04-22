#include <algorithm>
#include <iostream>
#include <functional>
#include <fstream>

#include <argos3/core/simulator/simulator.h>
#include <argos3/core/utility/logging/argos_log.h>
#include <argos3/core/utility/configuration/tinyxml/ticpp.h>

#include <loop_functions/segregation_loop_function.h>

#include "args.h"
#include "json.hpp"

int main(int argc, const char **argv) {
  args::ArgumentParser parser("simulate an argos config and controller params, and either print cost or save poses.");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::Positional<std::string> base_argos_config_flag(parser, "base_argos_filename", "base.argos config file",
                                                       args::Options::Required);
  args::Positional<std::string> params_filename_flag(parser, "params_filename", ".dat file of controller parameters",
                                                     args::Options::Required);
  args::Positional<std::string> library_path_flag(parser, "loop_function_library",
                                                  "the relative path to the library to load", args::Options::Required);
  args::Positional<std::string> library_label_flag(parser, "loop_function_label",
                                                   "the label used in the macro for that loop function",
                                                   args::Options::Required);
  args::ValueFlag<unsigned int> trials_flag(parser, "trials", "number of trails", {'t', "trials"}, 4);
  args::ValueFlag<unsigned int> num_classes_flag(parser, "num_classes", "number of classes", {'c', "classes"}, 4);
  args::Flag viz_flag(parser, "viz", "show argos visualizaiton", {'z', "viz"}, false);
  args::Flag generate_poses_flag(parser, "generate_poses", "generate a file of robots poses", {'p', "poses"}, false);
  args::Flag params_as_str_flag(parser, "params_as_string", "the params input is actually a string of numbers",
                                {"params-as-string"}, false);

  try {
    parser.ParseCLI(argc, argv);
  }
  catch (args::Help &e) {
    std::cout << e.what() << std::endl;
    std::cout << parser;
    return 0;
  }
  catch (args::RequiredError &e) {
    std::cout << e.what() << std::endl;
    std::cout << parser;
    return -1;
  }

  std::ofstream dev_null("/dev/null");
  LOG.DisableColoredOutput();
//  LOG.GetStream().rdbuf(dev_null.rdbuf());

  argos::CSimulator &simulator = argos::CSimulator::GetInstance();
  auto &argos_filename = args::get(base_argos_config_flag);
  auto library_path = args::get(library_path_flag);
  auto library_label = args::get(library_label_flag);
  auto generate_poses = args::get(generate_poses_flag);
  auto num_classes = args::get(num_classes_flag);
  auto viz = args::get(viz_flag);

  ticpp::Document argos_config;
  argos_config.LoadFile(argos_filename);
  auto loop_functions = argos_config.FirstChildElement()->FirstChildElement("loop_functions");
  loop_functions->SetAttribute("library", library_path);
  loop_functions->SetAttribute("label", library_label);
  loop_functions->SetAttribute("num_classes", num_classes);
  if (viz) {
    auto viz_element = argos_config.FirstChildElement()->FirstChildElement("visualization");
    ticpp::Element placement("placement");
    placement.SetAttribute("idx", "0");
    placement.SetAttribute("position", "2.5,2.5,6");
    placement.SetAttribute("look_at", "2.5,2.5,0");
    placement.SetAttribute("lens_focal_length", "30");
    ticpp::Element camera("camera");
    ticpp::Element qt("qt-opengl");
    camera.InsertEndChild(placement);
    qt.InsertEndChild(camera);
    viz_element->InsertEndChild(qt);
  }

  try {
    simulator.Load(argos_config);
//    simulator.SetExperimentFileName(argos_filename);
//    simulator.LoadExperiment();
    argos::LOG.Flush();
    argos::LOGERR.Flush();
  }
  catch (argos::CARGoSException &ex) {
    argos::LOGERR << ex.what() << std::endl;
  }

  /* Get a reference to the loop functions */
  auto &loop_function = dynamic_cast<SegregationLoopFunction &>(simulator.GetLoopFunctions());

  auto params_str = args::get(params_filename_flag);
  if (args::get(params_as_str_flag)) {
    std::stringstream ss(params_str);
    size_t n_params;
    ss >> n_params;
    Real params[n_params];
    Real param;
    auto i = 0u;
    while (ss >> param) {
      params[i++] = param;
    }
    loop_function.LoadParameters(n_params, params);
  } else {
    loop_function.LoadFromFile(params_str);
  }

  nlohmann::json j;
  std::ofstream of;
  if (generate_poses) {
    std::stringstream ss;
    std::replace(argos_filename.begin(), argos_filename.end(), '/', '-');
    ss << argos_filename << ".evaluate_output";
    of.open(ss.str());
    std::cout << ss.str() << "\n";

    if (!of.good()) {
      std::cout << strerror(errno) << "\n";
      std::cout << ss.str() << "\n";
      return -1;
    }
  }

  unsigned int N = args::get(trials_flag);
  for (unsigned int i = 0u; i < N; ++i) {
    simulator.Reset();
    loop_function.Reset();
    simulator.Execute();
    Real cost = loop_function.Cost();
    nlohmann::json trial_j;
    if (generate_poses) {
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
    }
    if (!generate_poses) {
      std::cout << i << " " << cost << "\n";
    }
  }

  if (generate_poses) {
    of << j.dump(2) << std::endl;
  }
}
