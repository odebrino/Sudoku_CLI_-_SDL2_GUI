// MRV + Bitmask backtracking solver (in-place on Grid).

#include "solver.h"

// ------------------------------------------------------
// Bitmask helpers
// ------------------------------------------------------

// Set bit for number n (1–9)
// bit(): helper function.
static inline int bit(int n) {
    return 1 << (n - 1);
}

// ------------------------------------------------------
// Utility: find next cell with minimum remaining values
// ------------------------------------------------------
// find_mrv_cell(): helper function.
static int find_mrv_cell(int grid[N][N], int rows[N], int cols[N], int boxes[N],
                         int *out_r, int *out_c) {
    int min_count = 10; // start higher than possible
    int found = 0;

    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            if (grid[r][c] != 0)
                continue;

            int box = (r / 3) * 3 + (c / 3);
            int used = rows[r] | cols[c] | boxes[box];
            int available = (~used) & 0x1FF; // bits 0–8 for numbers 1–9

            int count = 0;
            for (int n = 1; n <= 9; n++)
                if (available & bit(n))
                    count++;

            if (count < min_count) {
                min_count = count;
                *out_r = r;
                *out_c = c;
                found = 1;
            }
        }
    }

    return found;
}

// ------------------------------------------------------
// Recursive MRV + Bitmask solver
// ------------------------------------------------------
// solve_mrv_recursive(): helper function.
static int solve_mrv_recursive(int grid[N][N], int rows[N], int cols[N], int boxes[N]) {
    int r, c;
    if (!find_mrv_cell(grid, rows, cols, boxes, &r, &c))
        return 1; // no empty cell left -> solved

    int box = (r / 3) * 3 + (c / 3);
    int used = rows[r] | cols[c] | boxes[box];
    int available = (~used) & 0x1FF;

    while (available) {
        int lowest_bit = available & -available;
        int num = 1;
        while ((lowest_bit >> (num - 1)) != 1)
            num++;

        available &= ~lowest_bit;

        grid[r][c] = num;
        rows[r] |= bit(num);
        cols[c] |= bit(num);
        boxes[box] |= bit(num);

        if (solve_mrv_recursive(grid, rows, cols, boxes))
            return 1;

        // Backtrack
        grid[r][c] = 0;
        rows[r] &= ~bit(num);
        cols[c] &= ~bit(num);
        boxes[box] &= ~bit(num);
    }

    return 0;
}

// ------------------------------------------------------
// Public interface
// ------------------------------------------------------
// solve_backtracking_mrv_bitmask(): helper function.
int solve_backtracking_mrv_bitmask(Grid* g) {
    if(!g) return 0;

    int rows[N] = {0}, cols[N] = {0}, boxes[N] = {0};

    // Initialize bitmasks and validate input grid
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            int num = g->v[r][c];
            if(num == 0) continue;
            if(num < 1 || num > 9) return 0;
            int b = (r / 3) * 3 + (c / 3);
            int m = bit(num);
            if ((rows[r] & m) || (cols[c] & m) || (boxes[b] & m)) return 0;
            rows[r] |= m;
            cols[c] |= m;
            boxes[b] |= m;
        }
    }

    return solve_mrv_recursive(g->v, rows, cols, boxes);
}
