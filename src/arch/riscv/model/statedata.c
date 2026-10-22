/*
 * Copyright 2020, DornerWorks
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <util.h>
#समावेश <api/types.h>
#समावेश <arch/types.h>
#समावेश <arch/model/statedata.h>
#समावेश <arch/object/structures.h>
#समावेश <linker.h>
#समावेश <plat/machine/hardware.h>

/* The top level asid mapping table */
asid_pool_t *riscvKSASIDTable[nASIDPools];

/* Kernel Page Tables */
pte_t kernel_मूल_page_Table[BIT(PT_सूचकांक_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));

#अगर __riscv_xlen != 32
pte_t kernel_image_level1_pt[BIT(PT_सूचकांक_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));
pte_t kernel_image_level1_dev_pt[BIT(PT_सूचकांक_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));
#या_अगर defined(CONFIG_KERNEL_LOG_बफर)
pte_t kernel_image_level1_log_बफर_pt[BIT(PT_सूचकांक_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));
#पूर्ण_अगर

SMP_STATE_DEFINE(core_map_t, coreMap);
