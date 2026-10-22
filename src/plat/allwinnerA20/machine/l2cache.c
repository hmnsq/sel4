/*
 * Copyright 2015, DornerWorks
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <arch/machine.h>

स्थिर अन्तःस्थ word_t readACR(शून्य)
{
    word_t ACR;
    asm चञ्चल("mrc p15,0,%0,c1,c0,1" : "=r"(ACR));
    प्रत्यागम ACR;
}

स्थिर अन्तःस्थ शून्य writeACR(word_t ACR)
{
    asm चञ्चल("mcr p15,0,%0,c1,c0,1" : : "r"(ACR));
}

शून्य initL2Cache(शून्य)
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

शून्य plat_cleanL2Range(paddr_t start, paddr_t end) {}
शून्य plat_invalidateL2Range(paddr_t start, paddr_t end) {}
शून्य plat_cleanInvalidateL2Range(paddr_t start, paddr_t end) {}
शून्य plat_cleanInvalidateL2Cache(शून्य) {}
