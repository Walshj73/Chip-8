/* Program name : Chip-8 emulator 
 * File name : chip8memory.c */

#include "chip8memory.h"
#include <assert.h>

static void chip8_is_memory_in_bounds(int index)
{
    assert(index >= 0 && index < CHIP8_MEMORY_SIZE);
} /* End static void function */

void chip8_memory_set(struct chip8_memory *memory, int index, unsigned char val)
{
    chip8_is_memory_in_bounds(index);
    memory->memory[index] = val;
} /* End memory set function */

unsigned char chip8_memory_get(struct chip8_memory *memory, int index)
{
    chip8_is_memory_in_bounds(index);
    return memory->memory[index];
} /* End memory get function */

unsigned short chip8_memory_get_short(struct chip8_memory* memory, int index)
{
    unsigned char byte1 = chip8_memory_get(memory, index);
    unsigned char byte2 = chip8_memory_get(memory, index+1);
    return byte1 << 8 | byte2;
} /* End of get short function */
