// SDL2 GUI setup/teardown helpers.

#include "gui.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// join_path(): helper function.
static void join_path(char* out, size_t n, const char* dir, const char* rel){
    if(!dir || !*dir){ snprintf(out, n, "%s", rel); return; }
    size_t len = strlen(dir);
    if(len > 0 && dir[len-1] == '/') snprintf(out, n, "%s%s", dir, rel);
    else snprintf(out, n, "%s/%s", dir, rel);
}

// open_font_candidate(): helper function.
static TTF_Font* open_font_candidate(App* A, const char* path, int pts){
    TTF_Font* f = TTF_OpenFontIndex(path, pts, 0);
    if(f){
        snprintf(A->font_path, sizeof(A->font_path), "%s", path);
        return f;
    }
    return NULL;
}

// load_font(): helper function.
static TTF_Font* load_font(App* A, int pts){
    char buf[512];

    // 1) project-relative
    TTF_Font* f = open_font_candidate(A, "assets/fonts/DejaVuSans.ttf", pts);
    if(f) return f;

    // 2) relative to executable
    char* base = SDL_GetBasePath();
    if(base){
        join_path(buf, sizeof(buf), base, "../assets/fonts/DejaVuSans.ttf");
        f = open_font_candidate(A, buf, pts);
        SDL_free(base);
        if(f) return f;
    }

    // 3) system fallback
    f = open_font_candidate(A, "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", pts);
    if(f) return f;
    f = open_font_candidate(A, "DejaVuSans.ttf", pts);
    if(f) return f;

    A->font_path[0] = '\0';
    return NULL;
}

// env_int(): helper function.
static int env_int(const char* name, int def, int min, int max){
    const char* v = getenv(name);
    if(!v || !*v) return def;
    char* end = NULL;
    long val = strtol(v, &end, 10);
    if(end == v || *end != '\0') return def;
    if(val < min) val = min;
    if(val > max) val = max;
    return (int)val;
}

// save_screenshot(): helper function.
static int save_screenshot(App* A, const char* path){
    int w=0, h=0;
    if(SDL_GetRendererOutputSize(A->ren, &w, &h)!=0) return 0;
    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ARGB8888);
    if(!surf) return 0;
    if(SDL_RenderReadPixels(A->ren, NULL, SDL_PIXELFORMAT_ARGB8888, surf->pixels, surf->pitch)!=0){
        SDL_FreeSurface(surf);
        return 0;
    }
    int ok = (SDL_SaveBMP(surf, path) == 0);
    SDL_FreeSurface(surf);
    return ok;
}

// gui_init(): helper function.
int gui_init(App* A){
    memset(A, 0, sizeof(*A));
    if(SDL_Init(SDL_INIT_VIDEO)!=0) return -1;
    if(TTF_Init()!=0) return -2;
    rng_seed_from_env();
    A->win = SDL_CreateWindow("Sudoku (GUI) • 🎲 Theme Violet", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 860, 0);
    if(!A->win){ gui_shutdown(A); return -3; }
    A->ren = SDL_CreateRenderer(A->win, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(!A->ren) A->ren = SDL_CreateRenderer(A->win, -1, SDL_RENDERER_SOFTWARE);
    if(!A->ren){ gui_shutdown(A); return -4; }
    A->screenshot_path[0] = '\0';
    A->screenshot_done = 0;
    A->font = load_font(A, 20);
    if(!A->font){ gui_shutdown(A); return -5; }
    {
        const char* shot = getenv("GUI_SCREENSHOT");
        if(shot && *shot) snprintf(A->screenshot_path, sizeof(A->screenshot_path), "%s", shot);
    }
    A->running=1; A->cell=-1; A->show_conflicts=1;
    A->mode = GUI_MENU; A->menu_sel=2; A->custom_k=50;
    A->start_ticks = SDL_GetTicks();
    A->show_modal=0; A->solved_time_ms=0; A->cand_mode=1; A->hover_btn=-1; A->trace_collapsed=0;
    grid_clear(&A->grid);
    return 0;
}

// gui_shutdown(): helper function.
void gui_shutdown(App* A){
    if(A->font) TTF_CloseFont(A->font);
    if(A->ren) SDL_DestroyRenderer(A->ren);
    if(A->win) SDL_DestroyWindow(A->win);
    TTF_Quit();
    SDL_Quit();
}


// gui_show_message(): helper function.
void gui_show_message(App* A, const char* msg, int ms){ (void)ms;
    // reuse window title for quick message in console and keep a timer via show_modal with 0-time?
    // We'll store the message in the window title briefly.
    char title[256]; snprintf(title,sizeof(title),"Sudoku (GUI) • %s", msg);
    SDL_SetWindowTitle(A->win, title);
    // We won't block; caller may set again later. Title persists.
}

// run_gui(): helper function.
int run_gui(void){

    App A;
    if(gui_init(&A)!=0){ fprintf(stderr,"GUI initialization failed: %s\n", SDL_GetError()); return 1; }
    int screenshot_failed = 0;
    SDL_Event e;
    if(A.screenshot_path[0]){
        const char* mode = getenv("GUI_SCREENSHOT_MODE");
        if(mode && strcmp(mode, "game")==0){
            int k = env_int("GUI_SCREENSHOT_K", 50, 0, 60);
            gui_start_game(&A, k);
        }
    }

    while(A.running){
        while(SDL_PollEvent(&e)){
            gui_handle_event(&A, &e);
        }
        gui_draw(&A);
        if(A.screenshot_path[0] && !A.screenshot_done){
            if(!save_screenshot(&A, A.screenshot_path)){
                fprintf(stderr, "Failed to save screenshot: %s\n", SDL_GetError());
                screenshot_failed = 1;
            }
            A.screenshot_done = 1;
            A.running = 0;
        }
        SDL_Delay(16);
    }
    gui_shutdown(&A);
    return screenshot_failed ? 1 : 0;
}
