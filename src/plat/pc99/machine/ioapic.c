/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>

#అన్తర్భావ <linker.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <plat/machine/hardware.h>
#అన్తర్భావ <plat/machine/ioapic.h>

#నిర్వచించు IOAPIC_REGSEL 0x00
#నిర్వచించు IOAPIC_కిటికీ 0x10

#నిర్వచించు IOAPIC_REG_IOAPICID 0x00
#నిర్వచించు IOAPIC_REG_IOAPICVER 0x01
#నిర్వచించు IOAPIC_REG_IOREDTBL 0x10

/*
 * Maximum Redirection Entry, bits 23:16 in IOAPIC_REG_IOAPICVER -- corresponds
 * to number of interrupt input pins for the IOAPIC minus one.
 */
#నిర్వచించు IOAPICVER_MRE_ముసుగు 0xff0000

#నిర్వచించు IOREDTBL_దిగువ(reg) (IOAPIC_REG_IOREDTBL + (reg) * 2)
#నిర్వచించు IOREDTBL_ఎగువ(reg) (IOREDTBL_దిగువ(reg) + 1)

#నిర్వచించు IOREDTBL_దిగువ_అంతరాయం_ముసుగు బిట్టు(16)
#నిర్వచించు IOREDTBL_దిగువ_TRIGGER_బహుళకం_LEVEL బిట్టు(15)
#నిర్వచించు IOREDTBL_దిగువ_TRIGGER_బహుళకం_జరుపు    15
#నిర్వచించు IOREDTBL_దిగువ_POLARITY_దిగువ బిట్టు(13)
#నిర్వచించు IOREDTBL_దిగువ_POLARITY_జరుపు         13
#నిర్వచించు IOREDTBL_దిగువ_DEST_బహుళకం_LOGCIAL బిట్టు(11)

#నిర్వచించు IOAPICID_ID_బిట్లు 4
#నిర్వచించు IOAPICID_ID_స్థానభ్రంశం 24

#నిర్వచించు IOREDTBL_ఎగువ_RESERVED_బిట్లు 24

/*
 * Cache what we believe is in the low word of the IOREDTBL. This
 * has all the state of trigger modes etc etc.
 * IOAPIC_IRQ_LINES is the upper bound on the number of lines
 * per IOAPIC.
 */
స్థిర uint32_t ioredtbl_state[IOAPIC_IRQ_పంక్తులు * MAX(1, CONFIG_MAX_అంకె_IOAPIC)];
/*
 * The number of IRQ lines for each IOAPIC
 */
స్థిర uint8_t ioapic_nirqs[MAX(1, CONFIG_MAX_అంకె_IOAPIC)];

/* Number of IOAPICs in the system */
స్థిర uint32_t అంకె_ioapics = 0;

స్థిర శూన్య ioapic_write(uint32_t ioapic, మాట_t reg, uint32_t విలువ)
{
    *(అస్థిర uint32_t *)((మాట_t)(PPTR_IOAPIC_ప్రారంభం + ioapic * బిట్టు(పుట_బిట్లు)) + reg) = విలువ;
}

స్థిర uint32_t ioapic_read(uint32_t ioapic, మాట_t reg)
{
    తిరిగి *(అస్థిర uint32_t *)((మాట_t)(PPTR_IOAPIC_ప్రారంభం + ioapic * బిట్టు(పుట_బిట్లు)) + reg);
}

స్థిర శూన్య single_ioapic_init(మాట_t ioapic)
{
    uint32_t i;
    uint32_t nirqs;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOAPIC_REG_IOAPICVER);
    nirqs = ((ioapic_read(ioapic, IOAPIC_కిటికీ) & IOAPICVER_MRE_ముసుగు) >> 16) + 1;
    ioapic_nirqs[ioapic] = nirqs;

    /*
     * The MRE field of the IOAPICVER register is supposed to only be able to
     * return a max value of 239, which means a max nirqs value of IOAPIC_IRQ_LINES.
     */
    ఒకవేళ (nirqs > IOAPIC_IRQ_పంక్తులు) {
        userError("%s: ioapic %lu has %u IRQs,\n"
                  "which is greater than the max handled (%u)\n",
                  __func__, ioapic, nirqs, IOAPIC_IRQ_పంక్తులు);
        halt();
    }

    /* Mask all the IRQs. In doing so we happen to set
     * the vector to 0, which we can assert against in
     * mask_interrupt to ensure a vector is assigned
     * before we unmask */
    కొరకు (i = 0; i < nirqs; i++) {
        /* Send to ioapic_target_cpu, mask, and set 0 vector */
        ioapic_map_pin_గమ్యం_సదిశ(ioapic, i, 0, 0, 0);
    }
}

స్థిర  cpu_id_t ioapic_లక్ష్యం_cpu = 0;
శూన్య ioapic_init(uint32_t అంకె_nodes, cpu_id_t *cpu_list, uint32_t అంకె_ioapic)
{
    uint32_t ioapic;
    అంకె_ioapics = అంకె_ioapic;
    ioapic_లక్ష్యం_cpu = cpu_list[0];

    కొరకు (ioapic = 0; ioapic < అంకె_ioapic; ioapic++) {
        /* Init this ioapic */
        single_ioapic_init(ioapic);
    }
}

శూన్య ioapic_ముసుగు(bool_t ముసుగు_2, uint32_t ioapic, uint32_t pin)
{
    పూర్ణాంకం సూచీ = ioapic * IOAPIC_IRQ_పంక్తులు + pin;
    ఒకవేళ (ioapic >= అంకె_ioapics || pin >= ioapic_nirqs[ioapic]) {
        /* silently ignore requests to non existent parts of the interrupt space */
        తిరిగి;
    }
    ఒకవేళ (ముసుగు_2) {
        ioredtbl_state[సూచీ] |= IOREDTBL_దిగువ_అంతరాయం_ముసుగు;
    } లేకపోతే {
        ioredtbl_state[సూచీ] &= ~IOREDTBL_దిగువ_అంతరాయం_ముసుగు;
        /* it should not be possible to be unmasking an interrupt, without
         * it having been mapped to a vector, assert that this is the case */
        ప్రతిజ్ఞా((ioredtbl_state[సూచీ] & 0xff) != 0);
    }
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_దిగువ(pin));
    ioapic_write(ioapic, IOAPIC_కిటికీ, ioredtbl_state[సూచీ]);
}

exception_t ioapic_decode_map_pin_గమ్యం_సదిశ(మాట_t ioapic, మాట_t pin, మాట_t level,
                                            మాట_t polarity, మాట_t సదిశ)
{
    ఒకవేళ (అంకె_ioapics == 0) {
        userError("System has no IOAPICs");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (ioapic >= అంకె_ioapics) {
        userError("Invalid IOAPIC %ld, only have %ld", (దీర్ఘ)ioapic, (దీర్ఘ)అంకె_ioapics);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = అంకె_ioapics - 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (pin >= ioapic_nirqs[ioapic]) {
        userError("Invalid IOAPIC pin %ld, there are %d pins", (దీర్ఘ)pin, ioapic_nirqs[ioapic]);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ioapic_nirqs[ioapic] - 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (level != 0 && level != 1) {
        userError("Level should be 0 or 1, not %d", (పూర్ణాంకం)level);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (polarity != 0 && polarity != 1) {
        userError("Polarity should be 0 or 1, not %d", (పూర్ణాంకం)polarity);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    తిరిగి EXCEPTION_NONE;
}

శూన్య ioapic_map_pin_గమ్యం_సదిశ(మాట_t ioapic, మాట_t pin, మాట_t level,
                              మాట_t polarity, మాట_t సదిశ)
{
    uint32_t ioredtbl_ఎగువ = 0;
    uint32_t సూచీ = 0;

    సూచీ = ioapic * IOAPIC_IRQ_పంక్తులు + pin;
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_ఎగువ(pin));
    ioredtbl_ఎగువ = ioapic_read(ioapic, IOAPIC_కిటికీ) & ముసుగు(IOREDTBL_ఎగువ_RESERVED_బిట్లు);
    /* delivery mode: physical mode only, using APIC ID */
    ioredtbl_ఎగువ |= (ioapic_లక్ష్యం_cpu << IOREDTBL_ఎగువ_RESERVED_బిట్లు);
    ioapic_write(ioapic, IOAPIC_కిటికీ, ioredtbl_ఎగువ);
    /* we do not need to add IRQ_INT_OFFSET to the vector here */
    ioredtbl_state[సూచీ] = IOREDTBL_దిగువ_అంతరాయం_ముసుగు |
                            (level << IOREDTBL_దిగువ_TRIGGER_బహుళకం_జరుపు) |
                            (polarity << IOREDTBL_దిగువ_POLARITY_జరుపు) |
                            సదిశ;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_దిగువ(pin));
    /* the upper 16 bits are reserved */
    ioredtbl_state[సూచీ] |= ioapic_read(ioapic, IOAPIC_కిటికీ) & ~ముసుగు(16);
    ioapic_write(ioapic, IOAPIC_కిటికీ, ioredtbl_state[సూచీ]);
}
