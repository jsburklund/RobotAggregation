#!/usr/bin/env python3

import argparse
import os
import numpy as np
import xml.etree.ElementTree as ET


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("base_argos_file", help="argos file to use as a template")

    args = parser.parse_args()

    tree = ET.parse(args.base_argos_file)
    root = tree.getroot()

    noises = np.array(list(np.ndindex(6,6,6))) * 0.1
    for noise in noises:
        params = root.find('controllers').find('footbot_segregation_controller').find('params')
        params.set('kin_nonkin_confusion', "{:0.1f}".format(noise[0]))
        params.set('kin_nothing_confusion', "{:0.1f}".format(noise[1]))
        params.set('nonkin_nothing_confusion', "{:0.1f}".format(noise[2]))
        filename = "noise_study_{:0.1f}_{:0.1f}_{:0.1f}.argos".format(*noise)
        tree.write(filename)


if __name__ == '__main__':
    main()