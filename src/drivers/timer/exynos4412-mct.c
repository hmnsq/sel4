/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/timer/mct.h>

timer_t *mct = (timer_t *) EXYNOS_MCT_PPTR;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS

BOOT_ਕੋਡ ਖਾਲੀ initTimer(ਖਾਲੀ)
{

    mct_clear_write_ਸਥਿਤੀ();
    mct->global.comp0_ਜੋੜੋ_inc = 0;
    /* Enable interrupts */
    mct->global.int_en = GINT_COMP0_IRQ;
    ਜਦੋਂ (mct->global.wstat != (GWSTAT_COMP0_ਜੋੜੋ_INC));
    mct->global.wstat = (GWSTAT_COMP0_ਜੋੜੋ_INC);
    /* enable interrupts */
    mct->global.tcon = GTCON_EN | GTCON_COMP0_EN;
    ਜਦੋਂ (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;
}

#ਨਹੀਂਤਾਂ /* CONFIG_KERNEL_MCS */

BOOT_ਕੋਡ ਖਾਲੀ initTimer(ਖਾਲੀ)
{

    mct_clear_write_ਸਥਿਤੀ();

    /* Configure the comparator */
    mct->global.comp0_ਜੋੜੋ_inc = TIMER_RELOAD;

    uint64_t  comparator_ਮੁੱਲ = ((((uint64_t) mct->global.cnth) << 32llu)
                                  | mct->global.cntl) + TIMER_RELOAD;
    mct->global.comp0h = (uint32_t)(comparator_ਮੁੱਲ >> 32u);
    mct->global.comp0l = (uint32_t) comparator_ਮੁੱਲ;
    /* Enable interrupts */
    mct->global.int_en = GINT_COMP0_IRQ;

    /* Wait for update */
    ਜਦੋਂ (mct->global.wstat != (GWSTAT_COMP0H | GWSTAT_COMP0L | GWSTAT_COMP0_ਜੋੜੋ_INC));
    mct->global.wstat = (GWSTAT_COMP0H | GWSTAT_COMP0L | GWSTAT_COMP0_ਜੋੜੋ_INC);

    /* enable interrupts */
    mct->global.tcon = GTCON_EN | GTCON_COMP0_EN | GTCON_COMP0_AUTOINC;
    ਜਦੋਂ (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;
}
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_MCS */
