/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <kernel/vspace.h>
#અન્તર્ભાવ <arch/machine.h>
#અન્તર્ભાવ <arch/kernel/vspace.h>
#અન્તર્ભાવ <plat/machine.h>
#અન્તર્ભાવ <linker.h>
#અન્તર્ભાવ <plat/machine/devices_gen.h>
#અન્તર્ભાવ <plat/machine/hardware.h>
#અન્તર્ભાવ <drivers/timer/arm_generic.h>
#અન્તર્ભાવ <drivers/timer/mct.h>

timer_t *mct = (timer_t *) EXYNOS_MCT_PPTR;

BOOT_CODE શૂન્ય initTimer(શૂન્ય)
{
    mct_clear_write_status();

    /* use the arm generic timer, backed by the mct */
    /* enable the timer */
    mct->global.tcon = GTCON_EN;
    જ્યારે (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;

    initGenericTimer();
}

