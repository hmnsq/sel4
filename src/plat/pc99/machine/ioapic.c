/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>

#समावेश <linker.h>
#समावेश <machine/io.h>
#समावेश <plat/machine/hardware.h>
#समावेश <plat/machine/ioapic.h>

#घोषणा IOAPIC_REGSEL 0x00
#घोषणा IOAPIC_WINDOW 0x10

#घोषणा IOAPIC_REG_IOAPICID 0x00
#घोषणा IOAPIC_REG_IOAPICVER 0x01
#घोषणा IOAPIC_REG_IOREDTBL 0x10

/*
 * Maximum Redirection Entry, bits 23:16 in IOAPIC_REG_IOAPICVER -- corresponds
 * to number of interrupt input pins for the IOAPIC minus one.
 */
#घोषणा IOAPICVER_MRE_MASK 0xff0000

#घोषणा IOREDTBL_निचला(reg) (IOAPIC_REG_IOREDTBL + (reg) * 2)
#घोषणा IOREDTBL_ऊपरी(reg) (IOREDTBL_निचला(reg) + 1)

#घोषणा IOREDTBL_निचला_INTERRUPT_MASK BIT(16)
#घोषणा IOREDTBL_निचला_TRIGGER_MODE_LEVEL BIT(15)
#घोषणा IOREDTBL_निचला_TRIGGER_MODE_SHIFT    15
#घोषणा IOREDTBL_निचला_POLARITY_निचला BIT(13)
#घोषणा IOREDTBL_निचला_POLARITY_SHIFT         13
#घोषणा IOREDTBL_निचला_DEST_MODE_LOGCIAL BIT(11)

#घोषणा IOAPICID_ID_BITS 4
#घोषणा IOAPICID_ID_OFFSET 24

#घोषणा IOREDTBL_ऊपरी_RESERVED_BITS 24

/*
 * Cache what we believe is in the low word of the IOREDTBL. This
 * has all the state of trigger modes etc etc.
 * IOAPIC_IRQ_LINES is the upper bound on the number of lines
 * per IOAPIC.
 */
अटल uint32_t ioredtbl_state[IOAPIC_IRQ_पंक्तियाँ * MAX(1, CONFIG_MAX_अंक_IOAPIC)];
/*
 * The number of IRQ lines for each IOAPIC
 */
अटल uint8_t ioapic_nirqs[MAX(1, CONFIG_MAX_अंक_IOAPIC)];

/* Number of IOAPICs in the system */
अटल uint32_t अंक_ioapics = 0;

अटल व्योम ioapic_write(uint32_t ioapic, word_t reg, uint32_t मूल्य)
{
    *(अस्थिर uint32_t *)((word_t)(PPTR_IOAPIC_START + ioapic * BIT(PAGE_BITS)) + reg) = मूल्य;
}

अटल uint32_t ioapic_read(uint32_t ioapic, word_t reg)
{
    वापस *(अस्थिर uint32_t *)((word_t)(PPTR_IOAPIC_START + ioapic * BIT(PAGE_BITS)) + reg);
}

अटल व्योम single_ioapic_init(word_t ioapic)
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
    अगर (nirqs > IOAPIC_IRQ_पंक्तियाँ) {
        userError("%s: ioapic %lu has %u IRQs,\n"
                  "which is greater than the max handled (%u)\n",
                  __func__, ioapic, nirqs, IOAPIC_IRQ_पंक्तियाँ);
        halt();
    }

    /* Mask all the IRQs. In doing so we happen to set
     * the vector to 0, which we can assert against in
     * mask_interrupt to ensure a vector is assigned
     * before we unmask */
    क्रम (i = 0; i < nirqs; i++) {
        /* Send to ioapic_target_cpu, mask, and set 0 vector */
        ioapic_map_pin_to_vector(ioapic, i, 0, 0, 0);
    }
}

अटल  cpu_id_t ioapic_target_cpu = 0;
व्योम ioapic_init(uint32_t अंक_nodes, cpu_id_t *cpu_list, uint32_t अंक_ioapic)
{
    uint32_t ioapic;
    अंक_ioapics = अंक_ioapic;
    ioapic_target_cpu = cpu_list[0];

    क्रम (ioapic = 0; ioapic < अंक_ioapic; ioapic++) {
        /* Init this ioapic */
        single_ioapic_init(ioapic);
    }
}

व्योम ioapic_mask(bool_t mask, uint32_t ioapic, uint32_t pin)
{
    पूर्णांक सूचकांक = ioapic * IOAPIC_IRQ_पंक्तियाँ + pin;
    अगर (ioapic >= अंक_ioapics || pin >= ioapic_nirqs[ioapic]) {
        /* silently ignore requests to non existent parts of the interrupt space */
        वापस;
    }
    अगर (mask) {
        ioredtbl_state[सूचकांक] |= IOREDTBL_निचला_INTERRUPT_MASK;
    } अन्यथा {
        ioredtbl_state[सूचकांक] &= ~IOREDTBL_निचला_INTERRUPT_MASK;
        /* it should not be possible to be unmasking an interrupt, without
         * it having been mapped to a vector, assert that this is the case */
        निश्चित((ioredtbl_state[सूचकांक] & 0xff) != 0);
    }
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_निचला(pin));
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_state[सूचकांक]);
}

exception_t ioapic_decode_map_pin_to_vector(word_t ioapic, word_t pin, word_t level,
                                            word_t polarity, word_t vector)
{
    अगर (अंक_ioapics == 0) {
        userError("System has no IOAPICs");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    अगर (ioapic >= अंक_ioapics) {
        userError("Invalid IOAPIC %ld, only have %ld", (दीर्घ)ioapic, (दीर्घ)अंक_ioapics);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = अंक_ioapics - 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    अगर (pin >= ioapic_nirqs[ioapic]) {
        userError("Invalid IOAPIC pin %ld, there are %d pins", (दीर्घ)pin, ioapic_nirqs[ioapic]);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ioapic_nirqs[ioapic] - 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (level != 0 && level != 1) {
        userError("Level should be 0 or 1, not %d", (पूर्णांक)level);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    अगर (polarity != 0 && polarity != 1) {
        userError("Polarity should be 0 or 1, not %d", (पूर्णांक)polarity);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    वापस EXCEPTION_NONE;
}

व्योम ioapic_map_pin_to_vector(word_t ioapic, word_t pin, word_t level,
                              word_t polarity, word_t vector)
{
    uint32_t ioredtbl_ऊपरी = 0;
    uint32_t सूचकांक = 0;

    सूचकांक = ioapic * IOAPIC_IRQ_पंक्तियाँ + pin;
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_ऊपरी(pin));
    ioredtbl_ऊपरी = ioapic_read(ioapic, IOAPIC_WINDOW) & MASK(IOREDTBL_ऊपरी_RESERVED_BITS);
    /* delivery mode: physical mode only, using APIC ID */
    ioredtbl_ऊपरी |= (ioapic_target_cpu << IOREDTBL_ऊपरी_RESERVED_BITS);
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_ऊपरी);
    /* we do not need to add IRQ_INT_OFFSET to the vector here */
    ioredtbl_state[सूचकांक] = IOREDTBL_निचला_INTERRUPT_MASK |
                            (level << IOREDTBL_निचला_TRIGGER_MODE_SHIFT) |
                            (polarity << IOREDTBL_निचला_POLARITY_SHIFT) |
                            vector;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_निचला(pin));
    /* the upper 16 bits are reserved */
    ioredtbl_state[सूचकांक] |= ioapic_read(ioapic, IOAPIC_WINDOW) & ~MASK(16);
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_state[सूचकांक]);
}
