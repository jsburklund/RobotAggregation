#!/usr/bin/env python3

import argparse
import subprocess
import time

import numpy as np


def param_generator(n, min=-1, max=1):
    """ Fight me. """
    for vl0 in np.linspace(min, max, n):
        for vr0 in np.linspace(min, max, n):
            for vl1 in np.linspace(min, max, n):
                for vr1 in np.linspace(min, max, n):
                    for vl2 in np.linspace(min, max, n):
                        for vr2 in np.linspace(min, max, n):
                            yield [vl0, vr0, vl1, vr1, vl2, vr2]


def main():
    parser = argparse.ArgumentParser("Evaluate cost over a bunch of different argos files.")
    parser.add_argument("argos_files", help="all the argos files you want to run evaluate with", nargs="+")
    parser.add_argument("--trials", '-t', help="number of trials per argos configuration", type=int, default=5)
    parser.add_argument("-n", help="number values per parameter", type=int, default=5)
    parser.add_argument("--skip", help="skip this many of the first parameter pairs", type=int, default=0)
    parser.add_argument("--stop-at", help="stop after evaluating this many parameter pairs", type=int, default=-1)
    parser.add_argument("--verbose", "-v", help="print more shit", action="store_true")
    args = parser.parse_args()

    outfile_name = "grid_search_output_{:d}.txt".format(int(time.time()))
    with open(outfile_name, 'w')  as outfile:
        for param_idx, params in enumerate(param_generator(args.n)):
            if param_idx == args.stop_at:
                break
            if param_idx < args.skip:
                continue

            mean_costs_for_all_argos_configs = []
            for argos_file in args.argos_files:
                # Execute evaluate and save the poses of time
                params_str = "\"6 " + " ".join([str(p) for p in params]) + "\""
                cmd = ["./build/bin/evaluate", argos_file, params_str, "--params-as-string", "-t", str(args.trials)]
                cmd_str = " ".join(cmd)
                if args.verbose:
                    print("EXECUTING:", cmd_str)
                output = subprocess.run(cmd_str, stdout=subprocess.PIPE, shell=True)
                if output.returncode != 0:
                    print("subprocess failed:")
                    print(" ".join(cmd))
                    output = output.stdout.decode("UTF-8")
                    print(output)
                    return

                generated_filename = output.stdout.decode("UTF-8").split("\n")[8]
                data = np.genfromtxt(generated_filename, delimiter=',')
                mean_cost_over_trials = np.mean(data, axis=0)[1]
                mean_costs_for_all_argos_configs.append(mean_cost_over_trials)

            outfile.write("{:d}, ".format(param_idx))
            for p in params:
                outfile.write("{:.4f}".format(p))
                outfile.write(", ")
            outfile.write("{:.3f}".format(mean_cost_over_trials))
            outfile.write("\n")

    print("Finished evaluting paramaters #{:d} to #{:d}".format(args.skip, param_idx))


if __name__ == '__main__':
    main()
