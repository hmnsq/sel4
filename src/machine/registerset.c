/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <machine/registerset.h>

స్థిరరాశి రిజిస్టరు_t fault_messages[][MAX_సందేశం_కొలత] = {
    [సందేశం_ID_Syscall] = SYSCALL_సందేశం,
    [సందేశం_ID_Exception] = EXCEPTION_సందేశం,
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    [సందేశం_ID_Timeout_జవాబు] = TIMEOUT_జవాబు_సందేశం,
#అయితే_ముగింపు
};
