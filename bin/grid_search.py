#!/usr/bin/env python3

import argparse
import subprocess
import sys
import time

import numpy as np
from multiprocessing import Pool


def param_generator(resolution, min, max):
    """ Fight me. """
    for vl0 in np.linspace(min[0], max[0], resolution):
        for vr0 in np.linspace(min[1], max[1], resolution):
            for vl1 in np.linspace(min[2], max[2], resolution):
                for vr1 in np.linspace(min[3], max[3], resolution):
                    for vl2 in np.linspace(min[4], max[4], resolution):
                        for vr2 in np.linspace(min[5], max[5], resolution):
                            yield [vl0, vr0, vl1, vr1, vl2, vr2]


def evaluate_params(args):
    # Execute evaluate and save the poses of time
    params, argos_file, trials, verbose = args
    params_str = "\"6 " + " ".join([str(p) for p in params]) + "\""
    cmd = ["./build/bin/evaluate", argos_file, params_str, "--params-as-string", "-t", str(trials)]
    cmd_str = " ".join(cmd)
    output = subprocess.run(cmd_str, stdout=subprocess.PIPE, shell=True)
    if output.returncode != 0:
        print("subprocess failed:")
        print(" ".join(cmd))
        output = output.stdout.decode("UTF-8")
        print(output)
        return -1

    sum = 0
    output = output.stdout.decode("UTF-8").split("\n")[8:-1]
    for line in output:
        try:
            cost = float(line.split(" ")[-1])
        except ValueError:
            cost = 0
        sum += float(cost)
    mean = sum / len(output)
    if verbose:
        print(cmd_str, mean)
    return mean


def main():
    parser = argparse.ArgumentParser("Evaluate cost over a bunch of different argos files.")
    parser.add_argument("argos_files", help="all the argos files you want to run evaluate with", nargs="+")
    parser.add_argument("--pool-size", "-p", help="number of worker subprocesses to spawn", type=int, required=True)
    parser.add_argument("--trials", '-t', help="number of trials per argos configuration", type=int, default=4)
    parser.add_argument("--resolution", help="number values per parameter", type=int, default=5)
    parser.add_argument("--skip", help="skip this many of the first parameter pairs", type=int, default=0)
    parser.add_argument("--stop-at", help="stop after evaluating this many parameter pairs", type=int, default=-1)
    parser.add_argument("--minimum", help="min values for each param. A list of 6 numbers")
    parser.add_argument("--maximum", help="max values for each param. A list of 6 numbers")
    parser.add_argument("--verbose", "-v", help="print more shit", action="store_true")
    args = parser.parse_args()

    if args.maximum:
        maximum = [float(i) for i in args.maximum.split(" ")]
        if len(maximum) != 6:
            print("maximum must have 6 numbers")
            return
    else:
        maximum = [1] * 6

    if args.minimum:
        minimum = [float(i) for i in args.minimum.split(" ")]
        if len(minimum) != 6:
            print("minimum must have 6 numbers")
            return
    else:
        minimum = [-1] * 6

    print(minimum, maximum)
    outfile_name = "grid_search_output_{:d}.txt".format(int(time.time()))
    with open(outfile_name, 'w')  as outfile:
        outfile.write("- - - - - - - ")
        for argos_file in args.argos_files:
            outfile.write("{:s} ".format(argos_file))
        outfile.write("\n")

        for param_idx, params in enumerate(param_generator(args.resolution, minimum, maximum)):
            if param_idx == args.stop_at:
                break
            if param_idx < args.skip:
                continue

            # Evaluate these parameters for each configuration, with several trials on each configuration
            # We parallelize over configurations here
            with Pool(processes=args.pool_size) as pool:
                pool_args = [(params, f, args.trials, args.verbose) for f in args.argos_files]
                costs = pool.map(evaluate_params, pool_args)

                outfile.write("{:d} ".format(param_idx))
                for p in params:
                    outfile.write("{:.4f} ".format(p))
                for cost in costs:
                    outfile.write("{:.3f} ".format(cost))
                outfile.write("\n")

    print("Finished evaluting paramaters #{:d} to #{:d}".format(args.skip, param_idx))
    return 0


if __name__ == '__main__':
    sys.exit(main())
