// what

#include <iostream>
#include <SDL2/SDL.h>
#include <vector>

#include "../include/invkin.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main() {
    // init
    std::cout << "Hello, World" << std::endl;
    SDL_Window* window;
    SDL_Renderer* renderer;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "Failed to init sdl2\n");
        return 1;
    }
    window = SDL_CreateWindow("INVKIN", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "failed to create window:");
        fprintf(stderr, "%s", SDL_GetError());
    }
    renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "failed to create renderer:");
        fprintf(stderr, "%s", SDL_GetError());
    }

    // logic
    std::vector<double> lengths = {100, 90};
    Arm arm(Vector(300, 300), lengths);


    // while loop
    while (true) {
        // events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                goto quit;
            }
        }
        // logic
        double mouse_x;
        double mouse_y;
        {
            int mx, my;
            SDL_GetMouseState(&mx, &my);
            mouse_x = mx;
            mouse_y = my;
        }
        arm.move_to_target_with_anchoring(Vector(mouse_x, mouse_y));
        // render --- 
        // background
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderClear(renderer);
        // draw segment
        std::vector<Segment> segments = arm.get_segments();
        for (auto segment : segments) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            Vector end = segment.calculate_end();
            SDL_RenderDrawLine(renderer, segment.position.x, segment.position.y,
                    end.x, end.y);
        }
        // draw mouse
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect r;
        r.x = mouse_x - 5;
        r.y = mouse_y - 5;
        r.w = 10; r.h = 10;
        SDL_RenderFillRect(renderer, &r);
        // done
        SDL_RenderPresent(renderer);
    }

    // cleanup
quit:
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}
