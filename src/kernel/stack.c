/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <kernel/stack.h>

VISIBLE ALIGN(KERNEL_کەڵەکە_ALIGNMENT)
حرف kernel_کەڵەکە_alloc[CONFIG_MAX_ژمارە_NODES][BIT(CONFIG_KERNEL_کەڵەکە_BITS)];
