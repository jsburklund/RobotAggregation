#!/usr/bin/env python3

import re
import argparse
import csv
import os
import subprocess
import sys
import time

from multiprocessing import Pool
import numpy as np

def evaluate_params(args):
    # Execute evaluate and save the poses of time
    param_idx, params, argos_file, library_path, trials, verbose = args
    params_str = "\"6 " + " ".join([str(p) for p in params]) + "\""
    cmd = ["./build/bin/evaluate", "-s", "100", "--params-as-string", "-t", str(trials), argos_file, library_path, params_str]
    cmd_str = " ".join(cmd)
    output = subprocess.run(cmd_str, stdout=subprocess.PIPE, shell=True)
    if output.returncode != 0:
        print("subprocess failed:")
        print(" ".join(cmd))
        output = output.stdout.decode("UTF-8")
        print(output)
        return -1

    cost = -999
    output = output.stdout.decode("UTF-8").split("\n")[8:-1]
    # Find the last line in the output which is a number
    for line in output:
        try:
            cost = float(line.split(" ")[-1])
        except ValueError:
            continue
    if verbose:
        print("{:d} {:s} {:E}".format(param_idx, cmd_str, cost))
    return cost

def main():
    parser = argparse.ArgumentParser("Evaluate cost over the argos files.")
    parser.add_argument("argos_files", help="the argos files you want to run evaluate with", nargs="+")
    parser.add_argument("library_path", help="the path to the loop function *.so library to use")
    parser.add_argument("params", help="params file to evaluate")
    parser.add_argument("--pool-size", "-p", help="number of worker subprocesses to spawn", type=int, required=True)
    parser.add_argument("--trials", '-t', help="number of trials per argos configuration", type=int, default=1)
    parser.add_argument("--verbose", "-v", help="print more shit", action="store_true")

    args = parser.parse_args()

    params = np.genfromtxt(args.params)[1:]
    if args.verbose:
        print(params)
        pass

    with Pool(processes=args.pool_size) as pool:
        pool_args = [(-1, params, f, args.library_path, args.trials, args.verbose) for f in args.argos_files]
        costs = pool.map(evaluate_params, pool_args)
        print("mean", np.mean(costs))


if __name__ == '__main__':
    sys.exit(main())
