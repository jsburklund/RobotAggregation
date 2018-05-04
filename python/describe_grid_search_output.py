#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import argparse


def main():
    parser = argparse.ArgumentParser("Visualize a set of output files from grid search")
    parser.add_argument("grid_search_outputs", nargs="+")

    args = parser.parse_args()

    info = {}
    for grid_search_output_filename in args.grid_search_outputs:
        d = np.genfromtxt(grid_search_output_filename, skip_header=True)
        first_param_idx = int(d[0,0])
        last_param_idx = int(d[-1,0])
        i = first_param_idx
        for row in d:
            j = int(row[0])
            if j != i:
                print("expected param idx {:d}, got {:d}". format(i, j))
                return
            i += 1
        param_range = (first_param_idx, last_param_idx)
        info[grid_search_output_filename] = param_range

    info = sorted(info.items(), key=lambda item: item[1][0])

    # check that we have all the parameters and didn't skip any
    def check_continuous():
        range_min = info[0][1][0]
        last_param_range = None
        for _, param_range in info:
            if last_param_range:
                if last_param_range[1] + 1 != param_range[0]:
                    print("missing parameters {:d}-{:d}".format(param_range[0], last_param_range[1]+1))
                    return False
            last_param_range = param_range
            range_max = param_range[1]
        return range_min, range_max

    continuous_param_range = check_continuous()
    if continuous_param_range:
        print("all parameters from {:d} to {:d} are present".format(*continuous_param_range))


    for file, param_range in info:
        print("file [{:s}] has parameters {:d}-{:d}".format(file, *param_range))


if __name__ == "__main__":
    main()
