/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#अन्तर्भाव <linker.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <plat/machine/hardware.h>
#अन्तर्भाव <plat/machine/ioapic.h>

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

#define IOREDTBL_नीचः(reg) (IOAPIC_REG_IOREDTBL + (reg) * 2)
#define IOREDTBL_उच्चः(reg) (IOREDTBL_नीचः(reg) + 1)

#define IOREDTBL_नीचः_INTERRUPT_MASK BIT(16)
#define IOREDTBL_नीचः_TRIGGER_MODE_LEVEL BIT(15)
#define IOREDTBL_नीचः_TRIGGER_MODE_SHIFT    15
#define IOREDTBL_नीचः_POLARITY_नीचः BIT(13)
#define IOREDTBL_नीचः_POLARITY_SHIFT         13
#define IOREDTBL_नीचः_DEST_MODE_LOGCIAL BIT(11)

#define IOAPICID_ID_BITS 4
#define IOAPICID_ID_OFFSET 24

#define IOREDTBL_उच्चः_RESERVED_BITS 24

/*
 * Cache what we believe is in the low word of the IOREDTBL. This
 * has all the state of trigger modes etc etc.
 * IOAPIC_IRQ_LINES is the upper bound on the number of lines
 * per IOAPIC.
 */
स्थिर uint32_t ioredtbl_state[IOAPIC_IRQ_पङ्क्तयः * MAX(1, CONFIG_MAX_अङ्कः_IOAPIC)];
/*
 * The number of IRQ lines for each IOAPIC
 */
स्थिर uint8_t ioapic_nirqs[MAX(1, CONFIG_MAX_अङ्कः_IOAPIC)];

/* Number of IOAPICs in the system */
स्थिर uint32_t अङ्कः_ioapics = 0;

स्थिर शून्य ioapic_write(uint32_t ioapic, word_t reg, uint32_t मानम्)
{
    *(चञ्चल uint32_t *)((word_t)(PPTR_IOAPIC_START + ioapic * BIT(PAGE_BITS)) + reg) = मानम्;
}

स्थिर uint32_t ioapic_read(uint32_t ioapic, word_t reg)
{
    पटिनिवत्त *(चञ्चल uint32_t *)((word_t)(PPTR_IOAPIC_START + ioapic * BIT(PAGE_BITS)) + reg);
}

स्थिर शून्य single_ioapic_init(word_t ioapic)
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
    सचे (nirqs > IOAPIC_IRQ_पङ्क्तयः) {
        userError("%s: ioapic %lu has %u IRQs,\n"
                  "which is greater than the max handled (%u)\n",
                  __func__, ioapic, nirqs, IOAPIC_IRQ_पङ्क्तयः);
        halt();
    }

    /* Mask all the IRQs. In doing so we happen to set
     * the vector to 0, which we can assert against in
     * mask_interrupt to ensure a vector is assigned
     * before we unmask */
    अत्थाय (i = 0; i < nirqs; i++) {
        /* Send to ioapic_target_cpu, mask, and set 0 vector */
        ioapic_map_pin_to_vector(ioapic, i, 0, 0, 0);
    }
}

स्थिर  cpu_id_t ioapic_target_cpu = 0;
शून्य ioapic_init(uint32_t अङ्कः_nodes, cpu_id_t *cpu_list, uint32_t अङ्कः_ioapic)
{
    uint32_t ioapic;
    अङ्कः_ioapics = अङ्कः_ioapic;
    ioapic_target_cpu = cpu_list[0];

    अत्थाय (ioapic = 0; ioapic < अङ्कः_ioapic; ioapic++) {
        /* Init this ioapic */
        single_ioapic_init(ioapic);
    }
}

शून्य ioapic_mask(bool_t mask, uint32_t ioapic, uint32_t pin)
{
    पूर्णाङ्क index = ioapic * IOAPIC_IRQ_पङ्क्तयः + pin;
    सचे (ioapic >= अङ्कः_ioapics || pin >= ioapic_nirqs[ioapic]) {
        /* silently ignore requests to non existent parts of the interrupt space */
        पटिनिवत्त;
    }
    सचे (mask) {
        ioredtbl_state[index] |= IOREDTBL_नीचः_INTERRUPT_MASK;
    } अञ्ञथा {
        ioredtbl_state[index] &= ~IOREDTBL_नीचः_INTERRUPT_MASK;
        /* it should not be possible to be unmasking an interrupt, without
         * it having been mapped to a vector, assert that this is the case */
        प्रतिज्ञा((ioredtbl_state[index] & 0xff) != 0);
    }
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_नीचः(pin));
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_state[index]);
}

exception_t ioapic_decode_map_pin_to_vector(word_t ioapic, word_t pin, word_t level,
                                            word_t polarity, word_t vector)
{
    सचे (अङ्कः_ioapics == 0) {
        userError("System has no IOAPICs");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    सचे (ioapic >= अङ्कः_ioapics) {
        userError("Invalid IOAPIC %ld, only have %ld", (long)ioapic, (long)अङ्कः_ioapics);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = अङ्कः_ioapics - 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    सचे (pin >= ioapic_nirqs[ioapic]) {
        userError("Invalid IOAPIC pin %ld, there are %d pins", (long)pin, ioapic_nirqs[ioapic]);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ioapic_nirqs[ioapic] - 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    सचे (level != 0 && level != 1) {
        userError("Level should be 0 or 1, not %d", (पूर्णाङ्क)level);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    सचे (polarity != 0 && polarity != 1) {
        userError("Polarity should be 0 or 1, not %d", (पूर्णाङ्क)polarity);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    पटिनिवत्त EXCEPTION_NONE;
}

शून्य ioapic_map_pin_to_vector(word_t ioapic, word_t pin, word_t level,
                              word_t polarity, word_t vector)
{
    uint32_t ioredtbl_उच्चः = 0;
    uint32_t index = 0;

    index = ioapic * IOAPIC_IRQ_पङ्क्तयः + pin;
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_उच्चः(pin));
    ioredtbl_उच्चः = ioapic_read(ioapic, IOAPIC_WINDOW) & MASK(IOREDTBL_उच्चः_RESERVED_BITS);
    /* delivery mode: physical mode only, using APIC ID */
    ioredtbl_उच्चः |= (ioapic_target_cpu << IOREDTBL_उच्चः_RESERVED_BITS);
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_उच्चः);
    /* we do not need to add IRQ_INT_OFFSET to the vector here */
    ioredtbl_state[index] = IOREDTBL_नीचः_INTERRUPT_MASK |
                            (level << IOREDTBL_नीचः_TRIGGER_MODE_SHIFT) |
                            (polarity << IOREDTBL_नीचः_POLARITY_SHIFT) |
                            vector;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_नीचः(pin));
    /* the upper 16 bits are reserved */
    ioredtbl_state[index] |= ioapic_read(ioapic, IOAPIC_WINDOW) & ~MASK(16);
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_state[index]);
}
