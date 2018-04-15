/*
 * This is a simple example of a multi-process genetic algorithm that
 * uses multiple processes to parallelize the optimization process.
 */

#include <algorithm>
#include <iostream>
#include <functional>
#include <fstream>
#include <loop_functions/mpga.h>
#include <loop_functions/aggregation_loop_functions.h>

/*
 * Flush best individual
 */
void FlushIndividual(const CMPGA::SIndividual &s_ind,
                     UInt32 un_generation) {
  std::ostringstream cOSS;
  cOSS << ".best_" << un_generation << ".dat";
  std::ofstream cOFS(cOSS.str().c_str(), std::ios::out | std::ios::trunc);
  /* First write the number of values to dump */
  cOFS << GenericFootbotController::GENOME_SIZE;
  /* Then dump the genome */
  for (UInt32 i = 0; i < GenericFootbotController::GENOME_SIZE; ++i) {
    cOFS << " " << s_ind.Genome[i];
  }
  /* End line */
  cOFS << std::endl;
}

Real RobotAggregationScorer(const std::vector<Real> &vec_scores) {
  auto max = std::max_element(begin(vec_scores), end(vec_scores));
  return *max;
}

int main() {
  CMPGA cGA(CRange<Real>(-1.0, 1.0),            // Allele range
            GenericFootbotController::GENOME_SIZE, // Genome size
            10,                                   // Population size
            0.05,                                // Mutation probability
            5, // Number of trials
            1,                                 // Number of generations
            false,                               // Minimize score
            "experiments/aggregation.argos",            // .argos conf file
            &RobotAggregationScorer,             // The score aggregator
            12345                                // Random seed
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
