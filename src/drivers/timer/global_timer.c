/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <drivers/timer/arm_global.h>

timer_t *மாறிலி globalTimer = (timer_t *) TIMER_PPTR;

/** DONT_TRANSLATE */
BOOT_CODE வெறுமை initTimer(வெறுமை)
{
    /* disable the timer */
    globalTimer->control = 0;
    /* zero the timer */
    globalTimer->மொத்தம்_Lower = 0;
    globalTimer->மொத்தம்_Upper = 0;
    /* turn it on again, with interrupts on, comparator register off,
     * in one-shot mode, with standard prescaler */
    globalTimer->control = BIT(ENABLE) | BIT(IRQ_ENABLE);

    /* this timer will overflow in about 1000 years */
}
