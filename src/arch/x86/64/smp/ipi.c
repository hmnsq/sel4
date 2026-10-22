/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <mode/smp/ipi.h>
#அந்தர்பாவ <mode/kernel/tlb.h>

#ifdef ENABLE_SMP_SUPPORT

வெறுமை Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    தேர்வு (call) {
    நிலை IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (வெறுமை *)arg1, arg2);
        நிறுத்து;

    நிலை IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_வேர்_t *)arg0, arg1);
        நிறுத்து;

    இயல்பு:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
