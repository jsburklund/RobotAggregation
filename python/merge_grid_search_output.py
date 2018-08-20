#!/usr/bin/env python

import numpy as np
import os
import csv
import matplotlib.pyplot as plt
import argparse


def main():
    parser = argparse.ArgumentParser("Merge a set of output files from grid search")
    parser.add_argument("grid_search_outputs", nargs="+")
    parser.add_argument("outfile", help='save the resulting merged file here')

    args = parser.parse_args()

    merged = []
    for grid_search_output_filename in args.grid_search_outputs:
        d = np.genfromtxt(grid_search_output_filename, skip_header=True)
        if d.shape[0] == 0:
            to_delete.append(grid_search_output_filename)
            continue
        elif len(d.shape) == 1:
            d = np.expand_dims(d, axis=0)
        first_param_idx = int(d[0,0])
        last_param_idx = int(d[-1,0])
        print(grid_search_output_filename, d.shape)
        merged.extend(list(d))

    merged = np.array(merged)
    merged = merged[merged[:,0].argsort()]
    np.savetxt(args.outfile, merged, fmt="%12.6f")

if __name__ == '__main__':
    main()
