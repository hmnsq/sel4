/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/ioapic.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOAPIC_REGSEL 0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOAPIC_ਖਿੜਕੀ 0x10

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOAPIC_REG_IOAPICID 0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOAPIC_REG_IOAPICVER 0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOAPIC_REG_IOREDTBL 0x10

/*
 * Maximum Redirection Entry, bits 23:16 in IOAPIC_REG_IOAPICVER -- corresponds
 * to number of interrupt input pins for the IOAPIC minus one.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOAPICVER_MRE_ਨਕਾਬ 0xff0000

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOREDTBL_ਨੀਵਾਂ(reg) (IOAPIC_REG_IOREDTBL + (reg) * 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOREDTBL_ਉੱਚਾ(reg) (IOREDTBL_ਨੀਵਾਂ(reg) + 1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOREDTBL_ਨੀਵਾਂ_ਰੁਕਾਵਟ_ਨਕਾਬ ਦੋਆਧਾਰੀ_ਅੰਕ(16)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOREDTBL_ਨੀਵਾਂ_TRIGGER_ਬਹੁਲਕ_LEVEL ਦੋਆਧਾਰੀ_ਅੰਕ(15)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOREDTBL_ਨੀਵਾਂ_TRIGGER_ਬਹੁਲਕ_ਖਿਸਕਾਓ    15
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOREDTBL_ਨੀਵਾਂ_POLARITY_ਨੀਵਾਂ ਦੋਆਧਾਰੀ_ਅੰਕ(13)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOREDTBL_ਨੀਵਾਂ_POLARITY_ਖਿਸਕਾਓ         13
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOREDTBL_ਨੀਵਾਂ_DEST_ਬਹੁਲਕ_LOGCIAL ਦੋਆਧਾਰੀ_ਅੰਕ(11)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOAPICID_ID_ਬਿੱਟ 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOAPICID_ID_ਵਿੱਥ 24

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOREDTBL_ਉੱਚਾ_RESERVED_ਬਿੱਟ 24

/*
 * Cache what we believe is in the low word of the IOREDTBL. This
 * has all the state of trigger modes etc etc.
 * IOAPIC_IRQ_LINES is the upper bound on the number of lines
 * per IOAPIC.
 */
ਸਥਿਰ uint32_t ioredtbl_state[IOAPIC_IRQ_ਸਤਰਾਂ * MAX(1, CONFIG_MAX_ਅੰਕ_IOAPIC)];
/*
 * The number of IRQ lines for each IOAPIC
 */
ਸਥਿਰ uint8_t ioapic_nirqs[MAX(1, CONFIG_MAX_ਅੰਕ_IOAPIC)];

/* Number of IOAPICs in the system */
ਸਥਿਰ uint32_t ਅੰਕ_ioapics = 0;

ਸਥਿਰ ਖਾਲੀ ioapic_write(uint32_t ioapic, ਸ਼ਬਦ_t reg, uint32_t ਮੁੱਲ)
{
    *(ਅਸਥਿਰ uint32_t *)((ਸ਼ਬਦ_t)(PPTR_IOAPIC_ਸ਼ੁਰੂ + ioapic * ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ)) + reg) = ਮੁੱਲ;
}

ਸਥਿਰ uint32_t ioapic_read(uint32_t ioapic, ਸ਼ਬਦ_t reg)
{
    ਵਾਪਸ *(ਅਸਥਿਰ uint32_t *)((ਸ਼ਬਦ_t)(PPTR_IOAPIC_ਸ਼ੁਰੂ + ioapic * ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ)) + reg);
}

ਸਥਿਰ ਖਾਲੀ single_ioapic_init(ਸ਼ਬਦ_t ioapic)
{
    uint32_t i;
    uint32_t nirqs;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOAPIC_REG_IOAPICVER);
    nirqs = ((ioapic_read(ioapic, IOAPIC_ਖਿੜਕੀ) & IOAPICVER_MRE_ਨਕਾਬ) >> 16) + 1;
    ioapic_nirqs[ioapic] = nirqs;

    /*
     * The MRE field of the IOAPICVER register is supposed to only be able to
     * return a max value of 239, which means a max nirqs value of IOAPIC_IRQ_LINES.
     */
    ਜੇ (nirqs > IOAPIC_IRQ_ਸਤਰਾਂ) {
        userError("%s: ioapic %lu has %u IRQs,\n"
                  "which is greater than the max handled (%u)\n",
                  __func__, ioapic, nirqs, IOAPIC_IRQ_ਸਤਰਾਂ);
        halt();
    }

    /* Mask all the IRQs. In doing so we happen to set
     * the vector to 0, which we can assert against in
     * mask_interrupt to ensure a vector is assigned
     * before we unmask */
    ਲਈ (i = 0; i < nirqs; i++) {
        /* Send to ioapic_target_cpu, mask, and set 0 vector */
        ioapic_map_pin_ਮੰਜ਼ਿਲ_ਸਦਿਸ਼(ioapic, i, 0, 0, 0);
    }
}

ਸਥਿਰ  cpu_id_t ioapic_ਨਿਸ਼ਾਨਾ_cpu = 0;
ਖਾਲੀ ioapic_init(uint32_t ਅੰਕ_nodes, cpu_id_t *cpu_list, uint32_t ਅੰਕ_ioapic)
{
    uint32_t ioapic;
    ਅੰਕ_ioapics = ਅੰਕ_ioapic;
    ioapic_ਨਿਸ਼ਾਨਾ_cpu = cpu_list[0];

    ਲਈ (ioapic = 0; ioapic < ਅੰਕ_ioapic; ioapic++) {
        /* Init this ioapic */
        single_ioapic_init(ioapic);
    }
}

ਖਾਲੀ ioapic_ਨਕਾਬ(bool_t ਨਕਾਬ_2, uint32_t ioapic, uint32_t pin)
{
    ਪੂਰਨ ਸੂਚਕਾਂਕ = ioapic * IOAPIC_IRQ_ਸਤਰਾਂ + pin;
    ਜੇ (ioapic >= ਅੰਕ_ioapics || pin >= ioapic_nirqs[ioapic]) {
        /* silently ignore requests to non existent parts of the interrupt space */
        ਵਾਪਸ;
    }
    ਜੇ (ਨਕਾਬ_2) {
        ioredtbl_state[ਸੂਚਕਾਂਕ] |= IOREDTBL_ਨੀਵਾਂ_ਰੁਕਾਵਟ_ਨਕਾਬ;
    } ਨਹੀਂਤਾਂ {
        ioredtbl_state[ਸੂਚਕਾਂਕ] &= ~IOREDTBL_ਨੀਵਾਂ_ਰੁਕਾਵਟ_ਨਕਾਬ;
        /* it should not be possible to be unmasking an interrupt, without
         * it having been mapped to a vector, assert that this is the case */
        ਪ੍ਰਤਿਜ੍ਞਾ((ioredtbl_state[ਸੂਚਕਾਂਕ] & 0xff) != 0);
    }
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_ਨੀਵਾਂ(pin));
    ioapic_write(ioapic, IOAPIC_ਖਿੜਕੀ, ioredtbl_state[ਸੂਚਕਾਂਕ]);
}

exception_t ioapic_decode_map_pin_ਮੰਜ਼ਿਲ_ਸਦਿਸ਼(ਸ਼ਬਦ_t ioapic, ਸ਼ਬਦ_t pin, ਸ਼ਬਦ_t level,
                                            ਸ਼ਬਦ_t polarity, ਸ਼ਬਦ_t ਸਦਿਸ਼)
{
    ਜੇ (ਅੰਕ_ioapics == 0) {
        userError("System has no IOAPICs");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (ioapic >= ਅੰਕ_ioapics) {
        userError("Invalid IOAPIC %ld, only have %ld", (ਲੰਮਾ)ioapic, (ਲੰਮਾ)ਅੰਕ_ioapics);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ਅੰਕ_ioapics - 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (pin >= ioapic_nirqs[ioapic]) {
        userError("Invalid IOAPIC pin %ld, there are %d pins", (ਲੰਮਾ)pin, ioapic_nirqs[ioapic]);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ioapic_nirqs[ioapic] - 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (level != 0 && level != 1) {
        userError("Level should be 0 or 1, not %d", (ਪੂਰਨ)level);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (polarity != 0 && polarity != 1) {
        userError("Polarity should be 0 or 1, not %d", (ਪੂਰਨ)polarity);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਵਾਪਸ EXCEPTION_NONE;
}

ਖਾਲੀ ioapic_map_pin_ਮੰਜ਼ਿਲ_ਸਦਿਸ਼(ਸ਼ਬਦ_t ioapic, ਸ਼ਬਦ_t pin, ਸ਼ਬਦ_t level,
                              ਸ਼ਬਦ_t polarity, ਸ਼ਬਦ_t ਸਦਿਸ਼)
{
    uint32_t ioredtbl_ਉੱਚਾ = 0;
    uint32_t ਸੂਚਕਾਂਕ = 0;

    ਸੂਚਕਾਂਕ = ioapic * IOAPIC_IRQ_ਸਤਰਾਂ + pin;
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_ਉੱਚਾ(pin));
    ioredtbl_ਉੱਚਾ = ioapic_read(ioapic, IOAPIC_ਖਿੜਕੀ) & ਨਕਾਬ(IOREDTBL_ਉੱਚਾ_RESERVED_ਬਿੱਟ);
    /* delivery mode: physical mode only, using APIC ID */
    ioredtbl_ਉੱਚਾ |= (ioapic_ਨਿਸ਼ਾਨਾ_cpu << IOREDTBL_ਉੱਚਾ_RESERVED_ਬਿੱਟ);
    ioapic_write(ioapic, IOAPIC_ਖਿੜਕੀ, ioredtbl_ਉੱਚਾ);
    /* we do not need to add IRQ_INT_OFFSET to the vector here */
    ioredtbl_state[ਸੂਚਕਾਂਕ] = IOREDTBL_ਨੀਵਾਂ_ਰੁਕਾਵਟ_ਨਕਾਬ |
                            (level << IOREDTBL_ਨੀਵਾਂ_TRIGGER_ਬਹੁਲਕ_ਖਿਸਕਾਓ) |
                            (polarity << IOREDTBL_ਨੀਵਾਂ_POLARITY_ਖਿਸਕਾਓ) |
                            ਸਦਿਸ਼;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_ਨੀਵਾਂ(pin));
    /* the upper 16 bits are reserved */
    ioredtbl_state[ਸੂਚਕਾਂਕ] |= ioapic_read(ioapic, IOAPIC_ਖਿੜਕੀ) & ~ਨਕਾਬ(16);
    ioapic_write(ioapic, IOAPIC_ਖਿੜਕੀ, ioredtbl_state[ਸੂਚਕਾਂਕ]);
}
