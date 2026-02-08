# Generated Artifacts Policy

This repository includes some generated files for convenience and for the
report/plots that accompany the project.

Tracked generated artifacts:
- `plots/*.png` (charts produced by the benchmark plotting scripts)
- `data/results/results_k*.txt` and `results/solvers_comparison.csv`
- `assets/screenshots/gui.png` (GUI screenshot used in README)

How to regenerate:
```bash
make bench-run
python3 scripts/plot_benchmark.py
python3 plots/compare_solvers.py
scripts/capture_screenshot.sh
```

If you prefer a clean working tree, you can delete these files locally and
regenerate them when needed.
