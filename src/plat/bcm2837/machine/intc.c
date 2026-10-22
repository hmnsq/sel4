/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine.h>
BOOT_ਕੋਡ ਖਾਲੀ initIRQController(ਖਾਲੀ)
{
    /* Disable all interrupts */
    intc_regs->bfDisableIRQs[0] = 0xffffffff;
    intc_regs->bfDisableIRQs[1] = 0xffffffff;
    intc_regs->bfDisableBasicIRQs = 0xffffffff;
    /* Disable FIQ */
    intc_regs->FIQ_control &= ~FIQCTRL_FIQ_ENABLE;
    /* Enable IRQ control for GPU */
    intc_regs->bfEnableBasicIRQs = ਦੋਆਧਾਰੀ_ਅੰਕ(ਰੁਕਾਵਟ_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_ਵਿੱਥ);
    intc_regs->bfEnableBasicIRQs = ਦੋਆਧਾਰੀ_ਅੰਕ(ਰੁਕਾਵਟ_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_ਵਿੱਥ);

    core_regs->coreTimerPrescaler = 0x80000000;
}

BOOT_ਕੋਡ ਖਾਲੀ cpu_initLocalIRQController(ਖਾਲੀ) {}

ਸਥਿਰ ਸਤਰਅੰਦਰ irq_t get_ਸਰਗਰਮ_IRQ(ਖਾਲੀ)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= ਨਕਾਬ(12);
    /* If pending == 0 spurious interrupt */
    ਜੇ (pending == 0) {
        ਵਾਪਸ irqInvalid;
    }

    /* Get IRQ number */
    irq = (ਸ਼ਬਦ_ਬਿੱਟ - 1 - clzl(pending));
    ਜੇ (irq != ਰੁਕਾਵਟ_CORE_GPU) {
        ਵਾਪਸ irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~ਦੋਆਧਾਰੀ_ਅੰਕ(ਰੁਕਾਵਟ_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_ਵਿੱਥ);
    pending &= ~ਦੋਆਧਾਰੀ_ਅੰਕ(ਰੁਕਾਵਟ_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_ਵਿੱਥ);
    ਜੇ (pending) {
        ਵਾਪਸ (ਸ਼ਬਦ_ਬਿੱਟ - 1 - clzl(pending)) + BASIC_IRQ_ਵਿੱਥ;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    ਜੇ (pending) {
        ਵਾਪਸ (ਸ਼ਬਦ_ਬਿੱਟ - 1 - clzl(pending)) + 32 + ਸਾਧਾਰਨ_IRQ_ਵਿੱਥ;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    ਜੇ (pending) {
        ਵਾਪਸ (ਸ਼ਬਦ_ਬਿੱਟ - 1 - clzl(pending)) + 0 + ਸਾਧਾਰਨ_IRQ_ਵਿੱਥ;
    }

    ਵਾਪਸ irqInvalid;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ਨਕਾਬ_ਰੁਕਾਵਟ(bool_t disable, irq_t irq)
{
    ਚੋਣ (irq) {
    ਹਾਲਤ ਰੁਕਾਵਟ_CORE_CNTPSIRQ :
    ਹਾਲਤ ਰੁਕਾਵਟ_CORE_CNTPNSIRQ:
    ਹਾਲਤ ਰੁਕਾਵਟ_CORE_CNTHPIRQ :
    ਹਾਲਤ ਰੁਕਾਵਟ_CORE_CNTVIRQ  :
        ਜੇ (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~ਦੋਆਧਾਰੀ_ਅੰਕ(irq);
        } ਨਹੀਂਤਾਂ {
            core_regs->coreTimersIrqCtrl[0] |= ਦੋਆਧਾਰੀ_ਅੰਕ(irq);
        }
        ਵਾਪਸ;
    ਹਾਲਤ ਰੁਕਾਵਟ_CORE_MAILBOX_0:
    ਹਾਲਤ ਰੁਕਾਵਟ_CORE_MAILBOX_1:
    ਹਾਲਤ ਰੁਕਾਵਟ_CORE_MAILBOX_2:
    ਹਾਲਤ ਰੁਕਾਵਟ_CORE_MAILBOX_3:
        ਜੇ (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~ਦੋਆਧਾਰੀ_ਅੰਕ(irq);
        } ਨਹੀਂਤਾਂ {
            core_regs->coreMailboxesIrqCtrl[0] |= ਦੋਆਧਾਰੀ_ਅੰਕ(irq);
        }
        ਵਾਪਸ;
    ਹਾਲਤ ਰੁਕਾਵਟ_CORE_LOCAL_TIMER:
        ਜੇ (disable) {
            core_regs->localTimerCtl &= ~ਦੋਆਧਾਰੀ_ਅੰਕ(LOCAL_TIMER_CTRL_IRQ_ਦੋਆਧਾਰੀ_ਅੰਕ);
        } ਨਹੀਂਤਾਂ {
            core_regs->localTimerCtl |= ਦੋਆਧਾਰੀ_ਅੰਕ(LOCAL_TIMER_CTRL_IRQ_ਦੋਆਧਾਰੀ_ਅੰਕ);
        }
        ਵਾਪਸ;
    ਹਾਲਤ ਰੁਕਾਵਟ_CORE_GPU:
    // Not maskable
    ਹਾਲਤ ਰੁਕਾਵਟ_CORE_PMU:
    // Not currently handled
    ਹਾਲਤ ਰੁਕਾਵਟ_CORE_AXI:
        // Not currently handled
        ਵਾਪਸ;
    ਮੂਲ:
        ਰੋਕੋ;
    }
    ਜੇ (irq < BASIC_IRQ_ਵਿੱਥ) {
        // Other invalid irq
        ਵਾਪਸ;
    }

    ਜੇ (irq < ਸਾਧਾਰਨ_IRQ_ਵਿੱਥ) {
        ਜੇ (disable) {
            intc_regs->bfDisableBasicIRQs = ਦੋਆਧਾਰੀ_ਅੰਕ(irq - BASIC_IRQ_ਵਿੱਥ);
        } ਨਹੀਂਤਾਂ {
            intc_regs->bfEnableBasicIRQs = ਦੋਆਧਾਰੀ_ਅੰਕ(irq - BASIC_IRQ_ਵਿੱਥ);
        }
    } ਨਹੀਂਤਾਂ ਜੇ (irq <= maxIRQ) {
        ਪੂਰਨ ਸਾਧਾਰਨ_irq = irq - ਸਾਧਾਰਨ_IRQ_ਵਿੱਥ;
        ਪੂਰਨ ਸੂਚਕਾਂਕ = ਸਾਧਾਰਨ_irq / 32;
        ਜੇ (disable) {
            intc_regs->bfDisableIRQs[ਸੂਚਕਾਂਕ] = ਦੋਆਧਾਰੀ_ਅੰਕ(ਸਾਧਾਰਨ_irq % 32);
        } ਨਹੀਂਤਾਂ {
            intc_regs->bfEnableIRQs[ਸੂਚਕਾਂਕ] = ਦੋਆਧਾਰੀ_ਅੰਕ(ਸਾਧਾਰਨ_irq % 32);
        }
    }
}
