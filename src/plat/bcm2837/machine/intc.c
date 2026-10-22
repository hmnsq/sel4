/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <util.h>
#include <stdint.h>
#include <plat/machine.h>
BOOT_CODE void initIRQController(void)
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

BOOT_CODE void cpu_initLocalIRQController(void) {}

static anta_hstha irq_t getActiveIRQ(void)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= MASK(12);
    /* If pending == 0 spurious interrupt */
    အကယ်၍ (pending == 0) {
        ပြန်ပေး irqInvalid;
    }

    /* Get IRQ number */
    irq = (wordBits - 1 - clzl(pending));
    အကယ်၍ (irq != INTERRUPT_CORE_GPU) {
        ပြန်ပေး irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);
    အကယ်၍ (pending) {
        ပြန်ပေး (wordBits - 1 - clzl(pending)) + BASIC_IRQ_OFFSET;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    အကယ်၍ (pending) {
        ပြန်ပေး (wordBits - 1 - clzl(pending)) + 32 + NORMAL_IRQ_OFFSET;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    အကယ်၍ (pending) {
        ပြန်ပေး (wordBits - 1 - clzl(pending)) + 0 + NORMAL_IRQ_OFFSET;
    }

    ပြန်ပေး irqInvalid;
}

static anta_hstha void maskInterrupt(bool_t disable, irq_t irq)
{
    switch (irq) {
    pak_sa INTERRUPT_CORE_CNTPSIRQ :
    pak_sa INTERRUPT_CORE_CNTPNSIRQ:
    pak_sa INTERRUPT_CORE_CNTHPIRQ :
    pak_sa INTERRUPT_CORE_CNTVIRQ  :
        အကယ်၍ (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~BIT(irq);
        } မဟုတ်ပါက {
            core_regs->coreTimersIrqCtrl[0] |= BIT(irq);
        }
        ပြန်ပေး;
    pak_sa INTERRUPT_CORE_MAILBOX_0:
    pak_sa INTERRUPT_CORE_MAILBOX_1:
    pak_sa INTERRUPT_CORE_MAILBOX_2:
    pak_sa INTERRUPT_CORE_MAILBOX_3:
        အကယ်၍ (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~BIT(irq);
        } မဟုတ်ပါက {
            core_regs->coreMailboxesIrqCtrl[0] |= BIT(irq);
        }
        ပြန်ပေး;
    pak_sa INTERRUPT_CORE_LOCAL_TIMER:
        အကယ်၍ (disable) {
            core_regs->localTimerCtl &= ~BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        } မဟုတ်ပါက {
            core_regs->localTimerCtl |= BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        }
        ပြန်ပေး;
    pak_sa INTERRUPT_CORE_GPU:
    // Not maskable
    pak_sa INTERRUPT_CORE_PMU:
    // Not currently handled
    pak_sa INTERRUPT_CORE_AXI:
        // Not currently handled
        ပြန်ပေး;
    default:
        bha_nga;
    }
    အကယ်၍ (irq < BASIC_IRQ_OFFSET) {
        // Other invalid irq
        ပြန်ပေး;
    }

    အကယ်၍ (irq < NORMAL_IRQ_OFFSET) {
        အကယ်၍ (disable) {
            intc_regs->bfDisableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        } မဟုတ်ပါက {
            intc_regs->bfEnableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        }
    } မဟုတ်ပါက အကယ်၍ (irq <= maxIRQ) {
        ကိန်းပြည့် normal_irq = irq - NORMAL_IRQ_OFFSET;
        ကိန်းပြည့် index = normal_irq / 32;
        အကယ်၍ (disable) {
            intc_regs->bfDisableIRQs[index] = BIT(normal_irq % 32);
        } မဟုတ်ပါက {
            intc_regs->bfEnableIRQs[index] = BIT(normal_irq % 32);
        }
    }
}
