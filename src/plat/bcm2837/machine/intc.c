/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <util.h>
#അന്തര്ഭാവ <stdint.h>
#അന്തര്ഭാവ <plat/machine.h>
BOOT_CODE ശൂന്യം initIRQController(ശൂന്യം)
{
    /* Disable all interrupts */
    intc_regs->bfDisableIRQs[0] = 0xffffffff;
    intc_regs->bfDisableIRQs[1] = 0xffffffff;
    intc_regs->bfDisableBasicIRQs = 0xffffffff;
    /* Disable FIQ */
    intc_regs->FIQ_control &= ~FIQCTRL_FIQ_ENABLE;
    /* Enable IRQ control for GPU */
    intc_regs->bfEnableBasicIRQs = BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    intc_regs->bfEnableBasicIRQs = BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);

    core_regs->coreTimerPrescaler = 0x80000000;
}

BOOT_CODE ശൂന്യം cpu_initLocalIRQController(ശൂന്യം) {}

സ്ഥിതം വരിയിൽ irq_t getActiveIRQ(ശൂന്യം)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= MASK(12);
    /* If pending == 0 spurious interrupt */
    എങ്കിൽ (pending == 0) {
        തിരികെ irqInvalid;
    }

    /* Get IRQ number */
    irq = (wordBits - 1 - clzl(pending));
    എങ്കിൽ (irq != INTERRUPT_CORE_GPU) {
        തിരികെ irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);
    എങ്കിൽ (pending) {
        തിരികെ (wordBits - 1 - clzl(pending)) + BASIC_IRQ_OFFSET;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    എങ്കിൽ (pending) {
        തിരികെ (wordBits - 1 - clzl(pending)) + 32 + NORMAL_IRQ_OFFSET;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    എങ്കിൽ (pending) {
        തിരികെ (wordBits - 1 - clzl(pending)) + 0 + NORMAL_IRQ_OFFSET;
    }

    തിരികെ irqInvalid;
}

സ്ഥിതം വരിയിൽ ശൂന്യം maskInterrupt(bool_t disable, irq_t irq)
{
    തിരഞ്ഞെടുപ്പ് (irq) {
    അവസ്ഥ INTERRUPT_CORE_CNTPSIRQ :
    അവസ്ഥ INTERRUPT_CORE_CNTPNSIRQ:
    അവസ്ഥ INTERRUPT_CORE_CNTHPIRQ :
    അവസ്ഥ INTERRUPT_CORE_CNTVIRQ  :
        എങ്കിൽ (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~BIT(irq);
        } അല്ലെങ്കിൽ {
            core_regs->coreTimersIrqCtrl[0] |= BIT(irq);
        }
        തിരികെ;
    അവസ്ഥ INTERRUPT_CORE_MAILBOX_0:
    അവസ്ഥ INTERRUPT_CORE_MAILBOX_1:
    അവസ്ഥ INTERRUPT_CORE_MAILBOX_2:
    അവസ്ഥ INTERRUPT_CORE_MAILBOX_3:
        എങ്കിൽ (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~BIT(irq);
        } അല്ലെങ്കിൽ {
            core_regs->coreMailboxesIrqCtrl[0] |= BIT(irq);
        }
        തിരികെ;
    അവസ്ഥ INTERRUPT_CORE_LOCAL_TIMER:
        എങ്കിൽ (disable) {
            core_regs->localTimerCtl &= ~BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        } അല്ലെങ്കിൽ {
            core_regs->localTimerCtl |= BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        }
        തിരികെ;
    അവസ്ഥ INTERRUPT_CORE_GPU:
    // Not maskable
    അവസ്ഥ INTERRUPT_CORE_PMU:
    // Not currently handled
    അവസ്ഥ INTERRUPT_CORE_AXI:
        // Not currently handled
        തിരികെ;
    സ്ഥിരസ്ഥിതി:
        നിർത്തുക;
    }
    എങ്കിൽ (irq < BASIC_IRQ_OFFSET) {
        // Other invalid irq
        തിരികെ;
    }

    എങ്കിൽ (irq < NORMAL_IRQ_OFFSET) {
        എങ്കിൽ (disable) {
            intc_regs->bfDisableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        } അല്ലെങ്കിൽ {
            intc_regs->bfEnableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        }
    } അല്ലെങ്കിൽ എങ്കിൽ (irq <= maxIRQ) {
        പൂർണ്ണസംഖ്യ normal_irq = irq - NORMAL_IRQ_OFFSET;
        പൂർണ്ണസംഖ്യ index = normal_irq / 32;
        എങ്കിൽ (disable) {
            intc_regs->bfDisableIRQs[index] = BIT(normal_irq % 32);
        } അല്ലെങ്കിൽ {
            intc_regs->bfEnableIRQs[index] = BIT(normal_irq % 32);
        }
    }
}
