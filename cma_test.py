#!/usr/bin/env python3

import numpy as np
import cma


def compute_costs(pop):
    costs = []
    for p in pop:
        cost = abs(np.sum(p) - 100)
        costs.append(cost)
    return costs


def main():
    print("Problem: find three numbers that add up to 100")
    es = cma.CMAEvolutionStrategy(np.zeros(3), 0.5)
    while not es.stop():
        pop = es.ask()
        costs = compute_costs(pop)
        es.tell(pop, costs)
    print("finished in {:d} iterations", es.result.iterations)
    print("final population, cost")
    for p, c in zip(pop, costs):
        print(p, c)


if __name__ == '__main__':
    main()
