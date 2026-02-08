# Contributing

Thanks for your interest in improving this Sudoku project!

## Getting started
- Fork the repo and create a feature branch.
- Keep changes focused and small when possible.
- Use C11 and the existing Makefile targets.

## Build & test
```bash
make cli
make gui
make test
BF_TRIALS=200 make test-bf   # heavy brute-force run
```

## Code guidelines
- Prefer readable, small functions with clear names.
- Keep public APIs in `include/` and implementation in `src/`.
- Update documentation when behavior or CLI flags change.
- Avoid introducing external dependencies unless necessary.

## Pull requests
- Explain the motivation, key changes, and how you tested.
- If relevant, include screenshots for GUI changes.
- Ensure the CI workflow passes.

## Reporting issues
Include:
- OS and compiler version.
- Steps to reproduce.
- Expected vs. actual behavior.
- Logs or screenshots if applicable.
