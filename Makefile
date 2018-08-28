# Export all variables for the rest of the build system to use
export

# Commands and options
MKD    = mkdir -p
RMD    = rm -rf
CC     = gcc
CFLAGS = -Wall -g

# Compilation
OUT      = bin
ROOT_DIR = $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
SRC_DIR  = $(ROOT_DIR)/src
CL_SRC   = $(wildcard $(SRC_DIR)/*.c)
CL_OBJ   = $(patsubst $(SRC_DIR)/%.c,$(SRC_DIR)/$(OUT)/%.o,$(CL_SRC))

.PHONY: all src examples tests clean

all: src examples tests

src:
	$(MAKE) -C src

examples:
	$(MAKE) -C examples

tests:
	$(MAKE) -C tests

clean:
	$(MAKE) -C src clean
	$(MAKE) -C examples clean
	$(MAKE) -C tests clean
	$(RMD) $(OUT) %.o
