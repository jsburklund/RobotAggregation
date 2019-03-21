#!/usr/bin/env python
from math import pi, atan2, sqrt, asin
import argparse


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--r", "-r", type=float, required=True)
    parser.add_argument("--l", "-l", type=float, required=True)
    parser.add_argument("--dt", "-t", type=float, required=True)
    parser.add_argument("--V", "-V", type=float, required=True)

    args = parser.parse_args()

    r = args.r
    l = args.l
    dt = args.dt
    V = args.V

    lhs = (2 * V) / (3 * l) * dt
    rhs = pi - 2 * atan2(-r - 5 * l / 4, sqrt(3) * r) - 2 * asin((l / 4) / sqrt(3 * r ** 2 + (5 * l / 4 - r) ** 2))
    kin_aggregation_phase_2 = (lhs <= rhs)

    lhs = (2 * V) / (3 * l) * dt
    rhs = atan2(sqrt(3) * r, r - 2 * l)
    kin_aggregation_phase_3 = (lhs <= rhs)

    print("Kin Aggregation, #2", kin_aggregation_phase_2)
    print("Kin Aggregation, #3", kin_aggregation_phase_3)


if __name__ == '__main__':
    main()
