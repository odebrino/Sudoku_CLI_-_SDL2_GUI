// Glue between GUI buttons and solver/generator.

#include "gui.h"

// gui_new_k(): helper function.
void gui_new_k(App* A, int k){
    if(!generator_new_k(k, &A->grid)){
        grid_clear(&A->grid);
        gui_show_message(A, "Generation failed", 0);
    }
}
// gui_hint(): helper function.
void gui_hint(App* A){ solver_hint(&A->grid); }
// gui_solve(): helper function.
void gui_solve(App* A){ solver_solve(&A->grid); }
