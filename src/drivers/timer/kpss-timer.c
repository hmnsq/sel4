/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

/* @AUTHOR(akroh@ertos.nicta.com.au) */

#ضمن <plat/machine/hardware.h>

#وصف_ڪريو DGT_TIMER_PPTR (TIMER_PPTR + 0x024)

بنية timer {
    uint32_t mtch;        /* +0x024 */
    uint32_t cnt;         /* +0x028 */
    uint32_t en;          /* +0x02C */
    uint32_t clr;         /* +0x030 */
    uint32_t clk_ctl;     /* +0x034 */
};
تعريفنوع متطاير بنية timer timer_t;
timer_t *dgt_tmr = (timer_t *) DGT_TIMER_PPTR;

#وصف_ڪريو DGTTMR_EN_CLR_ON_MTCH_EN    (1U << 1)
#وصف_ڪريو DGTTMR_EN_EN                (1U << 0)
#وصف_ڪريو DGTTMR_CLK_CTRL(x)          ((x) << 0)
#وصف_ڪريو DGTTMR_CLK_CTRL_DIV1        DGTTMR_CLK_CTRL(0x0)
#وصف_ڪريو DGTTMR_CLK_CTRL_DIV2        DGTTMR_CLK_CTRL(0x1)
#وصف_ڪريو DGTTMR_CLK_CTRL_DIV3        DGTTMR_CLK_CTRL(0x2)
#وصف_ڪريو DGTTMR_CLK_CTRL_DIV4        DGTTMR_CLK_CTRL(0x3)
#وصف_ڪريو DGTTMR_CLK_CTRL_نقاب        DGTTMR_CLK_CTRL(0x3)

#وصف_ڪريو PRESCALER          DGTTMR_CLK_CTRL_DIV1
#وصف_ڪريو PRESCALE_VAL       1

#وصف_ڪريو TIMER_ميلاپ_VAL  (TIMER_RELOAD / PRESCALE_VAL)

BOOT_ڪوڊ فراغ initTimer(فراغ)
{
    /* Stop the timer */
    dgt_tmr->en = 0;
    dgt_tmr->clr = 0xC0FFEE;
    /* Configure the timer */
    dgt_tmr->mtch = TIMER_ميلاپ_VAL;
    /* Start the timer */
    dgt_tmr->en = DGTTMR_EN_CLR_ON_MTCH_EN | DGTTMR_EN_EN;
}
