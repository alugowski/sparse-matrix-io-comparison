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

using VT = float;
static int num_iterations = 1;

extern std::filesystem::path temporary_write_dir;

void BenchmarkArgument(benchmark::internal::Benchmark* b);

problem& get_problem(int i);