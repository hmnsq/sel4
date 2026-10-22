/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <types.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <kernel/vspace.h>
#অন্তর্ভাব <arch/machine.h>
#অন্তর্ভাব <arch/kernel/vspace.h>
#অন্তর্ভাব <plat/machine.h>
#অন্তর্ভাব <linker.h>
#অন্তর্ভাব <plat/machine/devices_gen.h>
#অন্তর্ভাব <plat/machine/hardware.h>
#অন্তর্ভাব <drivers/timer/arm_generic.h>
#অন্তর্ভাব <drivers/timer/mct.h>

timer_t *mct = (timer_t *) EXYNOS_MCT_PPTR;

BOOT_CODE শূন্য initTimer(শূন্য)
{
    mct_clear_write_status();

    /* use the arm generic timer, backed by the mct */
    /* enable the timer */
    mct->global.tcon = GTCON_EN;
    যাবত্ (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;

    initGenericTimer();
}

