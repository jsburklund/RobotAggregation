#!/usr/bin/env python3

import argparse

import numpy as np

from evolve_aggregation import compute_fitness


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('logs', nargs='+')

    args = parser.parse_args()

    poses = []
    for log in args.logs:
        robot_poses = np.genfromtxt(log, delimiter=',')
        poses.append(robot_poses)

    poses = np.array(poses)
    poses = np.transpose(poses, [1, 0, 2])
    fitness = compute_fitness(poses)
    print(fitness)


if __name__ == '__main__':
    main()
