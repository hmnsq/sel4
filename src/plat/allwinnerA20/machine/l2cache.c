/*
 * Copyright 2015, DornerWorks
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <arch/machine.h>

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶಬ್ದ_t readACR(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t ACR;
    asm ಅಸ್ಥಿರ("mrc p15,0,%0,c1,c0,1" : "=r"(ACR));
    ಹಿಂತಿರುಗಿ ACR;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ writeACR(ಶಬ್ದ_t ACR)
{
    asm ಅಸ್ಥಿರ("mcr p15,0,%0,c1,c0,1" : : "r"(ACR));
}

ಶೂನ್ಯ initL2Cache(ಶೂನ್ಯ)
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

ಶೂನ್ಯ plat_cleanL2Range(paddr_t ಆರಂಭ, paddr_t ಕೊನೆ) {}
ಶೂನ್ಯ plat_invalidateL2Range(paddr_t ಆರಂಭ, paddr_t ಕೊನೆ) {}
ಶೂನ್ಯ plat_cleanInvalidateL2Range(paddr_t ಆರಂಭ, paddr_t ಕೊನೆ) {}
ಶೂನ್ಯ plat_cleanInvalidateL2Cache(ಶೂನ್ಯ) {}
