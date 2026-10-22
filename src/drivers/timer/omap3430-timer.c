/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <machine/io.h>
#شامل <kernel/vspace.h>
#شامل <arch/machine.h>
#شامل <arch/kernel/vspace.h>
#شامل <plat/machine.h>
#شامل <linker.h>
#شامل <plat/machine/devices_gen.h>
#شامل <plat/machine/hardware.h>

#تعریف_کرو TIOCP_CFG_SOFTRESET ثنائی_ہندسہ(1)
#تعریف_کرو TCLR_AUTORELOAD     ثنائی_ہندسہ(1)
#تعریف_کرو TCLR_COMPAREENABLE  ثنائی_ہندسہ(6)
#تعریف_کرو TCLR_STARTTIMER     ثنائی_ہندسہ(0)
#تعریف_کرو TIER_MATCHENABLE    ثنائی_ہندسہ(0)
#تعریف_کرو TIER_OVERFLOWENABLE ثنائی_ہندسہ(1)

timer_t *timer = (timer_t *) TIMER_PPTR;

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
#تعریف_کرو INTCPS_SYSCONFIG_SOFTRESET ثنائی_ہندسہ(1)
#تعریف_کرو INTCPS_SYSSTATUS_RESETDONE ثنائی_ہندسہ(0)
uint32_t بلند_بٹ = 0;
BOOT_رمز خالی initTimer(خالی)
{
    /* Configure gptimer9 as kernel timer */
    timer->cfg = TIOCP_CFG_SOFTRESET;

    /* disable */
    timer->tclr = 0;

    /* wait for reset */
    جب (!timer->tistat);

    نقاب_مداخلت(/*disable*/ true, KERNEL_TIMER_IRQ);

    /* Set the reload value */
    timer->tldr = 0u;

    /* Enables interrupt on overflow and match */
    timer->tier |= (TIER_OVERFLOWENABLE | TIER_MATCHENABLE);

    /* Clear the read register */
    timer->tcrr = 0u;

    /* start the timer */
    timer->tclr = TCLR_AUTORELOAD | TCLR_STARTTIMER | TCLR_COMPAREENABLE;
}
#ورنہ /* CONFIG_KERNEL_MCS */
#تعریف_کرو TIMER_INTERVAL_MS (CONFIG_TIMER_TICK_MS)
BOOT_رمز خالی initTimer(خالی)
{
    /* Configure gptimer9 as kernel timer */
    timer->cfg = TIOCP_CFG_SOFTRESET;

    جب (!timer->tistat);

    نقاب_مداخلت(/*disable*/ true, KERNEL_TIMER_IRQ);

    /* Set the reload value */
    timer->tldr = 0xFFFFFFFFUL - TIMER_RELOAD;

    /* Enables interrupt on overflow */
    timer->tier = TIER_OVERFLOWENABLE;

    /* Clear the read register */
    timer->tcrr = 0xFFFFFFFFUL - TIMER_RELOAD;

    /* Set autoreload and start the timer */
    timer->tclr = TCLR_AUTORELOAD | TCLR_STARTTIMER;
}
#اگر_ختم /* !CONFIG_KERNEL_MCS */
