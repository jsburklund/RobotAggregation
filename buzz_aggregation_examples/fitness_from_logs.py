#!/usr/bin/env python3

import argparse
import os

import matplotlib.pyplot as plt
import numpy as np


def compute_dispersion_over_time(poses_over_time, robot_radius):
    centroid_over_time = np.expand_dims(poses_over_time.mean(axis=1), axis=1)
    u = np.sum(np.linalg.norm(poses_over_time - centroid_over_time, axis=2) ** 2, axis=1)
    dispersion_over_time = 1 / (4 * robot_radius ** 2) * u
    return dispersion_over_time


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('log_directories', nargs='+')
    parser.add_argument('--robot-radius', type=float, default=0.0704)

    args = parser.parse_args()

    plt.figure()

    dispersion_by_trial = []
    for directory in args.log_directories:
        if not os.path.isdir(directory):
            print(directory, "is not a directory. Aborting")
            return
        log_files = os.listdir(directory)

        poses = []
        for log in log_files:
            if log.endswith(".log"):
                full_log_path = os.path.join(directory, log)
                robot_poses = np.genfromtxt(full_log_path, delimiter=',')
                poses.append(robot_poses)

        poses = np.array(poses)[:, :, 1:]
        poses = np.transpose(poses, [1, 0, 2])
        dispersion_over_time = compute_dispersion_over_time(poses, args.robot_radius)
        dispersion_by_trial.append(dispersion_over_time)
        plt.plot(dispersion_over_time, color='black', alpha=0.2)

    mean_dispersion = np.mean(dispersion_by_trial, axis=0)

    opt_pack = 0.1482 / (4 * args.robot_radius ** 2)
    print("minimum dispersion from trials, optimal dispersion")
    print(np.min(dispersion_by_trial), opt_pack)

    plt.plot(mean_dispersion, color='red', label='mean')
    plt.plot([0, mean_dispersion.shape[0]], [opt_pack, opt_pack], label='optimal', color='green')
    plt.title("Robot Dispersion over Time")
    plt.xlabel("Time, t (s)")
    plt.ylabel("Robot dispersion, u(t)")
    plt.legend()
    plt.show()


if __name__ == '__main__':
    main()
