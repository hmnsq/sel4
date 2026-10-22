/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>

#அந்தர்பாவ <linker.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <plat/machine/hardware.h>
#அந்தர்பாவ <plat/machine/ioapic.h>

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

#define IOREDTBL_கீழ்(reg) (IOAPIC_REG_IOREDTBL + (reg) * 2)
#define IOREDTBL_உயர்வு(reg) (IOREDTBL_கீழ்(reg) + 1)

#define IOREDTBL_கீழ்_INTERRUPT_MASK BIT(16)
#define IOREDTBL_கீழ்_TRIGGER_MODE_LEVEL BIT(15)
#define IOREDTBL_கீழ்_TRIGGER_MODE_SHIFT    15
#define IOREDTBL_கீழ்_POLARITY_கீழ் BIT(13)
#define IOREDTBL_கீழ்_POLARITY_SHIFT         13
#define IOREDTBL_கீழ்_DEST_MODE_LOGCIAL BIT(11)

#define IOAPICID_ID_BITS 4
#define IOAPICID_ID_OFFSET 24

#define IOREDTBL_உயர்வு_RESERVED_BITS 24

/*
 * Cache what we believe is in the low word of the IOREDTBL. This
 * has all the state of trigger modes etc etc.
 * IOAPIC_IRQ_LINES is the upper bound on the number of lines
 * per IOAPIC.
 */
நிலையான uint32_t ioredtbl_state[IOAPIC_IRQ_வரிகள் * MAX(1, CONFIG_MAX_இலக்கம்_IOAPIC)];
/*
 * The number of IRQ lines for each IOAPIC
 */
நிலையான uint8_t ioapic_nirqs[MAX(1, CONFIG_MAX_இலக்கம்_IOAPIC)];

/* Number of IOAPICs in the system */
நிலையான uint32_t இலக்கம்_ioapics = 0;

நிலையான வெறுமை ioapic_write(uint32_t ioapic, word_t reg, uint32_t மதிப்பு)
{
    *(நிலையற்ற uint32_t *)((word_t)(PPTR_IOAPIC_START + ioapic * BIT(PAGE_BITS)) + reg) = மதிப்பு;
}

நிலையான uint32_t ioapic_read(uint32_t ioapic, word_t reg)
{
    திரும்பு *(நிலையற்ற uint32_t *)((word_t)(PPTR_IOAPIC_START + ioapic * BIT(PAGE_BITS)) + reg);
}

நிலையான வெறுமை single_ioapic_init(word_t ioapic)
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
    எனில் (nirqs > IOAPIC_IRQ_வரிகள்) {
        userError("%s: ioapic %lu has %u IRQs,\n"
                  "which is greater than the max handled (%u)\n",
                  __func__, ioapic, nirqs, IOAPIC_IRQ_வரிகள்);
        halt();
    }

    /* Mask all the IRQs. In doing so we happen to set
     * the vector to 0, which we can assert against in
     * mask_interrupt to ensure a vector is assigned
     * before we unmask */
    க்கு (i = 0; i < nirqs; i++) {
        /* Send to ioapic_target_cpu, mask, and set 0 vector */
        ioapic_map_pin_to_vector(ioapic, i, 0, 0, 0);
    }
}

நிலையான  cpu_id_t ioapic_target_cpu = 0;
வெறுமை ioapic_init(uint32_t இலக்கம்_nodes, cpu_id_t *cpu_list, uint32_t இலக்கம்_ioapic)
{
    uint32_t ioapic;
    இலக்கம்_ioapics = இலக்கம்_ioapic;
    ioapic_target_cpu = cpu_list[0];

    க்கு (ioapic = 0; ioapic < இலக்கம்_ioapic; ioapic++) {
        /* Init this ioapic */
        single_ioapic_init(ioapic);
    }
}

வெறுமை ioapic_mask(bool_t mask, uint32_t ioapic, uint32_t pin)
{
    முழு index = ioapic * IOAPIC_IRQ_வரிகள் + pin;
    எனில் (ioapic >= இலக்கம்_ioapics || pin >= ioapic_nirqs[ioapic]) {
        /* silently ignore requests to non existent parts of the interrupt space */
        திரும்பு;
    }
    எனில் (mask) {
        ioredtbl_state[index] |= IOREDTBL_கீழ்_INTERRUPT_MASK;
    } இல்லையெனில் {
        ioredtbl_state[index] &= ~IOREDTBL_கீழ்_INTERRUPT_MASK;
        /* it should not be possible to be unmasking an interrupt, without
         * it having been mapped to a vector, assert that this is the case */
        ப்ரதிஜ்ஞா((ioredtbl_state[index] & 0xff) != 0);
    }
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_கீழ்(pin));
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_state[index]);
}

exception_t ioapic_decode_map_pin_to_vector(word_t ioapic, word_t pin, word_t level,
                                            word_t polarity, word_t vector)
{
    எனில் (இலக்கம்_ioapics == 0) {
        userError("System has no IOAPICs");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    எனில் (ioapic >= இலக்கம்_ioapics) {
        userError("Invalid IOAPIC %ld, only have %ld", (long)ioapic, (long)இலக்கம்_ioapics);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = இலக்கம்_ioapics - 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    எனில் (pin >= ioapic_nirqs[ioapic]) {
        userError("Invalid IOAPIC pin %ld, there are %d pins", (long)pin, ioapic_nirqs[ioapic]);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ioapic_nirqs[ioapic] - 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (level != 0 && level != 1) {
        userError("Level should be 0 or 1, not %d", (முழு)level);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    எனில் (polarity != 0 && polarity != 1) {
        userError("Polarity should be 0 or 1, not %d", (முழு)polarity);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    திரும்பு EXCEPTION_NONE;
}

வெறுமை ioapic_map_pin_to_vector(word_t ioapic, word_t pin, word_t level,
                              word_t polarity, word_t vector)
{
    uint32_t ioredtbl_உயர்வு = 0;
    uint32_t index = 0;

    index = ioapic * IOAPIC_IRQ_வரிகள் + pin;
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_உயர்வு(pin));
    ioredtbl_உயர்வு = ioapic_read(ioapic, IOAPIC_WINDOW) & MASK(IOREDTBL_உயர்வு_RESERVED_BITS);
    /* delivery mode: physical mode only, using APIC ID */
    ioredtbl_உயர்வு |= (ioapic_target_cpu << IOREDTBL_உயர்வு_RESERVED_BITS);
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_உயர்வு);
    /* we do not need to add IRQ_INT_OFFSET to the vector here */
    ioredtbl_state[index] = IOREDTBL_கீழ்_INTERRUPT_MASK |
                            (level << IOREDTBL_கீழ்_TRIGGER_MODE_SHIFT) |
                            (polarity << IOREDTBL_கீழ்_POLARITY_SHIFT) |
                            vector;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_கீழ்(pin));
    /* the upper 16 bits are reserved */
    ioredtbl_state[index] |= ioapic_read(ioapic, IOAPIC_WINDOW) & ~MASK(16);
    ioapic_write(ioapic, IOAPIC_WINDOW, ioredtbl_state[index]);
}
