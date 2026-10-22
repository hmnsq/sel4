/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <types.h>
#ضمن <machine/io.h>
#ضمن <kernel/vspace.h>
#ضمن <arch/machine.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <plat/machine.h>
#ضمن <linker.h>
#ضمن <plat/machine/devices_gen.h>
#ضمن <plat/machine/hardware.h>

#تعریف_کرو TIOCP_CFG_SOFTRESET ثنائی_ہندسہ(1)
#تعریف_کرو TCLR_AUTORELOAD     ثنائی_ہندسہ(1)
#تعریف_کرو TCLR_COMPAREENABLE  ثنائی_ہندسہ(6)
#تعریف_کرو TCLR_STARTTIMER     ثنائی_ہندسہ(0)
#تعریف_کرو TIER_MATCHENABLE    ثنائی_ہندسہ(0)
#تعریف_کرو TIER_OVERFLOWENABLE ثنائی_ہندسہ(1)

timer_t *timer = (timer_t *) TIMER_PPTR;

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
#تعریف_کرو INTCPS_SYSCONFIG_SOFTRESET ثنائی_ہندسہ(1)
#تعریف_کرو INTCPS_SYSSTATUS_RESETDONE ثنائی_ہندسہ(0)
uint32_t بلند_بٹ = 0;
BOOT_کوڈ فراغ initTimer(فراغ)
{
    /* Configure gptimer9 as kernel timer */
    timer->cfg = TIOCP_CFG_SOFTRESET;

    /* disable */
    timer->tclr = 0;

    /* wait for reset */
    طالما (!timer->tistat);

    نقاب_رکاوٹ(/*disable*/ true, KERNEL_TIMER_IRQ);

    /* Set the reload value */
    timer->tldr = 0u;

    /* Enables interrupt on overflow and match */
    timer->tier |= (TIER_OVERFLOWENABLE | TIER_MATCHENABLE);

    /* Clear the read register */
    timer->tcrr = 0u;

    /* start the timer */
    timer->tclr = TCLR_AUTORELOAD | TCLR_STARTTIMER | TCLR_COMPAREENABLE;
}
#وإلا /* CONFIG_KERNEL_MCS */
#تعریف_کرو TIMER_INTERVAL_MS (CONFIG_TIMER_TICK_MS)
BOOT_کوڈ فراغ initTimer(فراغ)
{
    /* Configure gptimer9 as kernel timer */
    timer->cfg = TIOCP_CFG_SOFTRESET;

    طالما (!timer->tistat);

    نقاب_رکاوٹ(/*disable*/ true, KERNEL_TIMER_IRQ);

    /* Set the reload value */
    timer->tldr = 0xFFFFFFFFUL - TIMER_RELOAD;

    /* Enables interrupt on overflow */
    timer->tier = TIER_OVERFLOWENABLE;

    /* Clear the read register */
    timer->tcrr = 0xFFFFFFFFUL - TIMER_RELOAD;

    /* Set autoreload and start the timer */
    timer->tclr = TCLR_AUTORELOAD | TCLR_STARTTIMER;
}
#جے_انت /* !CONFIG_KERNEL_MCS */
