#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import argparse


def main():
    parser = argparse.ArgumentParser("Visualize a set of output files from grid search")
    parser.add_argument("grid_search_outputs", nargs="+")
    parser.add_argument("--resolution", type=int, default=7)
    parser.add_argument("--no-plot", action="store_true")

    args = parser.parse_args()

    costs = np.zeros((args.resolution ** 6))
    params = np.zeros((args.resolution ** 6, 6))
    for grid_search_output_filename in args.grid_search_outputs:
        f = np.genfromtxt(grid_search_output_filename, skip_header=True)
        first_param_idx = int(f[0,0])
        last_param_idx = int(f[-1,0])
        costs[first_param_idx:last_param_idx + 1] = np.mean(f[:,7:], axis=1)
        params[first_param_idx:last_param_idx + 1,:] = f[:,1:7]

    best_idx = np.argmin(costs)
    print("Best Params, Cost")
    print(params[best_idx], costs[best_idx])

    if not args.no_plot:
        for x_param in range(6):
            for y_param in range(x_param + 1, 6):

                plt.figure()
                plt.xlabel("param {:d}".format(x_param))
                plt.ylabel("param {:d}".format(y_param))
                plt.title("N-Class Segregation Cost (lighter is better)")

                shape = tuple([args.resolution]*6)
                cost_image = np.ones((args.resolution, args.resolution)) * 1e24
                for parameter_idx, cost in enumerate(costs):
                    indeces = np.unravel_index(parameter_idx, shape)
                    row = indeces[x_param]
                    col = indeces[y_param]
                    if cost < cost_image[row, col]:
                        cost_image[row, col] = cost

                plt.imshow(cost_image, cmap='Reds')

        plt.show()


if __name__ == "__main__":
    main()
