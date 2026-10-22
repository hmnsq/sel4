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
pte_t kernel_ریشه_صفحه_جدول[بیت(PT_شاخص_بیتها)] ALIGN_BSS(بیت(se_L4_صفحه_جدول_بیتها));

#إذا __riscv_xlen != 32
pte_t kernel_image_level1_pt[بیت(PT_شاخص_بیتها)] ALIGN_BSS(بیت(se_L4_صفحه_جدول_بیتها));
pte_t kernel_image_level1_dev_pt[بیت(PT_شاخص_بیتها)] ALIGN_BSS(بیت(se_L4_صفحه_جدول_بیتها));
#وإلاإذا defined(CONFIG_KERNEL_LOG_میانگیر)
pte_t kernel_image_level1_log_میانگیر_pt[بیت(PT_شاخص_بیتها)] ALIGN_BSS(بیت(se_L4_صفحه_جدول_بیتها));
#پایان_اگر

SMP_STATE_DEFINE(core_map_t, coreMap);
