/*
 * Copyright 2020, DornerWorks
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <util.h>
#अन्तर्भाव <api/types.h>
#अन्तर्भाव <arch/types.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <arch/object/structures.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine/hardware.h>

/* The top level asid mapping table */
asid_pool_t *riscvKSASIDTable[nASIDPools];

/* Kernel Page Tables */
pte_t kernel_मूल_page_Table[BIT(PT_INDEX_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));

#यदि __riscv_xlen != 32
pte_t kernel_image_level1_pt[BIT(PT_INDEX_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));
pte_t kernel_image_level1_dev_pt[BIT(PT_INDEX_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));
#नोचेद्यदि defined(CONFIG_KERNEL_LOG_BUFFER)
pte_t kernel_image_level1_log_buffer_pt[BIT(PT_INDEX_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));
#endif

SMP_STATE_DEFINE(core_map_t, coreMap);
