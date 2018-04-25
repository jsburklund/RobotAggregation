#!/usr/bin/env python3

import argparse
import subprocess
import sys
import time

import numpy as np
from multiprocessing import Pool


def evaluate(args):
    # Execute evaluate and save the poses of time
    params_file, argos_file, library_path, trials, verbose = args
    cmd = ["./build/bin/evaluate", "-t", str(trials), argos_file, library_path, params_file]
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
        print("{:s} {:E}".format(cmd_str, mean))
    return mean


def main():
    parser = argparse.ArgumentParser("Evaluate cost over the beam angle argos files.")
    parser.add_argument("argos_files", help="the argos files you want to run evaluate with", nargs="+")
    parser.add_argument("library_path", help="the path to the loop function *.so library to use")
    parser.add_argument("params", help="params file to evaluate")
    parser.add_argument("--pool-size", "-p", help="number of worker subprocesses to spawn", type=int, required=True)
    parser.add_argument("--trials", '-t', help="number of trials per argos configuration", type=int, default=100)
    parser.add_argument("--verbose", "-v", help="print more shit", action="store_true")
    args = parser.parse_args()

    params = np.genfromtxt(args.params)[1:]
    if args.verbose:
        print(params)

    outfile_name = "beam_angle_analysis_{:d}.txt".format(int(time.time()))
    with open(outfile_name, 'w')  as outfile:
        with Pool(processes=args.pool_size) as pool:
            pool_args = [(args.params, f, args.library_path, args.trials, args.verbose) for f in args.argos_files]
            costs = pool.map(evaluate, pool_args)

            for (argos_file, cost) in zip(args.argos_files, costs):
                outfile.write(argos_file)
                outfile.write(" ")
                outfile.write("{:.3f} ".format(cost))
                outfile.write("\n")

    return 0


if __name__ == '__main__':
    sys.exit(main())
