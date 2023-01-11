// what

#include <iostream>
#include <SDL2/SDL.h>
#include <vector>

#include "../include/invkin.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

double t = 0;
bool is_tracing = false;
std::vector<Vector> points;


Vector lerp(Vector a, Vector b, double percent) {
    return Vector(a.x + (b.x-a.x)*percent, a.y + (b.y-a.y)*percent);
}
Vector piecemeal(double percent) {
    std::vector<Vector> ps = points;
    if (points.size() > 1) {
        // drain
        while (ps.size() > 1) {
            std::vector<Vector> new_ps;
            for (int i = 1; i < ps.size(); i ++) {
                new_ps.push_back(lerp(ps[i-1], ps[i], percent));
            }
            ps = new_ps;
        }
        return ps[0]; // return the only element
    }
    return Vector(0, 0);
}

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
    std::vector<double> lengths = {200, 170};
    Arm arm(Vector(300, 300), lengths);


    // while loop
    while (true) {
        // events / input
        double mouse_x;
        double mouse_y;
        {
            int mx, my;
            SDL_GetMouseState(&mx, &my);
            mouse_x = mx;
            mouse_y = my;
        }
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                goto quit;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                points.push_back(Vector(mouse_x, mouse_y));
            }
            if (e.type == SDL_KEYDOWN) {
                is_tracing = true;
            }
        }
        // logic
        if (!is_tracing) {
            arm.move_to_target_with_anchoring(Vector(mouse_x, mouse_y));
        }
        else { // tracing
            Vector target = piecemeal(t);
            arm.move_to_target_with_anchoring(target);
        }
        if (t > 1) {
            is_tracing = false;
            points = {};
            t = 0;
        }
        if (is_tracing) {
            t += 0.001;
        }
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
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_Rect r;
            r.x = end.x - 3;
            r.y = end.y - 3;
            r.w = 6; r.h = 6;
            SDL_RenderFillRect(renderer, &r);
        }
        // draw mouse
        {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect r;
        r.x = mouse_x - 5;
        r.y = mouse_y - 5;
        r.w = 10; r.h = 10;
        SDL_RenderFillRect(renderer, &r);
        }
        // draw points
        for (auto point : points) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_Rect r;
            r.x = point.x - 5;
            r.y = point.y - 5;
            r.w = 10; r.h = 10;
            SDL_RenderFillRect(renderer, &r);
        }
        // draw path
        for (double i = 0; i < 1; i += 0.001) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            Vector point = piecemeal(i);
            SDL_RenderDrawPoint(renderer, point.x, point.y);
        }
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
