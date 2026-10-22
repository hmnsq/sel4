/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>

lookup_fault_t current_lookup_fault;
seL4_Fault_t current_fault;
syscall_error_t current_syscall_error;
#यदि_परिभाषितम् CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
debug_syscall_error_t current_debug_error;
#यदि_अन्तः

