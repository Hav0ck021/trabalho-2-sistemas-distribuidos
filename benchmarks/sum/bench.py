from __future__ import annotations

import csv
import re
import subprocess
from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent
PROJECT_ROOT = SCRIPT_DIR.parents[1]
RESULTS_FILE = SCRIPT_DIR / "sum_results.csv"
EXECUTABLE = PROJECT_ROOT / "src" / "sum"

VECTOR_SIZES = [10**7, 10**8]
THREAD_COUNTS = [1, 2, 4, 8, 16, 32, 64, 128, 256]
REPETITIONS = 10
TIME_PATTERN = re.compile(r"Tempo: ([0-9.]+)")


def run_benchmark() -> None:
    with RESULTS_FILE.open("w", newline="", encoding="utf-8") as csv_file:
        writer = csv.writer(csv_file)
        writer.writerow(["N", "K", "tempo_ms"])

        for vector_size in VECTOR_SIZES:
            for thread_count in THREAD_COUNTS:
                execution_times = []

                for repetition in range(REPETITIONS):
                    print(
                        f"Running benchmark: N={vector_size} "
                        f"K={thread_count} run={repetition + 1}"
                    )

                    completed = subprocess.run(
                        [str(EXECUTABLE), str(vector_size), str(thread_count)],
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

                writer.writerow([vector_size, thread_count, average_time])

                print(f"Average time: {average_time:.3f} ms")


if __name__ == "__main__":
    run_benchmark()