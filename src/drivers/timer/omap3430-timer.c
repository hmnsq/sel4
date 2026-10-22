/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <machine/io.h>
#समावेश <kernel/vspace.h>
#समावेश <arch/machine.h>
#समावेश <arch/kernel/vspace.h>
#समावेश <plat/machine.h>
#समावेश <linker.h>
#समावेश <plat/machine/devices_gen.h>
#समावेश <plat/machine/hardware.h>

#घोषणा TIOCP_CFG_SOFTRESET BIT(1)
#घोषणा TCLR_AUTORELOAD     BIT(1)
#घोषणा TCLR_COMPAREENABLE  BIT(6)
#घोषणा TCLR_STARTTIMER     BIT(0)
#घोषणा TIER_MATCHENABLE    BIT(0)
#घोषणा TIER_OVERFLOWENABLE BIT(1)

timer_t *timer = (timer_t *) TIMER_PPTR;

#अगर_घोषित CONFIG_KERNEL_MCS
#घोषणा INTCPS_SYSCONFIG_SOFTRESET BIT(1)
#घोषणा INTCPS_SYSSTATUS_RESETDONE BIT(0)
uint32_t ऊपरी_bits = 0;
BOOT_CODE व्योम initTimer(व्योम)
{
    /* Configure gptimer9 as kernel timer */
    timer->cfg = TIOCP_CFG_SOFTRESET;

    /* disable */
    timer->tclr = 0;

    /* wait for reset */
    जबतक (!timer->tistat);

    maskInterrupt(/*disable*/ true, KERNEL_TIMER_IRQ);

    /* Set the reload value */
    timer->tldr = 0u;

    /* Enables interrupt on overflow and match */
    timer->tier |= (TIER_OVERFLOWENABLE | TIER_MATCHENABLE);

    /* Clear the read register */
    timer->tcrr = 0u;

    /* start the timer */
    timer->tclr = TCLR_AUTORELOAD | TCLR_STARTTIMER | TCLR_COMPAREENABLE;
}
#अन्यथा /* CONFIG_KERNEL_MCS */
#घोषणा TIMER_INTERVAL_MS (CONFIG_TIMER_TICK_MS)
BOOT_CODE व्योम initTimer(व्योम)
{
    /* Configure gptimer9 as kernel timer */
    timer->cfg = TIOCP_CFG_SOFTRESET;

    जबतक (!timer->tistat);

    maskInterrupt(/*disable*/ true, KERNEL_TIMER_IRQ);

    /* Set the reload value */
    timer->tldr = 0xFFFFFFFFUL - TIMER_RELOAD;

    /* Enables interrupt on overflow */
    timer->tier = TIER_OVERFLOWENABLE;

    /* Clear the read register */
    timer->tcrr = 0xFFFFFFFFUL - TIMER_RELOAD;

    /* Set autoreload and start the timer */
    timer->tclr = TCLR_AUTORELOAD | TCLR_STARTTIMER;
}
#पूर्ण_अगर /* !CONFIG_KERNEL_MCS */
