/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <types.h>
#அந்தர்பாவ <arch/machine.h>
#அந்தர்பாவ <linker.h>

வெறுமை initL2Cache(வெறுமை) {}
வெறுமை plat_cleanL2Range(paddr_t start, paddr_t end) {}
வெறுமை plat_invalidateL2Range(paddr_t start, paddr_t end) {}
வெறுமை plat_cleanInvalidateL2Range(paddr_t start, paddr_t end) {}
வெறுமை plat_cleanInvalidateL2Cache(வெறுமை) {}
