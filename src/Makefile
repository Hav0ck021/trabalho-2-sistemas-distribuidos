CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -O2 -I../include
LDFLAGS = -pthread

SUM_TARGET = sum
SUM_SRC = sum.c
PUB_SUB_TARGET = pub_sub
PUB_SUB_SRC = pub_sub.c

all: $(SUM_TARGET) $(PUB_SUB_TARGET)

$(SUM_TARGET): $(SUM_SRC) ../include/sum.h
	$(CC) $(CFLAGS) $(SUM_SRC) -o $(SUM_TARGET) $(LDFLAGS)

$(PUB_SUB_TARGET): $(PUB_SUB_SRC) ../include/pub_sub.h
	$(CC) $(CFLAGS) $(PUB_SUB_SRC) -o $(PUB_SUB_TARGET) $(LDFLAGS)

sum: $(SUM_TARGET)

pub_sub: $(PUB_SUB_TARGET)

clean:
	rm -f $(SUM_TARGET) $(PUB_SUB_TARGET) buffer_occupancy.csv

.PHONY: all clean sum pub_sub
