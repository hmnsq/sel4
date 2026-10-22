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

#وصف_ڪريو IOAPIC_REGSEL 0x00
#وصف_ڪريو IOAPIC_دري 0x10

#وصف_ڪريو IOAPIC_REG_IOAPICID 0x00
#وصف_ڪريو IOAPIC_REG_IOAPICVER 0x01
#وصف_ڪريو IOAPIC_REG_IOREDTBL 0x10

/*
 * Maximum Redirection Entry, bits 23:16 in IOAPIC_REG_IOAPICVER -- corresponds
 * to number of interrupt input pins for the IOAPIC minus one.
 */
#وصف_ڪريو IOAPICVER_MRE_نقاب 0xff0000

#وصف_ڪريو IOREDTBL_هيٺيون(reg) (IOAPIC_REG_IOREDTBL + (reg) * 2)
#وصف_ڪريو IOREDTBL_مٿاهون(reg) (IOREDTBL_هيٺيون(reg) + 1)

#وصف_ڪريو IOREDTBL_هيٺيون_مداخلت_نقاب ٻٽو_انگ(16)
#وصف_ڪريو IOREDTBL_هيٺيون_TRIGGER_گهڻائي_LEVEL ٻٽو_انگ(15)
#وصف_ڪريو IOREDTBL_هيٺيون_TRIGGER_گهڻائي_سرڪايو    15
#وصف_ڪريو IOREDTBL_هيٺيون_POLARITY_هيٺيون ٻٽو_انگ(13)
#وصف_ڪريو IOREDTBL_هيٺيون_POLARITY_سرڪايو         13
#وصف_ڪريو IOREDTBL_هيٺيون_DEST_گهڻائي_LOGCIAL ٻٽو_انگ(11)

#وصف_ڪريو IOAPICID_ID_بٽ 4
#وصف_ڪريو IOAPICID_ID_وٿي 24

#وصف_ڪريو IOREDTBL_مٿاهون_RESERVED_بٽ 24

/*
 * Cache what we believe is in the low word of the IOREDTBL. This
 * has all the state of trigger modes etc etc.
 * IOAPIC_IRQ_LINES is the upper bound on the number of lines
 * per IOAPIC.
 */
ساكن uint32_t ioredtbl_state[IOAPIC_IRQ_سٽون * MAX(1, CONFIG_MAX_انگ_IOAPIC)];
/*
 * The number of IRQ lines for each IOAPIC
 */
ساكن uint8_t ioapic_nirqs[MAX(1, CONFIG_MAX_انگ_IOAPIC)];

/* Number of IOAPICs in the system */
ساكن uint32_t انگ_ioapics = 0;

ساكن فراغ ioapic_write(uint32_t ioapic, لفظ_t reg, uint32_t قدر)
{
    *(متطاير uint32_t *)((لفظ_t)(PPTR_IOAPIC_شروعات + ioapic * ٻٽو_انگ(صفحو_بٽ)) + reg) = قدر;
}

ساكن uint32_t ioapic_read(uint32_t ioapic, لفظ_t reg)
{
    أرجع *(متطاير uint32_t *)((لفظ_t)(PPTR_IOAPIC_شروعات + ioapic * ٻٽو_انگ(صفحو_بٽ)) + reg);
}

ساكن فراغ single_ioapic_init(لفظ_t ioapic)
{
    uint32_t i;
    uint32_t nirqs;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOAPIC_REG_IOAPICVER);
    nirqs = ((ioapic_read(ioapic, IOAPIC_دري) & IOAPICVER_MRE_نقاب) >> 16) + 1;
    ioapic_nirqs[ioapic] = nirqs;

    /*
     * The MRE field of the IOAPICVER register is supposed to only be able to
     * return a max value of 239, which means a max nirqs value of IOAPIC_IRQ_LINES.
     */
    إذا (nirqs > IOAPIC_IRQ_سٽون) {
        userError("%s: ioapic %lu has %u IRQs,\n"
                  "which is greater than the max handled (%u)\n",
                  __func__, ioapic, nirqs, IOAPIC_IRQ_سٽون);
        halt();
    }

    /* Mask all the IRQs. In doing so we happen to set
     * the vector to 0, which we can assert against in
     * mask_interrupt to ensure a vector is assigned
     * before we unmask */
    لكل (i = 0; i < nirqs; i++) {
        /* Send to ioapic_target_cpu, mask, and set 0 vector */
        ioapic_map_pin_منزل_ويڪٽر(ioapic, i, 0, 0, 0);
    }
}

ساكن  cpu_id_t ioapic_هدف_cpu = 0;
فراغ ioapic_init(uint32_t انگ_nodes, cpu_id_t *cpu_list, uint32_t انگ_ioapic)
{
    uint32_t ioapic;
    انگ_ioapics = انگ_ioapic;
    ioapic_هدف_cpu = cpu_list[0];

    لكل (ioapic = 0; ioapic < انگ_ioapic; ioapic++) {
        /* Init this ioapic */
        single_ioapic_init(ioapic);
    }
}

فراغ ioapic_نقاب(bool_t نقاب_2, uint32_t ioapic, uint32_t pin)
{
    صحيح اشاريو = ioapic * IOAPIC_IRQ_سٽون + pin;
    إذا (ioapic >= انگ_ioapics || pin >= ioapic_nirqs[ioapic]) {
        /* silently ignore requests to non existent parts of the interrupt space */
        أرجع;
    }
    إذا (نقاب_2) {
        ioredtbl_state[اشاريو] |= IOREDTBL_هيٺيون_مداخلت_نقاب;
    } وإلا {
        ioredtbl_state[اشاريو] &= ~IOREDTBL_هيٺيون_مداخلت_نقاب;
        /* it should not be possible to be unmasking an interrupt, without
         * it having been mapped to a vector, assert that this is the case */
        أكد((ioredtbl_state[اشاريو] & 0xff) != 0);
    }
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_هيٺيون(pin));
    ioapic_write(ioapic, IOAPIC_دري, ioredtbl_state[اشاريو]);
}

exception_t ioapic_decode_map_pin_منزل_ويڪٽر(لفظ_t ioapic, لفظ_t pin, لفظ_t level,
                                            لفظ_t polarity, لفظ_t ويڪٽر)
{
    إذا (انگ_ioapics == 0) {
        userError("System has no IOAPICs");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (ioapic >= انگ_ioapics) {
        userError("Invalid IOAPIC %ld, only have %ld", (ڊگهو)ioapic, (ڊگهو)انگ_ioapics);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = انگ_ioapics - 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (pin >= ioapic_nirqs[ioapic]) {
        userError("Invalid IOAPIC pin %ld, there are %d pins", (ڊگهو)pin, ioapic_nirqs[ioapic]);
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

فراغ ioapic_map_pin_منزل_ويڪٽر(لفظ_t ioapic, لفظ_t pin, لفظ_t level,
                              لفظ_t polarity, لفظ_t ويڪٽر)
{
    uint32_t ioredtbl_مٿاهون = 0;
    uint32_t اشاريو = 0;

    اشاريو = ioapic * IOAPIC_IRQ_سٽون + pin;
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_مٿاهون(pin));
    ioredtbl_مٿاهون = ioapic_read(ioapic, IOAPIC_دري) & نقاب(IOREDTBL_مٿاهون_RESERVED_بٽ);
    /* delivery mode: physical mode only, using APIC ID */
    ioredtbl_مٿاهون |= (ioapic_هدف_cpu << IOREDTBL_مٿاهون_RESERVED_بٽ);
    ioapic_write(ioapic, IOAPIC_دري, ioredtbl_مٿاهون);
    /* we do not need to add IRQ_INT_OFFSET to the vector here */
    ioredtbl_state[اشاريو] = IOREDTBL_هيٺيون_مداخلت_نقاب |
                            (level << IOREDTBL_هيٺيون_TRIGGER_گهڻائي_سرڪايو) |
                            (polarity << IOREDTBL_هيٺيون_POLARITY_سرڪايو) |
                            ويڪٽر;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_هيٺيون(pin));
    /* the upper 16 bits are reserved */
    ioredtbl_state[اشاريو] |= ioapic_read(ioapic, IOAPIC_دري) & ~نقاب(16);
    ioapic_write(ioapic, IOAPIC_دري, ioredtbl_state[اشاريو]);
}
