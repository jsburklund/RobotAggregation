# RobotAggregation
Final project for RBE595/CS525 Swarm Robotics class at WPI

## Build Everything

  mkdir build
  cd build
  cmake ..
  make

## Evolve aggregation behavior

This will probably find a good aggregation behavior. It will output a bunch of files called `individual_n_gen_m.dat`, which represent the genomes of each individual in each generation. Note you can pick any test setup for evolution, in this case we show a cluster of 10 robots, but you can't evolve on multiple test setups because argos is stupid and you can't change the `.argos` file once you load the simulator.

  # pick any of the 1_class argos files
  # this will generate a bunch of files of genomes.
  ./build/bin/evolve experiments/1_class/10_in_clusters.argos -p 5 -g 5 -t 10

You can then view the behavior of any of your evolved genomes like this

  # will launch argos and let you play it
  # this will generate a files of robot poses over time by trial
  ./build/bin/evaluate experiments/1_class-viz/5_in_lines-viz.argos individual_0_gen_5.dat -t 1

To do a rigorous evaluate of any paramter file on _all_ the different start configurations, we have to python scripts. Here's an example usage

  # test aggregation behavior on all the aggregation configs, 10 trials per config
  # this will generate a ton of files of robot poses over time by trial, one per config
  ./bin/generate_pose_files.py individual_0_gen_5.dat experiments/1_class/*in*.argos -t 10

  # visualize the results
  # first you MUST edit the generated_files.txt and write a description for each line
  ./bin/evaluate_pose_files.py generated_files.txt

