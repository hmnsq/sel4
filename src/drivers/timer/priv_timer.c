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

#تعریف_کړه TMR_CTRL_ENABLE      بټ(0)
#تعریف_کړه TMR_CTRL_AUTORELOAD  بټ(1)
#تعریف_کړه TMR_CTRL_IRQEN       بټ(2)
#تعریف_کړه TMR_CTRL_PRESCALE    8

#تعریف_کړه TIMER_INTERVAL_MS    (CONFIG_TIMER_TICK_MS)
#تعریف_کړه TIMER_شمېر_بټونه 32

#تعریف_کړه PRESCALE ((TIMER_RELOAD) >> TIMER_شمېر_بټونه)
#تعریف_کړه TMR_بارول ((TIMER_RELOAD) / (PRESCALE + 1))

BOOT_کوډ فراغ initTimer(فراغ)
{
    /* reset */
    priv_timer->ctrl = 0;
    priv_timer->ints = 0;

    /*تیاری*/
    priv_timer->بارول = TMR_بارول;
    priv_timer->ctrl |= ((PRESCALE) << (TMR_CTRL_PRESCALE))
                        | TMR_CTRL_AUTORELOAD | TMR_CTRL_IRQEN;

    /* Enable */
    priv_timer->ctrl |= TMR_CTRL_ENABLE;
}
