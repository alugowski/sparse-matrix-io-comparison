# sparse matrix I/O comparison
Compare I/O of sparse matrix libraries.

* [fast_matrix_market](https://github.com/alugowski/fast_matrix_market)
  * Matrix Market read/write
* [PIGO](https://github.com/GT-TDAlab/PIGO)
  * Matrix Market read
  * proprietary binary write
  * ASCII format write (like Matrix Market body only)
* [GraphBLAS](https://github.com/DrTimothyAldenDavis/GraphBLAS)
  * Matrix Market read/write using fast_matrix_market's GraphBLAS binding

Libraries are fetched from their main branches on GitHub. To pin a version modify the appropriate file in [cmake/](cmake).

# Build

CMake will pull in all dependencies.

Exception is GraphBLAS, its benchmark is skipped if GraphBLAS is not found. Up to you to install that, `brew install suite-sparse` works on macOS.

```shell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

builds everything into the `build` subdirectory.

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

### Example results

M1 Macbook Pro. ARM with 6 performance and 2 efficiency cores.

#### `fmm`
```
-------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                           Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------------------------------------------------------------
op:read/impl:FMM/format:MatrixMarket/problem:0/p:8/iterations:1/real_time       0.499 s         0.228 s             1 bytes_per_second=2.00416G/s problem_name=1024MiB.mtx
op:read/impl:FMM/format:MatrixMarket/problem:1/p:8/iterations:1/real_time       0.497 s         0.219 s             1 bytes_per_second=2.01385G/s problem_name=1024MiB.sorted.mtx
op:write/impl:FMM/format:MatrixMarket/problem:0/p:8/iterations:1/real_time       1.27 s         0.374 s             1 bytes_per_second=869.828M/s problem_name=1024MiB.mtx
op:write/impl:FMM/format:MatrixMarket/problem:1/p:8/iterations:1/real_time       1.27 s         0.380 s             1 bytes_per_second=866.261M/s problem_name=1024MiB.sorted.mtx
```

#### `pigo_bench`
```
-------------------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                             Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------------------------------------------------------------------------------
op:read/impl:PIGO/format:MatrixMarket/problem:0/p:8/iterations:1/real_time                        0.359 s         0.324 s             1 bytes_per_second=2.78799G/s problem_name=1024MiB.mtx
op:read/impl:PIGO/format:MatrixMarket/problem:1/p:8/iterations:1/real_time                        0.361 s         0.298 s             1 bytes_per_second=2.77114G/s problem_name=1024MiB.sorted.mtx
op:write/impl:PIGO/format:binary/problem:0/p:8/iterations:1/real_time                             0.681 s         0.288 s             1 bytes_per_second=1.40955G/s problem_name=1024MiB.mtx
op:write/impl:PIGO/format:binary/problem:1/p:8/iterations:1/real_time                             0.718 s         0.301 s             1 bytes_per_second=1.337G/s problem_name=1024MiB.sorted.mtx
op:write/impl:PIGO/format:ASCII(MatrixMarket_body_only)/problem:0/p:8/iterations:1/real_time       16.3 s          14.3 s             1 bytes_per_second=62.9753M/s problem_name=1024MiB.mtx
op:write/impl:PIGO/format:ASCII(MatrixMarket_body_only)/problem:1/p:8/iterations:1/real_time       16.3 s          14.2 s             1 bytes_per_second=62.9798M/s problem_name=1024MiB.sorted.mtx
```

#### `graphblas_fmm`
```
-----------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                     Time             CPU   Iterations UserCounters...
-----------------------------------------------------------------------------------------------------------------------------------------------
op:read/impl:GraphBLAS_FMM/format:MatrixMarket/problem:0/p:8/iterations:1/real_time        5.40 s          5.14 s             1 bytes_per_second=189.543M/s problem_name=1024MiB.mtx
op:read/impl:GraphBLAS_FMM/format:MatrixMarket/problem:1/p:8/iterations:1/real_time       0.925 s         0.676 s             1 bytes_per_second=1106.64M/s problem_name=1024MiB.sorted.mtx
op:write/impl:GraphBLAS_FMM/format:MatrixMarket/problem:0/p:8/iterations:1/real_time       1.27 s         0.200 s             1 bytes_per_second=864.388M/s problem_name=1024MiB.mtx
op:write/impl:GraphBLAS_FMM/format:MatrixMarket/problem:1/p:8/iterations:1/real_time       1.16 s         0.206 s             1 bytes_per_second=951.295M/s problem_name=1024MiB.sorted.mtx
```