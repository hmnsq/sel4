/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>

#অন্তর্ভাৱ <linker.h>
#অন্তর্ভাৱ <machine/io.h>
#অন্তর্ভাৱ <plat/machine/hardware.h>
#অন্তর্ভাৱ <plat/machine/ioapic.h>

#define IOAPIC_REGSEL 0x00
#define IOAPIC_WINDOW 0x10

#define IOAPIC_REG_IOAPICID 0x00
#define IOAPIC_REG_IOAPICVER 0x01
#define IOAPIC_REG_IOREDTBL 0x10

/*
 * Maximum Redirection Entry, bits 23:16 in IOAPIC_REG_IOAPICVER -- corresponds
 * to number of interrupt input pins for the IOAPIC minus one.
 */
#define IOAPICVER_MRE_MASK 0xff0000

#define IOREDTBL_নিচেৰ(reg) (IOAPIC_REG_IOREDTBL + (reg) * 2)
#define IOREDTBL_উপৰেৰ(reg) (IOREDTBL_নিচেৰ(reg) + 1)

#define IOREDTBL_নিচেৰ_INTERRUPT_MASK BIT(16)
#define IOREDTBL_নিচেৰ_TRIGGER_MODE_LEVEL BIT(15)
#define IOREDTBL_নিচেৰ_TRIGGER_MODE_SHIFT    15
#define IOREDTBL_নিচেৰ_POLARITY_নিচেৰ BIT(13)
#define IOREDTBL_নিচেৰ_POLARITY_SHIFT         13
#define IOREDTBL_নিচেৰ_DEST_MODE_LOGCIAL BIT(11)

#define IOAPICID_ID_BITS 4
#define IOAPICID_ID_OFFSET 24

#define IOREDTBL_উপৰেৰ_RESERVED_BITS 24

/*
 * Cache what we believe is in the low word of the IOREDTBL. This
 * has all the state of trigger modes etc etc.
 * IOAPIC_IRQ_LINES is the upper bound on the number of lines
 * per IOAPIC.
 */
স্থির uint32_t ioredtbl_state[IOAPIC_IRQ_সাৰিগুলি * MAX(1, CONFIG_MAX_অঙ্ক_IOAPIC)];
/*
 * The number of IRQ lines for each IOAPIC
 */
স্থির uint8_t ioapic_nirqs[MAX(1, CONFIG_MAX_অঙ্ক_IOAPIC)];

/* Number of IOAPICs in the system */
স্থির uint32_t অঙ্ক_ioapics = 0;

স্থির শূন্য ioapic_write(uint32_t ioapic, word_t reg, uint32_t মান)
{
    *(চঞ্চল uint32_t *)((word_t)(PPTR_IOAPIC_START + ioapic * BIT(PAGE_BITS)) + reg) = মান;
}

স্থির uint32_t ioapic_read(uint32_t ioapic, word_t reg)
{
    ঘূৰাই *(চঞ্চল uint32_t *)((word_t)(PPTR_IOAPIC_START + ioapic * BIT(PAGE_BITS)) + reg);
}

স্থির শূন্য single_ioapic_init(word_t ioapic)
{
    uint32_t i;
    uint32_t nirqs;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOAPIC_REG_IOAPICVER);
    nirqs = ((ioapic_read(ioapic, IOAPIC_WINDOW) & IOAPICVER_MRE_MASK) >> 16) + 1;
    ioapic_nirqs[ioapic] = nirqs;

    /*
     * The MRE field of the IOAPICVER register is supposed to only be able to
     * return a max value of 239, which means a max nirqs value of IOAPIC_IRQ_LINES.
     */
    যদি (nirqs > IOAPIC_IRQ_সাৰিগুলি) {
        userError("%s: ioapic %lu has %u IRQs,\n"
                  "which is greater than the max handled (%u)\n",
                  __func__, ioapic, nirqs, IOAPIC_IRQ_সাৰিগুলি);
        halt();
    }

    /* Mask all the IRQs. In doing so we happen to set
     * the vector to 0, which we can assert against in
     * mask_interrupt to ensure a vector is assigned
     * before we unmask */
    বাবে (i = 0; i < nirqs; i++) {
        /* Send to ioapic_target_cpu, mask, and set 0 vector */
        ioapic_map_pin_to_vector(ioapic, i, 0, 0, 0);
    }
}

স্থির  cpu_id_t ioapic_target_cpu = 0;
শূন্য ioapic_init(uint32_t অঙ্ক_nodes, cpu_id_t *cpu_list, uint32_t অঙ্ক_ioapic)
{
    uint32_t ioapic;
    অঙ্ক_ioapics = অঙ্ক_ioapic;
    ioapic_target_cpu = cpu_list[0];

    বাবে (ioapic = 0; ioapic < অঙ্ক_ioapic; ioapic++) {
        /* Init this ioapic */
        single_ioapic_init(ioapic);
    }
}

শূন্য ioapic_mask(bool_t mask, uint32_t ioapic, uint32_t pin)
{
    পূর্ণাঙ্ক index = ioapic * IOAPIC_IRQ_সাৰিগুলি + pin;
    যদি (ioapic >= অঙ্ক_ioapics || pin >= ioapic_nirqs[ioapic]) {
        /* silently ignore requests to non existent parts of the interrupt space */
        ঘূৰাই;
    }
    যদি (mask) {
        ioredtbl_state[index] |= IOREDTBL_নিচেৰ_INTERRUPT_MASK;
    } অন্যথা {
        ioredtbl_state[index] &= ~IOREDTBL_নিচেৰ_INTERRUPT_MASK;
        /* it should not be possible to be unmasking an interrupt, without
         * it having been mapped to a vector, assert that this is the case */
        প্রতিজ্ঞা((ioredtbl_state[index] & 0xff) != 0);
    }
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_নিচেৰ(pin));
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_state[index]);
}

exception_t ioapic_decode_map_pin_to_vector(word_t ioapic, word_t pin, word_t level,
                                            word_t polarity, word_t vector)
{
    যদি (অঙ্ক_ioapics == 0) {
        userError("System has no IOAPICs");
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
    যদি (ioapic >= অঙ্ক_ioapics) {
        userError("Invalid IOAPIC %ld, only have %ld", (long)ioapic, (long)অঙ্ক_ioapics);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = অঙ্ক_ioapics - 1;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
    যদি (pin >= ioapic_nirqs[ioapic]) {
        userError("Invalid IOAPIC pin %ld, there are %d pins", (long)pin, ioapic_nirqs[ioapic]);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ioapic_nirqs[ioapic] - 1;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    যদি (level != 0 && level != 1) {
        userError("Level should be 0 or 1, not %d", (পূর্ণাঙ্ক)level);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
    যদি (polarity != 0 && polarity != 1) {
        userError("Polarity should be 0 or 1, not %d", (পূর্ণাঙ্ক)polarity);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
    ঘূৰাই EXCEPTION_NONE;
}

শূন্য ioapic_map_pin_to_vector(word_t ioapic, word_t pin, word_t level,
                              word_t polarity, word_t vector)
{
    uint32_t ioredtbl_উপৰেৰ = 0;
    uint32_t index = 0;

    index = ioapic * IOAPIC_IRQ_সাৰিগুলি + pin;
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_উপৰেৰ(pin));
    ioredtbl_উপৰেৰ = ioapic_read(ioapic, IOAPIC_WINDOW) & MASK(IOREDTBL_উপৰেৰ_RESERVED_BITS);
    /* delivery mode: physical mode only, using APIC ID */
    ioredtbl_উপৰেৰ |= (ioapic_target_cpu << IOREDTBL_উপৰেৰ_RESERVED_BITS);
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_উপৰেৰ);
    /* we do not need to add IRQ_INT_OFFSET to the vector here */
    ioredtbl_state[index] = IOREDTBL_নিচেৰ_INTERRUPT_MASK |
                            (level << IOREDTBL_নিচেৰ_TRIGGER_MODE_SHIFT) |
                            (polarity << IOREDTBL_নিচেৰ_POLARITY_SHIFT) |
                            vector;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_নিচেৰ(pin));
    /* the upper 16 bits are reserved */
    ioredtbl_state[index] |= ioapic_read(ioapic, IOAPIC_WINDOW) & ~MASK(16);
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_state[index]);
}
