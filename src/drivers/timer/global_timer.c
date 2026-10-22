/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <drivers/timer/arm_global.h>

timer_t *ಸ್ಥಿರಾಂಕ globalTimer = (timer_t *) TIMER_PPTR;

/** DONT_TRANSLATE */
BOOT_ಸಂಕೇತ ಶೂನ್ಯ initTimer(ಶೂನ್ಯ)
{
    /* disable the timer */
    globalTimer->control = 0;
    /* zero the timer */
    globalTimer->ಎಣಿಕೆ_ಅಧೋ = 0;
    globalTimer->ಎಣಿಕೆ_ಊರ್ಧ್ವ = 0;
    /* turn it on again, with interrupts on, comparator register off,
     * in one-shot mode, with standard prescaler */
    globalTimer->control = ಬಿಟ್(ENABLE) | ಬಿಟ್(IRQ_ENABLE);

    /* this timer will overflow in about 1000 years */
}
