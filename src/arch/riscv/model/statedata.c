/*
 * Copyright 2020, DornerWorks
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <util.h>
#شامل <api/types.h>
#شامل <arch/types.h>
#شامل <arch/model/statedata.h>
#شامل <arch/object/structures.h>
#شامل <linker.h>
#شامل <plat/machine/hardware.h>

/* The top level asid mapping table */
asid_pool_t *riscvKSASIDTable[nASIDPools];

/* Kernel Page Tables */
pte_t kernel_جڑ_صفحہ_جدول[ثنائی_ہندسہ(PT_اشاریہ_بٹ)] ALIGN_BSS(ثنائی_ہندسہ(se_L4_صفحہ_جدول_بٹ));

#اگر __riscv_xlen != 32
pte_t kernel_image_level1_pt[ثنائی_ہندسہ(PT_اشاریہ_بٹ)] ALIGN_BSS(ثنائی_ہندسہ(se_L4_صفحہ_جدول_بٹ));
pte_t kernel_image_level1_dev_pt[ثنائی_ہندسہ(PT_اشاریہ_بٹ)] ALIGN_BSS(ثنائی_ہندسہ(se_L4_صفحہ_جدول_بٹ));
#وگرنہ_اگر defined(CONFIG_KERNEL_LOG_بفر)
pte_t kernel_image_level1_log_بفر_pt[ثنائی_ہندسہ(PT_اشاریہ_بٹ)] ALIGN_BSS(ثنائی_ہندسہ(se_L4_صفحہ_جدول_بٹ));
#اگر_ختم

SMP_STATE_DEFINE(core_map_t, coreMap);
