/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <types.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <kernel/vspace.h>
#அந்தர்பாவ <arch/machine.h>
#அந்தர்பாவ <arch/kernel/vspace.h>
#அந்தர்பாவ <plat/machine.h>
#அந்தர்பாவ <linker.h>
#அந்தர்பாவ <plat/machine/devices_gen.h>
#அந்தர்பாவ <plat/machine/hardware.h>
#அந்தர்பாவ <drivers/timer/arm_generic.h>
#அந்தர்பாவ <drivers/timer/mct.h>

timer_t *mct = (timer_t *) EXYNOS_MCT_PPTR;

BOOT_CODE வெறுமை initTimer(வெறுமை)
{
    mct_clear_write_status();

    /* use the arm generic timer, backed by the mct */
    /* enable the timer */
    mct->global.tcon = GTCON_EN;
    போது (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;

    initGenericTimer();
}

