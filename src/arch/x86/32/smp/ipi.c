/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <mode/smp/ipi.h>

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT

خالی منوال_handle_Remote_Call(Ipi_منوال_Remote_Call_t call, لفظ_t arg0, لفظ_t arg1, لفظ_t arg2)
{
    انتخاب (call) {
    حالت IpiRemoteCall_InvalidateTLBEntry:
        invalidateLocalTLBEntry(arg0);
        توڑو;

    حالت Ipi_Remote_Call_Invalidate_صفحہ_Structure_Cache:
        invalidate_Local_صفحہ_Structure_Cache();
        توڑو;

    حالت IpiRemoteCall_InvalidateTLB:
        invalidateLocalTLB();
        توڑو;

    طےشدہ:
        fail("Invalid remote call");
    }
}

#اگر_ختم /* ENABLE_SMP_SUPPORT */
