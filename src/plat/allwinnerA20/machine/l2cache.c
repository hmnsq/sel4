/*
 * Copyright 2015, DornerWorks
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <arch/machine.h>

நிலையான உள்வரி word_t readACR(வெறுமை)
{
    word_t ACR;
    asm நிலையற்ற("mrc p15,0,%0,c1,c0,1" : "=r"(ACR));
    திரும்பு ACR;
}

நிலையான உள்வரி வெறுமை writeACR(word_t ACR)
{
    asm நிலையற்ற("mcr p15,0,%0,c1,c0,1" : : "r"(ACR));
}

வெறுமை initL2Cache(வெறுமை)
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

வெறுமை plat_cleanL2Range(paddr_t start, paddr_t end) {}
வெறுமை plat_invalidateL2Range(paddr_t start, paddr_t end) {}
வெறுமை plat_cleanInvalidateL2Range(paddr_t start, paddr_t end) {}
வெறுமை plat_cleanInvalidateL2Cache(வெறுமை) {}
