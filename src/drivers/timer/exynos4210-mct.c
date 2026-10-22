/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <machine/io.h>
#समावेश <kernel/vspace.h>
#समावेश <arch/machine.h>
#समावेश <arch/kernel/vspace.h>
#समावेश <plat/machine.h>
#समावेश <linker.h>
#समावेश <plat/machine/devices_gen.h>
#समावेश <plat/machine/hardware.h>
#समावेश <drivers/timer/arm_generic.h>
#समावेश <drivers/timer/mct.h>

timer_t *mct = (timer_t *) EXYNOS_MCT_PPTR;

BOOT_CODE व्योम initTimer(व्योम)
{
    mct_clear_write_status();

    /* use the arm generic timer, backed by the mct */
    /* enable the timer */
    mct->global.tcon = GTCON_EN;
    जबतक (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;

    initGenericTimer();
}

