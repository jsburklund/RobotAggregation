#!/usr/bin/env python

import numpy as np
import argparse


def main():
    parser = argparse.ArgumentParser("Visualize a set of output files from grid search")
    parser.add_argument("grid_search_outputs", nargs="+")

    args = parser.parse_args()

    info = {}
    to_delete = []
    for grid_search_output_filename in args.grid_search_outputs:
        d = np.genfromtxt(grid_search_output_filename, skip_header=True)
        if d.shape[0] == 0:
            to_delete.append(grid_search_output_filename)
            continue
        elif len(d.shape) == 1:
            d = np.expand_dims(d, axis=0)

        first_param_idx = int(d[0,0])
        last_param_idx = int(d[-1,0])
        # check for missing/skipped params within each file.
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
        ok = True
        for _, param_range in info:
            if last_param_range:
                if last_param_range[1] + 1 != param_range[0]:
                    print("missing parameters {:d}-{:d}".format(last_param_range[1] + 1, param_range[0] - 1))
                    ok = False
            last_param_range = param_range
            range_max = param_range[1]
        return ok, range_min, range_max

    ok, range_min, range_max = check_continuous()
    if ok:
        print("all parameters from {:d} to {:d} are present".format(range_min, range_max))

    for f, param_range in info:
        print("file [{:s}] has parameters {:d}-{:d}".format(f, *param_range))

    print("Delete these empty files:")
    for f in to_delete:
        print(f, end=' ')
    print()

if __name__ == "__main__":
    main()
