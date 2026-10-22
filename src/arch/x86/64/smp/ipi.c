/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <mode/smp/ipi.h>
#అన్తర్భావ <mode/kernel/tlb.h>

#నిర్వచితమైతే ENABLE_SMP_SUPPORT

శూన్య బహుళకం_handle_Remote_Call(Ipi_బహుళకం_Remote_Call_t call, మాట_t arg0, మాట_t arg1, మాట_t arg2)
{
    ఎంపిక (call) {
    స్థితి IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (శూన్య *)arg1, arg2);
        ఆపు;

    స్థితి IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_మూలం_t *)arg0, arg1);
        ఆపు;

    అప్రమేయ:
        fail("Invalid remote call");
    }
}

#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
