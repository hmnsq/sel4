/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <arch/machine/hardware.h>

#ఒకవేళ defined(CONFIG_ARM_HAS_TLB_తాళం)

శూన్య తాళం_TLBEntry(vptr_t vaddr)
{
    పూర్ణాంకం n = arm_KSTlb_తాళం_లెక్క;
    పూర్ణాంకం x, y;

    /* tlbLockCount is used only in this function, which is called at most 2 times for unicore
       platforms (and we only have unicore A8 platforms). */
    ప్రతిజ్ఞా(arm_KSTlb_తాళం_లెక్క < 2);
    /* Since asserts are off in release mode, we enforce the bound on tlbLockCount manually, so we
       don't have to verify calling context. We need the bound to be sure the bit operations below
       are not undefined behaviour. We leave the assert in, because we want to know about it when
       the calling context ever changes. */
    ఒకవేళ (arm_KSTlb_తాళం_లెక్క >= 2) {
        తిరిగి;
    }

    arm_KSTlb_తాళం_లెక్క++;
    /* Compute two values, x and y, to write to the lockdown register. */

    /* Before lockdown, base = victim = num_locked_tlb_entries. */
    x = 1 | (n << 22) | (n << 27);
    n ++;
    /* After lockdown, base = victim = num_locked_tlb_entries + 1. */
    y = (n << 22) | (n << 27);

    తాళం_TLBEntry_Critical(vaddr, x, y);
}

#లేకపోతే

/* We don't currently support TLB locking for other processors. */
శూన్య తాళం_TLBEntry(vptr_t vaddr)
{

}

#అయితే_ముగింపు
