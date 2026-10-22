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

#تعریف_کرو TMR_CTRL_ENABLE      ثنائی_ہندسہ(0)
#تعریف_کرو TMR_CTRL_AUTORELOAD  ثنائی_ہندسہ(1)
#تعریف_کرو TMR_CTRL_IRQEN       ثنائی_ہندسہ(2)
#تعریف_کرو TMR_CTRL_PRESCALE    8

#تعریف_کرو TIMER_INTERVAL_MS    (CONFIG_TIMER_TICK_MS)
#تعریف_کرو TIMER_گنتی_بٹ 32

#تعریف_کرو PRESCALE ((TIMER_RELOAD) >> TIMER_گنتی_بٹ)
#تعریف_کرو TMR_لدو ((TIMER_RELOAD) / (PRESCALE + 1))

BOOT_کوڈ فراغ initTimer(فراغ)
{
    /* reset */
    priv_timer->ctrl = 0;
    priv_timer->ints = 0;

    /*تیاری*/
    priv_timer->لدو = TMR_لدو;
    priv_timer->ctrl |= ((PRESCALE) << (TMR_CTRL_PRESCALE))
                        | TMR_CTRL_AUTORELOAD | TMR_CTRL_IRQEN;

    /* Enable */
    priv_timer->ctrl |= TMR_CTRL_ENABLE;
}
