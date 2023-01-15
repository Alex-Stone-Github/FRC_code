#include <iostream>
#include <SDL2/SDL.h>
#include "telescope.h"

int main() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "Failed to init SDL2\n");
    }
    SDL_Window* window = SDL_CreateWindow(
            "Simulation Test",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            800,
            600,
            0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window,
            -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL || window == NULL) {
        fprintf(stderr, "Failed to create SDL objects like window and renderer\n");
    }

    double mast_length = 80;
    double min_ext = 40;
    double max_ext = 70;
    ArmSimulation arm_simulator(mast_length, min_ext, max_ext);
    while (true) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                goto out;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        ArmControlPacketInfo info = arm_simulator.move_to_point(40, 40);
        std::cout << info.joint_angle << std::endl;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer, 0, 0, 0, mast_length);
        SDL_RenderDrawLine(renderer, 0, mast_length, std::cos(info.joint_angle) * min_ext,
                std::sin(info.joint_angle) * min_ext);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawLine(renderer, std::cos(info.joint_angle) * min_ext,
                std::sin(info.joint_angle) * min_ext, std::cos(info.joint_angle) * (
                info.extension_percent * (max_ext-min_ext) + min_ext), std::sin(
                info.joint_angle) * (info.extension_percent * (max_ext-min_ext) + min_ext));
        SDL_RenderPresent(renderer);
    }
out:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
