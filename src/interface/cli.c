// Minimal terminal CLI for Sudoku: generate/solve/print
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "generator.h"
#include "solver.h"

typedef enum {
    SOLVER_AUTO,
    SOLVER_DLX,
    SOLVER_BT,
    SOLVER_MRV,
    SOLVER_CP
} SolverKind;

// Pretty print a grid
static void print_grid(const Grid* g){
    for(int r=0;r<N;r++){
        for(int c=0;c<N;c++){
            int v = g->v[r][c];
            printf("%d%c", v, (c==N-1? '\n' : ' '));
        }
    }
}

// Read grid from text file: accepts digits 0-9 and '.'. Fills zeros for blanks.
// Expects at least 81 cells; extra characters are ignored.
static int read_grid_file(const char* path, Grid* g){
    for(int r=0;r<N;r++) for(int c=0;c<N;c++){ g->v[r][c]=0; g->fixed[r][c]=0; }
    FILE* f = strcmp(path,"-")==0 ? stdin : fopen(path,"r");
    if(!f){ perror(path); return 0; }
    int r=0,c=0,ch;
    while((ch=fgetc(f))!=EOF && r<N){
        if(isdigit(ch) || ch=='.'){
            int v = (ch=='.') ? 0 : (ch-'0');
            g->v[r][c] = v;
            g->fixed[r][c] = (v != 0);
            if(++c==N){ c=0; r++; }
        }
    }
    if(f!=stdin) fclose(f);
    return (r==N && c==0);
}

// solver_from_name(): helper function.
static int solver_from_name(const char* name, SolverKind* out){
    if(!name || !out) return 0;
    if(strcmp(name, "auto")==0){ *out = SOLVER_AUTO; return 1; }
    if(strcmp(name, "dlx")==0){ *out = SOLVER_DLX; return 1; }
    if(strcmp(name, "bt")==0){ *out = SOLVER_BT; return 1; }
    if(strcmp(name, "mrv")==0){ *out = SOLVER_MRV; return 1; }
    if(strcmp(name, "cp")==0){ *out = SOLVER_CP; return 1; }
    return 0;
}

// solver_label(): helper function.
static const char* solver_label(SolverKind k){
    switch(k){
        case SOLVER_DLX: return "DLX";
        case SOLVER_BT: return "Backtracking";
        case SOLVER_MRV: return "Backtracking+MRV/bitmask";
        case SOLVER_CP: return "Constraint Propagation";
        case SOLVER_AUTO: default: return "Auto";
    }
}

// run_solver(): helper function.
static int run_solver(SolverKind k, Grid* g){
    switch(k){
        case SOLVER_DLX: return solve_dlx(g);
        case SOLVER_BT: return solve_backtracking(g);
        case SOLVER_MRV: return solve_backtracking_mrv_bitmask(g);
        case SOLVER_CP: return solve_constraint_propagation(g);
        case SOLVER_AUTO: default: return solver_solve(g);
    }
}

static void usage(const char* prog){
    fprintf(stderr,
        "Usage:\n"
        "  %s --generate <k>        # gera um puzzle com k buracos e imprime\n"
        "  %s --solve <file|->      # resolve puzzle do arquivo (ou '-' = stdin)\n"
        "     [--solver auto|dlx|bt|mrv|cp]\n"
        "  %s                        # gera k=40, resolve e imprime\n",
        prog, prog, prog);
}

int main(int argc, char** argv){
    Grid g;
    const char* solve_path = NULL;
    int k = -1;
    SolverKind solver = SOLVER_AUTO;

    for(int i=1;i<argc;i++){
        if(strcmp(argv[i],"--generate")==0 && i+1<argc){
            k = atoi(argv[++i]);
        }else if(strcmp(argv[i],"--solve")==0 && i+1<argc){
            solve_path = argv[++i];
        }else if(strcmp(argv[i],"--solver")==0 && i+1<argc){
            if(!solver_from_name(argv[++i], &solver)){
                fprintf(stderr,"Unknown solver. Use: auto|dlx|bt|mrv|cp\n");
                return 1;
            }
        }else if(strcmp(argv[i],"--help")==0 || strcmp(argv[i],"-h")==0){
            usage(argv[0]);
            return 0;
        }else{
            usage(argv[0]);
            return 1;
        }
    }

    if(k >= 0){
        if(k<0 || k>80){ fprintf(stderr,"k must be 0..80\n"); return 1; }
        generator_new_k(k, &g);
        print_grid(&g);
        return 0;
    }

    if(solve_path){
        if(!read_grid_file(solve_path, &g)){
            fprintf(stderr,"Failed to read grid (need 81 cells 0-9 or '.')\n");
            return 2;
        }
        printf("Input grid:\n");
        print_grid(&g);
        Grid w = g;
        if(!run_solver(solver, &w)){
            fprintf(stderr,"No solution found (%s).\n", solver_label(solver));
            return 3;
        }
        printf("\nSolved grid (%s):\n", solver_label(solver));
        print_grid(&w);
        return 0;
    }

    // Default demo: generate k=40 and solve
    k = 40;
    generator_new_k(k, &g);
    printf("Generated grid (k=%d):\n", k);
    print_grid(&g);
    Grid w = g;
    if(!run_solver(solver, &w)){
        fprintf(stderr,"No solution found (%s).\n", solver_label(solver));
        return 3;
    }
    printf("\nSolved grid (%s):\n", solver_label(solver));
    print_grid(&w);
    return 0;
}
