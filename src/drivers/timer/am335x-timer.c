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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIOCP_CFG_SOFTRESET ਦੋਆਧਾਰੀ_ਅੰਕ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIER_ਮੇਲ_ENABLE ਦੋਆਧਾਰੀ_ਅੰਕ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIER_ਉਛਾਲ_ENABLE ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIER_COMPARE_ENABLE ਦੋਆਧਾਰੀ_ਅੰਕ(2)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TCLR_STARTTIMER ਦੋਆਧਾਰੀ_ਅੰਕ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TCLR_AUTORELOAD ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TCLR_PRESCALE_ENABLE ਦੋਆਧਾਰੀ_ਅੰਕ(5)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TCLR_COMPAREENABLE ਦੋਆਧਾਰੀ_ਅੰਕ(6)

timer_t *timer = (timer_t *) TIMER_PPTR;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WDT_REG(ਅਧਾਰ, off) ((ਅਸਥਿਰ uint32_t *)((ਅਧਾਰ) + (off)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WDT_REG_WWPS 0x34
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WDT_REG_WSPR 0x48
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WDT_WWPS_PEND_WSPR ਦੋਆਧਾਰੀ_ਅੰਕ(4)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_ਰਜਿਸਟਰ_2(reg, ਨਕਾਬ_2) \
        ਕਰੋ {                    \
            reg |= ਨਕਾਬ_2;        \
        } ਜਦੋਂ((reg & ਨਕਾਬ_2) != ਨਕਾਬ_2);

ਸਥਿਰ BOOT_ਕੋਡ ਖਾਲੀ disableWatchdog(ਖਾਲੀ)
{
    uint32_t wdt = WDT1_PPTR;

    // am335x ref man, sec 20.4.3.8
    *WDT_REG(wdt, WDT_REG_WSPR) = 0xaaaa;
    ਜਦੋਂ ((*WDT_REG(wdt, WDT_REG_WWPS) & WDT_WWPS_PEND_WSPR)) {
        ;
    }
    *WDT_REG(wdt, WDT_REG_WSPR) = 0x5555;
    ਜਦੋਂ ((*WDT_REG(wdt, WDT_REG_WWPS) & WDT_WWPS_PEND_WSPR)) {
        ;
    }
}

/*
 * Enable DMTIMER clocks, otherwise their registers wont be accessible.
 * This could be moved out of kernel.
 */
ਸਥਿਰ BOOT_ਕੋਡ ਖਾਲੀ enableTimers(ਖਾਲੀ)
{
    uint32_t cmper = CMPER_PPTR;

    /* XXX repeat this for DMTIMER4..7 */
    /* select clock - Timer 3 */
    *CMPER_REG(cmper, CMPER_CLKSEL_TIMER3) = CMPER_CKLSEL_MOSC;
    ਜਦੋਂ ((*CMPER_REG(cmper, CMPER_CLKSEL_TIMER3) & RESERVED) != CMPER_CKLSEL_MOSC) {
        ;
    }

    /* enable clock */
    *CMPER_REG(cmper, CMPER_TIMER3_CLKCTRL) = CMPER_CLKCTRL_ENABLE;
    ਜਦੋਂ ((*CMPER_REG(cmper, CMPER_TIMER3_CLKCTRL) & RESERVED) != CMPER_CLKCTRL_ENABLE) {
        ;
    }

    /* select clock - Timer 4 */
    *CMPER_REG(cmper, CMPER_CLKSEL_TIMER4) = CMPER_CKLSEL_MOSC;
    ਜਦੋਂ ((*CMPER_REG(cmper, CMPER_CLKSEL_TIMER4) & RESERVED) != CMPER_CKLSEL_MOSC) {
        ;
    }

    /* enable clock */
    *CMPER_REG(cmper, CMPER_TIMER4_CLKCTRL) = CMPER_CLKCTRL_ENABLE;
    ਜਦੋਂ ((*CMPER_REG(cmper, CMPER_TIMER4_CLKCTRL) & RESERVED) != CMPER_CLKCTRL_ENABLE) {
        ;
    }
}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS

/* The idea here is to use the functionality of overflow interrupts to calculate time
   and use match interrupts for setting a deadline. */

uint32_t ਉੱਚਾ_ਬਿੱਟ = 0;
BOOT_ਕੋਡ ਖਾਲੀ initTimer(ਖਾਲੀ)
{
    ਪੂਰਨ timeout;
    disableWatchdog();
    enableTimers();

    /* Configure dmtimer0 as kernel timer */
    ਸੈੱਟ_ਰਜਿਸਟਰ_2(timer->cfg, TIOCP_CFG_SOFTRESET);

    /* disable */
    ਸੈੱਟ_ਰਜਿਸਟਰ_2(timer->tclr, 0u);

    /* wait for reset */
    ਲਈ (timeout = 10000; (timer->cfg & TIOCP_CFG_SOFTRESET) && timeout > 0; timeout--);

    ਜੇ (!timeout) {
        ਲਿਖੋ("init timer failed\n");
        ਵਾਪਸ;
    }

    ਨਕਾਬ_ਰੁਕਾਵਟ(/*disable*/ true, KERNEL_TIMER_IRQ);

    /* Set the reload value */
    ਸੈੱਟ_ਰਜਿਸਟਰ_2(timer->tldr, 0u);

    /* Enables interrupt on overflow and match */
    ਸੈੱਟ_ਰਜਿਸਟਰ_2(timer->tier, (TIER_ਉਛਾਲ_ENABLE | TIER_ਮੇਲ_ENABLE));

    /* Clear the read register */
    ਸੈੱਟ_ਰਜਿਸਟਰ_2(timer->tcrr, 0u);

    /* start the timer */
    ਸੈੱਟ_ਰਜਿਸਟਰ_2(timer->tclr, (TCLR_AUTORELOAD | TCLR_STARTTIMER | TCLR_COMPAREENABLE));
}

#ਨਹੀਂਤਾਂ /* CONFIG_KERNEL_MCS */


/* Configure dmtimer0 as kernel preemption timer */
BOOT_ਕੋਡ ਖਾਲੀ initTimer(ਖਾਲੀ)
{
    ਪੂਰਨ timeout;

    disableWatchdog();
    enableTimers();

    timer->cfg = TIOCP_CFG_SOFTRESET;

    ਲਈ (timeout = 10000; (timer->cfg & TIOCP_CFG_SOFTRESET) && timeout > 0; timeout--)
        ;
    ਜੇ (!timeout) {
        ਲਿਖੋ("init timer failed\n");
        ਵਾਪਸ;
    }

    ਨਕਾਬ_ਰੁਕਾਵਟ(/*disable*/ true, KERNEL_TIMER_IRQ);

    /* Set the reload value */
    timer->tldr = 0xFFFFFFFFUL - TIMER_RELOAD;

    /* Enables interrupt on overflow */
    timer->tier = TIER_ਉਛਾਲ_ENABLE;

    /* Clear the read register */
    timer->tcrr = 0xFFFFFFFFUL - TIMER_RELOAD;

    /* Set autoreload and start the timer */
    timer->tclr = TCLR_AUTORELOAD | TCLR_STARTTIMER;
}
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_MCS */
