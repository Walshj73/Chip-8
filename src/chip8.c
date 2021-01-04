/* Program name : Chip-8 emulator */

#include <memory.h>
#include <assert.h>
#include <stdbool.h>
#include "chip8.h"

const char chip8_default_character_set[] = {
    0xf0, 0x90, 0x90, 0x90, 0xf0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xf0, 0x10, 0xf0, 0x80, 0xf0,
    0xf0, 0x10, 0xf0, 0x10, 0xf0,
    0x90, 0x90, 0xf0, 0x10, 0xf0,
    0xf0, 0x80, 0xf0, 0x10, 0xf0,
    0xf0, 0x80, 0xf0, 0x90, 0xf0,
    0xf0, 0x10, 0x20, 0x40, 0x40,
    0xf0, 0x90, 0xf0, 0x90, 0xf0,
    0xf0, 0x90, 0xf0, 0x10, 0xf0,
    0xf0, 0x90, 0xf0, 0x90, 0x90,
    0xe0, 0x90, 0xe0, 0x90, 0xe0,
    0xf0, 0x80, 0x80, 0x80, 0xf0,
    0xe0, 0x90, 0x90, 0x90, 0xe0,
    0xf0, 0x80, 0xf0, 0x80, 0xf0,
    0xf0, 0x80, 0xf0, 0x80, 0x80
}; /* End character set array */

void chip8_init(struct chip8* chip8)
{
    memset(chip8, 0, sizeof(struct chip8));
    memcpy(&chip8->memory.memory, chip8_default_character_set, sizeof(chip8_default_character_set));
} /* End init function */

void chip8_load(struct chip8* chip8, const char* buf, size_t size)
{
    assert(size+CHIP8_PROGRAM_LOAD_ADDRESS < CHIP8_MEMORY_SIZE);
    memcpy(&chip8->memory.memory[CHIP8_PROGRAM_LOAD_ADDRESS], buf, size);
    chip8->registers.PC = CHIP8_PROGRAM_LOAD_ADDRESS;
} /* End of load function */

static void chip8_exec_extended(struct chip8* chip8, unsigned short opcode)
{
    unsigned short nnn = opcode & 0x0fff;
    unsigned char x = (opcode >> 8) & 0x000f;
    unsigned char y = (opcode >> 4) & 0x000f;
    unsigned char kk = opcode & 0x00ff;
    unsigned short tmp = 0;

    switch (opcode & 0xf000)
    {
        /* 1nnn : Jump to location nnn */
        case 0x1000:
            chip8->registers.PC = nnn;
            break;

        /* 2nnn : Call subroutine at location nnn */
        case 0x2000:
            chip8_stack_push(chip8, chip8->registers.PC);
            chip8->registers.PC = nnn;
            break;

        /* 3xkk : Skip next instruction if Vx = kk */
        case 0x3000:
            if (chip8->registers.V[x] == kk)
            {
                chip8->registers.PC += 2;
            } /* End of if statement */
            break;
        /* 4xkk : Skip next instruction if Vx != kk */
        case 0x4000:
            if (chip8->registers.V[x] != kk)
            {
                chip8->registers.PC += 2;
            } /* End of if statement */
            break;
        /* 5xy0 : Skip the next instruction if Vx = Vy */
        case 0x5000:
            if (chip8->registers.V[x] == chip8->registers.V[y])
            {
                chip8->registers.PC += 2;
            } /* End if statement */
            break;

        /* 6xkk : Set Vx = kk */
        case 0x6000:
            chip8->registers.V[x] = kk;
            break;

        /* 7xkk : Set Vx = Vx + kk */
        case 0x7000:
            chip8->registers.V[x] += kk;
            break;

        /* Opcodes for 0x8000 instruction set */
        case 0x8000:
            switch (opcode & 0x000f)
            {
                /* 8xy0 : Set Vx = Vy */
                case 0x00:
                    chip8->registers.V[x] = chip8->registers.V[y];
                    break;

                /* 8xy1 : Set Vx = Vx OR Vy */
                case 0x01:
                    chip8->registers.V[x] = chip8->registers.V[x] |= chip8->registers.V[y];
                    break;

                /* 8xy2 : Set Vx = Vx AND Vy */
                case 0x02:
                    chip8->registers.V[x] = chip8->registers.V[x] &= chip8->registers.V[y];
                    break;

                /* 8xy3 : Set Vx = Vx XOR Vy */
                case 0x03:
                    chip8->registers.V[x] = chip8->registers.V[x] ^= chip8->registers.V[y];
                    break;

                /* 8xy4 : Set Vx = Vx + Vy, set VF = carry */
                case 0x04:
                    tmp = chip8->registers.V[x] + chip8->registers.V[y];
                    chip8->registers.V[0x0f] = tmp > 0xff;
                    chip8->registers.V[x] = tmp;
                    break;

                /* 8xy5 : Set Vx = Vx - Vy, Set VF = Not borrow */
                case 0x05:
                    chip8->registers.V[0x0f] = chip8->registers.V[x] > chip8->registers.V[y];
                    chip8->registers.V[x] = chip8->registers.V[x] - chip8->registers.V[y];
                    break;
                
                /* 8xy6 : Set Vx = Vx SHR 1 least-significant bit*/
                case 0x06:
                    chip8->registers.V[0x0f] = chip8->registers.V[x] & 0x01;
                    chip8->registers.V[x] /= 2;
                    break;

                /* 8xy7 : Set Vx = Vy - Vx, Set VF = Not borrow */ 
                case 0x07:
                    chip8->registers.V[0x0f] = chip8->registers.V[y] > chip8->registers.V[x];
                    chip8->registers.V[x] = chip8->registers.V[y] - chip8->registers.V[x];
                    break;

                /* 8xye : Set Vx = Vx SHL 1 most-significant bit */
                case 0x0e:
                    chip8->registers.V[0x0f] = chip8->registers.V[x] & 0x80;
                    chip8->registers.V[x] *= 2;
                    break;
            } /* End of nested switch */
            break;

        /* 9xy0 : Skip the next instruction if Vx != Vy */    
        case 0x9000:
            if (chip8->registers.V[x] != chip8->registers.V[y])
            {
                chip8->registers.PC += 2;
            } /* End of nested if statement */
            break;
    } /* End of switch statement */
} /* End of exec extended function */

void chip8_exec(struct chip8* chip8, unsigned short opcode)
{
    switch (opcode)
    {
        /* CLS : Clears the screen */
        case 0x00E0:
            chip8_screen_clear(&chip8->screen);
            break;

        /* Ret : Return from subroutine */
        case 0x00EE:
            chip8->registers.PC = chip8_stack_pop(chip8);
            break;

        default:
            chip8_exec_extended(chip8, opcode);
    } /* End of switch statement */
} /* End of exec function */
