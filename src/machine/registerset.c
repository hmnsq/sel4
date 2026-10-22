/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <machine/registerset.h>

ਅਚਲ ਰਜਿਸਟਰ_t fault_messages[][MAX_ਸੁਨੇਹਾ_ਮਾਪ] = {
    [ਸੁਨੇਹਾ_ID_Syscall] = SYSCALL_ਸੁਨੇਹਾ,
    [ਸੁਨੇਹਾ_ID_Exception] = EXCEPTION_ਸੁਨੇਹਾ,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    [ਸੁਨੇਹਾ_ID_Timeout_ਜਵਾਬ] = TIMEOUT_ਜਵਾਬ_ਸੁਨੇਹਾ,
#ਜੇ_ਅੰਤ
};
