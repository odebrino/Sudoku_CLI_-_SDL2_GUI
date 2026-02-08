// Tiny PRNG utilities for the generator.

#include <stdlib.h>
#include <time.h>
#include <errno.h>

static int seeded=0;

// parse_seed_env(): helper function.
static int parse_seed_env(unsigned int* out){
    const char* s = getenv("SEED");
    if(!s || !*s) return 0;
    char* end = NULL;
    errno = 0;
    unsigned long val = strtoul(s, &end, 10);
    if(errno != 0 || end == s || *end != '\0') return 0;
    *out = (unsigned int)val;
    return 1;
}

// rng_seed(): helper function.
void rng_seed(unsigned int seed){
    srand(seed);
    seeded = 1;
}

// rng_seed_from_env(): helper function.
void rng_seed_from_env(void){
    unsigned int seed;
    if(!seeded && parse_seed_env(&seed)) rng_seed(seed);
}

// ensure_seed(): helper function.
static void ensure_seed(void){
    if(seeded) return;
    unsigned int seed;
    if(parse_seed_env(&seed)) rng_seed(seed);
    else rng_seed((unsigned)time(NULL));
}

// rnd_int(): helper function.
int rnd_int(int lo, int hi){
    ensure_seed();
    if(hi<=lo) return lo;
    return lo + rand() % (hi-lo+1);
}

// rnd_shuffle_int(): helper function.
void rnd_shuffle_int(int* a, int n){
    ensure_seed();
    for(int i=n-1;i>0;i--){
        int j = rand() % (i+1);
        int t=a[i]; a[i]=a[j]; a[j]=t;
    }
}
