// Regression for failed propagation branches; all solvers must agree with DLX.
#include "interface.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void){
    const char* puzzle="000405013300000400040000867700000004050764009900230070000970000600308700001046300";
    Grid g; grid_clear(&g);
    for(int i=0;i<81;i++) g.v[i/9][i%9]=puzzle[i]-'0';
    Grid expected=g;
    assert(count_solutions_dlx(&g,2)==1);
    assert(solve_dlx(&expected));
    int (*solvers[])(Grid*)={solve_dlx,solve_backtracking,solve_backtracking_mrv_bitmask,solve_constraint_propagation};
    for(int s=0;s<4;s++) {
        Grid out=g;
        assert(solvers[s](&out));
        assert(memcmp(out.v,expected.v,sizeof(out.v))==0);
    }
    rng_seed(123);
    assert(generator_new_k(50,&g));
    assert(grid_count_empty(&g)==50);
    assert(count_solutions_dlx(&g,2)==1);
    assert(solver_solve(&g));
    assert(grid_is_complete(&g));
    printf("Regression and unique-generation checks passed\n");
    return 0;
}
