/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <arch/machine.h>

ایستا درونخطی word_t readACR(تهی)
{
    word_t ACR;
    asm فرار("mrc p15,0,%0,c1,c0,1" : "=r"(ACR));
    بازگشت ACR;
}

ایستا درونخطی تهی writeACR(word_t ACR)
{
    asm فرار("mcr p15,0,%0,c1,c0,1" : : "r"(ACR));
}

تهی initL2Cache(تهی)
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

تهی plat_cleanL2Range(paddr_t start, paddr_t end) {}
تهی plat_invalidateL2Range(paddr_t start, paddr_t end) {}
تهی plat_cleanInvalidateL2Range(paddr_t start, paddr_t end) {}
تهی plat_cleanInvalidateL2Cache(تهی) {}
