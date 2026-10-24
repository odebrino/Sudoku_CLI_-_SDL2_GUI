// Constraint propagation + backtracking solver (in-place on Grid).

#include <stdint.h>
#include <string.h>
#include "solver.h"

// ---------- Candidate bitmask (bits 1..9) ----------
#define BIT(d) (1u << (d))
#define FULL_MASK (BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9))

// ---------- Internal state ----------
static int grid[N][N];
static uint16_t candMask[N][N];

// ------------------------------------------------------
// Utility functions
// ------------------------------------------------------

// can_place(): helper function.
static inline int can_place(int r, int c, int d) {
    for (int cc = 0; cc < N; cc++) if (grid[r][cc] == d) return 0;
    for (int rr = 0; rr < N; rr++) if (grid[rr][c] == d) return 0;
    int r0 = (r / 3) * 3, c0 = (c / 3) * 3;
    for (int rr = r0; rr < r0 + 3; rr++)
        for (int cc = c0; cc < c0 + 3; cc++)
            if (grid[rr][cc] == d) return 0;
    return 1;
}

// place(): helper function.
static inline void place(int r, int c, int d) { grid[r][c] = d; }

// unplace(): helper function.
// count_bits(): helper function.
static inline int count_bits(uint16_t m) {
    int cnt = 0;
    for (int d = 1; d <= 9; d++)
        if (m & BIT(d)) cnt++;
    return cnt;
}

// singleton_digit(): helper function.
static inline int singleton_digit(uint16_t m) {
    for (int d = 1; d <= 9; d++)
        if (m & BIT(d)) return d;
    return 0;
}

// validate_initial_grid(): helper function.
static int validate_initial_grid(void) {
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            int v = grid[r][c];
            if (v == 0) continue;
            if (v < 1 || v > 9) return 0;
            grid[r][c] = 0;
            if (!can_place(r, c, v)) {
                grid[r][c] = v;
                return 0;
            }
            grid[r][c] = v;
        }
    }
    return 1;
}

// ------------------------------------------------------
// Candidate recomputation and propagation
// ------------------------------------------------------

// recompute_candidates(): helper function.
static int recompute_candidates(void) {
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            if (grid[r][c] != 0) {
                candMask[r][c] = 0;
            } else {
                uint16_t mask = FULL_MASK;

                // Eliminate row, col, box digits
                for (int cc = 0; cc < N; cc++)
                    if (grid[r][cc]) mask &= ~BIT(grid[r][cc]);
                for (int rr = 0; rr < N; rr++)
                    if (grid[rr][c]) mask &= ~BIT(grid[rr][c]);
                int r0 = (r / 3) * 3, c0 = (c / 3) * 3;
                for (int rr = r0; rr < r0 + 3; rr++)
                    for (int cc = c0; cc < c0 + 3; cc++)
                        if (grid[rr][cc]) mask &= ~BIT(grid[rr][cc]);

                candMask[r][c] = mask;
                if (mask == 0) return 0; // Contradiction
            }
        }
    }
    return 1;
}

// propagate(): helper function.
static int propagate(void) {
    if (!recompute_candidates()) return 0;
    int progress;
    do {
        progress = 0;
        for (int r = 0; r < N; r++) {
            for (int c = 0; c < N; c++) {
                if (grid[r][c] == 0) {
                    uint16_t m = candMask[r][c];
                    int cnt = count_bits(m);
                    if (cnt == 0) return 0;
                    if (cnt == 1) {
                        int d = singleton_digit(m);
                        if (!can_place(r, c, d)) return 0;
                        place(r, c, d);
                        progress = 1;
                    }
                }
            }
        }
        if (progress && !recompute_candidates()) return 0;
    } while (progress);
    return 1;
}

// pick_cell(): helper function.
static int pick_cell(int *out_r, int *out_c) {
    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++)
            if (grid[r][c] == 0) { *out_r = r; *out_c = c; return 1; }
    return 0;
}

// ------------------------------------------------------
// Recursive solver
// ------------------------------------------------------

// solve_recursive(): helper function.
static int solve_recursive(void) {
    if (!propagate()) return 0;

    int r, c;
    if (!pick_cell(&r, &c)) return 1; // Solved

    uint16_t m = candMask[r][c];
    int snapshot[N][N];
    memcpy(snapshot, grid, sizeof(grid));
    for (int d = 1; d <= 9; d++) {
        if (m & BIT(d)) {
            memcpy(grid, snapshot, sizeof(grid));
            if (!can_place(r, c, d)) continue;
            place(r, c, d);
            if (solve_recursive()) return 1;
        }
    }
    memcpy(grid, snapshot, sizeof(grid));
    return 0;
}

// ------------------------------------------------------
// Public interface
// ------------------------------------------------------

/*
 * Solve a Sudoku using constraint propagation combined with backtracking.
 * Returns 1 on success and 0 if no solution exists.
 */
// solve_constraint_propagation(): helper function.
int solve_constraint_propagation(Grid* g) {
    if(!g) return 0;

    // Copy puzzle to internal grid
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            grid[r][c] = g->v[r][c];
        }
    }

    if (!validate_initial_grid()) return 0;
    if (!recompute_candidates()) return 0;

    if (solve_recursive()) {
        for (int r = 0; r < N; r++)
            for (int c = 0; c < N; c++)
                g->v[r][c] = grid[r][c];
        return 1;
    } else {
        return 0;
    }
}
