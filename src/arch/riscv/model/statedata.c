/*
 * Copyright 2020, DornerWorks
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <util.h>
#অন্তর্ভাব <api/types.h>
#অন্তর্ভাব <arch/types.h>
#অন্তর্ভাব <arch/model/statedata.h>
#অন্তর্ভাব <arch/object/structures.h>
#অন্তর্ভাব <linker.h>
#অন্তর্ভাব <plat/machine/hardware.h>

/* The top level asid mapping table */
asid_pool_t *riscvKSASIDTable[nASIDPools];

/* Kernel Page Tables */
pte_t kernel_মূল_page_Table[BIT(PT_সূচক_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));

#যদি __riscv_xlen != 32
pte_t kernel_image_level1_pt[BIT(PT_সূচক_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));
pte_t kernel_image_level1_dev_pt[BIT(PT_সূচক_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));
#নতুবা_যদি defined(CONFIG_KERNEL_LOG_বাফার)
pte_t kernel_image_level1_log_বাফার_pt[BIT(PT_সূচক_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));
#endif

SMP_STATE_DEFINE(core_map_t, coreMap);
