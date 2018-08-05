#include <iostream>
#include <fstream>
#include <string>

#include "loop_functions/cluster_metric.h"
#include "loop_functions/centroid_of_centroids.h"

#include "args.h"

int main(int argc, const char **argv) {
  args::ArgumentParser parser("compute cost and print");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::Positional<std::string> infile_flag(parser, "infile", "input file", args::Options::Required);

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

  auto &filename = args::get(infile_flag);
  std::ifstream fs(filename);

  std::string line;
  GroupPosMap poses;
  while (fs >> line) {
    float x = -1, y = -1;
    int class_id = -1;
    sscanf(line.c_str(), "%f,%f,%i", &x, &y, &class_id);
    CVector3 pos{x, y, 0.0};
    poses[class_id].push_back(pos);
  }

  std::cout << "cluster metric: " <<  cluster_metric(poses) << '\n';
  std::cout << "centroid of centroids metric: " <<  centroid_of_centroids(poses) << '\n';

}
