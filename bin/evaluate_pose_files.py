#!/usr/bin/env python3

import argparse
import os
import json
import numpy as np
import matplotlib.pyplot as plt


def compute_dispersion_per_class_over_time(trial, robot_radius):
    classes = np.transpose(trial, (1, 0, 2, 3))
    dispersion_per_class_over_time = np.empty((classes.shape[0], classes.shape[1]))
    for class_idx, class_poses_over_time in enumerate(classes):
        centroid_over_time = np.expand_dims(class_poses_over_time.mean(axis=1), axis=1)
        u = np.sum(np.linalg.norm(class_poses_over_time - centroid_over_time, axis=2) ** 2, axis=1)
        dispersion_over_time = 1 / (4 * robot_radius ** 2) * u
        dispersion_per_class_over_time[class_idx] = dispersion_over_time

    return dispersion_per_class_over_time


def main():
    parser = argparse.ArgumentParser("Evaluate cost over a bunch of different argos files")
    parser.add_argument("generated_files", help="the generated_files.txt you get from generate_pose_files.py")
    parser.add_argument('--robot-radius', type=float, default=0.0704)
    parser.add_argument("--no-plot", help="skip plotting", action="store_true")
    args = parser.parse_args()

    if not args.no_plot:
        style_dir = os.path.dirname(os.path.realpath(__file__))
        style = os.path.join(style_dir, "mpl.style")
        plt.style.use(style)

        plt.figure()
        plt.ylabel("Dispersion")
        plt.xlabel("Time Steps")

    with open(args.generated_files, 'r') as f:
        final_info = []
        for generated_filename in f.readlines():
            generated_filename = generated_filename.strip("\n")
            trials = np.array(json.load(open(generated_filename)))
            n_classes = trials.shape[2]
            n_trials = trials.shape[0]

            # compute the mean final dispersion for each trial for each configuration
            avg_dispersions = []
            for trial_idx, trial in enumerate(trials):
                # compute final dispersion
                dispersion_per_class_over_time = compute_dispersion_per_class_over_time(trial, args.robot_radius)
                avg_dispersion_over_time = np.mean(dispersion_per_class_over_time, axis=0)
                final_dispersions_per_class = dispersion_per_class_over_time[:, -1]
                avg_final_dispersion = np.mean(final_dispersions_per_class, axis=0)
                final_info.append((generated_filename, n_classes, n_trials, avg_final_dispersion))
                avg_dispersions.append(avg_dispersion_over_time)

                if not args.no_plot:
                    for g, d in enumerate(dispersion_per_class_over_time):
                        plt.plot(d, alpha=0.3)
                        pass

            plt.plot(np.mean(avg_dispersions, axis=0), label=generated_filename)

        print("argos config file, # classes, # trials, mean final dispersion")
        for name, g, t, dispersion in final_info:
            print("{:75s}, {:2d}, {:2d}, {:10.0f}".format(name, g, t, np.mean(dispersion)))

        if not args.no_plot:
            plt.legend()
            plt.show()


if __name__ == '__main__':
    main()
