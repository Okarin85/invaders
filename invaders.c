#include "invaders.h"

void invaders_init(invaders* const si) {
    cpu_init(&si->cpu);

    si->port1 = 0;
    si->port2 = 0;
    si->next_interrupt = 0x08;

    si->shift0 = 0;
    si->shift1 = 0;
    si->shift_offset = 0;
}

u8 invaders_step(invaders* const si) {
    // Space Invaders specific INT/OUT opcodes
    const u8 opcode = mmu_rb(&si->cpu, si->cpu.pc);
    if (opcode == 0xDB) { // IN
        mmu_next_byte(&si->cpu);
        u8 port = mmu_next_byte(&si->cpu);
        u8 value = 0;

        if (port == 1) {
            value = si->port1;
        }
        else if (port == 2) {
            value = si->port2;
        }
        else if (port == 3) {
            u16 v = (si->shift1 << 8) | si->shift0;
            value = v >> (8 - si->shift_offset);
        }
        else {
            printf("error in IN: unknown port %i\n", port);
        }
        si->cpu.reg.A = value;
    }
    else if (opcode == 0xD3) { // OUT
        // space invaders specific implementation
        mmu_next_byte(&si->cpu);
        u8 port = mmu_next_byte(&si->cpu);
        const u8 value = si->cpu.reg.A;

        if (port == 2) {
            si->shift_offset = value & 0x7;
        }
        else if (port == 3) {
            // play sound
        }
        else if (port == 4) {
            si->shift0 = si->shift1;
            si->shift1 = value;
        }
        else if (port == 5) {
            // play sound
        }
        else if (port == 6) {
            // unused port (debug port?)
        }
        else {
            printf("error in OUT: unknown port %i\n", port);
        }
    }

    cpu_step(&si->cpu);

    // interrupts handling
    if (si->cpu.cycles_count >= HALF_CYCLES_PER_FRAME) {
        if (si->cpu.iff) {
            // generate interrupt
            si->cpu.iff = 0;
            cpu_call(&si->cpu, si->next_interrupt);
        }
        si->cpu.cycles_count = si->cpu.cycles_count - HALF_CYCLES_PER_FRAME;
        si->next_interrupt = si->next_interrupt == 0x08 ? 0x10 : 0x08;
    }

    return si->cpu.cycles_this_opcode;
}

void invaders_update(invaders* const si) {
    // emulates the correct number of cycles for one frame
    // function to execute every 1/60s
    u16 count = 0;
    while (count <= CYCLES_PER_FRAME) {
        count += invaders_step(si);
    }
}
