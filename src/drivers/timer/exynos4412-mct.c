/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <plat/machine.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <drivers/timer/mct.h>

timer_t *mct = (timer_t *) EXYNOS_MCT_PPTR;

#నిర్వచితమైతే CONFIG_KERNEL_MCS

BOOT_సంకేతం శూన్య initTimer(శూన్య)
{

    mct_clear_write_స్థితి();
    mct->global.comp0_కూడు_inc = 0;
    /* Enable interrupts */
    mct->global.int_en = GINT_COMP0_IRQ;
    అయితే (mct->global.wstat != (GWSTAT_COMP0_కూడు_INC));
    mct->global.wstat = (GWSTAT_COMP0_కూడు_INC);
    /* enable interrupts */
    mct->global.tcon = GTCON_EN | GTCON_COMP0_EN;
    అయితే (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;
}

#లేకపోతే /* CONFIG_KERNEL_MCS */

BOOT_సంకేతం శూన్య initTimer(శూన్య)
{

    mct_clear_write_స్థితి();

    /* Configure the comparator */
    mct->global.comp0_కూడు_inc = TIMER_RELOAD;

    uint64_t  comparator_విలువ = ((((uint64_t) mct->global.cnth) << 32llu)
                                  | mct->global.cntl) + TIMER_RELOAD;
    mct->global.comp0h = (uint32_t)(comparator_విలువ >> 32u);
    mct->global.comp0l = (uint32_t) comparator_విలువ;
    /* Enable interrupts */
    mct->global.int_en = GINT_COMP0_IRQ;

    /* Wait for update */
    అయితే (mct->global.wstat != (GWSTAT_COMP0H | GWSTAT_COMP0L | GWSTAT_COMP0_కూడు_INC));
    mct->global.wstat = (GWSTAT_COMP0H | GWSTAT_COMP0L | GWSTAT_COMP0_కూడు_INC);

    /* enable interrupts */
    mct->global.tcon = GTCON_EN | GTCON_COMP0_EN | GTCON_COMP0_AUTOINC;
    అయితే (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;
}
#అయితే_ముగింపు /* CONFIG_KERNEL_MCS */
