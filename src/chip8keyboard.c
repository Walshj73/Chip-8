/* Program name : Chip-8 emulator */

#include "chip8keyboard.h"
#include <assert.h>

static void chip8_keyboard_ensure_in_bounds(int key)
{
    assert(key >= 0 && key < CHIP8_TOTAL_KEYS);
} /* End static void function */

int chip8_keyboard_map(const char *map, char key)
{
    for (int i = 0; i < CHIP8_TOTAL_KEYS; i++)
    {
        if (map[i] == key)
        {
            return i;
        } /* End if nested if statement */
        return -1;
    } /* End for loop */
}

void chip8_keyboard_down(struct chip8_keyboard *keyboard, int key)
{
    keyboard->keyboard[key] = true;
} /* End keyboard down function */

void chip8_keyboard_up(struct chip8_keyboard *keyboard, int key)
{
    keyboard->keyboard[key] = false;
} /* End keyboard up function */

bool chip8_keyboard_is_down(struct chip8_keyboard *keyboard, int key)
{
    return keyboard->keyboard[key];
} /* End keyboard is down function */
