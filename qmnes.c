/*
 * qmnes.c
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <stdio.h>
#include <unistd.h>
#include <SDL.h>

#include "cpu.h"
#include "rom.h"
#include "ins.h"
#include "ppu.h"

SDL_Window *gWindow = NULL;
SDL_Surface *gScreen = NULL;
SDL_Renderer *gRender = NULL;
SDL_Texture *gTexture = NULL;

void render(uint32_t *pixels)
{
    uint32_t *p;
    int pitch;

    SDL_LockTexture(gTexture, NULL, (void **)&p, &pitch);
    memcpy(p, pixels, sizeof(uint32_t) * 256 * 240);
    SDL_UnlockTexture(gTexture);
    SDL_RenderCopy(gRender, gTexture, NULL, NULL);
    SDL_RenderPresent(gRender);
}

void graphic_init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        perror("Video cannot be initialized");
        exit(1);
    }

    gWindow = SDL_CreateWindow("qmnes", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 240, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
        perror("Video cannot be initialized");
        exit(1);
    }
    gRender = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    gScreen = SDL_GetWindowSurface(gWindow);
    gTexture = SDL_CreateTexture(gRender, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 256, 240);
}

void sighdl(int sig)
{
    if (sig == SIGINT) {
        SDL_DestroyRenderer(gRender);
        SDL_DestroyWindow(gWindow);
        gWindow = NULL;

        SDL_Quit();
        SDL_Quit();

        printf("Bye my dear\n");
        exit(0);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("oops\n");
        return 1;
    }

    char *filename = argv[1];

    struct rom test_rom;
    struct cpu_6502 cpu;
    struct ppu ppu;

    // Init cpu and ppu.
    cpu_setup(&cpu);
    ppu_setup(&ppu);
    cpu.ppu = &ppu;
    ppu.r = render;

    // Get ready for SDL.
    graphic_init();
    // Exit at ctrl+c
    if (signal(SIGINT, sighdl) == SIG_ERR) {
        return 1;
    }

    // Loads the rom and sets the cpu and ppu memory mapping.
    load_rom(&test_rom, filename);
    cpu.rom_prg = test_rom.prg_rom_data;
    ppu.rom_chr = test_rom.chr_rom_data;

    cpu_reset(&cpu);
    cpu_run(&cpu);

    return 0;
}
