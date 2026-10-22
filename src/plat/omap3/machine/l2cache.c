/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <arch/machine.h>

સ્થિર અંતઃરેખ word_t readACR(શૂન્ય)
{
    word_t ACR;
    asm અસ્થિર("mrc p15,0,%0,c1,c0,1" : "=r"(ACR));
    પાછા ACR;
}

સ્થિર અંતઃરેખ શૂન્ય writeACR(word_t ACR)
{
    asm અસ્થિર("mcr p15,0,%0,c1,c0,1" : : "r"(ACR));
}

શૂન્ય initL2Cache(શૂન્ય)
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

શૂન્ય plat_cleanL2Range(paddr_t start, paddr_t end) {}
શૂન્ય plat_invalidateL2Range(paddr_t start, paddr_t end) {}
શૂન્ય plat_cleanInvalidateL2Range(paddr_t start, paddr_t end) {}
શૂન્ય plat_cleanInvalidateL2Cache(શૂન્ય) {}
