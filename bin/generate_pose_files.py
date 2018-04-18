#!/usr/bin/env python3

import argparse
import subprocess


def main():
    parser = argparse.ArgumentParser("Evaluate cost over a bunch of different argos files.")
    parser.add_argument("params", help="the .dat file of parameters to evaluate")
    parser.add_argument("argos_files", help="all the argos files you want to run evaluate with", nargs="+")
    parser.add_argument("--outfile", help="the filename you want to output paths into", default="generated_files.txt")
    parser.add_argument("--trials", '-t', help="number of trials per argos configuration", type=int, default=5)
    args = parser.parse_args()

    with open(args.outfile, 'w') as f:
        f.write("title_goes_here\n")
        for argos_file in args.argos_files:
            print("Processing {:s}...".format(argos_file))
            cmd = ["./build/bin/evaluate", argos_file, args.params, "-t", str(args.trials)]
            output = subprocess.run(cmd, stdout=subprocess.PIPE)
            if output.returncode != 0:
                print("subprocess failed:")
                print(" ".join(cmd))
                return

            generated_filename = output.stdout.decode("UTF-8").split("\n")[-2]
            start = generated_filename.index("dat")
            label = generated_filename[start + 16:-23]
            f.write(generated_filename)
            f.write(", ")
            f.write(label)
            f.write("\n")


if __name__ == '__main__':
    main()
