#!/usr/bin/env python3

import argparse
import os

import matplotlib.pyplot as plt
import numpy as np

def compute_dispersion(poses_over_time, robot_radius=0.07):
    centroid = poses_over_time.mean(axis=0)
    dispersion = 1 / (4 * robot_radius ** 2) * np.sum(np.linalg.norm(poses_over_time - centroid, axis=2) ** 2, axis=1)
    return dispersion

def main():
    np.set_printoptions(suppress=True)

    parser = argparse.ArgumentParser()
    parser.add_argument('log_directories', nargs='+')

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
        dispersion_over_time = compute_dispersion(poses)
        dispersion_by_trial.append(dispersion_over_time)
        plt.plot(dispersion_over_time, color='black', alpha=0.2)

    mean_dispersion = np.mean(dispersion_by_trial, axis=0)

    plt.plot(mean_dispersion, color='red')
    plt.title("Robot Dispersion over Time")
    plt.xlabel("Time, t (s)")
    plt.ylabel("Robot dispersion, u(t)")
    plt.show()


if __name__ == '__main__':
    main()
