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
pte_t kernel_मूलम्_page_सारणी[BIT(PT_सूचकांक_द्वयङ्काः)] ALIGN_BSS(BIT(se_L4_Page_सारणी_द्वयङ्काः));

#यदि __riscv_xlen != 32
pte_t kernel_image_level1_pt[BIT(PT_सूचकांक_द्वयङ्काः)] ALIGN_BSS(BIT(se_L4_Page_सारणी_द्वयङ्काः));
pte_t kernel_image_level1_dev_pt[BIT(PT_सूचकांक_द्वयङ्काः)] ALIGN_BSS(BIT(se_L4_Page_सारणी_द्वयङ्काः));
#नोचेद्यदि defined(CONFIG_KERNEL_LOG_बफर)
pte_t kernel_image_level1_log_बफर_pt[BIT(PT_सूचकांक_द्वयङ्काः)] ALIGN_BSS(BIT(se_L4_Page_सारणी_द्वयङ्काः));
#यदि_अन्तः

SMP_STATE_DEFINE(core_map_t, coreMap);
