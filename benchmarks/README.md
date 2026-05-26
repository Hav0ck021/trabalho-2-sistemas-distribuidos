# Benchmark Suite

## Purpose

This directory contains the scripts used to measure and visualize the performance of the two programs in the project:

- `sum`, which compares sequential and parallel summation using threads;
- `pub_sub`, which evaluates a producer-consumer implementation based on semaphores and a circular buffer.

The benchmark scripts collect execution times, store them in CSV files, and generate plots for later analysis.

## Directory Layout

```text
benchmarks/
  Makefile
  README.md
  sum/
    bench.py
    plot.py
  pub_sub/
    bench.py
    plot.py
```

Each Python script uses paths anchored to its own location, which makes it possible to run the scripts from the benchmark directory through the provided `Makefile`.

## Requirements

The benchmark suite requires:

- `python3`
- `pandas`
- `matplotlib`
- the project binaries built from `src/`

## Build and Execution

The recommended entry point is the `Makefile` in this directory.

```bash
cd benchmarks
make
```

This command performs the following steps:

1. Builds the C programs in `src/`.
2. Runs the benchmark scripts for both parts of the project.
3. Generates the corresponding plots.

If only one stage is needed, the following targets are available:

```bash
make build
make bench
make plot
make sum-bench
make sum-plot
make pub_sub-bench
make pub_sub-plot
make clean
```

## Benchmark Methodology

### `sum`

The `sum` benchmark evaluates the parallel summation program for different vector sizes and thread counts. For each configuration, the script executes the program multiple times and computes the average execution time.

The collected data is written to `benchmarks/sum/sum_results.csv`, and the plot is saved as `benchmarks/sum/sum_performance.png`.

### `pub_sub`

The `pub_sub` benchmark evaluates the producer-consumer program for different buffer sizes and thread configurations. For each configuration, the script performs repeated executions and computes the average elapsed time.

The collected data is written to `benchmarks/pub_sub/pub_sub_results.csv`, and the plot is saved as `benchmarks/pub_sub/pub_sub_performance.png`.

## Notes on the Python Scripts

The benchmark scripts were written to follow standard Python practices:

- use of `Path` for path handling;
- explicit `main`-style functions;
- batch execution with `subprocess.run`;
- output parsing with regular expressions;
- plot generation in headless mode using the `Agg` backend.

## Notes on Reproducibility

The programs under test generate random input data. As a result, the exact timings may vary between executions. The benchmark scripts mitigate this variability by repeating each configuration several times and reporting the arithmetic mean.