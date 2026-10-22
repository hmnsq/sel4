/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <arch/machine.h>

ساكن مضمن کلمه_t readACR(فراغ)
{
    کلمه_t ACR;
    asm متطاير("mrc p15,0,%0,c1,c0,1" : "=r"(ACR));
    أرجع ACR;
}

ساكن مضمن فراغ writeACR(کلمه_t ACR)
{
    asm متطاير("mcr p15,0,%0,c1,c0,1" : : "r"(ACR));
}

فراغ initL2Cache(فراغ)
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

فراغ plat_cleanL2Range(paddr_t شروع, paddr_t انتها) {}
فراغ plat_invalidateL2Range(paddr_t شروع, paddr_t انتها) {}
فراغ plat_cleanInvalidateL2Range(paddr_t شروع, paddr_t انتها) {}
فراغ plat_cleanInvalidateL2Cache(فراغ) {}
