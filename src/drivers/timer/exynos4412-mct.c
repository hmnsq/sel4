/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <plat/machine.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <drivers/timer/mct.h>

timer_t *mct = (timer_t *) EXYNOS_MCT_PPTR;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS

BOOT_ಸಂಕೇತ ಶೂನ್ಯ initTimer(ಶೂನ್ಯ)
{

    mct_clear_write_ಸ್ಥಿತಿ();
    mct->global.comp0_ಕೂಡಿಸು_inc = 0;
    /* Enable interrupts */
    mct->global.int_en = GINT_COMP0_IRQ;
    ಆಗ (mct->global.wstat != (GWSTAT_COMP0_ಕೂಡಿಸು_INC));
    mct->global.wstat = (GWSTAT_COMP0_ಕೂಡಿಸು_INC);
    /* enable interrupts */
    mct->global.tcon = GTCON_EN | GTCON_COMP0_EN;
    ಆಗ (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;
}

#ಇಲ್ಲದಿದ್ದರೆ /* CONFIG_KERNEL_MCS */

BOOT_ಸಂಕೇತ ಶೂನ್ಯ initTimer(ಶೂನ್ಯ)
{

    mct_clear_write_ಸ್ಥಿತಿ();

    /* Configure the comparator */
    mct->global.comp0_ಕೂಡಿಸು_inc = TIMER_RELOAD;

    uint64_t  comparator_ಮೌಲ್ಯ = ((((uint64_t) mct->global.cnth) << 32llu)
                                  | mct->global.cntl) + TIMER_RELOAD;
    mct->global.comp0h = (uint32_t)(comparator_ಮೌಲ್ಯ >> 32u);
    mct->global.comp0l = (uint32_t) comparator_ಮೌಲ್ಯ;
    /* Enable interrupts */
    mct->global.int_en = GINT_COMP0_IRQ;

    /* Wait for update */
    ಆಗ (mct->global.wstat != (GWSTAT_COMP0H | GWSTAT_COMP0L | GWSTAT_COMP0_ಕೂಡಿಸು_INC));
    mct->global.wstat = (GWSTAT_COMP0H | GWSTAT_COMP0L | GWSTAT_COMP0_ಕೂಡಿಸು_INC);

    /* enable interrupts */
    mct->global.tcon = GTCON_EN | GTCON_COMP0_EN | GTCON_COMP0_AUTOINC;
    ಆಗ (mct->global.wstat != GWSTAT_TCON);
    mct->global.wstat = GWSTAT_TCON;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_MCS */
