/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/smp/ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/kernel/tlb.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT

ਖਾਲੀ ਬਹੁਲਕ_handle_Remote_Call(Ipi_ਬਹੁਲਕ_Remote_Call_t call, ਸ਼ਬਦ_t arg0, ਸ਼ਬਦ_t arg1, ਸ਼ਬਦ_t arg2)
{
    ਚੋਣ (call) {
    ਹਾਲਤ IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (ਖਾਲੀ *)arg1, arg2);
        ਰੋਕੋ;

    ਹਾਲਤ IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_ਜੜ੍ਹ_t *)arg0, arg1);
        ਰੋਕੋ;

    ਮੂਲ:
        fail("Invalid remote call");
    }
}

#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */
