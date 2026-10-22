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

#تعریف_کړه IOAPIC_REGSEL 0x00
#تعریف_کړه IOAPIC_کړکۍ 0x10

#تعریف_کړه IOAPIC_REG_IOAPICID 0x00
#تعریف_کړه IOAPIC_REG_IOAPICVER 0x01
#تعریف_کړه IOAPIC_REG_IOREDTBL 0x10

/*
 * Maximum Redirection Entry, bits 23:16 in IOAPIC_REG_IOAPICVER -- corresponds
 * to number of interrupt input pins for the IOAPIC minus one.
 */
#تعریف_کړه IOAPICVER_MRE_ماسک 0xff0000

#تعریف_کړه IOREDTBL_ټیټ(reg) (IOAPIC_REG_IOREDTBL + (reg) * 2)
#تعریف_کړه IOREDTBL_لوړ(reg) (IOREDTBL_ټیټ(reg) + 1)

#تعریف_کړه IOREDTBL_ټیټ_مداخله_ماسک بټ(16)
#تعریف_کړه IOREDTBL_ټیټ_TRIGGER_ډېرپېښ_LEVEL بټ(15)
#تعریف_کړه IOREDTBL_ټیټ_TRIGGER_ډېرپېښ_ښويدنه    15
#تعریف_کړه IOREDTBL_ټیټ_POLARITY_ټیټ بټ(13)
#تعریف_کړه IOREDTBL_ټیټ_POLARITY_ښويدنه         13
#تعریف_کړه IOREDTBL_ټیټ_DEST_ډېرپېښ_LOGCIAL بټ(11)

#تعریف_کړه IOAPICID_ID_بټونه 4
#تعریف_کړه IOAPICID_ID_دننه_واټن 24

#تعریف_کړه IOREDTBL_لوړ_RESERVED_بټونه 24

/*
 * Cache what we believe is in the low word of the IOREDTBL. This
 * has all the state of trigger modes etc etc.
 * IOAPIC_IRQ_LINES is the upper bound on the number of lines
 * per IOAPIC.
 */
ساكن uint32_t ioredtbl_state[IOAPIC_IRQ_کرښې * MAX(1, CONFIG_MAX_عدد_IOAPIC)];
/*
 * The number of IRQ lines for each IOAPIC
 */
ساكن uint8_t ioapic_nirqs[MAX(1, CONFIG_MAX_عدد_IOAPIC)];

/* Number of IOAPICs in the system */
ساكن uint32_t عدد_ioapics = 0;

ساكن فراغ ioapic_write(uint32_t ioapic, کلمه_t reg, uint32_t ارزښت)
{
    *(متطاير uint32_t *)((کلمه_t)(PPTR_IOAPIC_پیل + ioapic * بټ(پاڼه_بټونه)) + reg) = ارزښت;
}

ساكن uint32_t ioapic_read(uint32_t ioapic, کلمه_t reg)
{
    أرجع *(متطاير uint32_t *)((کلمه_t)(PPTR_IOAPIC_پیل + ioapic * بټ(پاڼه_بټونه)) + reg);
}

ساكن فراغ single_ioapic_init(کلمه_t ioapic)
{
    uint32_t i;
    uint32_t nirqs;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOAPIC_REG_IOAPICVER);
    nirqs = ((ioapic_read(ioapic, IOAPIC_کړکۍ) & IOAPICVER_MRE_ماسک) >> 16) + 1;
    ioapic_nirqs[ioapic] = nirqs;

    /*
     * The MRE field of the IOAPICVER register is supposed to only be able to
     * return a max value of 239, which means a max nirqs value of IOAPIC_IRQ_LINES.
     */
    إذا (nirqs > IOAPIC_IRQ_کرښې) {
        userError("%s: ioapic %lu has %u IRQs,\n"
                  "which is greater than the max handled (%u)\n",
                  __func__, ioapic, nirqs, IOAPIC_IRQ_کرښې);
        halt();
    }

    /* Mask all the IRQs. In doing so we happen to set
     * the vector to 0, which we can assert against in
     * mask_interrupt to ensure a vector is assigned
     * before we unmask */
    لكل (i = 0; i < nirqs; i++) {
        /* Send to ioapic_target_cpu, mask, and set 0 vector */
        ioapic_map_pin_منزل_ویکټور(ioapic, i, 0, 0, 0);
    }
}

ساكن  cpu_id_t ioapic_موخه_cpu = 0;
فراغ ioapic_init(uint32_t عدد_nodes, cpu_id_t *cpu_list, uint32_t عدد_ioapic)
{
    uint32_t ioapic;
    عدد_ioapics = عدد_ioapic;
    ioapic_موخه_cpu = cpu_list[0];

    لكل (ioapic = 0; ioapic < عدد_ioapic; ioapic++) {
        /* Init this ioapic */
        single_ioapic_init(ioapic);
    }
}

فراغ ioapic_ماسک(bool_t ماسک_2, uint32_t ioapic, uint32_t pin)
{
    صحيح شاخص = ioapic * IOAPIC_IRQ_کرښې + pin;
    إذا (ioapic >= عدد_ioapics || pin >= ioapic_nirqs[ioapic]) {
        /* silently ignore requests to non existent parts of the interrupt space */
        أرجع;
    }
    إذا (ماسک_2) {
        ioredtbl_state[شاخص] |= IOREDTBL_ټیټ_مداخله_ماسک;
    } وإلا {
        ioredtbl_state[شاخص] &= ~IOREDTBL_ټیټ_مداخله_ماسک;
        /* it should not be possible to be unmasking an interrupt, without
         * it having been mapped to a vector, assert that this is the case */
        أكد((ioredtbl_state[شاخص] & 0xff) != 0);
    }
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_ټیټ(pin));
    ioapic_write(ioapic, IOAPIC_کړکۍ, ioredtbl_state[شاخص]);
}

exception_t ioapic_decode_map_pin_منزل_ویکټور(کلمه_t ioapic, کلمه_t pin, کلمه_t level,
                                            کلمه_t polarity, کلمه_t ویکټور)
{
    إذا (عدد_ioapics == 0) {
        userError("System has no IOAPICs");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (ioapic >= عدد_ioapics) {
        userError("Invalid IOAPIC %ld, only have %ld", (اوږد)ioapic, (اوږد)عدد_ioapics);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = عدد_ioapics - 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (pin >= ioapic_nirqs[ioapic]) {
        userError("Invalid IOAPIC pin %ld, there are %d pins", (اوږد)pin, ioapic_nirqs[ioapic]);
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

فراغ ioapic_map_pin_منزل_ویکټور(کلمه_t ioapic, کلمه_t pin, کلمه_t level,
                              کلمه_t polarity, کلمه_t ویکټور)
{
    uint32_t ioredtbl_لوړ = 0;
    uint32_t شاخص = 0;

    شاخص = ioapic * IOAPIC_IRQ_کرښې + pin;
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_لوړ(pin));
    ioredtbl_لوړ = ioapic_read(ioapic, IOAPIC_کړکۍ) & ماسک(IOREDTBL_لوړ_RESERVED_بټونه);
    /* delivery mode: physical mode only, using APIC ID */
    ioredtbl_لوړ |= (ioapic_موخه_cpu << IOREDTBL_لوړ_RESERVED_بټونه);
    ioapic_write(ioapic, IOAPIC_کړکۍ, ioredtbl_لوړ);
    /* we do not need to add IRQ_INT_OFFSET to the vector here */
    ioredtbl_state[شاخص] = IOREDTBL_ټیټ_مداخله_ماسک |
                            (level << IOREDTBL_ټیټ_TRIGGER_ډېرپېښ_ښويدنه) |
                            (polarity << IOREDTBL_ټیټ_POLARITY_ښويدنه) |
                            ویکټور;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_ټیټ(pin));
    /* the upper 16 bits are reserved */
    ioredtbl_state[شاخص] |= ioapic_read(ioapic, IOAPIC_کړکۍ) & ~ماسک(16);
    ioapic_write(ioapic, IOAPIC_کړکۍ, ioredtbl_state[شاخص]);
}
