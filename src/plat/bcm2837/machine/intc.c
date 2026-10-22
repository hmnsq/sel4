/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <util.h>
#شامل <stdint.h>
#شامل <plat/machine.h>
BOOT_رمز خالی initIRQController(خالی)
{
    /* Disable all interrupts */
    intc_regs->bfDisableIRQs[0] = 0xffffffff;
    intc_regs->bfDisableIRQs[1] = 0xffffffff;
    intc_regs->bfDisableBasicIRQs = 0xffffffff;
    /* Disable FIQ */
    intc_regs->FIQ_control &= ~FIQCTRL_FIQ_ENABLE;
    /* Enable IRQ control for GPU */
    intc_regs->bfEnableBasicIRQs = ثنائی_ہندسہ(مداخلت_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_انحراف);
    intc_regs->bfEnableBasicIRQs = ثنائی_ہندسہ(مداخلت_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_انحراف);

    core_regs->coreTimerPrescaler = 0x80000000;
}

BOOT_رمز خالی cpu_initLocalIRQController(خالی) {}

ساکن اندرونی irq_t get_فعال_IRQ(خالی)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= نقاب(12);
    /* If pending == 0 spurious interrupt */
    اگر (pending == 0) {
        واپس irqInvalid;
    }

    /* Get IRQ number */
    irq = (لفظ_بٹ - 1 - clzl(pending));
    اگر (irq != مداخلت_CORE_GPU) {
        واپس irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~ثنائی_ہندسہ(مداخلت_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_انحراف);
    pending &= ~ثنائی_ہندسہ(مداخلت_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_انحراف);
    اگر (pending) {
        واپس (لفظ_بٹ - 1 - clzl(pending)) + BASIC_IRQ_انحراف;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    اگر (pending) {
        واپس (لفظ_بٹ - 1 - clzl(pending)) + 32 + معمول_IRQ_انحراف;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    اگر (pending) {
        واپس (لفظ_بٹ - 1 - clzl(pending)) + 0 + معمول_IRQ_انحراف;
    }

    واپس irqInvalid;
}

ساکن اندرونی خالی نقاب_مداخلت(bool_t disable, irq_t irq)
{
    انتخاب (irq) {
    حالت مداخلت_CORE_CNTPSIRQ :
    حالت مداخلت_CORE_CNTPNSIRQ:
    حالت مداخلت_CORE_CNTHPIRQ :
    حالت مداخلت_CORE_CNTVIRQ  :
        اگر (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~ثنائی_ہندسہ(irq);
        } ورنہ {
            core_regs->coreTimersIrqCtrl[0] |= ثنائی_ہندسہ(irq);
        }
        واپس;
    حالت مداخلت_CORE_MAILBOX_0:
    حالت مداخلت_CORE_MAILBOX_1:
    حالت مداخلت_CORE_MAILBOX_2:
    حالت مداخلت_CORE_MAILBOX_3:
        اگر (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~ثنائی_ہندسہ(irq);
        } ورنہ {
            core_regs->coreMailboxesIrqCtrl[0] |= ثنائی_ہندسہ(irq);
        }
        واپس;
    حالت مداخلت_CORE_LOCAL_TIMER:
        اگر (disable) {
            core_regs->localTimerCtl &= ~ثنائی_ہندسہ(LOCAL_TIMER_CTRL_IRQ_ثنائی_ہندسہ);
        } ورنہ {
            core_regs->localTimerCtl |= ثنائی_ہندسہ(LOCAL_TIMER_CTRL_IRQ_ثنائی_ہندسہ);
        }
        واپس;
    حالت مداخلت_CORE_GPU:
    // Not maskable
    حالت مداخلت_CORE_PMU:
    // Not currently handled
    حالت مداخلت_CORE_AXI:
        // Not currently handled
        واپس;
    طےشدہ:
        توڑو;
    }
    اگر (irq < BASIC_IRQ_انحراف) {
        // Other invalid irq
        واپس;
    }

    اگر (irq < معمول_IRQ_انحراف) {
        اگر (disable) {
            intc_regs->bfDisableBasicIRQs = ثنائی_ہندسہ(irq - BASIC_IRQ_انحراف);
        } ورنہ {
            intc_regs->bfEnableBasicIRQs = ثنائی_ہندسہ(irq - BASIC_IRQ_انحراف);
        }
    } ورنہ اگر (irq <= maxIRQ) {
        صحیح معمول_irq = irq - معمول_IRQ_انحراف;
        صحیح اشاریہ = معمول_irq / 32;
        اگر (disable) {
            intc_regs->bfDisableIRQs[اشاریہ] = ثنائی_ہندسہ(معمول_irq % 32);
        } ورنہ {
            intc_regs->bfEnableIRQs[اشاریہ] = ثنائی_ہندسہ(معمول_irq % 32);
        }
    }
}
