#!/usr/bin/env python3

import os
import matplotlib.pyplot as plt
import numpy as np


def main():
    distances = []
    distance = 1
    W = 0.14
    r_j = W
    dt = 0.1
    theta = 0.8 * dt / W
    R = W / 4
    iters = 0
    while distance > r_j:
        iters += 1
        distance += 2*R*((r_j + R)*(1 - np.cos(theta)) - distance*np.sin(theta))
        distances.append(distance)

    print("Aggregation took {:d} iterations".format(iters))

    style_dir = os.path.dirname(os.path.realpath(__file__))
    style = os.path.join(style_dir, "mpl.style")
    plt.style.use(style)

    plt.figure()
    plt.xlabel("iteration")
    plt.ylabel(r"distance $\delta$")
    plt.plot(distances)
    plt.show()


if __name__ == '__main__':
    main()
