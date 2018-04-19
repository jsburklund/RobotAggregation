#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import argparse

def main():
    parser = argparse.ArgumentParser("Visualize a set of output files from grid search")
    parser.add_argument("grid_search_outputs", nargs="+")
    parser.add_argument("x_param", type=int)
    parser.add_argument("y_param", type=int)
    parser.add_argument("--resolution", type=int, default=10)

    args = parser.parse_args()

    plt.figure()
    plt.xlabel("param {:d}".format(args.x_param))
    plt.ylabel("param {:d}".format(args.y_param))
    plt.title("N-Class Segregation Cost")

    shape = (args.resolution, args.resolution)
    cost_image = np.ones(shape) * 1e24
    for grid_search_output_filename in args.grid_search_outputs:
        data = np.genfromtxt(grid_search_output_filename, delimiter=',')
        for parameter_evaluation in data:
            parameter_idx = int(parameter_evaluation[0])
            cost = parameter_evaluation[-1]
            row, col = np.unravel_index(parameter_idx, shape)
            if cost < cost_image[row, col]:
                cost_image[row, col] = parameter_evaluation[-1]

    plt.imshow(cost_image)

    plt.show()


if __name__ == "__main__":
    main()