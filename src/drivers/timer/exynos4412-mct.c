/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <plat/machine.h>
#અન્તર્ભાવ <linker.h>
#અન્તર્ભાવ <drivers/timer/mct.h>

timer_t *mct = (timer_t *) EXYNOS_MCT_PPTR;

#ifdef CONFIG_KERNEL_MCS

BOOT_CODE શૂન્ય initTimer(શૂન્ય)
{

    mct_clear_write_status();
    mct->global.comp0_add_inc = 0;
    /* Enable interrupts */
    mct->global.int_en = GINT_COMP0_IRQ;
    જ્યારે (mct->global.wstat != (GWSTAT_COMP0_ADD_INC));
    mct->global.wstat = (GWSTAT_COMP0_ADD_INC);
    /* enable interrupts */
    mct->global.tcon = GTCON_EN | GTCON_COMP0_EN;
    જ્યારે (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;
}

#નહીંતર /* CONFIG_KERNEL_MCS */

BOOT_CODE શૂન્ય initTimer(શૂન્ય)
{

    mct_clear_write_status();

    /* Configure the comparator */
    mct->global.comp0_add_inc = TIMER_RELOAD;

    uint64_t  comparator_કિંમત = ((((uint64_t) mct->global.cnth) << 32llu)
                                  | mct->global.cntl) + TIMER_RELOAD;
    mct->global.comp0h = (uint32_t)(comparator_કિંમત >> 32u);
    mct->global.comp0l = (uint32_t) comparator_કિંમત;
    /* Enable interrupts */
    mct->global.int_en = GINT_COMP0_IRQ;

    /* Wait for update */
    જ્યારે (mct->global.wstat != (GWSTAT_COMP0H | GWSTAT_COMP0L | GWSTAT_COMP0_ADD_INC));
    mct->global.wstat = (GWSTAT_COMP0H | GWSTAT_COMP0L | GWSTAT_COMP0_ADD_INC);

    /* enable interrupts */
    mct->global.tcon = GTCON_EN | GTCON_COMP0_EN | GTCON_COMP0_AUTOINC;
    જ્યારે (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;
}
#endif /* CONFIG_KERNEL_MCS */
