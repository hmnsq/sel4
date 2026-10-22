/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <arch/machine.h>

ساکن اندرونی لفظ_t readACR(خالی)
{
    لفظ_t ACR;
    asm متلون("mrc p15,0,%0,c1,c0,1" : "=r"(ACR));
    واپس ACR;
}

ساکن اندرونی خالی writeACR(لفظ_t ACR)
{
    asm متلون("mcr p15,0,%0,c1,c0,1" : : "r"(ACR));
}

خالی initL2Cache(خالی)
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

خالی plat_cleanL2Range(paddr_t آغاز, paddr_t انتہا) {}
خالی plat_invalidateL2Range(paddr_t آغاز, paddr_t انتہا) {}
خالی plat_cleanInvalidateL2Range(paddr_t آغاز, paddr_t انتہا) {}
خالی plat_cleanInvalidateL2Cache(خالی) {}
