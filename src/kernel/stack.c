/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <kernel/stack.h>

VISIBLE ALIGN(KERNEL_ढेर_ALIGNMENT)
अक्षर kernel_ढेर_alloc[CONFIG_MAX_अंक_NODES][BIT(CONFIG_KERNEL_ढेर_BITS)];
