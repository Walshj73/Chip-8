/* Program name : Chip-8 emulator 
 * File name : chip8.c */

#include <memory.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "chip8.h"
#include "SDL2/SDL.h"

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

static char chip8_wait_key_press(struct chip8* chip8)
{
    SDL_Event event;
    while (SDL_WaitEvent(&event))
    {
        if (event.type != SDL_KEYDOWN)
        {
            continue;
        } /* End of nested if statement */

        char c = event.key.keysym.sym;
        char chip8_key = chip8_keyboard_map(&chip8->keyboard, c);
        if (chip8_key != -1)
        {
            return chip8_key;
        } /* End of nested if statement */
    } /* End of while loop */

    return -1;
} /* End of wait for key press function */

static void chip8_exec_extended(struct chip8* chip8, unsigned short opcode)
{
    unsigned short nnn = opcode & 0x0fff;
    unsigned char x = (opcode >> 8) & 0x000f;
    unsigned char y = (opcode >> 4) & 0x000f;
    unsigned char kk = opcode & 0x00ff;
    unsigned short tmp = 0;
    unsigned char n = opcode & 0x000f;

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
            {
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
            } /* End of scope */
            break;

        /* 9xy0 : Skip the next instruction if Vx != Vy */    
        case 0x9000:
            if (chip8->registers.V[x] != chip8->registers.V[y])
            {
                chip8->registers.PC += 2;
            } /* End of nested if statement */
            break;

        /* Annn : Set I = nnn */
        case 0xA000:
            chip8->registers.I = nnn;
            break;

        /* Bnnn : Jump to location nnn + V0 */
        case 0xB000:
            chip8->registers.PC = nnn + chip8->registers.V[0x00];
            break;

        /* Cxkk : Set Vx = random byte AND kk */
        case 0xC000:
            srand(clock());
            chip8->registers.V[x] = (rand() % 255) & kk;
            break;

        /* 0xD000 : Draw to the screen */
        case 0xD000:
            {
                const char* sprite = (const char*) &chip8->memory.memory[chip8->registers.I];
                chip8->registers.V[0x0f] = chip8_screen_draw_sprite(
                        &chip8->screen,
                        chip8->registers.V[x],
                        chip8->registers.V[y],
                        sprite,
                        n
                );
            } /* End of scope */
            break;

        /* Opcodes for 0xE000 instruction set */
        case 0xE000:
            {
                switch (opcode & 0x00ff)
                {
                    /* Ex9E : Skip the next instruction if the key with the value of Vx is pressed */
                    case 0x9e:
                        if (chip8_keyboard_is_down(&chip8->keyboard, chip8->registers.V[x]))
                        {
                            chip8->registers.PC += 2;
                        }
                        break;

                    /* ExA1 : Skip the next instruction if the key with the value of Vx is not pressed */
                    case 0xa1:
                        if (!chip8_keyboard_is_down(&chip8->keyboard, chip8->registers.V[x]))
                        {
                            chip8->registers.PC += 2;
                        }
                        break;
                } /* End of switch statement */
            } /* End of scope */
            break;

        /* Opcodes for 0xF000 instruction set */
        case 0xF000:
            {
                switch (opcode & 0x00ff)
                {
                    /* Fx07 : Set Vx = delay timer value */
                    case 0x07:
                        chip8->registers.V[x] = chip8->registers.delay_timer;
                        break;

                    /* Fx0A : Wait for a key press, store the value of the key in Vx */
                    case 0x0A:
                        {
                            char pressed_key = chip8_wait_key_press(chip8);
                            chip8->registers.V[x] = pressed_key;
                        }
                        break;

                    /* Fx15 : Set delay timer = Vx */
                    case 0x15:
                        chip8->registers.delay_timer = chip8->registers.V[x];
                        break;

                    /* Fx18 : Set the sound timer = Vx */
                    case 0x18:
                        chip8->registers.sound_timer = chip8->registers.V[x];
                        break;

                    /* Fx1E : Set I = I + Vx */
                    case 0x1e:
                        chip8->registers.I += chip8->registers.V[x];
                        break;

                    /* Fx29 : Set I = location of sprite for digit Vx */
                    case 0x29:
                        chip8->registers.I = chip8->registers.V[x] * CHIP8_DEFAULT_SPRITE_HEIGHT;
                        break;

                    /* Fx33 : Store BCD representation of Vx in memory locations I, I+1, and I+2 */
                    case 0x33:
                        {
                            unsigned char hundreds = chip8->registers.V[x] / 100;
                            unsigned char tens = chip8->registers.V[x] / 10 % 10;
                            unsigned char units = chip8->registers.V[x] % 10;

                            chip8_memory_set(&chip8->memory, chip8->registers.I, hundreds);
                            chip8_memory_set(&chip8->memory, chip8->registers.I+1, tens);
                            chip8_memory_set(&chip8->memory, chip8->registers.I+2, units);
                        }
                        break;

                    /* Fx55 : Store the registers V0 through Vx in memory starting at location I */
                    case 0x55:
                        for (int i = 0; i <= x; i++) 
                        {
                            chip8_memory_set(&chip8->memory, chip8->registers.I+i, chip8->registers.V[i]);        
                        } /* End of for loop */
                        break;

                    /* Fx65 : Read registers V0 through Vx from memory starting at location I */
                    case 0x65:
                        for (int i = 0; i <= x; i++) 
                        {
                            chip8->registers.V[i] = chip8_memory_get(&chip8->memory, chip8->registers.I+i);
                        } /* End of for loop */
                        break;
                } /* End of switch statement */
            } /* End of scope */
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
