/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <kernel/vspace.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/kernel/vspace.h>
#अन्तर्भाव <plat/machine.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine/devices_gen.h>
#अन्तर्भाव <plat/machine/hardware.h>
#अन्तर्भाव <drivers/timer/arm_generic.h>
#अन्तर्भाव <drivers/timer/mct.h>

timer_t *mct = (timer_t *) EXYNOS_MCT_PPTR;

BOOT_CODE शून्य initTimer(शून्य)
{
    mct_clear_write_status();

    /* use the arm generic timer, backed by the mct */
    /* enable the timer */
    mct->global.tcon = GTCON_EN;
    यावत् (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;

    initGenericTimer();
}

