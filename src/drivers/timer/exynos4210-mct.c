/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine.h>
#ଅନ୍ତର୍ଭାଵ <linker.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/devices_gen.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/hardware.h>
#ଅନ୍ତର୍ଭାଵ <drivers/timer/arm_generic.h>
#ଅନ୍ତର୍ଭାଵ <drivers/timer/mct.h>

timer_t *mct = (timer_t *) EXYNOS_MCT_PPTR;

BOOT_CODE ଶୂନ୍ୟ initTimer(ଶୂନ୍ୟ)
{
    mct_clear_write_status();

    /* use the arm generic timer, backed by the mct */
    /* enable the timer */
    mct->global.tcon = GTCON_EN;
    ଯେତେବେଳେ (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;

    initGenericTimer();
}

