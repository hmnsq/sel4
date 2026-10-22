/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <kernel/vspace.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <arch/kernel/vspace.h>
#అన్తర్భావ <plat/machine.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/devices_gen.h>
#అన్తర్భావ <plat/machine/hardware.h>

#నిర్వచించు TIOCP_CFG_SOFTRESET బిట్టు(1)
#నిర్వచించు TCLR_AUTORELOAD     బిట్టు(1)
#నిర్వచించు TCLR_COMPAREENABLE  బిట్టు(6)
#నిర్వచించు TCLR_STARTTIMER     బిట్టు(0)
#నిర్వచించు TIER_MATCHENABLE    బిట్టు(0)
#నిర్వచించు TIER_OVERFLOWENABLE బిట్టు(1)

timer_t *timer = (timer_t *) TIMER_PPTR;

#నిర్వచితమైతే CONFIG_KERNEL_MCS
#నిర్వచించు INTCPS_SYSCONFIG_SOFTRESET బిట్టు(1)
#నిర్వచించు INTCPS_SYSSTATUS_RESETDONE బిట్టు(0)
uint32_t ఎగువ_బిట్లు = 0;
BOOT_సంకేతం శూన్య initTimer(శూన్య)
{
    /* Configure gptimer9 as kernel timer */
    timer->cfg = TIOCP_CFG_SOFTRESET;

    /* disable */
    timer->tclr = 0;

    /* wait for reset */
    అయితే (!timer->tistat);

    ముసుగు_అంతరాయం(/*disable*/ true, KERNEL_TIMER_IRQ);

    /* Set the reload value */
    timer->tldr = 0u;

    /* Enables interrupt on overflow and match */
    timer->tier |= (TIER_OVERFLOWENABLE | TIER_MATCHENABLE);

    /* Clear the read register */
    timer->tcrr = 0u;

    /* start the timer */
    timer->tclr = TCLR_AUTORELOAD | TCLR_STARTTIMER | TCLR_COMPAREENABLE;
}
#లేకపోతే /* CONFIG_KERNEL_MCS */
#నిర్వచించు TIMER_INTERVAL_MS (CONFIG_TIMER_TICK_MS)
BOOT_సంకేతం శూన్య initTimer(శూన్య)
{
    /* Configure gptimer9 as kernel timer */
    timer->cfg = TIOCP_CFG_SOFTRESET;

    అయితే (!timer->tistat);

    ముసుగు_అంతరాయం(/*disable*/ true, KERNEL_TIMER_IRQ);

    /* Set the reload value */
    timer->tldr = 0xFFFFFFFFUL - TIMER_RELOAD;

    /* Enables interrupt on overflow */
    timer->tier = TIER_OVERFLOWENABLE;

    /* Clear the read register */
    timer->tcrr = 0xFFFFFFFFUL - TIMER_RELOAD;

    /* Set autoreload and start the timer */
    timer->tclr = TCLR_AUTORELOAD | TCLR_STARTTIMER;
}
#అయితే_ముగింపు /* !CONFIG_KERNEL_MCS */
