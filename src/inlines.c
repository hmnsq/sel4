/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <types.h>
#समावेश <api/failures.h>

lookup_fault_t current_lookup_fault;
seL4_Fault_t current_fault;
syscall_error_t current_syscall_error;
#अगर_घोषित CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
debug_syscall_error_t current_debug_error;
#पूर्ण_अगर

