/*
 * Copyright 2020, DornerWorks
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <util.h>
#ضمن <api/types.h>
#ضمن <arch/types.h>
#ضمن <arch/model/statedata.h>
#ضمن <arch/object/structures.h>
#ضمن <linker.h>
#ضمن <plat/machine/hardware.h>

/* The top level asid mapping table */
asid_pool_t *riscvKSASIDTable[nASIDPools];

/* Kernel Page Tables */
pte_t kernel_ریښه_پاڼه_جدول[بټ(PT_شاخص_بټونه)] ALIGN_BSS(بټ(se_L4_پاڼه_جدول_بټونه));

#إذا __riscv_xlen != 32
pte_t kernel_image_level1_pt[بټ(PT_شاخص_بټونه)] ALIGN_BSS(بټ(se_L4_پاڼه_جدول_بټونه));
pte_t kernel_image_level1_dev_pt[بټ(PT_شاخص_بټونه)] ALIGN_BSS(بټ(se_L4_پاڼه_جدول_بټونه));
#وإلاإذا defined(CONFIG_KERNEL_LOG_بفر)
pte_t kernel_image_level1_log_بفر_pt[بټ(PT_شاخص_بټونه)] ALIGN_BSS(بټ(se_L4_پاڼه_جدول_بټونه));
#د_که_پای

SMP_STATE_DEFINE(core_map_t, coreMap);
