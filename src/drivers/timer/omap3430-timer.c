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

#وصف_ڪريو TIOCP_CFG_SOFTRESET ٻٽو_انگ(1)
#وصف_ڪريو TCLR_AUTORELOAD     ٻٽو_انگ(1)
#وصف_ڪريو TCLR_COMPAREENABLE  ٻٽو_انگ(6)
#وصف_ڪريو TCLR_STARTTIMER     ٻٽو_انگ(0)
#وصف_ڪريو TIER_MATCHENABLE    ٻٽو_انگ(0)
#وصف_ڪريو TIER_OVERFLOWENABLE ٻٽو_انگ(1)

timer_t *timer = (timer_t *) TIMER_PPTR;

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
#وصف_ڪريو INTCPS_SYSCONFIG_SOFTRESET ٻٽو_انگ(1)
#وصف_ڪريو INTCPS_SYSSTATUS_RESETDONE ٻٽو_انگ(0)
uint32_t مٿاهون_بٽ = 0;
BOOT_ڪوڊ فراغ initTimer(فراغ)
{
    /* Configure gptimer9 as kernel timer */
    timer->cfg = TIOCP_CFG_SOFTRESET;

    /* disable */
    timer->tclr = 0;

    /* wait for reset */
    طالما (!timer->tistat);

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
#وإلا /* CONFIG_KERNEL_MCS */
#وصف_ڪريو TIMER_INTERVAL_MS (CONFIG_TIMER_TICK_MS)
BOOT_ڪوڊ فراغ initTimer(فراغ)
{
    /* Configure gptimer9 as kernel timer */
    timer->cfg = TIOCP_CFG_SOFTRESET;

    طالما (!timer->tistat);

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
#جيڪڏهن_پورو /* !CONFIG_KERNEL_MCS */
