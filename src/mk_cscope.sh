#!/bin/bash

PROJ_DIR=$PWD
SRC_DIR=$PROJ_DIR/tar

# Directory where the source files are located
#SEARCH_DIR="$PROJ_DIR/include $PROJ_DIR/lib"
SEARCH_DIR="$PROJ_DIR"


# Directory where the resulting files will be located
DEST_DIR=$PROJ_DIR

cd $DEST_DIR
rm -rf cscope.out cscope.files
find $SEARCH_DIR  \( -name '*.c' -o -name '*.h' -o -name '*.cpp' -o -name '*.S' \) -print > cscope.files
cscope -b
