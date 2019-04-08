import matplotlib.pyplot as plt
from math import sin, cos, sqrt
import numpy as np
import argparse


def rot(theta):
    c = cos(theta)
    s = sin(theta)
    return np.array([[c, -s], [s, c]])


def main():
    parser = argparse.ArgumentParser()
    args = parser.parse_args()

    r = 0.085
    l = 0.14
    dt = 0.1
    V = 0.2
    theta_kin = 2 * V / (3 * l) * dt
    theta_non_kin = 2 * V / l * dt

    xs = np.arange(sqrt(3) * r, 0.88825, 0.001)
    ys = np.arange(-r, r, 0.001)
    alphas = np.deg2rad(np.arange(120, 240, 1))
    W = len(xs)
    H = len(ys)

    # KIN
    img = np.ones((W, H)) * 1e9
    for xi, x in enumerate(xs):
        for yi, y in enumerate(ys):
            for alpha in alphas:
                pj = np.array([x, y])
                qi = np.array([0, l])
                qj = pj + rot(alpha + np.pi / 2) @ np.array([l, 0])
                ci = np.array([0, -l / 4])
                cj = pj + rot(alpha - np.pi / 2) @ np.array([l / 4, 0])
                ci_prime = rot(theta_kin) @ (ci - qi) + qi
                cj_prime = rot(theta_kin) @ (cj - qj) + qj
                d = np.linalg.norm(cj - ci)
                d_prime = np.linalg.norm(cj_prime - ci_prime)
                delta = d - d_prime
                if delta < 0:
                    print("proof does not hold at : {}, {}, {} : ".format(x, y, alpha, delta))
                if delta < img[xi, H - 1 - yi]:
                    img[xi, H - 1 - yi] = delta

    plt.figure()
    plt.title("kin")
    plt.imshow(img.T, extent=[xs[0], xs[-1], ys[0], ys[-1]])

    # NON KIN
    img = np.ones((W, H)) * 1e9
    for xi, x in enumerate(xs):
        for yi, y in enumerate(ys):
            for alpha in alphas:
                pj = np.array([x, y])
                ci = np.array([0, -l / 4])
                cj = pj + rot(alpha - np.pi / 2) @ np.array([l / 4, 0])
                ci_prime = rot(theta_non_kin) @ ci
                cj_prime = rot(theta_non_kin) @ (cj - pj) + pj
                d = np.linalg.norm(cj - ci)
                d_prime = np.linalg.norm(cj_prime - ci_prime)
                delta = d - d_prime
                if delta < 0:
                    print("proof does not hold at : {}, {}, {} : ".format(x, y, alpha, delta))
                if delta < img[xi, H - 1 - yi]:
                    img[xi, H - 1 - yi] = delta

    plt.figure()
    plt.title("non-kin")
    plt.imshow(img.T, extent=[xs[0], xs[-1], ys[0], ys[-1]])
    plt.show()


if __name__ == '__main__':
    main()
