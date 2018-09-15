#!/usr/bin/env python

import numpy as np
import os
import csv
import matplotlib.pyplot as plt
import argparse


def main():
    parser = argparse.ArgumentParser("Visualize a set of output files from grid search")
    parser.add_argument("grid_search_outputs", nargs="+")
    parser.add_argument("--outfile")
    parser.add_argument("--resolution", type=int, default=7)
    parser.add_argument("--best-n", type=int, default=10)
    parser.add_argument("--ignore-known-controllers", action="store_true")
    parser.add_argument("--plot", action="store_true")
    parser.add_argument("--save", action="store_true")

    args = parser.parse_args()

    style_dir = os.path.dirname(os.path.realpath(__file__))
    style = os.path.join(style_dir, "mpl.style")
    plt.style.use(style)

    costs = np.zeros((args.resolution ** 6))
    params = np.zeros((args.resolution ** 6, 6))
    for grid_search_output_filename in args.grid_search_outputs:
        f = np.genfromtxt(grid_search_output_filename, skip_header=True)
        if f.shape[0] == 0:
            to_delete.append(grid_search_output_filename)
            continue
        elif len(f.shape) == 1:
            f = np.expand_dims(f, axis=0)

        first_param_idx = int(f[0, 0])
        last_param_idx = int(f[-1, 0])
        costs[first_param_idx:last_param_idx + 1] = np.mean(f[:, 7:], axis=1)
        params[first_param_idx:last_param_idx + 1, :] = f[:, 1:7]

    if args.outfile:
        writer = csv.writer(open(args.outfile, 'w'), delimiter=',')
        for i, c in enumerate(costs):
            if c != 0:
                writer.writerow([i, c])

    if args.plot or args.save:
        axes_titles = [r'$v_{l_0}$', r'$v_{r_0}$', r'$v_{l_1}$', r'$v_{r_1}$', r'$v_{l_2}$', r'$v_{r_2}$']
        for x_param in range(6):
            for y_param in range(x_param + 1, 6):

                plt.figure()
                plt.xlabel(axes_titles[x_param], fontsize=32)
                plt.ylabel(axes_titles[y_param], fontsize=32, rotation=0)
                labels = ['-1.0', '', '', '', '', '', '1.0']
                plt.xticks(np.arange(7), labels, fontsize=24)
                plt.yticks(np.arange(7), labels, fontsize=24)

                shape = tuple([args.resolution] * 6)
                cost_image = np.ones((args.resolution, args.resolution)) * 1e24
                for parameter_idx, cost in enumerate(costs):
                    indeces = np.unravel_index(parameter_idx, shape)
                    row = indeces[x_param]
                    col = indeces[y_param]
                    if cost < cost_image[row, col]:
                        cost_image[row, col] = cost

                plt.imshow(cost_image, cmap='Reds')
                if args.save:
                    plt.savefig("{:d}_{:d}_grid_img.png".format(x_param, y_param))

        if args.plot:
            plt.show()

    # Sorting messes up plotting so we have to do this after
    sorted_cost_indeces = costs.argsort(axis=0)
    costs.sort()
    params = params[sorted_cost_indeces]
    print("Best Params, Index, Cost")
    print(params[0], costs[0])

    print("Good params")
    unknown_controllers = 0
    for i, p in enumerate(params[:args.best_n]):
        # check if this matches the "patterns" of known segregating controllers
        # this never prints anything because turns out they all can be described this way
        if args.ignore_known_controllers:
            if p[0] > p[1]:
                if p[4] > p[5]:
                    if p[3] >= p[2]:
                        # left-hand circles segregating
                        continue
                    else:
                        # slow clustering segregation?
                        continue
            elif p[0] < p[1]:
                if p[5] > p[4]:
                    if p[2] >= p[3]:
                        # right-hand circles segregating
                        continue
                    else:
                        #slow clustering segregation?
                        continue
        unknown_controllers += 1
        print(p, "{:d}th {:0.4f}".format(i, costs[i]))



if __name__ == "__main__":
    main()
