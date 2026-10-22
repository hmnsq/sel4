/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <machine/registerset.h>

ध्रुव पञ्जिकाकोशः_t fault_messages[][MAX_सन्देशः_आकार] = {
    [सन्देशः_ID_Syscall] = SYSCALL_सन्देशः,
    [सन्देशः_ID_Exception] = EXCEPTION_सन्देशः,
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    [सन्देशः_ID_Timeout_प्रत्युत्तरम्] = TIMEOUT_प्रत्युत्तरम्_सन्देशः,
#यदि_अन्तः
};
