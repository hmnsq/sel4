/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/smp/ipi.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT

ਖਾਲੀ ਬਹੁਲਕ_handle_Remote_Call(Ipi_ਬਹੁਲਕ_Remote_Call_t call, ਸ਼ਬਦ_t arg0, ਸ਼ਬਦ_t arg1, ਸ਼ਬਦ_t arg2)
{
    ਚੋਣ (call) {
    ਹਾਲਤ IpiRemoteCall_InvalidateTLBEntry:
        invalidateLocalTLBEntry(arg0);
        ਰੋਕੋ;

    ਹਾਲਤ Ipi_Remote_Call_Invalidate_ਪੰਨਾ_Structure_Cache:
        invalidate_Local_ਪੰਨਾ_Structure_Cache();
        ਰੋਕੋ;

    ਹਾਲਤ IpiRemoteCall_InvalidateTLB:
        invalidateLocalTLB();
        ਰੋਕੋ;

    ਮੂਲ:
        fail("Invalid remote call");
    }
}

#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */
