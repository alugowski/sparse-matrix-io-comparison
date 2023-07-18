# Copyright (C) 2023 Adam Lugowski. All rights reserved.
# Use of this source code is governed by the BSD 2-clause license found in the LICENSE.txt file.
# SPDX-License-Identifier: BSD-2-Clause

from pathlib import Path

problems = []
problem_dir = Path.cwd()

num_iterations = 1
temp_write_dir = Path.cwd()
delete_written_files_on_finish = True


def _load_problems():
    global problems

    for mtx in sorted(problem_dir.glob("*.mtx"), key=lambda f: f.name):
        problems.append(dict(name=mtx.name, mm_path=mtx))

    for i, prob in enumerate(problems):
        print(f"Problem {i}: {prob['name']}")


def get_num_problems():
    return len(problems)


def get_problem(i):
    return problems[i]


_load_problems()
