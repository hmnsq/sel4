/*
 * Copyright 2020, DornerWorks
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <util.h>
#ଅନ୍ତର୍ଭାଵ <api/types.h>
#ଅନ୍ତର୍ଭାଵ <arch/types.h>
#ଅନ୍ତର୍ଭାଵ <arch/model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/structures.h>
#ଅନ୍ତର୍ଭାଵ <linker.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/hardware.h>

/* The top level asid mapping table */
asid_pool_t *riscvKSASIDTable[nASIDPools];

/* Kernel Page Tables */
pte_t kernel_ମୂଳ_page_Table[BIT(PT_INDEX_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));

#ଯଦି __riscv_xlen != 32
pte_t kernel_image_level1_pt[BIT(PT_INDEX_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));
pte_t kernel_image_level1_dev_pt[BIT(PT_INDEX_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));
#ନଚେତଯଦି defined(CONFIG_KERNEL_LOG_BUFFER)
pte_t kernel_image_level1_log_buffer_pt[BIT(PT_INDEX_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));
#endif

SMP_STATE_DEFINE(core_map_t, coreMap);
