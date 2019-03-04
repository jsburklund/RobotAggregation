# Minimalist Segregation

Code to accompany [this paper](https://arxiv.org/pdf/1901.10423.pdf) (currently preprint)

## Building

We use CMake

    mkdir build
    cd build
    cmake ..
    make

## Reproducing experimental results

see the `cluster_jobs` scripts folder. Running those scripts (which were designed for a SLURM managed cluster since they take many hours to run) will output results in text file. Our results are saved in the data folder. The same scripts that are used to generate the data are also used to plot the data.
