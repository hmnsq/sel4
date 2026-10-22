/*
 * Copyright 2020, DornerWorks
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <util.h>
#అన్తర్భావ <api/types.h>
#అన్తర్భావ <arch/types.h>
#అన్తర్భావ <arch/model/statedata.h>
#అన్తర్భావ <arch/object/structures.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/hardware.h>

/* The top level asid mapping table */
asid_pool_t *riscvKSASIDTable[nASIDPools];

/* Kernel Page Tables */
pte_t kernel_మూలం_పుట_పట్టిక[బిట్టు(PT_సూచీ_బిట్లు)] ALIGN_BSS(బిట్టు(se_L4_పుట_పట్టిక_బిట్లు));

#ఒకవేళ __riscv_xlen != 32
pte_t kernel_image_level1_pt[బిట్టు(PT_సూచీ_బిట్లు)] ALIGN_BSS(బిట్టు(se_L4_పుట_పట్టిక_బిట్లు));
pte_t kernel_image_level1_dev_pt[బిట్టు(PT_సూచీ_బిట్లు)] ALIGN_BSS(బిట్టు(se_L4_పుట_పట్టిక_బిట్లు));
#లేదావేళ defined(CONFIG_KERNEL_LOG_బఫరు)
pte_t kernel_image_level1_log_బఫరు_pt[బిట్టు(PT_సూచీ_బిట్లు)] ALIGN_BSS(బిట్టు(se_L4_పుట_పట్టిక_బిట్లు));
#అయితే_ముగింపు

SMP_STATE_DEFINE(core_map_t, coreMap);
