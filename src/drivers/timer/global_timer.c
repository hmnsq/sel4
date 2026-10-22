/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <drivers/timer/arm_global.h>

timer_t *స్థిరరాశి globalTimer = (timer_t *) TIMER_PPTR;

/** DONT_TRANSLATE */
BOOT_సంకేతం శూన్య initTimer(శూన్య)
{
    /* disable the timer */
    globalTimer->control = 0;
    /* zero the timer */
    globalTimer->లెక్క_క్రింది = 0;
    globalTimer->లెక్క_పై = 0;
    /* turn it on again, with interrupts on, comparator register off,
     * in one-shot mode, with standard prescaler */
    globalTimer->control = బిట్టు(ENABLE) | బిట్టు(IRQ_ENABLE);

    /* this timer will overflow in about 1000 years */
}
