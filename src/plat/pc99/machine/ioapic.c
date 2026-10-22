/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>

#અન્તર્ભાવ <linker.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <plat/machine/hardware.h>
#અન્તર્ભાવ <plat/machine/ioapic.h>

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

#define IOREDTBL_નીચલું(reg) (IOAPIC_REG_IOREDTBL + (reg) * 2)
#define IOREDTBL_ઉપલું(reg) (IOREDTBL_નીચલું(reg) + 1)

#define IOREDTBL_નીચલું_INTERRUPT_MASK BIT(16)
#define IOREDTBL_નીચલું_TRIGGER_MODE_LEVEL BIT(15)
#define IOREDTBL_નીચલું_TRIGGER_MODE_SHIFT    15
#define IOREDTBL_નીચલું_POLARITY_નીચલું BIT(13)
#define IOREDTBL_નીચલું_POLARITY_SHIFT         13
#define IOREDTBL_નીચલું_DEST_MODE_LOGCIAL BIT(11)

#define IOAPICID_ID_BITS 4
#define IOAPICID_ID_OFFSET 24

#define IOREDTBL_ઉપલું_RESERVED_BITS 24

/*
 * Cache what we believe is in the low word of the IOREDTBL. This
 * has all the state of trigger modes etc etc.
 * IOAPIC_IRQ_LINES is the upper bound on the number of lines
 * per IOAPIC.
 */
સ્થિર uint32_t ioredtbl_state[IOAPIC_IRQ_લીટીઓ * MAX(1, CONFIG_MAX_અંક_IOAPIC)];
/*
 * The number of IRQ lines for each IOAPIC
 */
સ્થિર uint8_t ioapic_nirqs[MAX(1, CONFIG_MAX_અંક_IOAPIC)];

/* Number of IOAPICs in the system */
સ્થિર uint32_t અંક_ioapics = 0;

સ્થિર શૂન્ય ioapic_write(uint32_t ioapic, word_t reg, uint32_t કિંમત)
{
    *(અસ્થિર uint32_t *)((word_t)(PPTR_IOAPIC_START + ioapic * BIT(PAGE_BITS)) + reg) = કિંમત;
}

સ્થિર uint32_t ioapic_read(uint32_t ioapic, word_t reg)
{
    પાછા *(અસ્થિર uint32_t *)((word_t)(PPTR_IOAPIC_START + ioapic * BIT(PAGE_BITS)) + reg);
}

સ્થિર શૂન્ય single_ioapic_init(word_t ioapic)
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
    જો (nirqs > IOAPIC_IRQ_લીટીઓ) {
        userError("%s: ioapic %lu has %u IRQs,\n"
                  "which is greater than the max handled (%u)\n",
                  __func__, ioapic, nirqs, IOAPIC_IRQ_લીટીઓ);
        halt();
    }

    /* Mask all the IRQs. In doing so we happen to set
     * the vector to 0, which we can assert against in
     * mask_interrupt to ensure a vector is assigned
     * before we unmask */
    માટે (i = 0; i < nirqs; i++) {
        /* Send to ioapic_target_cpu, mask, and set 0 vector */
        ioapic_map_pin_to_vector(ioapic, i, 0, 0, 0);
    }
}

સ્થિર  cpu_id_t ioapic_target_cpu = 0;
શૂન્ય ioapic_init(uint32_t અંક_nodes, cpu_id_t *cpu_list, uint32_t અંક_ioapic)
{
    uint32_t ioapic;
    અંક_ioapics = અંક_ioapic;
    ioapic_target_cpu = cpu_list[0];

    માટે (ioapic = 0; ioapic < અંક_ioapic; ioapic++) {
        /* Init this ioapic */
        single_ioapic_init(ioapic);
    }
}

શૂન્ય ioapic_mask(bool_t mask, uint32_t ioapic, uint32_t pin)
{
    પૂર્ણાંક index = ioapic * IOAPIC_IRQ_લીટીઓ + pin;
    જો (ioapic >= અંક_ioapics || pin >= ioapic_nirqs[ioapic]) {
        /* silently ignore requests to non existent parts of the interrupt space */
        પાછા;
    }
    જો (mask) {
        ioredtbl_state[index] |= IOREDTBL_નીચલું_INTERRUPT_MASK;
    } નહીંતર {
        ioredtbl_state[index] &= ~IOREDTBL_નીચલું_INTERRUPT_MASK;
        /* it should not be possible to be unmasking an interrupt, without
         * it having been mapped to a vector, assert that this is the case */
        પ્રતિજ્ઞા((ioredtbl_state[index] & 0xff) != 0);
    }
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_નીચલું(pin));
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_state[index]);
}

exception_t ioapic_decode_map_pin_to_vector(word_t ioapic, word_t pin, word_t level,
                                            word_t polarity, word_t vector)
{
    જો (અંક_ioapics == 0) {
        userError("System has no IOAPICs");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    જો (ioapic >= અંક_ioapics) {
        userError("Invalid IOAPIC %ld, only have %ld", (long)ioapic, (long)અંક_ioapics);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = અંક_ioapics - 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    જો (pin >= ioapic_nirqs[ioapic]) {
        userError("Invalid IOAPIC pin %ld, there are %d pins", (long)pin, ioapic_nirqs[ioapic]);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ioapic_nirqs[ioapic] - 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (level != 0 && level != 1) {
        userError("Level should be 0 or 1, not %d", (પૂર્ણાંક)level);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    જો (polarity != 0 && polarity != 1) {
        userError("Polarity should be 0 or 1, not %d", (પૂર્ણાંક)polarity);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    પાછા EXCEPTION_NONE;
}

શૂન્ય ioapic_map_pin_to_vector(word_t ioapic, word_t pin, word_t level,
                              word_t polarity, word_t vector)
{
    uint32_t ioredtbl_ઉપલું = 0;
    uint32_t index = 0;

    index = ioapic * IOAPIC_IRQ_લીટીઓ + pin;
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_ઉપલું(pin));
    ioredtbl_ઉપલું = ioapic_read(ioapic, IOAPIC_WINDOW) & MASK(IOREDTBL_ઉપલું_RESERVED_BITS);
    /* delivery mode: physical mode only, using APIC ID */
    ioredtbl_ઉપલું |= (ioapic_target_cpu << IOREDTBL_ઉપલું_RESERVED_BITS);
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_ઉપલું);
    /* we do not need to add IRQ_INT_OFFSET to the vector here */
    ioredtbl_state[index] = IOREDTBL_નીચલું_INTERRUPT_MASK |
                            (level << IOREDTBL_નીચલું_TRIGGER_MODE_SHIFT) |
                            (polarity << IOREDTBL_નીચલું_POLARITY_SHIFT) |
                            vector;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_નીચલું(pin));
    /* the upper 16 bits are reserved */
    ioredtbl_state[index] |= ioapic_read(ioapic, IOAPIC_WINDOW) & ~MASK(16);
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_state[index]);
}
