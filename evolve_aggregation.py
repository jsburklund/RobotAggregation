#!/usr/bin/env python3
import argparse
import sys
import time

import numpy as np
import re
from xml.etree import ElementTree
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
    parser.add_argument('buzz_file', help='bzz file that implements the controller')
    parser.add_argument('log_dir', help='directory that will contain log files and the intermediate params.bzz file')
    parser.add_argument('--sigma0', help='initial standard deviation of population', type=float, default=0.72)
    parser.add_argument('--population-size', help='number of controllers in each population', type=int, default=5)
    parser.add_argument('-q', '--quiet', help='hide standard output from argos', action="store_true")

    args = parser.parse_args()

    if not os.path.exists(args.log_dir):
        os.mkdir(args.log_dir)
    elif not os.path.isdir(args.log_dir):
        print("[{:s}] is not a directory".format(args.log_dir))
        return

    es = cma.CMAEvolutionStrategy(np.zeros(4), args.sigma0, {'popsize': args.population_size})

    while not es.stop():
        population = es.ask()
        fitnesses = np.empty(args.population_size)
        for i, individual_params in enumerate(population):
            t0 = time.time()
            # write the params buzz file
            params_filename = os.path.join(args.log_dir, "params.bzz")
            params_file = open(params_filename, 'w')
            params_table = "params={{.vl0={:f},.vr0={:f},.vl1={:f},.vr1={:f}}}\n".format(*individual_params)
            params_file.write(params_table)
            params_file.close()

            # recompile the buzz code
            buzz_file_basename = os.path.basename(args.buzz_file)
            bytecode_filename = os.path.join(args.log_dir, buzz_file_basename + ".bo")
            debug_filename = os.path.join(args.log_dir, buzz_file_basename + ".bdb")
            call(['bzzc', '-b', bytecode_filename, '-d', debug_filename, args.buzz_file])

            # set the buzz controller path
            tree = ElementTree.parse(args.argos_file)
            root = tree.getroot()
            experiment = root.find("framework").find("experiment")
            experiment.attrib["random_seed"] = str(np.random.randint(0, 4096))
            buzz_controller_params = root.find('controllers').find("buzz_controller_kheperaiv").find("params")
            buzz_controller_params.attrib['bytecode_file'] = bytecode_filename
            buzz_controller_params.attrib['debug_file'] = debug_filename
            tree.write(args.argos_file)

            # launch the ARGoS process
            command = ["argos3", "-c", args.argos_file]
            if args.quiet:
                call(command, stdout=subprocess.DEVNULL)
            else:
                call(command)

            # collect the results from the logs
            log_files = os.listdir(args.log_dir)
            poses = []
            for log_name in log_files:
                if re.match("robot_\d+.log", log_name):
                    full_log_name = os.path.join(args.log_dir, log_name)
                    robot_poses = np.genfromtxt(full_log_name, delimiter=',')
                    poses.append(robot_poses)

            poses = np.array(poses)
            poses = np.transpose(poses, [1, 0, 2])
            fitness = compute_fitness(poses)
            fitnesses[i] = fitness

            dt = time.time() - t0
            print("dt=", dt, "fitness=", fitness, "params=", individual_params)

        print("mean fitness = ", np.mean(fitnesses))
        es.tell(population, fitnesses)
        # es.logger.add()
        # es.disp()


if __name__ == '__main__':
    main()
