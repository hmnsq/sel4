/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/timer/arm_global.h>

timer_t *ਅਚਲ globalTimer = (timer_t *) TIMER_PPTR;

/** DONT_TRANSLATE */
BOOT_ਕੋਡ ਖਾਲੀ initTimer(ਖਾਲੀ)
{
    /* disable the timer */
    globalTimer->control = 0;
    /* zero the timer */
    globalTimer->ਗਿਣਤੀ_ਹੇਠਲਾ = 0;
    globalTimer->ਗਿਣਤੀ_ਉੱਤਲਾ = 0;
    /* turn it on again, with interrupts on, comparator register off,
     * in one-shot mode, with standard prescaler */
    globalTimer->control = ਦੋਆਧਾਰੀ_ਅੰਕ(ENABLE) | ਦੋਆਧਾਰੀ_ਅੰਕ(IRQ_ENABLE);

    /* this timer will overflow in about 1000 years */
}
