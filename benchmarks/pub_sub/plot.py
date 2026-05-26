from __future__ import annotations

from pathlib import Path

import matplotlib
import pandas as pd


matplotlib.use("Agg")

import matplotlib.pyplot as plt


SCRIPT_DIR = Path(__file__).resolve().parent
RESULTS_FILE = SCRIPT_DIR / "pub_sub_results.csv"
OUTPUT_FILE = SCRIPT_DIR / "pub_sub_performance.png"


def generate_plot() -> None:
    dataframe = pd.read_csv(RESULTS_FILE)

    plt.figure(figsize=(12, 6))

    for buffer_size in dataframe["buffer_size"].unique():
        subset = dataframe[dataframe["buffer_size"] == buffer_size]
        labels = [f"({row.producers}, {row.consumers})" for _, row in subset.iterrows()]

        plt.plot(
            labels,
            subset["average_time_ms"],
            marker="o",
            linewidth=2,
            label=f"Buffer={buffer_size}",
        )

    plt.xlabel("(Producers, Consumers)")
    plt.ylabel("Average Execution Time (ms)")
    plt.title("Producer-Consumer Performance")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(OUTPUT_FILE)


if __name__ == "__main__":
    generate_plot()