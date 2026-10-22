/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <util.h>
#ضمن <stdint.h>
#ضمن <plat/machine.h>
BOOT_کوڈ فراغ initIRQController(فراغ)
{
    /* Disable all interrupts */
    intc_regs->bfDisableIRQs[0] = 0xffffffff;
    intc_regs->bfDisableIRQs[1] = 0xffffffff;
    intc_regs->bfDisableBasicIRQs = 0xffffffff;
    /* Disable FIQ */
    intc_regs->FIQ_control &= ~FIQCTRL_FIQ_ENABLE;
    /* Enable IRQ control for GPU */
    intc_regs->bfEnableBasicIRQs = ثنائی_ہندسہ(رکاوٹ_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_وتھ);
    intc_regs->bfEnableBasicIRQs = ثنائی_ہندسہ(رکاوٹ_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_وتھ);

    core_regs->coreTimerPrescaler = 0x80000000;
}

BOOT_کوڈ فراغ cpu_initLocalIRQController(فراغ) {}

ساكن مضمن irq_t get_سرگرم_IRQ(فراغ)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= نقاب(12);
    /* If pending == 0 spurious interrupt */
    إذا (pending == 0) {
        أرجع irqInvalid;
    }

    /* Get IRQ number */
    irq = (لفظ_بٹ - 1 - clzl(pending));
    إذا (irq != رکاوٹ_CORE_GPU) {
        أرجع irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~ثنائی_ہندسہ(رکاوٹ_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_وتھ);
    pending &= ~ثنائی_ہندسہ(رکاوٹ_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_وتھ);
    إذا (pending) {
        أرجع (لفظ_بٹ - 1 - clzl(pending)) + BASIC_IRQ_وتھ;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    إذا (pending) {
        أرجع (لفظ_بٹ - 1 - clzl(pending)) + 32 + عام_IRQ_وتھ;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    إذا (pending) {
        أرجع (لفظ_بٹ - 1 - clzl(pending)) + 0 + عام_IRQ_وتھ;
    }

    أرجع irqInvalid;
}

ساكن مضمن فراغ نقاب_رکاوٹ(bool_t disable, irq_t irq)
{
    بدل (irq) {
    حالة رکاوٹ_CORE_CNTPSIRQ :
    حالة رکاوٹ_CORE_CNTPNSIRQ:
    حالة رکاوٹ_CORE_CNTHPIRQ :
    حالة رکاوٹ_CORE_CNTVIRQ  :
        إذا (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~ثنائی_ہندسہ(irq);
        } وإلا {
            core_regs->coreTimersIrqCtrl[0] |= ثنائی_ہندسہ(irq);
        }
        أرجع;
    حالة رکاوٹ_CORE_MAILBOX_0:
    حالة رکاوٹ_CORE_MAILBOX_1:
    حالة رکاوٹ_CORE_MAILBOX_2:
    حالة رکاوٹ_CORE_MAILBOX_3:
        إذا (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~ثنائی_ہندسہ(irq);
        } وإلا {
            core_regs->coreMailboxesIrqCtrl[0] |= ثنائی_ہندسہ(irq);
        }
        أرجع;
    حالة رکاوٹ_CORE_LOCAL_TIMER:
        إذا (disable) {
            core_regs->localTimerCtl &= ~ثنائی_ہندسہ(LOCAL_TIMER_CTRL_IRQ_ثنائی_ہندسہ);
        } وإلا {
            core_regs->localTimerCtl |= ثنائی_ہندسہ(LOCAL_TIMER_CTRL_IRQ_ثنائی_ہندسہ);
        }
        أرجع;
    حالة رکاوٹ_CORE_GPU:
    // Not maskable
    حالة رکاوٹ_CORE_PMU:
    // Not currently handled
    حالة رکاوٹ_CORE_AXI:
        // Not currently handled
        أرجع;
    افتراضي:
        اكسر;
    }
    إذا (irq < BASIC_IRQ_وتھ) {
        // Other invalid irq
        أرجع;
    }

    إذا (irq < عام_IRQ_وتھ) {
        إذا (disable) {
            intc_regs->bfDisableBasicIRQs = ثنائی_ہندسہ(irq - BASIC_IRQ_وتھ);
        } وإلا {
            intc_regs->bfEnableBasicIRQs = ثنائی_ہندسہ(irq - BASIC_IRQ_وتھ);
        }
    } وإلا إذا (irq <= maxIRQ) {
        صحيح عام_irq = irq - عام_IRQ_وتھ;
        صحيح اشاریہ = عام_irq / 32;
        إذا (disable) {
            intc_regs->bfDisableIRQs[اشاریہ] = ثنائی_ہندسہ(عام_irq % 32);
        } وإلا {
            intc_regs->bfEnableIRQs[اشاریہ] = ثنائی_ہندسہ(عام_irq % 32);
        }
    }
}
