#!/usr/bin/env python

import numpy as np
import os
import csv
import argparse


def main():
    parser = argparse.ArgumentParser("Visualize a set of output files from grid search")
    parser.add_argument("grid_search_outputs", nargs="+")
    parser.add_argument("--outfile")
    parser.add_argument("--resolution", type=int, default=7)
    parser.add_argument("--best-n", type=int, default=10)
    parser.add_argument("--ignore-known-controllers", action="store_true")
    parser.add_argument("--plot", action="store_true")
    parser.add_argument("--viz", action="store_true")
    parser.add_argument("--save", action="store_true")
    parser.add_argument("--exclude-one-class", action="store_true")

    args = parser.parse_args()

    if args.plot or args.viz:
      style_dir = os.path.dirname(os.path.realpath(__file__))
      style = os.path.join(style_dir, "mpl.style")
      import matplotlib.pyplot as plt
      plt.style.use(style)

    costs = np.zeros((args.resolution ** 6))
    stds = np.zeros((args.resolution ** 6))
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
        env_start_idx = 7
        costs_in_environments = f[:, env_start_idx:]
        if args.exclude_one_class:
            # skip the first 8 environments, they are for the 1-class scenarios
            costs_in_environments = costs_in_environments[:, 8:]
        mean_over_environments = np.mean(costs_in_environments, axis=1)
        std_over_environments = np.std(costs_in_environments, axis=1)
        costs[first_param_idx:last_param_idx + 1] = mean_over_environments
        stds[first_param_idx:last_param_idx + 1] = std_over_environments
        params[first_param_idx:last_param_idx + 1, :] = f[:, 1:7]

    if args.outfile:
        writer = csv.writer(open(args.outfile, 'w'), delimiter=',')
        for i, (p, c, s) in enumerate(zip(params, costs, stds)):
            writer.writerow([i, p, c, s])

    if args.viz or args.save:
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

    # Sorting messes up plotting so we have to do this after
    sorted_cost_indeces = costs.argsort(axis=0)
    costs.sort()
    params = params[sorted_cost_indeces]
    stds = stds[sorted_cost_indeces]
    print("Best Params, Index, Cost")
    print("{} {:0.0f} {:0.0f}".format(params[0], costs[0], stds[0]))
    print("="*85)

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
        print(p, "{:d}th {:0.0f} {:0.0f}".format(i, costs[i], stds[i]))

    if args.plot:
        plt.figure()
        c = costs[::1000]
        N = len(c)
        plt.bar(np.arange(N), c, yerr=stds[::1000])
        plt.ylabel("cost")
        plt.title("every 1000th parameter set, sorted from best to worst")
        plt.gca().set_xticklabels([])

    if args.plot or args.viz:
        plt.show()


if __name__ == "__main__":
    main()
