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
        info[grid_search_output_filename] = (first_param_idx, last_param_idx)

    info = sorted(info.items(), key=lambda item: item[1][0])
    for file, idxs in info:
        print("file [{:s}] has parameters {:d}-{:d}".format(file, *idxs))


if __name__ == "__main__":
    main()
