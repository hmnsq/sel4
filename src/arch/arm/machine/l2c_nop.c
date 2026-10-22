/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <arch/machine.h>
#شامل <linker.h>

تهی initL2Cache(تهی) {}
تهی plat_cleanL2Range(paddr_t start, paddr_t end) {}
تهی plat_invalidateL2Range(paddr_t start, paddr_t end) {}
تهی plat_cleanInvalidateL2Range(paddr_t start, paddr_t end) {}
تهی plat_cleanInvalidateL2Cache(تهی) {}
