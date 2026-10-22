/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <arch/machine/hardware.h>

#જો defined(CONFIG_ARM_HAS_TLB_LOCK)

શૂન્ય lockTLBEntry(vptr_t vaddr)
{
    પૂર્ણાંક n = arm_KSTlb_Lock_ગણતરી;
    પૂર્ણાંક x, y;

    /* tlbLockCount is used only in this function, which is called at most 2 times for unicore
       platforms (and we only have unicore A8 platforms). */
    પ્રતિજ્ઞા(arm_KSTlb_Lock_ગણતરી < 2);
    /* Since asserts are off in release mode, we enforce the bound on tlbLockCount manually, so we
       don't have to verify calling context. We need the bound to be sure the bit operations below
       are not undefined behaviour. We leave the assert in, because we want to know about it when
       the calling context ever changes. */
    જો (arm_KSTlb_Lock_ગણતરી >= 2) {
        પાછા;
    }

    arm_KSTlb_Lock_ગણતરી++;
    /* Compute two values, x and y, to write to the lockdown register. */

    /* Before lockdown, base = victim = num_locked_tlb_entries. */
    x = 1 | (n << 22) | (n << 27);
    n ++;
    /* After lockdown, base = victim = num_locked_tlb_entries + 1. */
    y = (n << 22) | (n << 27);

    lockTLBEntryCritical(vaddr, x, y);
}

#નહીંતર

/* We don't currently support TLB locking for other processors. */
શૂન્ય lockTLBEntry(vptr_t vaddr)
{

}

#endif
