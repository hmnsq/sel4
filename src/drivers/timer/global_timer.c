/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <drivers/timer/arm_global.h>

timer_t *ثابت globalTimer = (timer_t *) TIMER_PPTR;

/** DONT_TRANSLATE */
BOOT_ڪوڊ فراغ initTimer(فراغ)
{
    /* disable the timer */
    globalTimer->control = 0;
    /* zero the timer */
    globalTimer->ڳڻپ_هيٺاهون = 0;
    globalTimer->ڳڻپ_مٿانهون = 0;
    /* turn it on again, with interrupts on, comparator register off,
     * in one-shot mode, with standard prescaler */
    globalTimer->control = ٻٽو_انگ(ENABLE) | ٻٽو_انگ(IRQ_ENABLE);

    /* this timer will overflow in about 1000 years */
}
