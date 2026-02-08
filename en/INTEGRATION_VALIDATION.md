# Integration & Validation Plan (EN)
**Binôme:** ODE_BRINO_Rafael_RIZO_Maria_Eduarda — **Date:** 2025-10-17

## 0) Prerequisites
- Linux, `make`, `cc` (C11), `SDL2` (+ `SDL2_ttf` if used).

- Repository root; run from project folder.

## 1) Build targets
```bash
make clean && make cli
make gui
make test
```
**Expected:** `build/sudoku_cli`, `build/sudoku_gui` exist; no errors; warnings ≤ acceptable (none/blockers).  
`make test` runs `test_basic` and `bf_suite` (set `BF_TRIALS=200` for heavy runs).

## 2) CLI smoke tests
```bash
./build/sudoku_cli --generate 40 > k40.txt
./build/sudoku_cli --solve k40.txt --solver dlx > solved.txt
./build/sudoku_cli --solve k40.txt --solver mrv > solved_mrv.txt
diff <(./build/sudoku_cli --solve k40.txt | tr -d '\n ') <(./build/sudoku_cli --solve k40.txt | tr -d '\n ')
```
**Note:** the generator targets `k` in `0..60` to keep uniqueness guarantees.
**Checks:**

- Solved grid is valid (1–9 each row/col/box).

- Idempotency: solving twice yields same solution.

- Time budget: ≤ 1s for k≤50 on typical laptop (informative).
- Solver selector: `--solver auto|dlx|bt|mrv|cp` runs without errors.

## 3) Uniqueness check
Generate 10 puzzles per k ∈ {30,40,50} and verify DLX finds exactly one solution.

```bash
for k in 30 40 50; do
  for i in $(seq 1 10); do ./build/sudoku_cli --generate $k > tmp.txt && ./build/sudoku_cli --solve tmp.txt >/dev/null; done
done
```

## 4) Save/Load round-trip
```bash
echo "GUI: enter a few digits → K (save) → restart → L (load)"
```
**Check:** grid after load equals the saved state.

## 5) Hint behavior
- GUI: press **H**; one legal cell is revealed.

**Check:** revealed digit keeps puzzle solvable & consistent.

## 6) Trace / step-by-step
- GUI: **B** to follow the backtracking trace; ensure backtracks are visible.

- Validate: at least TRY/ASSIGN/BACKTRACK events appear; final solution consistent.

## 7) Negative tests
- Invalid input file (non-digit/size) → graceful error, non-zero exit code.

- Non-unique puzzle file → solver detects multiple solutions (when such mode is enabled) or generator prevents it.

## 8) Deliverables checklist
- Report (*rapport_en.md* / *rapport_fr.md*) present.

- This validation plan (EN/FR) present.

- Algorithmic justification (EN/FR) present.

- Architecture overview (EN/FR) present.

- Makefile with `cli`, `gui`, `bench` (optional) targets. 

## 9) Reproducibility
- Optional: fix RNG with env or CLI.
  - `SEED=123 ./build/sudoku_cli --generate 40 > k40.txt`
  - `./build/sudoku_cli --generate 40 --seed 123 > k40.txt`
