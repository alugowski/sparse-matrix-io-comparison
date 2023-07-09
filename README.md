# sparse matrix I/O comparison
Compare I/O of sparse matrix libraries.

* [fast_matrix_market](https://github.com/alugowski/fast_matrix_market)
  * Matrix Market read/write
* [PIGO](https://github.com/GT-TDAlab/PIGO)
  * Matrix Market read
  * proprietary binary write
  * ASCII format write (benchmark disabled by default, see comment in code)
* [GraphBLAS](https://github.com/DrTimothyAldenDavis/GraphBLAS)
  * Matrix Market read/write using fast_matrix_market's GraphBLAS binding

Libraries are fetched from their main branches on GitHub. To pin a version modify the appropriate file in [cmake/](cmake).

# Build

CMake will pull in all dependencies.

```shell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

# Datafiles

The benchmarks look for any `*.mtx` MatrixMarket files in the current directory and benchmark against these. For benchmarks of non Matrix Market formats, the datastructure is first populated with the MM file and then written to the tested format.

Use any method you wish to create the `.mtx` files.

### `generate_matrix_market`
Generate randomized matrix market files of a specified size (in megabytes):
```shell
build/generate_matrix_market 1024
```
creates a file named `1024MiB.mtx` in the current directory that is 1 GiB in size.

### `sort_matrix_market`
Some benchmarks like GraphBLAS perform much better if the indices are sorted. Use `sort_matrix_market` to create a sorted copy of a `.mtx` file:
```shell
build/sort_matrix_market 1024MiB.mtx
```

# Run

Run all benchmarks:
```shell
build/fmm
build/pigo_bench
build/graphblas_fmm
```

Or use Google Benchmark's filter option to run only some benchmarks:
```shell
build/fmm '--benchmark_filter=.*read.*'
build/pigo_bench '--benchmark_filter=.*read.*'
build/graphblas_fmm '--benchmark_filter=.*read.*'
```

# Results

The benchmarks report the end-to-end time, as that is the primary thing the end user cares about.
This includes overheads and any datastructure construction time. For example, the GraphBLAS benchmark may include the time for `GrB_Matrix_build` in addition to the I/O time. This is intentional.

In addition to the runtime in seconds each benchmark divides this time by the file size and reports an **effective read speed in bytes/second**.
This normalized value is very informative:
 * Directly comparable to other benchmarked files, which are almost certainly of different sizes.
 * Shows at a glance whether performance varies by file size or not.
 * Directly comparable to system I/O capabilities.