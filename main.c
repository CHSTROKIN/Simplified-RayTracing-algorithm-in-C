#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "vectors.h"
#include "rays.h"
#include "graphics.h"

#include <string.h>
#include <SDL2/SDL.h>

#define PI 3.1415926
#define WALL_HEIGHT 2

#define QUITKEY SDLK_ESCAPE
#define WIDTH 1024
#define HEIGHT 768
#define DEBUG false

SDL_Window* screen = NULL;
SDL_Renderer* renderer;
SDL_Event event;
SDL_Rect source, destination, dst;
int keypressed;

// SDL sections use resources from https://learncgames.com/tutorials/getting-started-with-sdl-and-c/

void FinishOff() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(screen);
	//Quit SDL
	SDL_Quit();
	exit(0);
}

void GameLoop(voxel **voxels, vector start, tilemap map, view init_view) {
	int gameRunning = 1;
    bool rerender = true;

    vector cur_pos = start;
    view view = init_view;
    
	while (gameRunning) {
        if (rerender) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            int midz = view.height / 2;
            int midx = view.width / 2;

            pixel *pixels = render(voxels, view, cur_pos, map, DEBUG);

            for (int vz = 0; vz < view.height; vz ++) {
                for (int vx = 0; vx < view.width; vx ++) {
                    pixel contact = pixels[vz * view.width + vx];
                    SDL_SetRenderDrawColor(renderer, contact.r, contact.g, contact.b, 255);
                    SDL_RenderDrawPoint(renderer, vx, vz);
                }
            }
            SDL_RenderPresent(renderer);

            rerender = false;
        }

		while (SDL_PollEvent(&event)) {
            double cur_x, cur_y;
            cur_x = cur_pos.x;
            cur_y = cur_pos.y;
			switch (event.type) {
                case SDL_KEYDOWN:
                    keypressed = event.key.keysym.sym;

                    switch (keypressed) {
                        case QUITKEY: gameRunning = 0; break;
                        case SDLK_a: cur_pos.x -= 0.1; rerender = true; break; 
                        case SDLK_d: cur_pos.x += 0.1; rerender = true; break;
                        case SDLK_w: cur_pos.y -= 0.1; rerender = true; break;
                        case SDLK_s: cur_pos.y += 0.1; rerender = true; break;
                        
                        case SDLK_LEFT:
                            view.x_rotation -= 0.04;
                            if (view.x_rotation <= 0) view.x_rotation = 2*PI;
                            rerender = true;
                            break;
                        
                        case SDLK_RIGHT:
                            view.x_rotation += 0.04;
                            if (view.x_rotation >= 2*PI) view.x_rotation = 0;
                            rerender = true;
                            break;
                        
                        case SDLK_DOWN:
                            view.z_rotation += 0.02;
                            if (view.z_rotation >= 2*PI) view.z_rotation = 0;
                            rerender = true;
                            break;
                        
                        case SDLK_UP:
                            view.z_rotation -= 0.02;
                            if (view.z_rotation <= 0) view.z_rotation = 2*PI;
                            rerender = true;
                            break;
                    }
                    break;
                    
                case SDL_QUIT: /* if mouse click to close window */
                    gameRunning = 0;
                    break;
                case SDL_KEYUP:
                    break;
			}
            tile cur_tile = map_get_tile(map, cur_pos.x, cur_pos.y, cur_pos.z);
            if (cur_tile != TILE_EMPTY) {
                cur_pos.x = cur_x;
                cur_pos.y = cur_y;
            }
		}
	}
}

int main(void) {

    char tiles[] = {
        "YYYYYYYYY"
        "YYYYYYYYY"
        "YYYYYYYYY"
        "YYYYYYYYY"
        "YYYYYYYYY"
        "YYYYYYYYY"
        "YYYYYYYYY"
        "YYYYYYYYY"
        "YYYYYYYYY"
        "CCCCCCCCC"
        "W       C"
        "W       C"
        "W   G   C"
        "W       C"
        "WW     WC"
        "W W   W C"
        "W   W   C"
        "CCCCMCCCC"
        "CCCCCCCCC"
        "W       C"
        "W       C"
        "W   G   C"
        "W       C"
        "W       C"
        "W       C"
        "W       C"
        "CCCCMCCCC"
        "         "
        "         "
        "         "
        "         "
        "         "
        "         "
        "         "
        "         "
        "         "
    };

    int max_x = 9;
    int max_y = 9;
    int max_z = 4;

    tilemap map = {
        .tiles = tiles,
        .max_x = max_x,
        .max_y = max_y,
        .max_z = max_z
    };

    vector start = { .x = 3.5, .y = 6, .z = 3 };

    // initialize a small view
    view viewport = {
        .width = 400,
        .height = 400,
        .x_rotation = 0.3,
        .z_rotation = 0.6
    };

    // 180 degrees by 40 degrees, approxiamtes the human eye
    set_camera_position(0.9 * PI, 0.222 * PI, &viewport);
    printf("focal length: %f\n", viewport.camera_pos.y);

    voxel **voxels = init_voxels();

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(viewport.width, viewport.height, 
        SDL_WINDOW_SHOWN, &screen, &renderer);
    GameLoop(voxels, start, map, viewport);
    FinishOff();

    return EXIT_SUCCESS;
}
