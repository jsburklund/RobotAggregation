#!/usr/bin/env python3
import argparse
import numpy as np
import os
import subprocess
from subprocess import call
import cma


def compute_fitness(poses_over_time, robot_radius=0.07):
    fitness = 0
    for t, robot_poses in enumerate(poses_over_time):
        centroid = robot_poses.mean(axis=0)
        u_t = 0
        for robot_pose in robot_poses:
            u_t += np.linalg.norm(robot_pose - centroid) ** 2
        u_t = u_t / (4 * robot_radius ** 2)
        fitness += t * u_t

    return fitness


def main():
    parser = argparse.ArgumentParser("Evolve Robot Aggregation Behavior")
    parser.add_argument('argos_file', help='the *.argos file to launch with')
    parser.add_argument('params_file', help='file to write the controller parameters to')
    parser.add_argument('log_dir', help='directory that will contain log files')
    parser.add_argument('--sigma0', help='initial standard deviation of population', type=float, default=0.72)
    parser.add_argument('--population-size', help='number of controllers in each population', type=int, default=10)

    args = parser.parse_args()

    if not os.path.isdir(args.log_dir):
        print("[{:s}] is not a directory".format(args.log_dir))

    initial_params = np.array([-10, 10, 5, 10, -10, 10])

    es = cma.CMAEvolutionStrategy(initial_params, args.sigma0, {'popsize': args.population_size})

    while not es.stop():
        population = es.ask()
        fitnesses = np.empty(args.population_size)
        for i, individual_params in enumerate(population):
            # write the params file
            # the buzz script will read this and use these in the simulation
            np.savetxt(args.params_file, individual_params)

            # launch the ARGoS process
            command = ["argos3", "-c", args.argos_file]
            call(command, stdout=subprocess.DEVNULL)

            # collect the results from the logs
            log_files = os.listdir(args.log_dir)
            poses = []
            for log_name in log_files:
                full_log_name = os.path.join(args.log_dir, log_name)
                robot_poses = np.genfromtxt(full_log_name, delimiter=',')
                poses.append(robot_poses)

            poses = np.array(poses)
            poses = np.transpose(poses, [1, 0, 2])
            fitness = compute_fitness(poses)
            fitnesses[i] = fitness

        print(fitnesses)
        es.tell(population, fitnesses)
        es.logger.add()
        es.disp()


if __name__ == '__main__':
    main()
