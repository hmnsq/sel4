/*
 * Copyright 2015, DornerWorks
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <arch/machine.h>

अटल अंतरभूत word_t readACR(व्योम)
{
    word_t ACR;
    यंत्र अस्थिर("mrc p15,0,%0,c1,c0,1" : "=r"(ACR));
    वापस ACR;
}

अटल अंतरभूत व्योम writeACR(word_t ACR)
{
    यंत्र अस्थिर("mcr p15,0,%0,c1,c0,1" : : "r"(ACR));
}

व्योम initL2Cache(व्योम)
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

व्योम plat_cleanL2Range(paddr_t start, paddr_t end) {}
व्योम plat_invalidateL2Range(paddr_t start, paddr_t end) {}
व्योम plat_cleanInvalidateL2Range(paddr_t start, paddr_t end) {}
व्योम plat_cleanInvalidateL2Cache(व्योम) {}
