/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <types.h>
#অন্তর্ভাৱ <machine/io.h>
#অন্তর্ভাৱ <kernel/vspace.h>
#অন্তর্ভাৱ <arch/machine.h>
#অন্তর্ভাৱ <arch/kernel/vspace.h>
#অন্তর্ভাৱ <plat/machine.h>
#অন্তর্ভাৱ <linker.h>
#অন্তর্ভাৱ <plat/machine/devices_gen.h>
#অন্তর্ভাৱ <plat/machine/hardware.h>
#অন্তর্ভাৱ <drivers/timer/arm_generic.h>
#অন্তর্ভাৱ <drivers/timer/mct.h>

timer_t *mct = (timer_t *) EXYNOS_MCT_PPTR;

BOOT_CODE শূন্য initTimer(শূন্য)
{
    mct_clear_write_status();

    /* use the arm generic timer, backed by the mct */
    /* enable the timer */
    mct->global.tcon = GTCON_EN;
    যেতিয়ালৈ (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;

    initGenericTimer();
}

