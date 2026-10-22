/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <plat/machine.h>
BOOT_ಸಂಕೇತ ಶೂನ್ಯ initIRQController(ಶೂನ್ಯ)
{
    /* Disable all interrupts */
    intc_regs->bfDisableIRQs[0] = 0xffffffff;
    intc_regs->bfDisableIRQs[1] = 0xffffffff;
    intc_regs->bfDisableBasicIRQs = 0xffffffff;
    /* Disable FIQ */
    intc_regs->FIQ_control &= ~FIQCTRL_FIQ_ENABLE;
    /* Enable IRQ control for GPU */
    intc_regs->bfEnableBasicIRQs = ಬಿಟ್(ಅಡಚಣೆ_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_ಸ್ಥಾನಾಂತರ);
    intc_regs->bfEnableBasicIRQs = ಬಿಟ್(ಅಡಚಣೆ_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_ಸ್ಥಾನಾಂತರ);

    core_regs->coreTimerPrescaler = 0x80000000;
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ cpu_initLocalIRQController(ಶೂನ್ಯ) {}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ irq_t get_ಸಕ್ರಿಯ_IRQ(ಶೂನ್ಯ)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= ಮುಖವಾಡ(12);
    /* If pending == 0 spurious interrupt */
    ಆದರೆ (pending == 0) {
        ಹಿಂತಿರುಗಿ irqInvalid;
    }

    /* Get IRQ number */
    irq = (ಶಬ್ದ_ಬಿಟ್ಟುಗಳು - 1 - clzl(pending));
    ಆದರೆ (irq != ಅಡಚಣೆ_CORE_GPU) {
        ಹಿಂತಿರುಗಿ irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~ಬಿಟ್(ಅಡಚಣೆ_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_ಸ್ಥಾನಾಂತರ);
    pending &= ~ಬಿಟ್(ಅಡಚಣೆ_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_ಸ್ಥಾನಾಂತರ);
    ಆದರೆ (pending) {
        ಹಿಂತಿರುಗಿ (ಶಬ್ದ_ಬಿಟ್ಟುಗಳು - 1 - clzl(pending)) + BASIC_IRQ_ಸ್ಥಾನಾಂತರ;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    ಆದರೆ (pending) {
        ಹಿಂತಿರುಗಿ (ಶಬ್ದ_ಬಿಟ್ಟುಗಳು - 1 - clzl(pending)) + 32 + ಸಾಮಾನ್ಯ_IRQ_ಸ್ಥಾನಾಂತರ;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    ಆದರೆ (pending) {
        ಹಿಂತಿರುಗಿ (ಶಬ್ದ_ಬಿಟ್ಟುಗಳು - 1 - clzl(pending)) + 0 + ಸಾಮಾನ್ಯ_IRQ_ಸ್ಥಾನಾಂತರ;
    }

    ಹಿಂತಿರುಗಿ irqInvalid;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಮುಖವಾಡ_ಅಡಚಣೆ(bool_t disable, irq_t irq)
{
    ಆಯ್ಕೆ (irq) {
    ಸ್ಥಿತಿ ಅಡಚಣೆ_CORE_CNTPSIRQ :
    ಸ್ಥಿತಿ ಅಡಚಣೆ_CORE_CNTPNSIRQ:
    ಸ್ಥಿತಿ ಅಡಚಣೆ_CORE_CNTHPIRQ :
    ಸ್ಥಿತಿ ಅಡಚಣೆ_CORE_CNTVIRQ  :
        ಆದರೆ (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~ಬಿಟ್(irq);
        } ಇಲ್ಲದಿದ್ದರೆ {
            core_regs->coreTimersIrqCtrl[0] |= ಬಿಟ್(irq);
        }
        ಹಿಂತಿರುಗಿ;
    ಸ್ಥಿತಿ ಅಡಚಣೆ_CORE_MAILBOX_0:
    ಸ್ಥಿತಿ ಅಡಚಣೆ_CORE_MAILBOX_1:
    ಸ್ಥಿತಿ ಅಡಚಣೆ_CORE_MAILBOX_2:
    ಸ್ಥಿತಿ ಅಡಚಣೆ_CORE_MAILBOX_3:
        ಆದರೆ (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~ಬಿಟ್(irq);
        } ಇಲ್ಲದಿದ್ದರೆ {
            core_regs->coreMailboxesIrqCtrl[0] |= ಬಿಟ್(irq);
        }
        ಹಿಂತಿರುಗಿ;
    ಸ್ಥಿತಿ ಅಡಚಣೆ_CORE_LOCAL_TIMER:
        ಆದರೆ (disable) {
            core_regs->localTimerCtl &= ~ಬಿಟ್(LOCAL_TIMER_CTRL_IRQ_ಬಿಟ್);
        } ಇಲ್ಲದಿದ್ದರೆ {
            core_regs->localTimerCtl |= ಬಿಟ್(LOCAL_TIMER_CTRL_IRQ_ಬಿಟ್);
        }
        ಹಿಂತಿರುಗಿ;
    ಸ್ಥಿತಿ ಅಡಚಣೆ_CORE_GPU:
    // Not maskable
    ಸ್ಥಿತಿ ಅಡಚಣೆ_CORE_PMU:
    // Not currently handled
    ಸ್ಥಿತಿ ಅಡಚಣೆ_CORE_AXI:
        // Not currently handled
        ಹಿಂತಿರುಗಿ;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (irq < BASIC_IRQ_ಸ್ಥಾನಾಂತರ) {
        // Other invalid irq
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (irq < ಸಾಮಾನ್ಯ_IRQ_ಸ್ಥಾನಾಂತರ) {
        ಆದರೆ (disable) {
            intc_regs->bfDisableBasicIRQs = ಬಿಟ್(irq - BASIC_IRQ_ಸ್ಥಾನಾಂತರ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            intc_regs->bfEnableBasicIRQs = ಬಿಟ್(irq - BASIC_IRQ_ಸ್ಥಾನಾಂತರ);
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (irq <= maxIRQ) {
        ಪೂರ್ಣಾಂಕ ಸಾಮಾನ್ಯ_irq = irq - ಸಾಮಾನ್ಯ_IRQ_ಸ್ಥಾನಾಂತರ;
        ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = ಸಾಮಾನ್ಯ_irq / 32;
        ಆದರೆ (disable) {
            intc_regs->bfDisableIRQs[ಸೂಚ್ಯಂಕ] = ಬಿಟ್(ಸಾಮಾನ್ಯ_irq % 32);
        } ಇಲ್ಲದಿದ್ದರೆ {
            intc_regs->bfEnableIRQs[ಸೂಚ್ಯಂಕ] = ಬಿಟ್(ಸಾಮಾನ್ಯ_irq % 32);
        }
    }
}
