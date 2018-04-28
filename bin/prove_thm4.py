#!/usr/bin/env python3

from math import sin, cos, atan2
import argparse
import numpy as np
import matplotlib.pyplot as plt
import os


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--delta-t", '-t', type=float, default=0.1)
    parser.add_argument("--inner-wheel-dist", "-W", type=float, default=0.14)
    parser.add_argument("--vl", type=float, default=0.2)
    parser.add_argument("--vr", type=float, default=0.6)

    args = parser.parse_args()

    dt = args.delta_t
    vl = args.vl
    vr = args.vr
    W = args.inner_wheel_dist

    theta = dt * (vr - vl) / W
    R = W / 4

    style_dir = os.path.dirname(os.path.realpath(__file__))
    style = os.path.join(style_dir, "mpl.style")
    plt.style.use(style)

    deltas = np.arange(0, 1, 0.1)
    plt.figure()
    r_i = 0.08
    for raylen in np.linspace(r_i * 2, 5, 20):
        xs = []
        ys = []
        static_ys = []
        for r_j in np.linspace(0.0, 1, 50):
            d_r = r_i - r_j
            alpha = atan2(r_i - r_j, raylen)
            d = np.sqrt(raylen ** 2 + r_j ** 2)
            pi = np.array([0, -R])
            pj = np.array([raylen, -(R + r_j)])
            pi_prime = np.array([R * sin(theta), -R * cos(theta)])
            pj_prime = pj + np.array([R * sin(alpha) - R * sin(theta + alpha),
                                      -R * cos(alpha) + R * cos(theta + alpha)])
            guarantee = np.linalg.norm(pj_prime - pi_prime)
            guarantee_static = np.linalg.norm(pj - pi_prime)
            xs.append(d_r)
            ys.append(guarantee - d)
            static_ys.append(guarantee_static - d)

        plt.plot(xs, ys, c='b')
        plt.plot(xs, static_ys, c='r')
    plt.ylabel(r"$d' - d$")
    plt.xlabel(r"$r_i - r_j$")
    plt.show()


if __name__ == '__main__':
    main()
