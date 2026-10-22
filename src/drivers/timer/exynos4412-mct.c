/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <plat/machine.h>
#समावेश <linker.h>
#समावेश <drivers/timer/mct.h>

timer_t *mct = (timer_t *) EXYNOS_MCT_PPTR;

#अगर_घोषित CONFIG_KERNEL_MCS

BOOT_CODE व्योम initTimer(व्योम)
{

    mct_clear_write_status();
    mct->global.comp0_add_inc = 0;
    /* Enable interrupts */
    mct->global.int_en = GINT_COMP0_IRQ;
    जबतक (mct->global.wstat != (GWSTAT_COMP0_ADD_INC));
    mct->global.wstat = (GWSTAT_COMP0_ADD_INC);
    /* enable interrupts */
    mct->global.tcon = GTCON_EN | GTCON_COMP0_EN;
    जबतक (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;
}

#अन्यथा /* CONFIG_KERNEL_MCS */

BOOT_CODE व्योम initTimer(व्योम)
{

    mct_clear_write_status();

    /* Configure the comparator */
    mct->global.comp0_add_inc = TIMER_RELOAD;

    uint64_t  comparator_मूल्य = ((((uint64_t) mct->global.cnth) << 32llu)
                                  | mct->global.cntl) + TIMER_RELOAD;
    mct->global.comp0h = (uint32_t)(comparator_मूल्य >> 32u);
    mct->global.comp0l = (uint32_t) comparator_मूल्य;
    /* Enable interrupts */
    mct->global.int_en = GINT_COMP0_IRQ;

    /* Wait for update */
    जबतक (mct->global.wstat != (GWSTAT_COMP0H | GWSTAT_COMP0L | GWSTAT_COMP0_ADD_INC));
    mct->global.wstat = (GWSTAT_COMP0H | GWSTAT_COMP0L | GWSTAT_COMP0_ADD_INC);

    /* enable interrupts */
    mct->global.tcon = GTCON_EN | GTCON_COMP0_EN | GTCON_COMP0_AUTOINC;
    जबतक (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;
}
#पूर्ण_अगर /* CONFIG_KERNEL_MCS */
