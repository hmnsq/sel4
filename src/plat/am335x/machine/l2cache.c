/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <arch/machine.h>

স্থির অন্তঃস্থ word_t readACR(শূন্য)
{
    word_t ACR;
    asm চঞ্চল("mrc p15,0,%0,c1,c0,1" : "=r"(ACR));
    প্রত্যাগম ACR;
}

স্থির অন্তঃস্থ শূন্য writeACR(word_t ACR)
{
    asm চঞ্চল("mcr p15,0,%0,c1,c0,1" : : "r"(ACR));
}

শূন্য initL2Cache(শূন্য)
{
    cleanInvalidateL1Caches();

    /*
     * Set the L2EN bit in the Auxially Control Register.
     *
     * We assume the C bit is already set in the system control register (from
     * head.S), and that the L2 Cache Auxiliary Control Register is correct
     * (as per reset).
     */
    writeACR(readACR() | 0x2);

    cleanInvalidateL1Caches();
}

শূন্য plat_cleanL2Range(paddr_t start, paddr_t end) {}
শূন্য plat_invalidateL2Range(paddr_t start, paddr_t end) {}
শূন্য plat_cleanInvalidateL2Range(paddr_t start, paddr_t end) {}
শূন্য plat_cleanInvalidateL2Cache(শূন্য) {}
