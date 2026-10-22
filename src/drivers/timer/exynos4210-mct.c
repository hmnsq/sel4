/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <machine/io.h>
#شامل <kernel/vspace.h>
#شامل <arch/machine.h>
#شامل <arch/kernel/vspace.h>
#شامل <plat/machine.h>
#شامل <linker.h>
#شامل <plat/machine/devices_gen.h>
#شامل <plat/machine/hardware.h>
#شامل <drivers/timer/arm_generic.h>
#شامل <drivers/timer/mct.h>

timer_t *mct = (timer_t *) EXYNOS_MCT_PPTR;

BOOT_CODE تهی initTimer(تهی)
{
    mct_clear_write_status();

    /* use the arm generic timer, backed by the mct */
    /* enable the timer */
    mct->global.tcon = GTCON_EN;
    وقتی (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;

    initGenericTimer();
}

