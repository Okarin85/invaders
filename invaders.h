#pragma once

#include "types.h"
#include "i8080.h"

static const u16 CYCLES_PER_FRAME = 2000000 / 60; // 2Mhz at 60 fps
static const u16 HALF_CYCLES_PER_FRAME = CYCLES_PER_FRAME / 2;

typedef struct invaders {
    i8080 cpu;

    u8 port1, port2;
    u8 shift0, shift1, shift_offset;

    u8 next_interrupt;
} invaders;

void invaders_init(invaders* const si);
u8 invaders_step(invaders* const si);
void invaders_update(invaders* const si);
