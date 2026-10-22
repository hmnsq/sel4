/*
 * Copyright 2015, DornerWorks
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>

ਸਥਿਰ ਸਤਰਅੰਦਰ ਸ਼ਬਦ_t readACR(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t ACR;
    asm ਅਸਥਿਰ("mrc p15,0,%0,c1,c0,1" : "=r"(ACR));
    ਵਾਪਸ ACR;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ writeACR(ਸ਼ਬਦ_t ACR)
{
    asm ਅਸਥਿਰ("mcr p15,0,%0,c1,c0,1" : : "r"(ACR));
}

ਖਾਲੀ initL2Cache(ਖਾਲੀ)
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

ਖਾਲੀ plat_cleanL2Range(paddr_t ਸ਼ੁਰੂ, paddr_t ਸਿਰਾ) {}
ਖਾਲੀ plat_invalidateL2Range(paddr_t ਸ਼ੁਰੂ, paddr_t ਸਿਰਾ) {}
ਖਾਲੀ plat_cleanInvalidateL2Range(paddr_t ਸ਼ੁਰੂ, paddr_t ਸਿਰਾ) {}
ਖਾਲੀ plat_cleanInvalidateL2Cache(ਖਾਲੀ) {}
