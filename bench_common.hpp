// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by the BSD 2-clause license found in the LICENSE.txt file.
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <benchmark/benchmark.h>

struct problem {
    std::string name;
    std::filesystem::path mm_path;
};

/**
 * row/column index type
 */
using INDEX_TYPE = int64_t;

/**
 * value type
 */
using VALUE_TYPE = double;

/**
 * default number of Google Benchmark iterations to run
 */
static int num_iterations = 1;

static bool delete_written_files_on_finish = true;

/**
 * Directory where benchmarks may write temporary data to.
 */
extern std::filesystem::path temporary_write_dir;

void BenchmarkArgument(benchmark::internal::Benchmark* b);

problem& get_problem(int i);