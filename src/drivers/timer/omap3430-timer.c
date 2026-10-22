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

#تعریف_کن TIOCP_CFG_SOFTRESET بیت(1)
#تعریف_کن TCLR_AUTORELOAD     بیت(1)
#تعریف_کن TCLR_COMPAREENABLE  بیت(6)
#تعریف_کن TCLR_STARTTIMER     بیت(0)
#تعریف_کن TIER_MATCHENABLE    بیت(0)
#تعریف_کن TIER_OVERFLOWENABLE بیت(1)

timer_t *timer = (timer_t *) TIMER_PPTR;

#اگر_تعریف_شده CONFIG_KERNEL_MCS
#تعریف_کن INTCPS_SYSCONFIG_SOFTRESET بیت(1)
#تعریف_کن INTCPS_SYSSTATUS_RESETDONE بیت(0)
uint32_t بالا_بیتها = 0;
BOOT_کد فراغ initTimer(فراغ)
{
    /* Configure gptimer9 as kernel timer */
    timer->cfg = TIOCP_CFG_SOFTRESET;

    /* disable */
    timer->tclr = 0;

    /* wait for reset */
    طالما (!timer->tistat);

    ماسک_وقفه(/*disable*/ true, KERNEL_TIMER_IRQ);

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
#تعریف_کن TIMER_INTERVAL_MS (CONFIG_TIMER_TICK_MS)
BOOT_کد فراغ initTimer(فراغ)
{
    /* Configure gptimer9 as kernel timer */
    timer->cfg = TIOCP_CFG_SOFTRESET;

    طالما (!timer->tistat);

    ماسک_وقفه(/*disable*/ true, KERNEL_TIMER_IRQ);

    /* Set the reload value */
    timer->tldr = 0xFFFFFFFFUL - TIMER_RELOAD;

    /* Enables interrupt on overflow */
    timer->tier = TIER_OVERFLOWENABLE;

    /* Clear the read register */
    timer->tcrr = 0xFFFFFFFFUL - TIMER_RELOAD;

    /* Set autoreload and start the timer */
    timer->tclr = TCLR_AUTORELOAD | TCLR_STARTTIMER;
}
#پایان_اگر /* !CONFIG_KERNEL_MCS */
