/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <kernel/vspace.h>
#അന്തര്ഭാവ <arch/machine.h>
#അന്തര്ഭാവ <arch/kernel/vspace.h>
#അന്തര്ഭാവ <plat/machine.h>
#അന്തര്ഭാവ <linker.h>
#അന്തര്ഭാവ <plat/machine/devices_gen.h>
#അന്തര്ഭാവ <plat/machine/hardware.h>
#അന്തര്ഭാവ <drivers/timer/arm_generic.h>
#അന്തര്ഭാവ <drivers/timer/mct.h>

timer_t *mct = (timer_t *) EXYNOS_MCT_PPTR;

BOOT_CODE ശൂന്യം initTimer(ശൂന്യം)
{
    mct_clear_write_status();

    /* use the arm generic timer, backed by the mct */
    /* enable the timer */
    mct->global.tcon = GTCON_EN;
    ആകുമ്പോൾ (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;

    initGenericTimer();
}

