/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

/* A9 MPCORE private timer */
#समावेश <machine/timer.h>
#समावेश <arch/machine/timer.h>
#समावेश <drivers/timer/arm_priv.h>

timer_t *स्थिर priv_timer = (timer_t *) ARM_MP_PRIV_TIMER_PPTR;

#घोषणा TMR_CTRL_ENABLE      BIT(0)
#घोषणा TMR_CTRL_AUTORELOAD  BIT(1)
#घोषणा TMR_CTRL_IRQEN       BIT(2)
#घोषणा TMR_CTRL_PRESCALE    8

#घोषणा TIMER_INTERVAL_MS    (CONFIG_TIMER_TICK_MS)
#घोषणा TIMER_गिनती_BITS 32

#घोषणा PRESCALE ((TIMER_RELOAD) >> TIMER_गिनती_BITS)
#घोषणा TMR_LOAD ((TIMER_RELOAD) / (PRESCALE + 1))

BOOT_CODE व्योम initTimer(व्योम)
{
    /* reset */
    priv_timer->ctrl = 0;
    priv_timer->ints = 0;

    /* setup */
    priv_timer->load = TMR_LOAD;
    priv_timer->ctrl |= ((PRESCALE) << (TMR_CTRL_PRESCALE))
                        | TMR_CTRL_AUTORELOAD | TMR_CTRL_IRQEN;

    /* Enable */
    priv_timer->ctrl |= TMR_CTRL_ENABLE;
}
