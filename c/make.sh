#!/bin/bash

# Constants
BIN_DIR='bin'

# Create bin directory, if it doesn't exist.
[ ! -d "${BIN_DIR}" ] && mkdir "${BIN_DIR}"

# Source File Definitions
C_FILES=()
C_FILES+=(StockData.c)
C_FILES+=(Momentum.c)
C_FILES+=(StockMomentum.c)

# Library Definitions
L_LINKS=()
L_LINKS+=(m)

# Output Files
O_FILES=()
O_FILES+=(StockAnalyze.c)


for output in `echo ${O_FILES[@]}`; do
  # Get the output name.
  OUTPUT_FILE="`echo ${output}|grep -Eo '^ *[^.]+'`"

  # Start the build command.
  BUILD_CMD=()
  BUILD_CMD+=(gcc)

  # TODO: Make this if debug
  BUILD_CMD+=(-g)

  # Add the source files.
  for file in `echo ${C_FILES[@]}`; do
    BUILD_CMD+=(${file})
  done

  # Add the links.
  for link in `echo ${L_LINKS[@]}`; do
    BUILD_CMD+=(-l${link})
  done

  BUILD_CMD+=(-o "${BIN_DIR}/${OUTPUT_FILE}")
  BUILD_CMD+=(${output})
  echo ${BUILD_CMD[@]}
  eval ${BUILD_CMD[@]}
done
