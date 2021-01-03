/* Program name : Chip-8 emulator */

#include <assert.h>
#include <memory.h>
#include "chip8screen.h"

static void chip8_screen_check_bounds(int x, int y)
{
    assert(x >= 0 && x < CHIP8_WIDTH && y >= 0 && y < CHIP8_HEIGHT);
} /* End check bounds function */

void chip8_screen_clear(struct chip8_screen* screen)
{
    memset(screen->pixels, 0, sizeof(screen->pixels));
} /* End of screen clear function */

void chip8_screen_set(struct chip8_screen* screen, int x, int y)
{
    chip8_screen_check_bounds(x, y);
    screen->pixels[y][x] = true;
} /* End screen set function */

bool chip8_screen_is_set(struct chip8_screen* screen, int x, int y)
{
    chip8_screen_check_bounds(x, y);
    return screen->pixels[y][x];
} /* End screen is set function */

bool chip8_screen_draw_sprite(struct chip8_screen* screen, int x, int y, const char* sprite, int num)
{
    bool pixel_collison = false;

    for (int ly = 0; ly < num; ly++)
    {
        char c = sprite[ly];
        for (int lx = 0; lx < 8; lx++)
        {
            if ((c & (0b10000000 >> lx)) == 0)
            {
                continue;
            } /* End of if statement */
            if (screen->pixels[(ly+y) % CHIP8_HEIGHT][(lx+x) % CHIP8_WIDTH] ^= true)
            {
                pixel_collison = true;
            } /* End of if statement */
            screen->pixels[(ly+y) % CHIP8_HEIGHT][(lx+x) % CHIP8_WIDTH] ^= true;
        } /* End of nested for loop */
    } /* End of for loop */
} /* End draw sprite */
