/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <types.h>
#ضمن <plat/machine.h>
#ضمن <linker.h>
#ضمن <drivers/timer/mct.h>

timer_t *mct = (timer_t *) EXYNOS_MCT_PPTR;

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS

BOOT_ڪوڊ فراغ initTimer(فراغ)
{

    mct_clear_write_حالت();
    mct->global.comp0_جوڙيو_inc = 0;
    /* Enable interrupts */
    mct->global.int_en = GINT_COMP0_IRQ;
    طالما (mct->global.wstat != (GWSTAT_COMP0_جوڙيو_INC));
    mct->global.wstat = (GWSTAT_COMP0_جوڙيو_INC);
    /* enable interrupts */
    mct->global.tcon = GTCON_EN | GTCON_COMP0_EN;
    طالما (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;
}

#وإلا /* CONFIG_KERNEL_MCS */

BOOT_ڪوڊ فراغ initTimer(فراغ)
{

    mct_clear_write_حالت();

    /* Configure the comparator */
    mct->global.comp0_جوڙيو_inc = TIMER_RELOAD;

    uint64_t  comparator_قدر = ((((uint64_t) mct->global.cnth) << 32llu)
                                  | mct->global.cntl) + TIMER_RELOAD;
    mct->global.comp0h = (uint32_t)(comparator_قدر >> 32u);
    mct->global.comp0l = (uint32_t) comparator_قدر;
    /* Enable interrupts */
    mct->global.int_en = GINT_COMP0_IRQ;

    /* Wait for update */
    طالما (mct->global.wstat != (GWSTAT_COMP0H | GWSTAT_COMP0L | GWSTAT_COMP0_جوڙيو_INC));
    mct->global.wstat = (GWSTAT_COMP0H | GWSTAT_COMP0L | GWSTAT_COMP0_جوڙيو_INC);

    /* enable interrupts */
    mct->global.tcon = GTCON_EN | GTCON_COMP0_EN | GTCON_COMP0_AUTOINC;
    طالما (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;
}
#جيڪڏهن_پورو /* CONFIG_KERNEL_MCS */
