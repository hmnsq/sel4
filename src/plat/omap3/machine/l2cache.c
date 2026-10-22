/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <arch/machine.h>

ସ୍ଥିର ଧାଡ଼ିଭିତରେ word_t readACR(ଶୂନ୍ୟ)
{
    word_t ACR;
    asm ଅସ୍ଥିର("mrc p15,0,%0,c1,c0,1" : "=r"(ACR));
    ଫେରନ୍ତୁ ACR;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ writeACR(word_t ACR)
{
    asm ଅସ୍ଥିର("mcr p15,0,%0,c1,c0,1" : : "r"(ACR));
}

ଶୂନ୍ୟ initL2Cache(ଶୂନ୍ୟ)
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

ଶୂନ୍ୟ plat_cleanL2Range(paddr_t start, paddr_t end) {}
ଶୂନ୍ୟ plat_invalidateL2Range(paddr_t start, paddr_t end) {}
ଶୂନ୍ୟ plat_cleanInvalidateL2Range(paddr_t start, paddr_t end) {}
ଶୂନ୍ୟ plat_cleanInvalidateL2Cache(ଶୂନ୍ୟ) {}
