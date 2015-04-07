#!/bin/bash

# Constants
BINARY_NAME='StockAnalyze'

# Source File Definitions
C_FILES=()
C_FILES+=(StockAnalyze.c)
C_FILES+=(StockData.c)
C_FILES+=(Momentum.c)

# Library Definitions
L_LINKS=()
L_LINKS+=(m)

# Start Building the Command
BUILD_CMD=()
BUILD_CMD+=(gcc)

# TODO: Make this if debug
BUILD_CMD+=(-g)

# Add the source files.
for file in `echo ${C_FILES[@]}`; do
  BUILD_CMD+=(${file})
done

for link in `echo ${L_LINKS[@]}`; do
  BUILD_CMD+=(-l${link})
done

BUILD_CMD+=(-o ${BINARY_NAME})
eval ${BUILD_CMD[@]}
