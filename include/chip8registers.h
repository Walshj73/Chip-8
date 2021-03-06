/* Program name : Chip-8 emulator 
 * File name : chip8registers.h */

#ifndef CHIP8REGISTERS_H
#define CHIPREGISTERS_H

#include "config.h"

struct chip8_registers
{
    unsigned char V[CHIP8_TOTAL_DATA_REGISTERS];
    unsigned short I;
    unsigned char delay_timer;
    unsigned char sound_timer;
    unsigned short PC;
    unsigned char SP;
}; /* End registers struct */

#endif
