/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <mode/model/statedata.h>

/* Current active page directory. This is really just a shadow of CR3 */
UP_STATE_DEFINE(paddr_t, ia32KSCurrentPD VISIBLE);

/* The privileged kernel mapping PD & PT */
pde_t ia32KSGlobalPD[BIT(PD_INDEX_BITS)] ALIGN(BIT(seL4_PageDirBits));
pte_t ia32KSGlobalPT[BIT(PT_INDEX_BITS)] ALIGN(BIT(seL4_PageTableBits));

#ifdef CONFIG_KERNEL_LOG_BUFFER
pte_t ia32KSGlobalLogPT[BIT(PT_INDEX_BITS)] ALIGN(BIT(seL4_PageTableBits));
#endif /* CONFIG_KERNEL_LOG_BUFFER */
