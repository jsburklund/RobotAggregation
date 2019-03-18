#!/usr/bin/env python3
import argparse
import numpy as np
import matplotlib.pyplot as plt


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("infile")
    parser.add_argument("--skip", type=int, default=1)
    args = parser.parse_args()

    poses_over_time = np.genfromtxt(args.infile, delimiter=",")
    poses_over_time = poses_over_time[:, :-1]  # chop off the last empty comma

    poses_by_robot = {}
    for poses in poses_over_time:
        pose_by_robot = poses.reshape(-1, 5)
        for robot_data in pose_by_robot:
            id = robot_data[0]
            class_id = robot_data[1]
            pose = robot_data[2:]
            if id not in poses_by_robot:
                poses_by_robot[id] = []
            poses_by_robot[id].append(pose)

    fig, ax = plt.subplots()
    s = 14
    e = 100
    for id, poses in poses_by_robot.items():
        poses = np.array(poses)
        plt.scatter(poses[s:e, 0], poses[s:e, 1], s=1)
    ax.set_aspect("equal")
    plt.show()


if __name__ == '__main__':
    main()
