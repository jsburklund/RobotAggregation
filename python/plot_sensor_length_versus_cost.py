#!/usr/bin/env python3

import argparse
import os
import numpy as np
import matplotlib.pyplot as plt

from myboxplot import my_boxplot


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("csv", help="csv of (sensor range, cost). Expects there to be a header")

    args = parser.parse_args()

    data = np.genfromtxt(args.csv, skip_header=True, delimiter=',')
    range_meters = data[:,0] / 100
    proportion_to_max = [int(r*100) for r in range_meters / np.sqrt(2 * 5 ** 2)]
    costs = data[:,1:]

    style_dir = os.path.dirname(os.path.realpath(__file__))
    style = os.path.join(style_dir, "mpl.style")
    plt.style.use(style)

    proportion_to_max = np.array(proportion_to_max)
    costs = np.array(costs)
    sorted_indeces = np.argsort(proportion_to_max)
    proportion_to_max = proportion_to_max[sorted_indeces]
    costs = costs[sorted_indeces]

    fig, ax = plt.subplots()
    my_boxplot(ax, proportion_to_max, costs, width=1)
    plt.xlabel("Sensing range, percentage of maximum")
    plt.ylabel("Cost")
    plt.plot([0, 0.6], [-720, -720], color='b')
    plt.show()


if __name__ == '__main__':
    main()
