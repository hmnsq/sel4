/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <arch/machine.h>
#شامل <linker.h>

خالی initL2Cache(خالی) {}
خالی plat_cleanL2Range(paddr_t آغاز, paddr_t انتہا) {}
خالی plat_invalidateL2Range(paddr_t آغاز, paddr_t انتہا) {}
خالی plat_cleanInvalidateL2Range(paddr_t آغاز, paddr_t انتہا) {}
خالی plat_cleanInvalidateL2Cache(خالی) {}
