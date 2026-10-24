# Recorded strict comparison

Captured 24 October 2026 after fixing propagation rollback. The revision base below precedes the release-preparation changes; the tagged source contains the exact harness and solver fix.

- CPU: 13th Gen Intel(R) Core(TM) i9-13900H
- Platform: Linux-7.2.6-arch2-1-x86_64-with-glibc2.44
- Compiler: cc (GCC) 16.2.1 20260810
- Flags: `-O2 -Wall -Wextra -Wshadow -pedantic -std=c11 -g`
- Corpus SHA-256: `513e3ba6a2cd7f5082af08d1b76618a84727fa707600756060e150824c6270d6`
- Seed: 123; 20 distinct generated puzzles for each of 30/40/50/60 holes.

Each solver receives a fresh copy of the same puzzle. `CLOCK_MONOTONIC` covers only its solve call; generation, uniqueness checks and output validation are excluded. Execution order rotates by trial. All 320 solves succeeded and produced valid, clue-preserving solutions. Raw timings are milliseconds. There are no warmup or repeated-per-puzzle measurements; short timings are sensitive to overhead and noise. Results support this corpus and environment only.

`corpus.csv`, `raw.csv`, `summary.csv`, and `environment.json` preserve the evidence. Re-run `python3 scripts/benchmark_repro.py --output build/benchmarks --seed 123 --trials 20`. Seeded C `rand()` depends on libc; the supplied corpus hash identifies this specific sample. Historical plots used a less strict harness and should not be compared as equivalent measurements.
