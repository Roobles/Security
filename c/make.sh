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
C_FILES+=(DbConnection.c)
C_FILES+=(DbUtils.c)
C_FILES+=(LibRoo.c)

# Library Definitions
L_LINKS=()
L_LINKS+=(m)
L_LINKS+=(mysqlclient)

I_INCLUDES=()
I_INCLUDES+=(/usr/include/mysql)

# Output Files
O_FILES=()
O_FILES+=(StockAnalyze.c)
O_FILES+=(Calibrate.c)

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

  for include in `echo ${I_INCLUDES[@]}`;  do
    BUILD_CMD+=(-I${include})
  done

  BUILD_CMD+=(-o "${BIN_DIR}/${OUTPUT_FILE}")
  BUILD_CMD+=(${output})
  echo ${BUILD_CMD[@]}
  eval ${BUILD_CMD[@]}
done
