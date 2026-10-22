/*
 * Copyright 2015, DornerWorks
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <arch/machine.h>

స్థిర అంతఃవరుస మాట_t readACR(శూన్య)
{
    మాట_t ACR;
    asm అస్థిర("mrc p15,0,%0,c1,c0,1" : "=r"(ACR));
    తిరిగి ACR;
}

స్థిర అంతఃవరుస శూన్య writeACR(మాట_t ACR)
{
    asm అస్థిర("mcr p15,0,%0,c1,c0,1" : : "r"(ACR));
}

శూన్య initL2Cache(శూన్య)
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

శూన్య plat_cleanL2Range(paddr_t ప్రారంభం, paddr_t చివర) {}
శూన్య plat_invalidateL2Range(paddr_t ప్రారంభం, paddr_t చివర) {}
శూన్య plat_cleanInvalidateL2Range(paddr_t ప్రారంభం, paddr_t చివర) {}
శూన్య plat_cleanInvalidateL2Cache(శూన్య) {}
