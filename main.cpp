// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by the BSD 2-clause license found in the LICENSE.txt file.
// SPDX-License-Identifier: BSD-2-Clause

#include <algorithm>
#include <numeric>
#include <mutex>
#include <thread>
#include "common.hpp"

namespace fs = std::filesystem;

void load_problems(const fs::path& dir, std::vector<problem>& ret) {
    for (const auto & entry : fs::directory_iterator(dir)) {
        if (entry.path().extension() != ".mtx") {
            continue;
        }

        problem p;
        p.name = entry.path().filename();
        p.mm_path = entry.path();
        ret.push_back(p);
    }

    std::sort(ret.begin(), ret.end(), [](const problem& a, const problem& b) {
        return a.name < b.name;
    });
}

void create_problems(std::vector<problem>& ret) {
    load_problems(std::filesystem::current_path(), ret);

    for (std::size_t i = 0; i < ret.size(); ++i) {
        std::cout << "Problem " << i << ": " << ret[i].name << std::endl;
    }
}

std::vector<problem> problems;
std::once_flag problems_initialized_flag;
std::filesystem::path temporary_write_dir = std::filesystem::current_path();

void BenchmarkArgument(benchmark::internal::Benchmark* b) {
    std::call_once(problems_initialized_flag, []{ create_problems(problems); });
    b->ArgNames({"problem", "p"});

    std::vector<int64_t> problem_args(problems.size());
    std::iota(problem_args.begin(), problem_args.end(), 0);

    std::vector<int64_t> p_args {
//        1,
        std::thread::hardware_concurrency()
    };

    b->ArgsProduct({problem_args, p_args});

    // report times in seconds
    b->Unit(benchmark::kSecond);
}

problem& get_problem(int i) {
    return problems[i];
}

// Google Benchmark provides main()
BENCHMARK_MAIN();
