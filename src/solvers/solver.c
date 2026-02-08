// Solver entry points used by GUI/CLI.

#include "solver.h"

// solver_hint(): helper function.
int solver_hint(Grid* g){
    int n=0;
    n += apply_naked_single(g);
    if(n) return 1;
    n += apply_hidden_single(g);
    return n>0;
}

// solver_solve(): helper function.
int solver_solve(Grid* g){
    if(!g) return 0;
    Grid base = *g;

    while(solver_hint(&base)){}
    if(grid_is_complete(&base)){ *g = base; return 1; }

    Grid tmp = base;
    if(solve_constraint_propagation(&tmp)){ *g = tmp; return 1; }

    tmp = base;
    if(solve_backtracking_mrv_bitmask(&tmp)){ *g = tmp; return 1; }

    tmp = base;
    if(solve_dlx(&tmp)){ *g = tmp; return 1; }

    return 0;
}


// solve_get_cell(): helper function.
int solve_get_cell(const Grid* g_in, int r, int c, int* out){
    if(!g_in || !out) return 0;
    if(r<0||r>=9||c<0||c>=9) return 0;
    Grid g = *g_in;
    // run logic then solve
    while(solver_hint(&g)) {}
    if(!solve_dlx(&g)) return 0;
    *out = g.v[r][c];
    return 1;
}
