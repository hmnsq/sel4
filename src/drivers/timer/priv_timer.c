/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

/* A9 MPCORE private timer */
#ضمن <machine/timer.h>
#ضمن <arch/machine/timer.h>
#ضمن <drivers/timer/arm_priv.h>

timer_t *ثابت priv_timer = (timer_t *) ARM_MP_PRIV_TIMER_PPTR;

#وصف_ڪريو TMR_CTRL_ENABLE      ٻٽو_انگ(0)
#وصف_ڪريو TMR_CTRL_AUTORELOAD  ٻٽو_انگ(1)
#وصف_ڪريو TMR_CTRL_IRQEN       ٻٽو_انگ(2)
#وصف_ڪريو TMR_CTRL_PRESCALE    8

#وصف_ڪريو TIMER_INTERVAL_MS    (CONFIG_TIMER_TICK_MS)
#وصف_ڪريو TIMER_ڳڻپ_بٽ 32

#وصف_ڪريو PRESCALE ((TIMER_RELOAD) >> TIMER_ڳڻپ_بٽ)
#وصف_ڪريو TMR_ڀريو ((TIMER_RELOAD) / (PRESCALE + 1))

BOOT_ڪوڊ فراغ initTimer(فراغ)
{
    /* reset */
    priv_timer->ctrl = 0;
    priv_timer->ints = 0;

    /*تياري*/
    priv_timer->ڀريو = TMR_ڀريو;
    priv_timer->ctrl |= ((PRESCALE) << (TMR_CTRL_PRESCALE))
                        | TMR_CTRL_AUTORELOAD | TMR_CTRL_IRQEN;

    /* Enable */
    priv_timer->ctrl |= TMR_CTRL_ENABLE;
}
