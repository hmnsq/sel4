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
pte_t kernel_پاڙ_صفحو_جدول[ٻٽو_انگ(PT_اشاريو_بٽ)] ALIGN_BSS(ٻٽو_انگ(se_L4_صفحو_جدول_بٽ));

#إذا __riscv_xlen != 32
pte_t kernel_image_level1_pt[ٻٽو_انگ(PT_اشاريو_بٽ)] ALIGN_BSS(ٻٽو_انگ(se_L4_صفحو_جدول_بٽ));
pte_t kernel_image_level1_dev_pt[ٻٽو_انگ(PT_اشاريو_بٽ)] ALIGN_BSS(ٻٽو_انگ(se_L4_صفحو_جدول_بٽ));
#وإلاإذا defined(CONFIG_KERNEL_LOG_بفر)
pte_t kernel_image_level1_log_بفر_pt[ٻٽو_انگ(PT_اشاريو_بٽ)] ALIGN_BSS(ٻٽو_انگ(se_L4_صفحو_جدول_بٽ));
#جيڪڏهن_پورو

SMP_STATE_DEFINE(core_map_t, coreMap);
