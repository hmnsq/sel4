/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#ضمن <linker.h>
#ضمن <machine/io.h>
#ضمن <plat/machine/hardware.h>
#ضمن <plat/machine/ioapic.h>

#تعریف_کرو IOAPIC_REGSEL 0x00
#تعریف_کرو IOAPIC_کھڑکی 0x10

#تعریف_کرو IOAPIC_REG_IOAPICID 0x00
#تعریف_کرو IOAPIC_REG_IOAPICVER 0x01
#تعریف_کرو IOAPIC_REG_IOREDTBL 0x10

/*
 * Maximum Redirection Entry, bits 23:16 in IOAPIC_REG_IOAPICVER -- corresponds
 * to number of interrupt input pins for the IOAPIC minus one.
 */
#تعریف_کرو IOAPICVER_MRE_نقاب 0xff0000

#تعریف_کرو IOREDTBL_کم(reg) (IOAPIC_REG_IOREDTBL + (reg) * 2)
#تعریف_کرو IOREDTBL_بلند(reg) (IOREDTBL_کم(reg) + 1)

#تعریف_کرو IOREDTBL_کم_رکاوٹ_نقاب ثنائی_ہندسہ(16)
#تعریف_کرو IOREDTBL_کم_TRIGGER_منوال_LEVEL ثنائی_ہندسہ(15)
#تعریف_کرو IOREDTBL_کم_TRIGGER_منوال_کھسکاؤ    15
#تعریف_کرو IOREDTBL_کم_POLARITY_کم ثنائی_ہندسہ(13)
#تعریف_کرو IOREDTBL_کم_POLARITY_کھسکاؤ         13
#تعریف_کرو IOREDTBL_کم_DEST_منوال_LOGCIAL ثنائی_ہندسہ(11)

#تعریف_کرو IOAPICID_ID_بٹ 4
#تعریف_کرو IOAPICID_ID_وتھ 24

#تعریف_کرو IOREDTBL_بلند_RESERVED_بٹ 24

/*
 * Cache what we believe is in the low word of the IOREDTBL. This
 * has all the state of trigger modes etc etc.
 * IOAPIC_IRQ_LINES is the upper bound on the number of lines
 * per IOAPIC.
 */
ساكن uint32_t ioredtbl_state[IOAPIC_IRQ_سطریں * MAX(1, CONFIG_MAX_عدد_IOAPIC)];
/*
 * The number of IRQ lines for each IOAPIC
 */
ساكن uint8_t ioapic_nirqs[MAX(1, CONFIG_MAX_عدد_IOAPIC)];

/* Number of IOAPICs in the system */
ساكن uint32_t عدد_ioapics = 0;

ساكن فراغ ioapic_write(uint32_t ioapic, لفظ_t reg, uint32_t قدر)
{
    *(متطاير uint32_t *)((لفظ_t)(PPTR_IOAPIC_شروع + ioapic * ثنائی_ہندسہ(ورقہ_بٹ)) + reg) = قدر;
}

ساكن uint32_t ioapic_read(uint32_t ioapic, لفظ_t reg)
{
    أرجع *(متطاير uint32_t *)((لفظ_t)(PPTR_IOAPIC_شروع + ioapic * ثنائی_ہندسہ(ورقہ_بٹ)) + reg);
}

ساكن فراغ single_ioapic_init(لفظ_t ioapic)
{
    uint32_t i;
    uint32_t nirqs;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOAPIC_REG_IOAPICVER);
    nirqs = ((ioapic_read(ioapic, IOAPIC_کھڑکی) & IOAPICVER_MRE_نقاب) >> 16) + 1;
    ioapic_nirqs[ioapic] = nirqs;

    /*
     * The MRE field of the IOAPICVER register is supposed to only be able to
     * return a max value of 239, which means a max nirqs value of IOAPIC_IRQ_LINES.
     */
    إذا (nirqs > IOAPIC_IRQ_سطریں) {
        userError("%s: ioapic %lu has %u IRQs,\n"
                  "which is greater than the max handled (%u)\n",
                  __func__, ioapic, nirqs, IOAPIC_IRQ_سطریں);
        halt();
    }

    /* Mask all the IRQs. In doing so we happen to set
     * the vector to 0, which we can assert against in
     * mask_interrupt to ensure a vector is assigned
     * before we unmask */
    لكل (i = 0; i < nirqs; i++) {
        /* Send to ioapic_target_cpu, mask, and set 0 vector */
        ioapic_map_pin_منزل_سمتیہ(ioapic, i, 0, 0, 0);
    }
}

ساكن  cpu_id_t ioapic_نشانہ_cpu = 0;
فراغ ioapic_init(uint32_t عدد_nodes, cpu_id_t *cpu_list, uint32_t عدد_ioapic)
{
    uint32_t ioapic;
    عدد_ioapics = عدد_ioapic;
    ioapic_نشانہ_cpu = cpu_list[0];

    لكل (ioapic = 0; ioapic < عدد_ioapic; ioapic++) {
        /* Init this ioapic */
        single_ioapic_init(ioapic);
    }
}

فراغ ioapic_نقاب(bool_t نقاب_2, uint32_t ioapic, uint32_t pin)
{
    صحيح اشاریہ = ioapic * IOAPIC_IRQ_سطریں + pin;
    إذا (ioapic >= عدد_ioapics || pin >= ioapic_nirqs[ioapic]) {
        /* silently ignore requests to non existent parts of the interrupt space */
        أرجع;
    }
    إذا (نقاب_2) {
        ioredtbl_state[اشاریہ] |= IOREDTBL_کم_رکاوٹ_نقاب;
    } وإلا {
        ioredtbl_state[اشاریہ] &= ~IOREDTBL_کم_رکاوٹ_نقاب;
        /* it should not be possible to be unmasking an interrupt, without
         * it having been mapped to a vector, assert that this is the case */
        أكد((ioredtbl_state[اشاریہ] & 0xff) != 0);
    }
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_کم(pin));
    ioapic_write(ioapic, IOAPIC_کھڑکی, ioredtbl_state[اشاریہ]);
}

exception_t ioapic_decode_map_pin_منزل_سمتیہ(لفظ_t ioapic, لفظ_t pin, لفظ_t level,
                                            لفظ_t polarity, لفظ_t سمتیہ)
{
    إذا (عدد_ioapics == 0) {
        userError("System has no IOAPICs");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (ioapic >= عدد_ioapics) {
        userError("Invalid IOAPIC %ld, only have %ld", (لما)ioapic, (لما)عدد_ioapics);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = عدد_ioapics - 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (pin >= ioapic_nirqs[ioapic]) {
        userError("Invalid IOAPIC pin %ld, there are %d pins", (لما)pin, ioapic_nirqs[ioapic]);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ioapic_nirqs[ioapic] - 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (level != 0 && level != 1) {
        userError("Level should be 0 or 1, not %d", (صحيح)level);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (polarity != 0 && polarity != 1) {
        userError("Polarity should be 0 or 1, not %d", (صحيح)polarity);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع EXCEPTION_NONE;
}

فراغ ioapic_map_pin_منزل_سمتیہ(لفظ_t ioapic, لفظ_t pin, لفظ_t level,
                              لفظ_t polarity, لفظ_t سمتیہ)
{
    uint32_t ioredtbl_بلند = 0;
    uint32_t اشاریہ = 0;

    اشاریہ = ioapic * IOAPIC_IRQ_سطریں + pin;
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_بلند(pin));
    ioredtbl_بلند = ioapic_read(ioapic, IOAPIC_کھڑکی) & نقاب(IOREDTBL_بلند_RESERVED_بٹ);
    /* delivery mode: physical mode only, using APIC ID */
    ioredtbl_بلند |= (ioapic_نشانہ_cpu << IOREDTBL_بلند_RESERVED_بٹ);
    ioapic_write(ioapic, IOAPIC_کھڑکی, ioredtbl_بلند);
    /* we do not need to add IRQ_INT_OFFSET to the vector here */
    ioredtbl_state[اشاریہ] = IOREDTBL_کم_رکاوٹ_نقاب |
                            (level << IOREDTBL_کم_TRIGGER_منوال_کھسکاؤ) |
                            (polarity << IOREDTBL_کم_POLARITY_کھسکاؤ) |
                            سمتیہ;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_کم(pin));
    /* the upper 16 bits are reserved */
    ioredtbl_state[اشاریہ] |= ioapic_read(ioapic, IOAPIC_کھڑکی) & ~نقاب(16);
    ioapic_write(ioapic, IOAPIC_کھڑکی, ioredtbl_state[اشاریہ]);
}
