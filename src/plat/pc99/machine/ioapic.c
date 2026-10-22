/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>

#ଅନ୍ତର୍ଭାଵ <linker.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/hardware.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/ioapic.h>

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

#define IOREDTBL_ତଳ(reg) (IOAPIC_REG_IOREDTBL + (reg) * 2)
#define IOREDTBL_ଉପର(reg) (IOREDTBL_ତଳ(reg) + 1)

#define IOREDTBL_ତଳ_INTERRUPT_MASK BIT(16)
#define IOREDTBL_ତଳ_TRIGGER_MODE_LEVEL BIT(15)
#define IOREDTBL_ତଳ_TRIGGER_MODE_SHIFT    15
#define IOREDTBL_ତଳ_POLARITY_ତଳ BIT(13)
#define IOREDTBL_ତଳ_POLARITY_SHIFT         13
#define IOREDTBL_ତଳ_DEST_MODE_LOGCIAL BIT(11)

#define IOAPICID_ID_BITS 4
#define IOAPICID_ID_OFFSET 24

#define IOREDTBL_ଉପର_RESERVED_BITS 24

/*
 * Cache what we believe is in the low word of the IOREDTBL. This
 * has all the state of trigger modes etc etc.
 * IOAPIC_IRQ_LINES is the upper bound on the number of lines
 * per IOAPIC.
 */
ସ୍ଥିର uint32_t ioredtbl_state[IOAPIC_IRQ_ଧାଡ଼ିଗୁଡ଼ିକ * MAX(1, CONFIG_MAX_ଅଙ୍କ_IOAPIC)];
/*
 * The number of IRQ lines for each IOAPIC
 */
ସ୍ଥିର uint8_t ioapic_nirqs[MAX(1, CONFIG_MAX_ଅଙ୍କ_IOAPIC)];

/* Number of IOAPICs in the system */
ସ୍ଥିର uint32_t ଅଙ୍କ_ioapics = 0;

ସ୍ଥିର ଶୂନ୍ୟ ioapic_write(uint32_t ioapic, word_t reg, uint32_t ମୂଲ୍ୟ)
{
    *(ଅସ୍ଥିର uint32_t *)((word_t)(PPTR_IOAPIC_START + ioapic * BIT(PAGE_BITS)) + reg) = ମୂଲ୍ୟ;
}

ସ୍ଥିର uint32_t ioapic_read(uint32_t ioapic, word_t reg)
{
    ଫେରନ୍ତୁ *(ଅସ୍ଥିର uint32_t *)((word_t)(PPTR_IOAPIC_START + ioapic * BIT(PAGE_BITS)) + reg);
}

ସ୍ଥିର ଶୂନ୍ୟ single_ioapic_init(word_t ioapic)
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
    ଯଦି (nirqs > IOAPIC_IRQ_ଧାଡ଼ିଗୁଡ଼ିକ) {
        userError("%s: ioapic %lu has %u IRQs,\n"
                  "which is greater than the max handled (%u)\n",
                  __func__, ioapic, nirqs, IOAPIC_IRQ_ଧାଡ଼ିଗୁଡ଼ିକ);
        halt();
    }

    /* Mask all the IRQs. In doing so we happen to set
     * the vector to 0, which we can assert against in
     * mask_interrupt to ensure a vector is assigned
     * before we unmask */
    ପାଇଁ (i = 0; i < nirqs; i++) {
        /* Send to ioapic_target_cpu, mask, and set 0 vector */
        ioapic_map_pin_to_vector(ioapic, i, 0, 0, 0);
    }
}

ସ୍ଥିର  cpu_id_t ioapic_target_cpu = 0;
ଶୂନ୍ୟ ioapic_init(uint32_t ଅଙ୍କ_nodes, cpu_id_t *cpu_list, uint32_t ଅଙ୍କ_ioapic)
{
    uint32_t ioapic;
    ଅଙ୍କ_ioapics = ଅଙ୍କ_ioapic;
    ioapic_target_cpu = cpu_list[0];

    ପାଇଁ (ioapic = 0; ioapic < ଅଙ୍କ_ioapic; ioapic++) {
        /* Init this ioapic */
        single_ioapic_init(ioapic);
    }
}

ଶୂନ୍ୟ ioapic_mask(bool_t mask, uint32_t ioapic, uint32_t pin)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = ioapic * IOAPIC_IRQ_ଧାଡ଼ିଗୁଡ଼ିକ + pin;
    ଯଦି (ioapic >= ଅଙ୍କ_ioapics || pin >= ioapic_nirqs[ioapic]) {
        /* silently ignore requests to non existent parts of the interrupt space */
        ଫେରନ୍ତୁ;
    }
    ଯଦି (mask) {
        ioredtbl_state[index] |= IOREDTBL_ତଳ_INTERRUPT_MASK;
    } ଅନ୍ୟଥା {
        ioredtbl_state[index] &= ~IOREDTBL_ତଳ_INTERRUPT_MASK;
        /* it should not be possible to be unmasking an interrupt, without
         * it having been mapped to a vector, assert that this is the case */
        ପ୍ରତିଜ୍ଞା((ioredtbl_state[index] & 0xff) != 0);
    }
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_ତଳ(pin));
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_state[index]);
}

exception_t ioapic_decode_map_pin_to_vector(word_t ioapic, word_t pin, word_t level,
                                            word_t polarity, word_t vector)
{
    ଯଦି (ଅଙ୍କ_ioapics == 0) {
        userError("System has no IOAPICs");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଯଦି (ioapic >= ଅଙ୍କ_ioapics) {
        userError("Invalid IOAPIC %ld, only have %ld", (long)ioapic, (long)ଅଙ୍କ_ioapics);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ଅଙ୍କ_ioapics - 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଯଦି (pin >= ioapic_nirqs[ioapic]) {
        userError("Invalid IOAPIC pin %ld, there are %d pins", (long)pin, ioapic_nirqs[ioapic]);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ioapic_nirqs[ioapic] - 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଯଦି (level != 0 && level != 1) {
        userError("Level should be 0 or 1, not %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)level);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଯଦି (polarity != 0 && polarity != 1) {
        userError("Polarity should be 0 or 1, not %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)polarity);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ଶୂନ୍ୟ ioapic_map_pin_to_vector(word_t ioapic, word_t pin, word_t level,
                              word_t polarity, word_t vector)
{
    uint32_t ioredtbl_ଉପର = 0;
    uint32_t index = 0;

    index = ioapic * IOAPIC_IRQ_ଧାଡ଼ିଗୁଡ଼ିକ + pin;
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_ଉପର(pin));
    ioredtbl_ଉପର = ioapic_read(ioapic, IOAPIC_WINDOW) & MASK(IOREDTBL_ଉପର_RESERVED_BITS);
    /* delivery mode: physical mode only, using APIC ID */
    ioredtbl_ଉପର |= (ioapic_target_cpu << IOREDTBL_ଉପର_RESERVED_BITS);
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_ଉପର);
    /* we do not need to add IRQ_INT_OFFSET to the vector here */
    ioredtbl_state[index] = IOREDTBL_ତଳ_INTERRUPT_MASK |
                            (level << IOREDTBL_ତଳ_TRIGGER_MODE_SHIFT) |
                            (polarity << IOREDTBL_ତଳ_POLARITY_SHIFT) |
                            vector;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_ତଳ(pin));
    /* the upper 16 bits are reserved */
    ioredtbl_state[index] |= ioapic_read(ioapic, IOAPIC_WINDOW) & ~MASK(16);
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_state[index]);
}
