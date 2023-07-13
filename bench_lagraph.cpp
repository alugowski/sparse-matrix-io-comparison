// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by the BSD 2-clause license found in the LICENSE.txt file.
// SPDX-License-Identifier: BSD-2-Clause

#include "common.hpp"
#include <fast_matrix_market/app/GraphBLAS.hpp>

extern "C" {
#include <LAGraph.h>
}

/**
 * Initialize and finalize LAGraph using a global so the rest of the code doesn't have to worry about it.
 */
struct LAGraphInitializer {
    LAGraphInitializer() {
        LAGraph_Init(msg);
    }

    ~LAGraphInitializer() {
        LAGraph_Finalize(msg);
    }
    char msg[LAGRAPH_MSG_LEN];
};
[[maybe_unused]] LAGraphInitializer lagraph_init_and_finalizer{};

/**
 * Read MatrixMarket with fast_matrix_market.
 */
void GraphBLAS_read_FMM(benchmark::State& state) {
    problem& prob = get_problem((int)state.range(0));

    std::size_t num_bytes = 0;

    for ([[maybe_unused]] auto _ : state) {
        char msg[LAGRAPH_MSG_LEN];

        FILE *in_file  = fopen(prob.mm_path.c_str(), "r");
        if (!in_file) {
            std::cerr << "Could not open " << prob.mm_path << std::endl;
            break;
        }


        GrB_Matrix mat;
        LAGraph_MMRead (&mat, in_file, msg) ;

        fclose(in_file);
        GrB_Matrix_free(&mat);

        num_bytes += std::filesystem::file_size(prob.mm_path);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(GraphBLAS_read_FMM)->Name("op:read/impl:LAGraph/format:MatrixMarket")->UseRealTime()->Iterations(num_iterations)->Apply(BenchmarkArgument);

/**
 * Write MatrixMarket with fast_matrix_market.
 */
void GraphBLAS_write_FMM(benchmark::State& state) {
    std::size_t num_bytes = 0;

    problem& prob = get_problem((int)state.range(0));

    fast_matrix_market::write_options options;
    options.parallel_ok = true;
    options.num_threads = (int)state.range(1);

    // load the problem to be written later
    GrB_Matrix mat;
    {
        std::ifstream f(prob.mm_path);
        fast_matrix_market::read_matrix_market_graphblas(f, &mat);
    }

    auto out_path = temporary_write_dir / ("write_" + prob.name + ".mtx");

    for ([[maybe_unused]] auto _ : state) {
        char msg[LAGRAPH_MSG_LEN];

        FILE *out_file  = fopen(out_path.c_str(), "wb");
        if (!out_file) {
            std::cerr << "Could not open " << out_path << std::endl;
            break;
        }

        LAGraph_MMWrite(mat, out_file, nullptr, msg);

        num_bytes += std::filesystem::file_size(out_path);
        benchmark::ClobberMemory();
    }
    GrB_Matrix_free(&mat);

    if (delete_written_files_on_finish) {
        std::filesystem::remove(out_path);
    }
    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(GraphBLAS_write_FMM)->Name("op:write/impl:LAGraph/format:MatrixMarket")->UseRealTime()->Iterations(num_iterations)->Apply(BenchmarkArgument);
