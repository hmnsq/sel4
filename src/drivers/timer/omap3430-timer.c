/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/devices_gen.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIOCP_CFG_SOFTRESET ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TCLR_AUTORELOAD     ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TCLR_COMPAREENABLE  ਦੋਆਧਾਰੀ_ਅੰਕ(6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TCLR_STARTTIMER     ਦੋਆਧਾਰੀ_ਅੰਕ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIER_MATCHENABLE    ਦੋਆਧਾਰੀ_ਅੰਕ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIER_OVERFLOWENABLE ਦੋਆਧਾਰੀ_ਅੰਕ(1)

timer_t *timer = (timer_t *) TIMER_PPTR;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INTCPS_SYSCONFIG_SOFTRESET ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INTCPS_SYSSTATUS_RESETDONE ਦੋਆਧਾਰੀ_ਅੰਕ(0)
uint32_t ਉੱਚਾ_ਬਿੱਟ = 0;
BOOT_ਕੋਡ ਖਾਲੀ initTimer(ਖਾਲੀ)
{
    /* Configure gptimer9 as kernel timer */
    timer->cfg = TIOCP_CFG_SOFTRESET;

    /* disable */
    timer->tclr = 0;

    /* wait for reset */
    ਜਦੋਂ (!timer->tistat);

    ਨਕਾਬ_ਰੁਕਾਵਟ(/*disable*/ true, KERNEL_TIMER_IRQ);

    /* Set the reload value */
    timer->tldr = 0u;

    /* Enables interrupt on overflow and match */
    timer->tier |= (TIER_OVERFLOWENABLE | TIER_MATCHENABLE);

    /* Clear the read register */
    timer->tcrr = 0u;

    /* start the timer */
    timer->tclr = TCLR_AUTORELOAD | TCLR_STARTTIMER | TCLR_COMPAREENABLE;
}
#ਨਹੀਂਤਾਂ /* CONFIG_KERNEL_MCS */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIMER_INTERVAL_MS (CONFIG_TIMER_TICK_MS)
BOOT_ਕੋਡ ਖਾਲੀ initTimer(ਖਾਲੀ)
{
    /* Configure gptimer9 as kernel timer */
    timer->cfg = TIOCP_CFG_SOFTRESET;

    ਜਦੋਂ (!timer->tistat);

    ਨਕਾਬ_ਰੁਕਾਵਟ(/*disable*/ true, KERNEL_TIMER_IRQ);

    /* Set the reload value */
    timer->tldr = 0xFFFFFFFFUL - TIMER_RELOAD;

    /* Enables interrupt on overflow */
    timer->tier = TIER_OVERFLOWENABLE;

    /* Clear the read register */
    timer->tcrr = 0xFFFFFFFFUL - TIMER_RELOAD;

    /* Set autoreload and start the timer */
    timer->tclr = TCLR_AUTORELOAD | TCLR_STARTTIMER;
}
#ਜੇ_ਅੰਤ /* !CONFIG_KERNEL_MCS */
