# Poisson Dataset Generator

Use the code in this repository to generate a dataset with the specified total size.
It is intended for use with the included Bash script to test filesystem (FS) performance
under various simulated workloads.

## Pseudo-Random Data Generation

Pseudo-randomization is achieved using the C++11 `<random>` library. The filesizes follow a Poisson
distribution using `std::poisson_distribution<unsigned long>`, driven by the Mersenne Twister engine
defined by `std::default_random_engine`.

Directory names, file names, and file data are alphabetical and follow a uniform distribution,
`std::uniform_int_distribution<char>`. These are driven by the linear congruential engine defined by
`std::minstd_rand`.

## Build and Dependencies

The C++ program defined in `data-generator.cpp` depends on the C++11 STL and the Boost Filesystem
header. The GNU Makefile assumes the GNU C++ Compiler, `g++`. On Debian-derivative GNU/Linux
distributions, these can be installed using the package manager:

```
$ sudo apt install g++ libboost-filesystem-dev make
```

With the dependencies satisfied, build the executable with

```
$ make
```

## Usage

The `data-generator` program can be used stand-alone. You must specify the total dataset size
(bytes), mean filesize (bytes), and number of repetitions on the command line. For example, to time
the generation of a 1 MB dataset with mean filesize of 1 KB over 3 repetitions, run

```
$ ./data-generator 1048576 1024 3
1048576      1024          0.037456     0.028739     0.028854     0.031683     0.004082
```

The output summarizes the result. The columns indicate dataset size, mean filesize, first runtime,
second runtime, third runtime, mean runtime, and standard deviation of the runtimes, respectively.
Note that if the number of repetitions changes, the number of runtime columns will also change.

The program can also be run with the accompanying Bash script. This is recommended, as it improves
reproducibility by providing more opportunities to double-check values than the standalone program
does. To use the script, you must first customize it to your preference using a text editor. Then,

```
$ ./FS-test.sh
```

The script will write timing summaries to the terminal, with the detailed output redirected to
a file named `<timestamp>_FS-test.log`.

## Results

For a sample of performance measurements taken with this tool, please open `RESULTS.md`.
