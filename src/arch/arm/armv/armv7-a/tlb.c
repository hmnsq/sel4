/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/hardware.h>

#ਜੇ defined(CONFIG_ARM_HAS_TLB_ਤਾਲਾ)

ਖਾਲੀ ਤਾਲਾ_TLBEntry(vptr_t vaddr)
{
    ਪੂਰਨ n = arm_KSTlb_ਤਾਲਾ_ਗਿਣਤੀ;
    ਪੂਰਨ x, y;

    /* tlbLockCount is used only in this function, which is called at most 2 times for unicore
       platforms (and we only have unicore A8 platforms). */
    ਪ੍ਰਤਿਜ੍ਞਾ(arm_KSTlb_ਤਾਲਾ_ਗਿਣਤੀ < 2);
    /* Since asserts are off in release mode, we enforce the bound on tlbLockCount manually, so we
       don't have to verify calling context. We need the bound to be sure the bit operations below
       are not undefined behaviour. We leave the assert in, because we want to know about it when
       the calling context ever changes. */
    ਜੇ (arm_KSTlb_ਤਾਲਾ_ਗਿਣਤੀ >= 2) {
        ਵਾਪਸ;
    }

    arm_KSTlb_ਤਾਲਾ_ਗਿਣਤੀ++;
    /* Compute two values, x and y, to write to the lockdown register. */

    /* Before lockdown, base = victim = num_locked_tlb_entries. */
    x = 1 | (n << 22) | (n << 27);
    n ++;
    /* After lockdown, base = victim = num_locked_tlb_entries + 1. */
    y = (n << 22) | (n << 27);

    ਤਾਲਾ_TLBEntry_Critical(vaddr, x, y);
}

#ਨਹੀਂਤਾਂ

/* We don't currently support TLB locking for other processors. */
ਖਾਲੀ ਤਾਲਾ_TLBEntry(vptr_t vaddr)
{

}

#ਜੇ_ਅੰਤ
