/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <kernel/stack.h>

VISIBLE ALIGN(KERNEL_স্তূপ_ALIGNMENT)
অক্ষর kernel_স্তূপ_alloc[CONFIG_MAX_অঙ্ক_NODES][BIT(CONFIG_KERNEL_স্তূপ_BITS)];
