/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <arch/machine/hardware.h>

#ಯದಿ defined(CONFIG_ARM_HAS_TLB_LOCK)

ಶೂನ್ಯ lockTLBEntry(vptr_t vaddr)
{
    ಪೂರ್ಣಾಙ್ಕ n = arm_KSTlb_Lock_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ x, y;

    /* tlbLockCount is used only in this function, which is called at most 2 times for unicore
       platforms (and we only have unicore A8 platforms). */
    ಪ್ರತಿಜ್ಞಾ(arm_KSTlb_Lock_ಎಣಿಕೆ < 2);
    /* Since asserts are off in release mode, we enforce the bound on tlbLockCount manually, so we
       don't have to verify calling context. We need the bound to be sure the bit operations below
       are not undefined behaviour. We leave the assert in, because we want to know about it when
       the calling context ever changes. */
    ಯದಿ (arm_KSTlb_Lock_ಎಣಿಕೆ >= 2) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    arm_KSTlb_Lock_ಎಣಿಕೆ++;
    /* Compute two values, x and y, to write to the lockdown register. */

    /* Before lockdown, base = victim = num_locked_tlb_entries. */
    x = 1 | (n << 22) | (n << 27);
    n ++;
    /* After lockdown, base = victim = num_locked_tlb_entries + 1. */
    y = (n << 22) | (n << 27);

    lockTLBEntryCritical(vaddr, x, y);
}

#ಅನ್ಯಥಾ

/* We don't currently support TLB locking for other processors. */
ಶೂನ್ಯ lockTLBEntry(vptr_t vaddr)
{

}

#endif
