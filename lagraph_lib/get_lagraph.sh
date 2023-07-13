#!/bin/sh

# LAGraph cannot be used directly from CMake.
# Must build it and then link against it the old-fashioned way.

git clone --depth 1 https://github.com/GraphBLAS/LAGraph

cd LAGraph/

make library