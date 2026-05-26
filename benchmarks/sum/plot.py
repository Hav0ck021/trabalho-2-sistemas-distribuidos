from __future__ import annotations

from pathlib import Path

import matplotlib
import pandas as pd


matplotlib.use("Agg")

import matplotlib.pyplot as plt


SCRIPT_DIR = Path(__file__).resolve().parent
RESULTS_FILE = SCRIPT_DIR / "sum_results.csv"
OUTPUT_FILE = SCRIPT_DIR / "sum_performance.png"


def generate_plot() -> None:
    dataframe = pd.read_csv(RESULTS_FILE)

    plt.figure(figsize=(10, 6))

    for vector_size in dataframe["N"].unique():
        subset = dataframe[dataframe["N"] == vector_size]

        plt.plot(
            subset["K"],
            subset["tempo_ms"],
            marker="o",
            label=f"N={vector_size}",
        )

    plt.xscale("log", base=2)
    plt.xlabel("Number of Threads (K)")
    plt.ylabel("Average Time (ms)")
    plt.title("Parallel Sum with Spinlock")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(OUTPUT_FILE)


if __name__ == "__main__":
    generate_plot()