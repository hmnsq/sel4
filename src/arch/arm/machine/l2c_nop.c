/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <linker.h>

శూన్య initL2Cache(శూన్య) {}
శూన్య plat_cleanL2Range(paddr_t ప్రారంభం, paddr_t చివర) {}
శూన్య plat_invalidateL2Range(paddr_t ప్రారంభం, paddr_t చివర) {}
శూన్య plat_cleanInvalidateL2Range(paddr_t ప్రారంభం, paddr_t చివర) {}
శూన్య plat_cleanInvalidateL2Cache(శూన్య) {}
