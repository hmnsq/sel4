/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <mode/smp/ipi.h>
#ಅನ್ತರ್ಭಾವ <smp/lock.h>
#ಅನ್ತರ್ಭಾವ <util.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT

ಸ್ಥಿರ ಅಸ್ಥಿರ irq_t ipiIrq[CONFIG_MAX_ಅಂಕೆ_NODES];

ಶೂನ್ಯ handleRemoteCall(IpiRemoteCall_t call, ಶಬ್ದ_t arg0, ಶಬ್ದ_t arg1, ಶಬ್ದ_t arg2, bool_t irq_ಮಾರ್ಗ)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    ಆದರೆ (clh_is_ipi_pending(getCurrentCPUIndex())) {
        ಆಯ್ಕೆ (call) {
        ಸ್ಥಿತಿ IpiRemoteCall_Stall:
            ipiStallCoreCallback(irq_ಮಾರ್ಗ);
            ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
        ಸ್ಥಿತಿ IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_HAVE_FPU */

        ಪೂರ್ವನಿಯೋಜಿತ:
            fail("Invalid remote call");
            ನಿಲ್ಲಿಸು;
        }

        big_kernel_ಬೀಗ.ಗಂಟು[getCurrentCPUIndex()].ipi = 0;
        ipiIrq[getCurrentCPUIndex()] = irqInvalid;
        ipi_wait();
    }
}

ಶೂನ್ಯ ipi_send_ಮುಖವಾಡ(irq_t ipi, ಶಬ್ದ_t ಮುಖವಾಡ_2, bool_t isBlocking)
{
    generic_ipi_send_ಮುಖವಾಡ(ipi, ಮುಖವಾಡ_2, isBlocking);
}

irq_t ipi_get_irq(ಶೂನ್ಯ)
{
    ಪ್ರತಿಜ್ಞಾ(!(ipiIrq[getCurrentCPUIndex()] == irqInvalid && clh_is_ipi_pending(getCurrentCPUIndex())));
    ಹಿಂತಿರುಗಿ ipiIrq[getCurrentCPUIndex()];
}

ಶೂನ್ಯ ipi_clear_irq(irq_t irq)
{
    ipiIrq[getCurrentCPUIndex()] = irqInvalid;
    ಹಿಂತಿರುಗಿ;
}

/* this function is called with a single hart id. */
ಶೂನ್ಯ ipi_send_ಗುರಿ(irq_t irq, ಶಬ್ದ_t hart_id)
{
    ಶಬ್ದ_t hart_ಮುಖವಾಡ = ಬಿಟ್(hart_id);
    ಶಬ್ದ_t core_id = hartIDToCoreID(hart_id);
    ಪ್ರತಿಜ್ಞಾ(core_id < CONFIG_MAX_ಅಂಕೆ_NODES);

    ಪ್ರತಿಜ್ಞಾ((ipiIrq[core_id] == irqInvalid) || (ipiIrq[core_id] == irq_reschedule_ipi) ||
           (ipiIrq[core_id] == irq_remote_call_ipi && !clh_is_ipi_pending(core_id)));

    ipiIrq[core_id] = irq;
    fence_rw_rw();
    sbi_send_ipi(hart_ಮುಖವಾಡ);
}

#ಆದರೆ_ಅಂತ್ಯ
