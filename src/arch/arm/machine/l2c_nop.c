/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <types.h>
#অন্তর্ভাৱ <arch/machine.h>
#অন্তর্ভাৱ <linker.h>

শূন্য initL2Cache(শূন্য) {}
শূন্য plat_cleanL2Range(paddr_t start, paddr_t end) {}
শূন্য plat_invalidateL2Range(paddr_t start, paddr_t end) {}
শূন্য plat_cleanInvalidateL2Range(paddr_t start, paddr_t end) {}
শূন্য plat_cleanInvalidateL2Cache(শূন্য) {}
