/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

/* @AUTHOR(akroh@ertos.nicta.com.au) */

#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DGT_TIMER_PPTR (TIMER_PPTR + 0x024)

ਬਣਤਰ timer {
    uint32_t mtch;        /* +0x024 */
    uint32_t cnt;         /* +0x028 */
    uint32_t en;          /* +0x02C */
    uint32_t clr;         /* +0x030 */
    uint32_t clk_ctl;     /* +0x034 */
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅਸਥਿਰ ਬਣਤਰ timer timer_t;
timer_t *dgt_tmr = (timer_t *) DGT_TIMER_PPTR;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DGTTMR_EN_CLR_ON_MTCH_EN    (1U << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DGTTMR_EN_EN                (1U << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DGTTMR_CLK_CTRL(x)          ((x) << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DGTTMR_CLK_CTRL_DIV1        DGTTMR_CLK_CTRL(0x0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DGTTMR_CLK_CTRL_DIV2        DGTTMR_CLK_CTRL(0x1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DGTTMR_CLK_CTRL_DIV3        DGTTMR_CLK_CTRL(0x2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DGTTMR_CLK_CTRL_DIV4        DGTTMR_CLK_CTRL(0x3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DGTTMR_CLK_CTRL_ਨਕਾਬ        DGTTMR_CLK_CTRL(0x3)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRESCALER          DGTTMR_CLK_CTRL_DIV1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRESCALE_VAL       1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIMER_ਮੇਲ_VAL  (TIMER_RELOAD / PRESCALE_VAL)

BOOT_ਕੋਡ ਖਾਲੀ initTimer(ਖਾਲੀ)
{
    /* Stop the timer */
    dgt_tmr->en = 0;
    dgt_tmr->clr = 0xC0FFEE;
    /* Configure the timer */
    dgt_tmr->mtch = TIMER_ਮੇਲ_VAL;
    /* Start the timer */
    dgt_tmr->en = DGTTMR_EN_CLR_ON_MTCH_EN | DGTTMR_EN_EN;
}
