# Copyright (C) 2023 Adam Lugowski. All rights reserved.
# Use of this source code is governed by the BSD 2-clause license found in the LICENSE.txt file.
# SPDX-License-Identifier: BSD-2-Clause

from pathlib import Path
import google_benchmark as benchmark
from google_benchmark import Counter
import fast_matrix_market as fmm

import pandas as pd

from common import *


@benchmark.register(name="op:read/impl:Pandas/format:Parquet")
@benchmark.option.use_real_time()
@benchmark.option.unit(benchmark.kSecond)
@benchmark.option.dense_range(0, get_num_problems()-1, step=1)
@benchmark.option.iterations(num_iterations)
def pandas_read_parquet(state):
    prob = get_problem(state.range(0))
    mat = fmm.mmread(prob["mm_path"])
    # create dataframe
    df = pd.DataFrame(dict(col=mat.col, row=mat.row, data=mat.data))
    del mat

    tmp_path = temp_write_dir / f"write_{prob['name']}.pqt"
    df.to_parquet(tmp_path)

    while state:
        _ = pd.read_parquet(tmp_path)

    state.bytes_processed = state.iterations * tmp_path.stat().st_size

    # read speed where file length is the length of the original Matrix Market file
    state.counters["MM_equivalent_bytes_per_second"] = Counter(
        state.iterations * prob["mm_path"].stat().st_size,
        Counter.kIsRate)
    state.counters[prob['name']] = Counter(state.range(0))

    if delete_written_files_on_finish:
        tmp_path.unlink()


@benchmark.register(name="op:write/impl:Pandas/format:Parquet")
@benchmark.option.use_real_time()
@benchmark.option.unit(benchmark.kSecond)
@benchmark.option.dense_range(0, get_num_problems()-1, step=1)
@benchmark.option.iterations(num_iterations)
def pandas_write_parquet(state):
    prob = get_problem(state.range(0))
    mat = fmm.mmread(prob["mm_path"])
    # create dataframe
    df = pd.DataFrame(dict(col=mat.col, row=mat.row, data=mat.data))
    del mat

    out_path = temp_write_dir / f"write_{prob['name']}.pqt"
    while state:
        df.to_parquet(out_path)

    state.bytes_processed = state.iterations * out_path.stat().st_size

    # read speed where file length is the length of the original Matrix Market file
    state.counters["MM_equivalent_bytes_per_second"] = Counter(
        state.iterations * prob["mm_path"].stat().st_size,
        Counter.kIsRate)
    state.counters[prob['name']] = Counter(state.range(0))

    if delete_written_files_on_finish:
        out_path.unlink()


if __name__ == "__main__":
    benchmark.main()
