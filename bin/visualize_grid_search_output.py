#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import argparse


def main():
    parser = argparse.ArgumentParser("Visualize a set of output files from grid search")
    parser.add_argument("grid_search_outputs", nargs="+")
    parser.add_argument("--resolution", type=int, default=10)

    args = parser.parse_args()

    for x_param in range(6):
        for y_param in range(x_param + 1, 6):

            plt.figure()
            plt.xlabel("param {:d}".format(x_param))
            plt.ylabel("param {:d}".format(y_param))
            plt.title("N-Class Segregation Cost")

            shape = tuple([args.resolution]*6)
            cost_image = np.ones((args.resolution, args.resolution)) * 1e24
            for grid_search_output_filename in args.grid_search_outputs:
                data = np.genfromtxt(grid_search_output_filename, delimiter=',')
                for parameter_evaluation in data:
                    parameter_idx = int(parameter_evaluation[0])
                    cost = parameter_evaluation[-1]
                    indeces = np.unravel_index(parameter_idx, shape)
                    row = indeces[x_param]
                    col = indeces[y_param]
                    if cost < cost_image[row, col]:
                        cost_image[row, col] = parameter_evaluation[-1]

            plt.imshow(1 / cost_image)

    plt.show()


if __name__ == "__main__":
    main()
