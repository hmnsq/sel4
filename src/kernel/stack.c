/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <kernel/stack.h>

VISIBLE ALIGN(KERNEL_پشته_ALIGNMENT)
نویسه kernel_پشته_alloc[CONFIG_MAX_رقم_NODES][BIT(CONFIG_KERNEL_پشته_BITS)];
