/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <kernel/stack.h>

VISIBLE ALIGN(KERNEL_सालानसान_ALIGNMENT)
char kernel_सालानसान_alloc[CONFIG_MAX_नुमेरो_NODES][BIT(CONFIG_KERNEL_सालानसान_BITS)];
