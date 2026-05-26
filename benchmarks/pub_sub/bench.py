from __future__ import annotations

import csv
import re
import subprocess
from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent
PROJECT_ROOT = SCRIPT_DIR.parents[1]
RESULTS_FILE = SCRIPT_DIR / "pub_sub_results.csv"
EXECUTABLE = PROJECT_ROOT / "src" / "pub_sub"

BUFFER_SIZES = [1, 10, 100, 1000]
CONFIGURATIONS = [(1, 1), (1, 2), (1, 4), (1, 8), (2, 1), (4, 1), (8, 1)]
MAX_ITEMS = 100000
REPETITIONS = 10
TIME_PATTERN = re.compile(r"Elapsed time: ([0-9.]+)")


def run_benchmark() -> None:
    with RESULTS_FILE.open("w", newline="", encoding="utf-8") as csv_file:
        writer = csv.writer(csv_file)
        writer.writerow(["buffer_size", "producers", "consumers", "average_time_ms"])

        for buffer_size in BUFFER_SIZES:
            for producers, consumers in CONFIGURATIONS:
                execution_times = []

                for repetition in range(REPETITIONS):
                    print(
                        "Running benchmark: "
                        f"buffer_size={buffer_size} "
                        f"producers={producers} "
                        f"consumers={consumers} "
                        f"run={repetition + 1}"
                    )

                    completed = subprocess.run(
                        [
                            str(EXECUTABLE),
                            str(buffer_size),
                            str(producers),
                            str(consumers),
                            str(MAX_ITEMS),
                        ],
                        check=True,
                        capture_output=True,
                        text=True,
                    )

                    match = TIME_PATTERN.search(completed.stdout)

                    if match is None:
                        raise RuntimeError(
                            "Unable to parse execution time from output"
                        )

                    execution_times.append(float(match.group(1)))

                average_time = sum(execution_times) / len(execution_times)

                writer.writerow([buffer_size, producers, consumers, average_time])

                print(f"Average time: {average_time:.3f} ms")


if __name__ == "__main__":
    run_benchmark()