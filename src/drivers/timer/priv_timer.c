/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

/* A9 MPCORE private timer */
#अन्तर्भाव <machine/timer.h>
#अन्तर्भाव <arch/machine/timer.h>
#अन्तर्भाव <drivers/timer/arm_priv.h>

timer_t *ध्रुव priv_timer = (timer_t *) ARM_MP_PRIV_TIMER_PPTR;

#परिभाषय TMR_CTRL_ENABLE      BIT(0)
#परिभाषय TMR_CTRL_AUTORELOAD  BIT(1)
#परिभाषय TMR_CTRL_IRQEN       BIT(2)
#परिभाषय TMR_CTRL_PRESCALE    8

#परिभाषय TIMER_INTERVAL_MS    (CONFIG_TIMER_TICK_MS)
#परिभाषय TIMER_गणनम्_द्वयङ्काः 32

#परिभाषय PRESCALE ((TIMER_RELOAD) >> TIMER_गणनम्_द्वयङ्काः)
#परिभाषय TMR_आरोपय ((TIMER_RELOAD) / (PRESCALE + 1))

BOOT_संहिता शून्य initTimer(शून्य)
{
    /* reset */
    priv_timer->ctrl = 0;
    priv_timer->ints = 0;

    /*सज्जता*/
    priv_timer->आरोपय = TMR_आरोपय;
    priv_timer->ctrl |= ((PRESCALE) << (TMR_CTRL_PRESCALE))
                        | TMR_CTRL_AUTORELOAD | TMR_CTRL_IRQEN;

    /* Enable */
    priv_timer->ctrl |= TMR_CTRL_ENABLE;
}
