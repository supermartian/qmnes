/*
 * qmnes-aa.c
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <aalib.h>

#include "cpu.h"
#include "rom.h"
#include "ins.h"
#include "ppu.h"

#define XSIZ aa_imgwidth(context)
#define YSIZ (aa_imgheight(context)-4)
#define MAXTABLE (256*5)

static aa_palette palette;
static unsigned int table[MAXTABLE];
__AA_CONST static int pal[] =
   {
     0, 0, 0, 0, 0, 6, 0, 0, 6, 0, 0, 7, 0, 0, 8, 0, 0, 8, 0, 0, 9, 0, 0, 10,
     2, 0, 10, 4, 0, 9, 6, 0, 9, 8, 0, 8, 10, 0, 7, 12, 0, 7, 14, 0, 6, 16, 0, 5,
     18, 0, 5, 20, 0, 4, 22, 0, 4, 24, 0, 3, 26, 0, 2, 28, 0, 2, 30, 0, 1, 32, 0, 0,
     32, 0, 0, 33, 0, 0, 34, 0, 0, 35, 0, 0, 36, 0, 0, 36, 0, 0, 37, 0, 0, 38, 0, 0,
     39, 0, 0, 40, 0, 0, 40, 0, 0, 41, 0, 0, 42, 0, 0, 43, 0, 0, 44, 0, 0, 45, 0, 0,
     46, 1, 0, 47, 1, 0, 48, 2, 0, 49, 2, 0, 50, 3, 0, 51, 3, 0, 52, 4, 0, 53, 4, 0,
     54, 5, 0, 55, 5, 0, 56, 6, 0, 57, 6, 0, 58, 7, 0, 59, 7, 0, 60, 8, 0, 61, 8, 0,
     63, 9, 0, 63, 9, 0, 63, 10, 0, 63, 10, 0, 63, 11, 0, 63, 11, 0, 63, 12, 0, 63, 12, 0,
     63, 13, 0, 63, 13, 0, 63, 14, 0, 63, 14, 0, 63, 15, 0, 63, 15, 0, 63, 16, 0, 63, 16, 0,
     63, 17, 0, 63, 17, 0, 63, 18, 0, 63, 18, 0, 63, 19, 0, 63, 19, 0, 63, 20, 0, 63, 20, 0,
     63, 21, 0, 63, 21, 0, 63, 22, 0, 63, 22, 0, 63, 23, 0, 63, 24, 0, 63, 24, 0, 63, 25, 0,
     63, 25, 0, 63, 26, 0, 63, 26, 0, 63, 27, 0, 63, 27, 0, 63, 28, 0, 63, 28, 0, 63, 29, 0,
     63, 29, 0, 63, 30, 0, 63, 30, 0, 63, 31, 0, 63, 31, 0, 63, 32, 0, 63, 32, 0, 63, 33, 0,
     63, 33, 0, 63, 34, 0, 63, 34, 0, 63, 35, 0, 63, 35, 0, 63, 36, 0, 63, 36, 0, 63, 37, 0,
     63, 38, 0, 63, 38, 0, 63, 39, 0, 63, 39, 0, 63, 40, 0, 63, 40, 0, 63, 41, 0, 63, 41, 0,
     63, 42, 0, 63, 42, 0, 63, 43, 0, 63, 43, 0, 63, 44, 0, 63, 44, 0, 63, 45, 0, 63, 45, 0,
     63, 46, 0, 63, 46, 0, 63, 47, 0, 63, 47, 0, 63, 48, 0, 63, 48, 0, 63, 49, 0, 63, 49, 0,
     63, 50, 0, 63, 50, 0, 63, 51, 0, 63, 52, 0, 63, 52, 0, 63, 52, 0, 63, 52, 0, 63, 52, 0,
     63, 53, 0, 63, 53, 0, 63, 53, 0, 63, 53, 0, 63, 54, 0, 63, 54, 0, 63, 54, 0, 63, 54, 0,
     63, 54, 0, 63, 55, 0, 63, 55, 0, 63, 55, 0, 63, 55, 0, 63, 56, 0, 63, 56, 0, 63, 56, 0,
     63, 56, 0, 63, 57, 0, 63, 57, 0, 63, 57, 0, 63, 57, 0, 63, 57, 0, 63, 58, 0, 63, 58, 0,
     63, 58, 0, 63, 58, 0, 63, 59, 0, 63, 59, 0, 63, 59, 0, 63, 59, 0, 63, 60, 0, 63, 60, 0,
     63, 60, 0, 63, 60, 0, 63, 60, 0, 63, 61, 0, 63, 61, 0, 63, 61, 0, 63, 61, 0, 63, 62, 0,
     63, 62, 0, 63, 62, 0, 63, 62, 0, 63, 63, 0, 63, 63, 1, 63, 63, 2, 63, 63, 3, 63, 63, 4,
     63, 63, 5, 63, 63, 6, 63, 63, 7, 63, 63, 8, 63, 63, 9, 63, 63, 10, 63, 63, 10, 63, 63, 11,
     63, 63, 12, 63, 63, 13, 63, 63, 14, 63, 63, 15, 63, 63, 16, 63, 63, 17, 63, 63, 18, 63, 63, 19,
     63, 63, 20, 63, 63, 21, 63, 63, 21, 63, 63, 22, 63, 63, 23, 63, 63, 24, 63, 63, 25, 63, 63, 26,
     63, 63, 27, 63, 63, 28, 63, 63, 29, 63, 63, 30, 63, 63, 31, 63, 63, 31, 63, 63, 32, 63, 63, 33,
     63, 63, 34, 63, 63, 35, 63, 63, 36, 63, 63, 37, 63, 63, 38, 63, 63, 39, 63, 63, 40, 63, 63, 41,
     63, 63, 42, 63, 63, 42, 63, 63, 43, 63, 63, 44, 63, 63, 45, 63, 63, 46, 63, 63, 47, 63, 63, 48,
     63, 63, 49, 63, 63, 50, 63, 63, 51, 63, 63, 52, 63, 63, 52, 63, 63, 53, 63, 63, 54, 63, 63, 55,
     63, 63, 56, 63, 63, 57, 63, 63, 58, 63, 63, 59, 63, 63, 60, 63, 63, 61, 63, 63, 62, 63, 63, 63};

aa_context *context;

/* 
 * The following function (fastscale) is taken from Greag Alexander's aavga
 * library. Jan Hubicka <hubicka@freesoft.cz> is the current maintainer of
 * aavga.
 *
 * http://aa-project.sourceforge.net/aavga/
 * 
 */
void fastscale (register char *b1, register char *b2, int x1, int x2, int y1, int y2)
{
  register int ex, spx = 0, ddx, ddx1;
  int ddy1, ddy, spy = 0, ey;
  int x;
  char *bb1 = b1;
  if (!x1 || !x2 || !y1 || !y2)
    return;
  ddx = x1 + x1;
  ddx1 = x2 + x2;
  if (ddx1 < ddx)
    spx = ddx / ddx1, ddx %= ddx1;
  ddy = y1 + y1;
  ddy1 = y2 + y2;
  if (ddy1 < ddy)
    spy = (ddy / ddy1) * x1, ddy %= ddy1;
  ey = -ddy1;
  for (; y2; y2--)
    {
      ex = -ddx1;
      for (x = x2; x; x--)
        {
          *b2 = *b1;
          b2++;
          b1 += spx;
          ex += ddx;
          if (ex > 0)
            {
              b1++;
              ex -= ddx1;
            }
        }
      bb1 += spy;
      ey += ddy;
      if (ey > 0)
        {
          bb1 += x1;
          ey -= ddy1;
        }
      b1 = bb1;
    }
}

/* end of fastscale */
void input_handler(void *cpu)
{
    struct cpu_6502 *p = (struct cpu_6502 *) cpu;
    int key;
    switch (key = aa_getevent(context, 0)) {
        case AA_UP:
            memset(p->keypad, 0, 8 * sizeof(uint8_t));
            p->keypad[KEYPAD_UP] = 1;
            break;
        case AA_DOWN:
            memset(p->keypad, 0, 8 * sizeof(uint8_t));
            p->keypad[KEYPAD_DOWN] = 1;
            break;
        case AA_LEFT:
            memset(p->keypad, 0, 8 * sizeof(uint8_t));
            p->keypad[KEYPAD_LEFT] = 1;
            break;
        case AA_RIGHT:
            memset(p->keypad, 0, 8 * sizeof(uint8_t));
            p->keypad[KEYPAD_RIGHT] = 1;
            break;
        case 'u':
            memset(p->keypad, 0, 8 * sizeof(uint8_t));
            p->keypad[KEYPAD_START] = 1;
            break;
        case 'i':
            memset(p->keypad, 0, 8 * sizeof(uint8_t));
            p->keypad[KEYPAD_SELECT] = 1;
            break;
        case 'j':
            memset(p->keypad, 0, 8 * sizeof(uint8_t));
            p->keypad[KEYPAD_A] = 1;
            break;
        case 'k':
            memset(p->keypad, 0, 8 * sizeof(uint8_t));
            p->keypad[KEYPAD_B] = 1;
            break;
        default:
            break;
    }
    printf("%d key\n", key);
}

void render(uint32_t *pixels)
{
    int i = 0;
    fastscale(pixels, aa_image(context), 256, aa_imgwidth(context), 240, aa_imgheight(context));
    aa_fastrender(context, 0, 0, aa_imgwidth(context), aa_imgheight(context));
    //aa_renderpalette(context, palette, &aa_defrenderparams, 0, 0, aa_scrwidth(context), aa_scrheight(context));
    aa_flush(context);
}

void input_init()
{
    if (!aa_autoinitkbd(context, 0)) {
        aa_close(context);
        printf("Failed to initialize keyboard\n");
        exit(1);
    }
}

void graphic_init()
{
    // Use curses as aa_driver
    int i;
    for (i = 0; aa_drivers[i] != NULL; i++) {
        if (!strcmp(aa_drivers[i]->name, "X11") ||
                !strcmp(aa_drivers[i]->shortname, "X11")) {
            break;
        }
    }

    if (aa_drivers[i] == NULL) {
        fprintf(stderr,"Cannot initialize AA-lib with curses. Sorry\n");
        exit(1);
    }

    context = aa_init(aa_drivers[i], &aa_defparams, NULL);
    if (context == NULL) {
        fprintf(stderr,"Cannot initialize AA-lib. Sorry\n");
        exit(1);
    }

    /*
     *for (i = 0; i < 256; i++) {
     *    aa_setpalette (palette, i, pal[i * 3] * 4, pal[i * 3 + 1] * 4, pal[i * 3 + 2] * 4);
     *}
     */

    aa_hidecursor (context);
    if (aa_image(context) == NULL) {
        fprintf(stderr,"Cannot initialize AA-lib. Sorry\n");
        exit(1);
    }
    //context->imagebuffer = malloc(sizeof(uint32_t) * 256 * 240);
}

void sighdl(int sig)
{
    aa_close(context);
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

    // Get ready for AA.
    graphic_init();
    input_init();
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
