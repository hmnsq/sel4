/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

/* @AUTHOR(akroh@ertos.nicta.com.au) */

#अन्तर्भाव <plat/machine/hardware.h>

#परिभाषय DGT_TIMER_PPTR (TIMER_PPTR + 0x024)

संरचना timer {
    uint32_t mtch;        /* +0x024 */
    uint32_t cnt;         /* +0x028 */
    uint32_t en;          /* +0x02C */
    uint32_t clr;         /* +0x030 */
    uint32_t clk_ctl;     /* +0x034 */
};
प्रकारनिर्देश चञ्चल संरचना timer timer_t;
timer_t *dgt_tmr = (timer_t *) DGT_TIMER_PPTR;

#परिभाषय DGTTMR_EN_CLR_ON_MTCH_EN    (1U << 1)
#परिभाषय DGTTMR_EN_EN                (1U << 0)
#परिभाषय DGTTMR_CLK_CTRL(x)          ((x) << 0)
#परिभाषय DGTTMR_CLK_CTRL_DIV1        DGTTMR_CLK_CTRL(0x0)
#परिभाषय DGTTMR_CLK_CTRL_DIV2        DGTTMR_CLK_CTRL(0x1)
#परिभाषय DGTTMR_CLK_CTRL_DIV3        DGTTMR_CLK_CTRL(0x2)
#परिभाषय DGTTMR_CLK_CTRL_DIV4        DGTTMR_CLK_CTRL(0x3)
#परिभाषय DGTTMR_CLK_CTRL_आवरणम्        DGTTMR_CLK_CTRL(0x3)

#परिभाषय PRESCALER          DGTTMR_CLK_CTRL_DIV1
#परिभाषय PRESCALE_VAL       1

#परिभाषय TIMER_मेलनम्_VAL  (TIMER_RELOAD / PRESCALE_VAL)

BOOT_संहिता शून्य initTimer(शून्य)
{
    /* Stop the timer */
    dgt_tmr->en = 0;
    dgt_tmr->clr = 0xC0FFEE;
    /* Configure the timer */
    dgt_tmr->mtch = TIMER_मेलनम्_VAL;
    /* Start the timer */
    dgt_tmr->en = DGTTMR_EN_CLR_ON_MTCH_EN | DGTTMR_EN_EN;
}
