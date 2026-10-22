/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <drivers/timer/arm_global.h>

timer_t *സ്ഥിരം globalTimer = (timer_t *) TIMER_PPTR;

/** DONT_TRANSLATE */
BOOT_CODE ശൂന്യം initTimer(ശൂന്യം)
{
    /* disable the timer */
    globalTimer->control = 0;
    /* zero the timer */
    globalTimer->എണ്ണം_Lower = 0;
    globalTimer->എണ്ണം_Upper = 0;
    /* turn it on again, with interrupts on, comparator register off,
     * in one-shot mode, with standard prescaler */
    globalTimer->control = BIT(ENABLE) | BIT(IRQ_ENABLE);

    /* this timer will overflow in about 1000 years */
}
