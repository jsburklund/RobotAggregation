/*
 * This is a simple example of a multi-process genetic algorithm that
 * uses multiple processes to parallelize the optimization process.
 */

#include <algorithm>
#include <iostream>
#include <functional>
#include <fstream>
#include <loop_functions/mpga.h>
#include <loop_functions/gauci_loop_function.h>

#include "args.h"

/*
 * Flush best individual
 */
void FlushIndividual(const CMPGA::SIndividual &s_ind,
                     UInt32 un_generation) {
  std::ostringstream cOSS;
  cOSS << ".best_" << un_generation << ".dat";
  std::ofstream cOFS(cOSS.str().c_str(), std::ios::out | std::ios::trunc);
  /* First write the number of values to dump */
  cOFS << SegregationFootbotController::GENOME_SIZE;
  /* Then dump the genome */
  for (UInt32 i = 0; i < SegregationFootbotController::GENOME_SIZE; ++i) {
    cOFS << " " << s_ind.Genome[i];
  }
  /* End line */
  cOFS << std::endl;
}

Real MeanScoreAggregator(const std::vector<Real> &vec_scores) {
  auto mean = std::accumulate(begin(vec_scores), end(vec_scores), 0.0) / vec_scores.size();
  return mean;
}

int main(int argc, const char **argv) {
  args::ArgumentParser parser("main");
  args::Positional<std::string> argos_filename(parser, "argos_filename", "", args::Options::Required);

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

  CMPGA cGA(CRange<Real>(-1.0, 1.0),                   // Allele range
            SegregationFootbotController::GENOME_SIZE, // Genome size
            10,                                        // Population size
            0.15,                                      // Mutation probability
            5,                                         // Number of trials
            1000,                                        // Number of generations
            false,                                     // Maximize score?
            args::get(argos_filename),                 // .argos conf file
            &MeanScoreAggregator,                      // The score aggregator
            0                                          // Random seed
  );
  cGA.Evaluate();
  argos::LOG << "Generation #" << cGA.GetGeneration() << "...";
  argos::LOG << " scores:";
  for (auto i : cGA.GetPopulation()) {
    argos::LOG << " " << i->Score;
  }
  argos::LOG << " [Flushing genome... ";
  /* Flush scores of best individual */
  FlushIndividual(*cGA.GetPopulation()[0], cGA.GetGeneration());
  argos::LOG << "done.]";
  LOG << std::endl;
  LOG.Flush();
  while (!cGA.Done()) {
    cGA.NextGen();
    cGA.Evaluate();
    argos::LOG << "Generation #" << cGA.GetGeneration() << "...";
    argos::LOG << " scores:";
    for (auto i : cGA.GetPopulation()) {
      argos::LOG << " " << i->Score;
    }
    if (cGA.GetGeneration() % 5 == 0) {
      argos::LOG << " [Flushing genome... ";
      /* Flush scores of best individual */
      FlushIndividual(*cGA.GetPopulation()[0], cGA.GetGeneration());
      argos::LOG << "done.]";
    }
    LOG << std::endl;
    LOG.Flush();
  }
  return 0;
}
