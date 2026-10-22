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

#define IOREDTBL_निचला(reg) (IOAPIC_REG_IOREDTBL + (reg) * 2)
#define IOREDTBL_ऊपरी(reg) (IOREDTBL_निचला(reg) + 1)

#define IOREDTBL_निचला_INTERRUPT_MASK BIT(16)
#define IOREDTBL_निचला_TRIGGER_MODE_LEVEL BIT(15)
#define IOREDTBL_निचला_TRIGGER_MODE_SHIFT    15
#define IOREDTBL_निचला_POLARITY_निचला BIT(13)
#define IOREDTBL_निचला_POLARITY_SHIFT         13
#define IOREDTBL_निचला_DEST_MODE_LOGCIAL BIT(11)

#define IOAPICID_ID_BITS 4
#define IOAPICID_ID_OFFSET 24

#define IOREDTBL_ऊपरी_RESERVED_BITS 24

/*
 * Cache what we believe is in the low word of the IOREDTBL. This
 * has all the state of trigger modes etc etc.
 * IOAPIC_IRQ_LINES is the upper bound on the number of lines
 * per IOAPIC.
 */
स्थिर uint32_t ioredtbl_state[IOAPIC_IRQ_पंक्तियाँ * MAX(1, CONFIG_MAX_अंक_IOAPIC)];
/*
 * The number of IRQ lines for each IOAPIC
 */
स्थिर uint8_t ioapic_nirqs[MAX(1, CONFIG_MAX_अंक_IOAPIC)];

/* Number of IOAPICs in the system */
स्थिर uint32_t अंक_ioapics = 0;

स्थिर शून्य ioapic_write(uint32_t ioapic, word_t reg, uint32_t मूल्य)
{
    *(चञ्चल uint32_t *)((word_t)(PPTR_IOAPIC_START + ioapic * BIT(PAGE_BITS)) + reg) = मूल्य;
}

स्थिर uint32_t ioapic_read(uint32_t ioapic, word_t reg)
{
    प्रत्यागम *(चञ्चल uint32_t *)((word_t)(PPTR_IOAPIC_START + ioapic * BIT(PAGE_BITS)) + reg);
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
    यदि (nirqs > IOAPIC_IRQ_पंक्तियाँ) {
        userError("%s: ioapic %lu has %u IRQs,\n"
                  "which is greater than the max handled (%u)\n",
                  __func__, ioapic, nirqs, IOAPIC_IRQ_पंक्तियाँ);
        halt();
    }

    /* Mask all the IRQs. In doing so we happen to set
     * the vector to 0, which we can assert against in
     * mask_interrupt to ensure a vector is assigned
     * before we unmask */
    प्रति (i = 0; i < nirqs; i++) {
        /* Send to ioapic_target_cpu, mask, and set 0 vector */
        ioapic_map_pin_to_vector(ioapic, i, 0, 0, 0);
    }
}

स्थिर  cpu_id_t ioapic_target_cpu = 0;
शून्य ioapic_init(uint32_t अंक_nodes, cpu_id_t *cpu_list, uint32_t अंक_ioapic)
{
    uint32_t ioapic;
    अंक_ioapics = अंक_ioapic;
    ioapic_target_cpu = cpu_list[0];

    प्रति (ioapic = 0; ioapic < अंक_ioapic; ioapic++) {
        /* Init this ioapic */
        single_ioapic_init(ioapic);
    }
}

शून्य ioapic_mask(bool_t mask, uint32_t ioapic, uint32_t pin)
{
    पूर्णाङ्क index = ioapic * IOAPIC_IRQ_पंक्तियाँ + pin;
    यदि (ioapic >= अंक_ioapics || pin >= ioapic_nirqs[ioapic]) {
        /* silently ignore requests to non existent parts of the interrupt space */
        प्रत्यागम;
    }
    यदि (mask) {
        ioredtbl_state[index] |= IOREDTBL_निचला_INTERRUPT_MASK;
    } अन्यथा {
        ioredtbl_state[index] &= ~IOREDTBL_निचला_INTERRUPT_MASK;
        /* it should not be possible to be unmasking an interrupt, without
         * it having been mapped to a vector, assert that this is the case */
        प्रतिज्ञा((ioredtbl_state[index] & 0xff) != 0);
    }
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_निचला(pin));
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_state[index]);
}

exception_t ioapic_decode_map_pin_to_vector(word_t ioapic, word_t pin, word_t level,
                                            word_t polarity, word_t vector)
{
    यदि (अंक_ioapics == 0) {
        userError("System has no IOAPICs");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (ioapic >= अंक_ioapics) {
        userError("Invalid IOAPIC %ld, only have %ld", (long)ioapic, (long)अंक_ioapics);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = अंक_ioapics - 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (pin >= ioapic_nirqs[ioapic]) {
        userError("Invalid IOAPIC pin %ld, there are %d pins", (long)pin, ioapic_nirqs[ioapic]);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ioapic_nirqs[ioapic] - 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (level != 0 && level != 1) {
        userError("Level should be 0 or 1, not %d", (पूर्णाङ्क)level);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (polarity != 0 && polarity != 1) {
        userError("Polarity should be 0 or 1, not %d", (पूर्णाङ्क)polarity);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    प्रत्यागम EXCEPTION_NONE;
}

शून्य ioapic_map_pin_to_vector(word_t ioapic, word_t pin, word_t level,
                              word_t polarity, word_t vector)
{
    uint32_t ioredtbl_ऊपरी = 0;
    uint32_t index = 0;

    index = ioapic * IOAPIC_IRQ_पंक्तियाँ + pin;
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_ऊपरी(pin));
    ioredtbl_ऊपरी = ioapic_read(ioapic, IOAPIC_WINDOW) & MASK(IOREDTBL_ऊपरी_RESERVED_BITS);
    /* delivery mode: physical mode only, using APIC ID */
    ioredtbl_ऊपरी |= (ioapic_target_cpu << IOREDTBL_ऊपरी_RESERVED_BITS);
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_ऊपरी);
    /* we do not need to add IRQ_INT_OFFSET to the vector here */
    ioredtbl_state[index] = IOREDTBL_निचला_INTERRUPT_MASK |
                            (level << IOREDTBL_निचला_TRIGGER_MODE_SHIFT) |
                            (polarity << IOREDTBL_निचला_POLARITY_SHIFT) |
                            vector;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_निचला(pin));
    /* the upper 16 bits are reserved */
    ioredtbl_state[index] |= ioapic_read(ioapic, IOAPIC_WINDOW) & ~MASK(16);
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_state[index]);
}
