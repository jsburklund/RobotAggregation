#!/usr/bin/env python

import argparse
import subprocess
import sys
import time

import cma
from multiprocessing import Pool
import numpy as np


def evaluate_params(args):
    params, argos_file, library_path, trials, verbose = args
    params_str = "\"6 " + " ".join([str(p) for p in params]) + "\""
    cmd = ["./build/bin/evaluate", "--params-as-string", "-t", str(trials), argos_file, library_path, params_str]
    cmd_str = " ".join(cmd)
    if verbose:
        print(cmd_str)
    output = subprocess.run(cmd_str, stdout=subprocess.PIPE, shell=True)
    if output.returncode != 0:
        print("subprocess failed:")
        print(" ".join(cmd))
        output = output.stdout.decode("UTF-8")
        print(output)
        return -1

    sum = 0
    output = output.stdout.decode("UTF-8").split("\n")[8:-1]
    costs = []
    for line in output:
        try:
            cost = float(line.split(" ")[-1])
        except ValueError:
            continue
        sum += float(cost)
        costs.append(cost)
    mean = sum / len(costs)
    if verbose:
        print(params, "{:s} -> {:0.2f}".format(argos_file, mean))
    return mean


def main():
    np.set_printoptions(suppress=True, precision=2)
    np.random.seed(0)

    parser = argparse.ArgumentParser("Evolve controller parameters")
    parser.add_argument("argos_files", help="all the argos files you want to run evaluate with", nargs="+")
    parser.add_argument("library_path", help="the path to the loop function *.so library to use")
    parser.add_argument("--pool-size", "-p", help="number of worker subprocesses to spawn", type=int, required=True)
    parser.add_argument("--generations", '-g', help="number of generations", type=int, default=10)
    parser.add_argument("--trials", '-t', help="number of trials per argos configuration", type=int, default=1)
    parser.add_argument("--popsize", help="number of genomes per population", type=int, default=10)
    parser.add_argument("--cma-seed", help="seed for CMA-ES", type=int, default=1)  # cannot be 0
    parser.add_argument("--verbose", "-v", help="print more shit", action="store_true")
    args = parser.parse_args()

    outfile = open("cma_evolve_output_{:d}.txt".format(int(time.time())), 'w')
    es = cma.CMAEvolutionStrategy(np.zeros(6), 0.72, {'seed': args.cma_seed, 'popsize': args.popsize, 'bounds': [-1, 1]})
    generation_idx = 0
    pool = Pool(processes=args.pool_size)
    while not es.stop() and generation_idx < args.generations:
        pop = es.ask()
        costs_per_genome = []
        print("Generation:", generation_idx)
        for genome in pop:
            pool_args = [(genome, f, args.library_path, args.trials, args.verbose) for f in args.argos_files]
            costs = pool.map(evaluate_params, pool_args)
            genome_mean_cost = np.mean(costs)
            costs_per_genome.append(genome_mean_cost)
            print("evaluated: {} {}".format(genome, genome_mean_cost))

        es.tell(pop, costs_per_genome)
        generation_idx += 1

        print("mean cost of generation:", np.mean(costs_per_genome))

    print("finished in {:d} iterations".format(es.result.iterations))
    print("final population, cost")

    outfile.write("vl0 vr0 vl1 vr1 vl2 vr2 cost")
    pop_and_costs = sorted(zip(pop, costs_per_genome), key=lambda pair: pair[1])
    for p, c in pop_and_costs:
        print(p, c)
        population_as_string = "".join(["{:.8f} ".format(i) for i in p])[:-2]
        outfile.write(population_as_string)
        outfile.write(" ")
        outfile.write("{:.8f}".format(c))
        outfile.write("\n")

    return 0


if __name__ == '__main__':
    sys.exit(main())
