PYTHON ?= python3
SOURCE_DIR := ../src

.PHONY: all build bench plot sum-bench sum-plot pub_sub-bench pub_sub-plot clean

all: build bench plot

build:
	$(MAKE) -C $(SOURCE_DIR) all

bench: sum-bench pub_sub-bench

plot: sum-plot pub_sub-plot

sum-bench: build
	$(PYTHON) sum/bench.py

sum-plot: sum-bench
	$(PYTHON) sum/plot.py

pub_sub-bench: build
	$(PYTHON) pub_sub/bench.py

pub_sub-plot: pub_sub-bench
	$(PYTHON) pub_sub/plot.py

clean:
	rm -f sum/sum_results.csv sum/sum_performance.png
	rm -f pub_sub/pub_sub_results.csv pub_sub/pub_sub_performance.png
	$(MAKE) -C $(SOURCE_DIR) clean