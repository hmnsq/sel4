/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <object.h>
#ಅನ್ತರ್ಭಾವ <machine.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <linker.h>

ಬಾಹ್ಯ ಅಕ್ಷರ kernel_ರಾಶಿ_alloc[CONFIG_MAX_ಅಂಕೆ_NODES][ಬಿಟ್(CONFIG_KERNEL_ರಾಶಿ_ಬಿಟ್ಟುಗಳು)];

ಶೂನ್ಯ Arch_switch_ಗಮ್ಯ_Thread(tcb_t *tcb)
{
    ಗಣ_VMRoot(tcb);
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ Arch_configureIdleThread(tcb_t *tcb)
{
    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, ಮುಂದಿನ_IP, (ಶಬ್ದ_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, SSTATUS, (ಶಬ್ದ_t) SSTATUS_SPP | SSTATUS_SPIE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < CONFIG_MAX_ಅಂಕೆ_NODES; i++) {
        ಆದರೆ (ಗಂಟು_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, SP, (ಶಬ್ದ_t)kernel_ರಾಶಿ_alloc + (i + 1) * ಬಿಟ್(CONFIG_KERNEL_ರಾಶಿ_ಬಿಟ್ಟುಗಳು));
            ನಿಲ್ಲಿಸು;
        }
    }
#ಇಲ್ಲದಿದ್ದರೆ
    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, SP, (ಶಬ್ದ_t)kernel_ರಾಶಿ_alloc + ಬಿಟ್(CONFIG_KERNEL_ರಾಶಿ_ಬಿಟ್ಟುಗಳು));
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ Arch_switch_ಗಮ್ಯ_Idle_Thread(ಶೂನ್ಯ)
{
    tcb_t *tcb = ಗಂಟು_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    ಗಣ_VMRoot(tcb);
}

ಶೂನ್ಯ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

ಶೂನ್ಯ Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

ಶೂನ್ಯ Arch_prepare_ಮುಂದಿನ_Domain(ಶೂನ್ಯ)
{
    /* Don't need to do anything */
}

ಶೂನ್ಯ Arch_prepare_ಗಣ_Domain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
