/*
 * This is a simple example of a multi-process genetic algorithm that
 * uses multiple processes to parallelize the optimization process.
 */

#include <iostream>
#include <numeric>
#include <iterator>
#include <fstream>
#include <loop_functions/mpga.h>
#include <loop_functions/gauci_loop_function.h>

#include "args.h"

void FlushPopulation(const CMPGA::TPopulation population, UInt32 generation) {
  auto individual_idx = 0;
  for (const auto &individual : population) {
    std::ostringstream oss;
    oss << "individual_" << individual_idx++ << "_gen_" << generation << ".dat";
    std::ofstream ofs(oss.str().c_str(), std::ios::out | std::ios::trunc);
    ofs << SegregationFootbotController::GENOME_SIZE << " ";
    for (const auto gene : individual->Genome) {
      ofs << gene << " ";
    }
    ofs << std::endl;
  };
}

Real MeanScoreAggregator(const std::vector<Real> &vec_scores) {
  auto mean = std::accumulate(begin(vec_scores), end(vec_scores), 0.0) / vec_scores.size();
  return mean;
}

int main(int argc, const char **argv) {
  args::ArgumentParser parser("main");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::Positional<std::string> argos_filename(parser, "argos_filename", "", args::Options::Required);
  args::ValueFlag<unsigned int>
      population_size_flag(parser, "population size", "population size", {'p', "population"}, 10);
  args::ValueFlag<double> mutation_flag(parser, "mutation", "probability of a gene mutating", {'m', "mutation"}, 0.05);
  args::ValueFlag<unsigned int>
      num_trials_flag(parser, "trials", "number of trials each genome undergoes", {'t', "trials"}, 10);
  args::ValueFlag<unsigned int>
      num_generations_flag(parser, "generations", "number of generations", {'g', "generations"}, 10);
  args::ValueFlag<unsigned int> random_seed_flag(parser, "seed", "seed for the genetic algorithm", {'s', "seed"}, 0);

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

  auto pop_size = args::get(population_size_flag);

  CMPGA cGA(CRange<Real>(-1.0, 1.0),
            SegregationFootbotController::GENOME_SIZE,
            pop_size,
            args::get(mutation_flag),
            args::get(num_trials_flag),
            args::get(num_generations_flag),
            false,
            args::get(argos_filename),
            &MeanScoreAggregator,
            args::get(random_seed_flag),
            false);
  cGA.Evaluate();
  argos::LOG << "Generation #" << cGA.GetGeneration() << "...";
  argos::LOG << " costs:";
  for (auto i : cGA.GetPopulation()) {
    argos::LOG << " " << i->Score;
  }
  FlushPopulation(cGA.GetPopulation(), cGA.GetGeneration());
  LOG << std::endl;
  LOG.Flush();
  while (!cGA.Done()) {
    cGA.NextGen();
    cGA.Evaluate();
    argos::LOG << "Generation #" << cGA.GetGeneration() << "...";
    argos::LOG << " costs:";
    for (auto i : cGA.GetPopulation()) {
      argos::LOG << " " << i->Score;
    }

    FlushPopulation(cGA.GetPopulation(), cGA.GetGeneration());

    LOG << std::endl;
    LOG.Flush();
  }
  return 0;
}
