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

#تعریف_کن TMR_CTRL_ENABLE      بیت(0)
#تعریف_کن TMR_CTRL_AUTORELOAD  بیت(1)
#تعریف_کن TMR_CTRL_IRQEN       بیت(2)
#تعریف_کن TMR_CTRL_PRESCALE    8

#تعریف_کن TIMER_INTERVAL_MS    (CONFIG_TIMER_TICK_MS)
#تعریف_کن TIMER_شمار_بیتها 32

#تعریف_کن PRESCALE ((TIMER_RELOAD) >> TIMER_شمار_بیتها)
#تعریف_کن TMR_بارگیری ((TIMER_RELOAD) / (PRESCALE + 1))

BOOT_کد فراغ initTimer(فراغ)
{
    /* reset */
    priv_timer->ctrl = 0;
    priv_timer->ints = 0;

    /*راهاندازی*/
    priv_timer->بارگیری = TMR_بارگیری;
    priv_timer->ctrl |= ((PRESCALE) << (TMR_CTRL_PRESCALE))
                        | TMR_CTRL_AUTORELOAD | TMR_CTRL_IRQEN;

    /* Enable */
    priv_timer->ctrl |= TMR_CTRL_ENABLE;
}
