/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

/* A9 MPCORE private timer */
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/timer/arm_priv.h>

timer_t *ਅਚਲ priv_timer = (timer_t *) ARM_MP_PRIV_TIMER_PPTR;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TMR_CTRL_ENABLE      ਦੋਆਧਾਰੀ_ਅੰਕ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TMR_CTRL_AUTORELOAD  ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TMR_CTRL_IRQEN       ਦੋਆਧਾਰੀ_ਅੰਕ(2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TMR_CTRL_PRESCALE    8

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIMER_INTERVAL_MS    (CONFIG_TIMER_TICK_MS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIMER_ਗਿਣਤੀ_ਬਿੱਟ 32

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRESCALE ((TIMER_RELOAD) >> TIMER_ਗਿਣਤੀ_ਬਿੱਟ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TMR_ਲੱਦੋ ((TIMER_RELOAD) / (PRESCALE + 1))

BOOT_ਕੋਡ ਖਾਲੀ initTimer(ਖਾਲੀ)
{
    /* reset */
    priv_timer->ctrl = 0;
    priv_timer->ints = 0;

    /*ਤਿਆਰੀ*/
    priv_timer->ਲੱਦੋ = TMR_ਲੱਦੋ;
    priv_timer->ctrl |= ((PRESCALE) << (TMR_CTRL_PRESCALE))
                        | TMR_CTRL_AUTORELOAD | TMR_CTRL_IRQEN;

    /* Enable */
    priv_timer->ctrl |= TMR_CTRL_ENABLE;
}
