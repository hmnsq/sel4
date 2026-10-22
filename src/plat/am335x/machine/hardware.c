/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <types.h>
#அந்தர்பாவ <plat/machine/hardware.h>

BOOT_CODE வெறுமை initIRQController(வெறுமை)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    போது (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_CODE வெறுமை cpu_initLocalIRQController(வெறுமை) {}
