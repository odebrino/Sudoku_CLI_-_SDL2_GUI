Verified C11 Sudoku application with CLI, optional SDL2 GUI, four solvers, seeded unique-puzzle generation, and strict solver comparisons.

The Linux x86_64 CLI archive is built on Ubuntu 24.04 (glibc 2.39 baseline). Extract it, enter its folder, and run `./sudoku_cli --solve example.txt --solver mrv`. Other platforms and the GUI can be built from the tagged source; see README for dependencies and commands.

This release fixes constraint-propagation branch restoration and removes a benchmark fallback that could hide solver failures. Existing historical timing plots are not comparable verification evidence for this release. New raw timings and the exact generated corpus are supplied in `docs/benchmarks/`.

The project author and original-code copyright holder is Rafael Ode Brino. Version 0.1.1 corrects the earlier attribution following his confirmation of sole authorship. The bundled DejaVu font has its own license.
