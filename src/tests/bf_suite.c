#define _POSIX_C_SOURCE 200809L
// Strict comparison: every solver must solve the same unique puzzle itself.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#include "utils.h"
#include "generator.h"
#include "solver.h"

static double now_s(void){
    struct timespec ts;
    if(clock_gettime(CLOCK_MONOTONIC, &ts)) { perror("clock_gettime"); exit(1); }
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// env_int(): helper function.
static int env_int(const char* name, int def){
    const char* v = getenv(name);
    if(!v || !*v) return def;
    char* end = NULL;
    long val = strtol(v, &end, 10);
    if(end == v || val <= 0 || val > INT_MAX) return def;
    return (int)val;
}

static int valid_complete_grid(const Grid* g){
    int seen[10];
    // rows
    for(int r=0;r<N;r++){
        memset(seen,0,sizeof(seen));
        for(int c=0;c<N;c++){
            int v = g->v[r][c];
            if(v < 1 || v > 9) return 0;
            if(seen[v]) return 0;
            seen[v]=1;
        }
    }
    // cols
    for(int c=0;c<N;c++){
        memset(seen,0,sizeof(seen));
        for(int r=0;r<N;r++){
            int v = g->v[r][c];
            if(v < 1 || v > 9) return 0;
            if(seen[v]) return 0;
            seen[v]=1;
        }
    }
    // 3x3 boxes
    for(int br=0;br<3;br++){
        for(int bc=0;bc<3;bc++){
            memset(seen,0,sizeof(seen));
            for(int dr=0;dr<3;dr++){
                for(int dc=0;dc<3;dc++){
                    int r = br*3+dr, c = bc*3+dc;
                    int v = g->v[r][c];
                    if(v < 1 || v > 9) return 0;
                    if(seen[v]) return 0;
                    seen[v]=1;
                }
            }
        }
    }
    return 1;
}

static int count_zeros(const Grid* g){
    int z=0;
    for(int r=0;r<N;r++)
        for(int c=0;c<N;c++)
            if(g->v[r][c]==0) z++;
    return z;
}

static void csv_header(FILE* f){ fprintf(f,"k,solver,mean_ms\n"); }
static void csv_row(FILE* f,int k,const char* s,double ms){ fprintf(f,"%d,%s,%.6f\n",k,s,ms); }

static double time_solver(const char* label, int (*solver)(Grid*), const Grid* in){
    Grid w = *in;
    double t0 = now_s();
    int ok = solver(&w);
    double ms = (now_s() - t0) * 1000.0;
    if(!ok || !valid_complete_grid(&w)) {
        fprintf(stderr, "%s failed to solve its puzzle\n", label); exit(7);
    }
    for(int r=0;r<N;r++) for(int c=0;c<N;c++)
        if(in->v[r][c] && in->v[r][c] != w.v[r][c]) {
            fprintf(stderr, "%s changed a clue\n", label); exit(8);
        }
    return ms;
}

int main(void){
    int ks[] = {30,40,50,60};
    int nK = (int)(sizeof(ks)/sizeof(ks[0]));
    int trials_per_k = env_int("BF_TRIALS", 200);
    int max_solutions_to_count = 2;

    rng_seed_from_env();
    const char* output = getenv("BF_OUTPUT");
    const char* raw_output = getenv("BF_RAW_OUTPUT");
    const char* corpus_output = getenv("BF_CORPUS");
    FILE* csv = fopen(output ? output : "build/benchmarks/summary.csv", "w");
    FILE* raw = fopen(raw_output ? raw_output : "build/benchmarks/raw.csv", "w");
    FILE* corpus = fopen(corpus_output ? corpus_output : "build/benchmarks/corpus.csv", "w");
    if(!csv || !raw || !corpus) { perror("benchmark output"); return 1; }
    csv_header(csv);
    fprintf(raw,"k,trial,solver,elapsed_ms\n");
    fprintf(corpus,"k,trial,puzzle\n");

    for(int i=0;i<nK;i++){
        int k = ks[i];
        double s_dlx=0, s_mrv=0, s_bt=0, s_cp=0;

        for(int t=0;t<trials_per_k;t++){
            Grid g;
            int gen_fail = 0;
            while(!generator_new_k(k,&g)){
                gen_fail++;
                if(gen_fail > 5){
                    fprintf(stderr,"ERROR: generator failed for k=%d\n", k);
                    return 4;
                }
            }

            int z = count_zeros(&g);
            if(z != k) { fprintf(stderr,"Wrong hole count\n"); return 5; }

            int solc = count_solutions_dlx(&g, max_solutions_to_count);
            if(solc != 1){
                fprintf(stderr,"ERROR: uniqueness failed for k=%d (solutions=%d)\n",k,solc);
                return 2;
            }

            fprintf(corpus,"%d,%d,",k,t);
            for(int r=0;r<N;r++) for(int c=0;c<N;c++) fprintf(corpus,"%d",g.v[r][c]);
            fprintf(corpus,"\n");
            const char* labels[] = {"DLX", "Backtracking+MRV/bitmask", "Backtracking", "Constraint Propagation"};
            int (*solvers[])(Grid*) = {solve_dlx, solve_backtracking_mrv_bitmask, solve_backtracking, solve_constraint_propagation};
            double* sums[] = {&s_dlx, &s_mrv, &s_bt, &s_cp};
            // Rotate execution order to reduce a fixed first-solver bias.
            for(int j=0;j<4;j++) {
                int ix=(j+t)%4;
                double ms=time_solver(labels[ix],solvers[ix],&g);
                *sums[ix] += ms;
                fprintf(raw,"%d,%d,%s,%.9f\n",k,t,labels[ix],ms);
            }
        }

        csv_row(csv,k,"DLX",s_dlx/trials_per_k);
        csv_row(csv,k,"Backtracking+MRV/bitmask",s_mrv/trials_per_k);
        csv_row(csv,k,"Backtracking",s_bt/trials_per_k);
        csv_row(csv,k,"Constraint Propagation",s_cp/trials_per_k);

        fprintf(stderr,"k=%d → DLX %.3f ms | MRV %.3f | BT %.3f | CP %.3f\n",
                k, s_dlx/trials_per_k, s_mrv/trials_per_k, s_bt/trials_per_k, s_cp/trials_per_k);
    }

    fclose(csv);
    fclose(raw); fclose(corpus);
    fprintf(stderr,"Wrote strict benchmark summary, raw timings and corpus\n");
    return 0;
}
