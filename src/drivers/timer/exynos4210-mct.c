/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <kernel/vspace.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <arch/kernel/vspace.h>
#అన్తర్భావ <plat/machine.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/devices_gen.h>
#అన్తర్భావ <plat/machine/hardware.h>
#అన్తర్భావ <drivers/timer/arm_generic.h>
#అన్తర్భావ <drivers/timer/mct.h>

timer_t *mct = (timer_t *) EXYNOS_MCT_PPTR;

BOOT_సంకేతం శూన్య initTimer(శూన్య)
{
    mct_clear_write_స్థితి();

    /* use the arm generic timer, backed by the mct */
    /* enable the timer */
    mct->global.tcon = GTCON_EN;
    అయితే (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;

    initGenericTimer();
}

