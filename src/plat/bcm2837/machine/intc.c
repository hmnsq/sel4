/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <util.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <plat/machine.h>
BOOT_సంకేతం శూన్య initIRQController(శూన్య)
{
    /* Disable all interrupts */
    intc_regs->bfDisableIRQs[0] = 0xffffffff;
    intc_regs->bfDisableIRQs[1] = 0xffffffff;
    intc_regs->bfDisableBasicIRQs = 0xffffffff;
    /* Disable FIQ */
    intc_regs->FIQ_control &= ~FIQCTRL_FIQ_ENABLE;
    /* Enable IRQ control for GPU */
    intc_regs->bfEnableBasicIRQs = బిట్టు(అంతరాయం_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_స్థానభ్రంశం);
    intc_regs->bfEnableBasicIRQs = బిట్టు(అంతరాయం_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_స్థానభ్రంశం);

    core_regs->coreTimerPrescaler = 0x80000000;
}

BOOT_సంకేతం శూన్య cpu_initLocalIRQController(శూన్య) {}

స్థిర అంతఃవరుస irq_t get_చురుకైన_IRQ(శూన్య)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= ముసుగు(12);
    /* If pending == 0 spurious interrupt */
    ఒకవేళ (pending == 0) {
        తిరిగి irqInvalid;
    }

    /* Get IRQ number */
    irq = (మాట_బిట్లు - 1 - clzl(pending));
    ఒకవేళ (irq != అంతరాయం_CORE_GPU) {
        తిరిగి irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~బిట్టు(అంతరాయం_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_స్థానభ్రంశం);
    pending &= ~బిట్టు(అంతరాయం_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_స్థానభ్రంశం);
    ఒకవేళ (pending) {
        తిరిగి (మాట_బిట్లు - 1 - clzl(pending)) + BASIC_IRQ_స్థానభ్రంశం;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    ఒకవేళ (pending) {
        తిరిగి (మాట_బిట్లు - 1 - clzl(pending)) + 32 + సాధారణ_IRQ_స్థానభ్రంశం;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    ఒకవేళ (pending) {
        తిరిగి (మాట_బిట్లు - 1 - clzl(pending)) + 0 + సాధారణ_IRQ_స్థానభ్రంశం;
    }

    తిరిగి irqInvalid;
}

స్థిర అంతఃవరుస శూన్య ముసుగు_అంతరాయం(bool_t disable, irq_t irq)
{
    ఎంపిక (irq) {
    స్థితి అంతరాయం_CORE_CNTPSIRQ :
    స్థితి అంతరాయం_CORE_CNTPNSIRQ:
    స్థితి అంతరాయం_CORE_CNTHPIRQ :
    స్థితి అంతరాయం_CORE_CNTVIRQ  :
        ఒకవేళ (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~బిట్టు(irq);
        } లేకపోతే {
            core_regs->coreTimersIrqCtrl[0] |= బిట్టు(irq);
        }
        తిరిగి;
    స్థితి అంతరాయం_CORE_MAILBOX_0:
    స్థితి అంతరాయం_CORE_MAILBOX_1:
    స్థితి అంతరాయం_CORE_MAILBOX_2:
    స్థితి అంతరాయం_CORE_MAILBOX_3:
        ఒకవేళ (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~బిట్టు(irq);
        } లేకపోతే {
            core_regs->coreMailboxesIrqCtrl[0] |= బిట్టు(irq);
        }
        తిరిగి;
    స్థితి అంతరాయం_CORE_LOCAL_TIMER:
        ఒకవేళ (disable) {
            core_regs->localTimerCtl &= ~బిట్టు(LOCAL_TIMER_CTRL_IRQ_బిట్టు);
        } లేకపోతే {
            core_regs->localTimerCtl |= బిట్టు(LOCAL_TIMER_CTRL_IRQ_బిట్టు);
        }
        తిరిగి;
    స్థితి అంతరాయం_CORE_GPU:
    // Not maskable
    స్థితి అంతరాయం_CORE_PMU:
    // Not currently handled
    స్థితి అంతరాయం_CORE_AXI:
        // Not currently handled
        తిరిగి;
    అప్రమేయ:
        ఆపు;
    }
    ఒకవేళ (irq < BASIC_IRQ_స్థానభ్రంశం) {
        // Other invalid irq
        తిరిగి;
    }

    ఒకవేళ (irq < సాధారణ_IRQ_స్థానభ్రంశం) {
        ఒకవేళ (disable) {
            intc_regs->bfDisableBasicIRQs = బిట్టు(irq - BASIC_IRQ_స్థానభ్రంశం);
        } లేకపోతే {
            intc_regs->bfEnableBasicIRQs = బిట్టు(irq - BASIC_IRQ_స్థానభ్రంశం);
        }
    } లేకపోతే ఒకవేళ (irq <= maxIRQ) {
        పూర్ణాంకం సాధారణ_irq = irq - సాధారణ_IRQ_స్థానభ్రంశం;
        పూర్ణాంకం సూచీ = సాధారణ_irq / 32;
        ఒకవేళ (disable) {
            intc_regs->bfDisableIRQs[సూచీ] = బిట్టు(సాధారణ_irq % 32);
        } లేకపోతే {
            intc_regs->bfEnableIRQs[సూచీ] = బిట్టు(సాధారణ_irq % 32);
        }
    }
}
