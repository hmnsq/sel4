/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <arch/machine.h>

സ്ഥിതം വരിയിൽ word_t readACR(ശൂന്യം)
{
    word_t ACR;
    asm അസ്ഥിര("mrc p15,0,%0,c1,c0,1" : "=r"(ACR));
    തിരികെ ACR;
}

സ്ഥിതം വരിയിൽ ശൂന്യം writeACR(word_t ACR)
{
    asm അസ്ഥിര("mcr p15,0,%0,c1,c0,1" : : "r"(ACR));
}

ശൂന്യം initL2Cache(ശൂന്യം)
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

ശൂന്യം plat_cleanL2Range(paddr_t start, paddr_t end) {}
ശൂന്യം plat_invalidateL2Range(paddr_t start, paddr_t end) {}
ശൂന്യം plat_cleanInvalidateL2Range(paddr_t start, paddr_t end) {}
ശൂന്യം plat_cleanInvalidateL2Cache(ശൂന്യം) {}
