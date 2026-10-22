/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

/* A9 MPCORE private timer */
#అన్తర్భావ <machine/timer.h>
#అన్తర్భావ <arch/machine/timer.h>
#అన్తర్భావ <drivers/timer/arm_priv.h>

timer_t *స్థిరరాశి priv_timer = (timer_t *) ARM_MP_PRIV_TIMER_PPTR;

#నిర్వచించు TMR_CTRL_ENABLE      బిట్టు(0)
#నిర్వచించు TMR_CTRL_AUTORELOAD  బిట్టు(1)
#నిర్వచించు TMR_CTRL_IRQEN       బిట్టు(2)
#నిర్వచించు TMR_CTRL_PRESCALE    8

#నిర్వచించు TIMER_INTERVAL_MS    (CONFIG_TIMER_TICK_MS)
#నిర్వచించు TIMER_లెక్క_బిట్లు 32

#నిర్వచించు PRESCALE ((TIMER_RELOAD) >> TIMER_లెక్క_బిట్లు)
#నిర్వచించు TMR_ఎక్కించు ((TIMER_RELOAD) / (PRESCALE + 1))

BOOT_సంకేతం శూన్య initTimer(శూన్య)
{
    /* reset */
    priv_timer->ctrl = 0;
    priv_timer->ints = 0;

    /*అమరిక*/
    priv_timer->ఎక్కించు = TMR_ఎక్కించు;
    priv_timer->ctrl |= ((PRESCALE) << (TMR_CTRL_PRESCALE))
                        | TMR_CTRL_AUTORELOAD | TMR_CTRL_IRQEN;

    /* Enable */
    priv_timer->ctrl |= TMR_CTRL_ENABLE;
}
