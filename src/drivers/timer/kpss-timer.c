/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

/* @AUTHOR(akroh@ertos.nicta.com.au) */

#समावेश <plat/machine/hardware.h>

#घोषणा DGT_TIMER_PPTR (TIMER_PPTR + 0x024)

काष्ठा timer {
    uint32_t mtch;        /* +0x024 */
    uint32_t cnt;         /* +0x028 */
    uint32_t en;          /* +0x02C */
    uint32_t clr;         /* +0x030 */
    uint32_t clk_ctl;     /* +0x034 */
};
प्रकार अस्थिर काष्ठा timer timer_t;
timer_t *dgt_tmr = (timer_t *) DGT_TIMER_PPTR;

#घोषणा DGTTMR_EN_CLR_ON_MTCH_EN    (1U << 1)
#घोषणा DGTTMR_EN_EN                (1U << 0)
#घोषणा DGTTMR_CLK_CTRL(x)          ((x) << 0)
#घोषणा DGTTMR_CLK_CTRL_DIV1        DGTTMR_CLK_CTRL(0x0)
#घोषणा DGTTMR_CLK_CTRL_DIV2        DGTTMR_CLK_CTRL(0x1)
#घोषणा DGTTMR_CLK_CTRL_DIV3        DGTTMR_CLK_CTRL(0x2)
#घोषणा DGTTMR_CLK_CTRL_DIV4        DGTTMR_CLK_CTRL(0x3)
#घोषणा DGTTMR_CLK_CTRL_MASK        DGTTMR_CLK_CTRL(0x3)

#घोषणा PRESCALER          DGTTMR_CLK_CTRL_DIV1
#घोषणा PRESCALE_VAL       1

#घोषणा TIMER_MATCH_VAL  (TIMER_RELOAD / PRESCALE_VAL)

BOOT_CODE व्योम initTimer(व्योम)
{
    /* Stop the timer */
    dgt_tmr->en = 0;
    dgt_tmr->clr = 0xC0FFEE;
    /* Configure the timer */
    dgt_tmr->mtch = TIMER_MATCH_VAL;
    /* Start the timer */
    dgt_tmr->en = DGTTMR_EN_CLR_ON_MTCH_EN | DGTTMR_EN_EN;
}
