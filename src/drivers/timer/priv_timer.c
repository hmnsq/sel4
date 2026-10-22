/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

/* A9 MPCORE private timer */
#ಅನ್ತರ್ಭಾವ <machine/timer.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/timer.h>
#ಅನ್ತರ್ಭಾವ <drivers/timer/arm_priv.h>

timer_t *ಸ್ಥಿರಾಂಕ priv_timer = (timer_t *) ARM_MP_PRIV_TIMER_PPTR;

#ವ್ಯಾಖ್ಯಾನಿಸು TMR_CTRL_ENABLE      ಬಿಟ್(0)
#ವ್ಯಾಖ್ಯಾನಿಸು TMR_CTRL_AUTORELOAD  ಬಿಟ್(1)
#ವ್ಯಾಖ್ಯಾನಿಸು TMR_CTRL_IRQEN       ಬಿಟ್(2)
#ವ್ಯಾಖ್ಯಾನಿಸು TMR_CTRL_PRESCALE    8

#ವ್ಯಾಖ್ಯಾನಿಸು TIMER_INTERVAL_MS    (CONFIG_TIMER_TICK_MS)
#ವ್ಯಾಖ್ಯಾನಿಸು TIMER_ಎಣಿಕೆ_ಬಿಟ್ಟುಗಳು 32

#ವ್ಯಾಖ್ಯಾನಿಸು PRESCALE ((TIMER_RELOAD) >> TIMER_ಎಣಿಕೆ_ಬಿಟ್ಟುಗಳು)
#ವ್ಯಾಖ್ಯಾನಿಸು TMR_ತುಂಬು ((TIMER_RELOAD) / (PRESCALE + 1))

BOOT_ಸಂಕೇತ ಶೂನ್ಯ initTimer(ಶೂನ್ಯ)
{
    /* reset */
    priv_timer->ctrl = 0;
    priv_timer->ints = 0;

    /*ಸಿದ್ಧತೆ*/
    priv_timer->ತುಂಬು = TMR_ತುಂಬು;
    priv_timer->ctrl |= ((PRESCALE) << (TMR_CTRL_PRESCALE))
                        | TMR_CTRL_AUTORELOAD | TMR_CTRL_IRQEN;

    /* Enable */
    priv_timer->ctrl |= TMR_CTRL_ENABLE;
}
