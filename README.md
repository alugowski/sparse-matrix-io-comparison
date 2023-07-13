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

Exception is GraphBLAS, its benchmark is skipped if GraphBLAS is not found. Up to you to install GraphBLAS, `brew install suite-sparse` works on macOS.

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
build/PIGO
build/graphblas_fmm
```

Or use Google Benchmark's filter option to run only some benchmarks:
```shell
build/fmm '--benchmark_filter=.*read.*'
build/PIGO '--benchmark_filter=.*read.*'
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

M1 Macbook Pro with 16 GB RAM, 6 performance and 2 efficiency cores (ARM).

#### `bench_fmm`
```
----------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                    Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------------------------------------------------------------
op:read/impl:FMM/format:MatrixMarket/problem:0/p:8/iterations:1/real_time                0.494 s         0.214 s             1 bytes_per_second=2.02622G/s problem_name=1024MiB.mtx
op:read/impl:FMM/format:MatrixMarket/problem:1/p:8/iterations:1/real_time                0.491 s         0.201 s             1 bytes_per_second=2.03837G/s problem_name=1024MiB.sorted.mtx
op:write/impl:FMM/format:MatrixMarket/problem:0/p:8/iterations:1/real_time                1.26 s         0.227 s             1 bytes_per_second=876.407M/s problem_name=1024MiB.mtx
op:write/impl:FMM/format:MatrixMarket/problem:1/p:8/iterations:1/real_time                1.25 s         0.231 s             1 bytes_per_second=877.677M/s problem_name=1024MiB.sorted.mtx
op:write/impl:FMM/format:MatrixMarket(pattern)/problem:0/p:8/iterations:1/real_time      0.815 s         0.187 s             1 bytes_per_second=804.211M/s problem_name=1024MiB.mtx
op:write/impl:FMM/format:MatrixMarket(pattern)/problem:1/p:8/iterations:1/real_time      0.824 s         0.185 s             1 bytes_per_second=795.726M/s problem_name=1024MiB.sorted.mtx
```

#### `bench_pigo`
```
----------------------------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                                      Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------------------------------------------------------------------------------
op:read/impl:PIGO/format:MatrixMarket/problem:0/p:8/iterations:1/real_time                                 0.351 s         0.314 s             1 bytes_per_second=2.84972G/s problem_name=1024MiB.mtx
op:read/impl:PIGO/format:MatrixMarket/problem:1/p:8/iterations:1/real_time                                 0.391 s         0.299 s             1 bytes_per_second=2.55914G/s problem_name=1024MiB.sorted.mtx
op:write/impl:PIGO/format:binary/problem:0/p:8/iterations:1/real_time                                      0.922 s         0.356 s             1 bytes_per_second=1066.59M/s problem_name=1024MiB.mtx
op:write/impl:PIGO/format:binary/problem:1/p:8/iterations:1/real_time                                      0.718 s         0.306 s             1 bytes_per_second=1.33625G/s problem_name=1024MiB.sorted.mtx
op:write/impl:PIGO/format:ASCII(MatrixMarket_body_only)/problem:0/p:8/iterations:1/real_time                16.4 s          14.8 s             1 bytes_per_second=62.5738M/s problem_name=1024MiB.mtx
op:write/impl:PIGO/format:ASCII(MatrixMarket_body_only)/problem:1/p:8/iterations:1/real_time                16.4 s          14.8 s             1 bytes_per_second=62.4265M/s problem_name=1024MiB.sorted.mtx
op:write/impl:PIGO/format:ASCII(MatrixMarket_body_only(pattern))/problem:0/p:8/iterations:1/real_time      0.604 s         0.316 s             1 bytes_per_second=1085.65M/s problem_name=1024MiB.mtx
op:write/impl:PIGO/format:ASCII(MatrixMarket_body_only(pattern))/problem:1/p:8/iterations:1/real_time      0.587 s         0.337 s             1 bytes_per_second=1116.13M/s problem_name=1024MiB.sorted.mtx
```

#### `bench_graphblas_fmm`
```
-----------------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                     Time             CPU   Iterations UserCounters...
-----------------------------------------------------------------------------------------------------------------------------------------------
op:read/impl:GraphBLAS_FMM/format:MatrixMarket/problem:0/p:8/iterations:1/real_time        5.40 s          5.14 s             1 bytes_per_second=189.543M/s problem_name=1024MiB.mtx
op:read/impl:GraphBLAS_FMM/format:MatrixMarket/problem:1/p:8/iterations:1/real_time       0.925 s         0.676 s             1 bytes_per_second=1106.64M/s problem_name=1024MiB.sorted.mtx
op:write/impl:GraphBLAS_FMM/format:MatrixMarket/problem:0/p:8/iterations:1/real_time       1.27 s         0.200 s             1 bytes_per_second=864.388M/s problem_name=1024MiB.mtx
op:write/impl:GraphBLAS_FMM/format:MatrixMarket/problem:1/p:8/iterations:1/real_time       1.16 s         0.206 s             1 bytes_per_second=951.295M/s problem_name=1024MiB.sorted.mtx
```

#### `bench_lagraph`
```
-----------------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                               Time             CPU   Iterations UserCounters...
-----------------------------------------------------------------------------------------------------------------------------------------
op:read/impl:LAGraph/format:MatrixMarket/problem:0/p:8/iterations:1/real_time        18.4 s          18.1 s             1 bytes_per_second=55.5359M/s problem_name=1024MiB.mtx
op:read/impl:LAGraph/format:MatrixMarket/problem:1/p:8/iterations:1/real_time        12.0 s          12.0 s             1 bytes_per_second=85.0187M/s problem_name=1024MiB.sorted.mtx
op:write/impl:LAGraph/format:MatrixMarket/problem:0/p:8/iterations:1/real_time       26.1 s          25.5 s             1 bytes_per_second=37.6224M/s problem_name=1024MiB.mtx
op:write/impl:LAGraph/format:MatrixMarket/problem:1/p:8/iterations:1/real_time       26.3 s          25.4 s             1 bytes_per_second=37.3481M/s problem_name=1024MiB.sorted.mtx
```