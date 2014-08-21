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

void input_handler(void *cpu)
{
    SDL_Event event;
    struct cpu_6502 *p = (struct cpu_6502 *) cpu;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        p->keypad[KEYPAD_UP] = 1;
                        break;
                    case SDLK_s:
                        p->keypad[KEYPAD_DOWN] = 1;
                        break;
                    case SDLK_a:
                        p->keypad[KEYPAD_LEFT] = 1;
                        break;
                    case SDLK_d:
                        p->keypad[KEYPAD_RIGHT] = 1;
                        break;
                    case SDLK_u:
                        p->keypad[KEYPAD_START] = 1;
                        break;
                    case SDLK_i:
                        p->keypad[KEYPAD_SELECT] = 1;
                        break;
                    case SDLK_j:
                        p->keypad[KEYPAD_A] = 1;
                        break;
                    case SDLK_k:
                        p->keypad[KEYPAD_B] = 1;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        p->keypad[KEYPAD_UP] = 0;
                        break;
                    case SDLK_s:
                        p->keypad[KEYPAD_DOWN] = 0;
                        break;
                    case SDLK_a:
                        p->keypad[KEYPAD_LEFT] = 0;
                        break;
                    case SDLK_d:
                        p->keypad[KEYPAD_RIGHT] = 0;
                        break;
                    case SDLK_u:
                        p->keypad[KEYPAD_START] = 0;
                        break;
                    case SDLK_i:
                        p->keypad[KEYPAD_SELECT] = 0;
                        break;
                    case SDLK_j:
                        p->keypad[KEYPAD_A] = 0;
                        break;
                    case SDLK_k:
                        p->keypad[KEYPAD_B] = 0;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}

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
    gTexture = SDL_CreateTexture(gRender, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 256, 240);
}

void sighdl(int sig)
{
    if (sig == SIGINT) {
        SDL_DestroyRenderer(gRender);
        SDL_DestroyWindow(gWindow);
        gWindow = NULL;

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
    struct cpu_6502 *cpu = malloc(sizeof(struct cpu_6502));
    struct ppu *ppu = malloc(sizeof(struct ppu));

    // Init cpu and ppu.
    cpu_setup(cpu);
    ppu_setup(ppu);
    cpu->ppu = ppu;
    cpu->handle_input = input_handler;
    ppu->r = render;

    // Get ready for SDL.
    graphic_init();
    // Exit at ctrl+c
    if (signal(SIGINT, sighdl) == SIG_ERR) {
        return 1;
    }

    // Loads the rom and sets the cpu and ppu memory mapping.
    load_rom(&test_rom, filename);
    cpu->rom_prg = test_rom.prg_rom_data;
    ppu->rom_chr = test_rom.chr_rom_data;
    printf("chr rom %x\n", ppu->rom_chr);
    ppu->rom_mirroring = test_rom.rom_mirroring;

    cpu_reset(cpu);
    cpu_run(cpu);

    return 0;
}
