/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <api/types.h>
#अन्तर्भाव <arch/types.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <arch/object/structures.h>
#अन्तर्भाव <arch/machine/debug_conf.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine/hardware.h>

/* The top level asid mapping table */
asid_pool_t *armKSASIDTable[nASIDPools];

/* The hardware ASID to virtual ASID mapping table */
asid_t armKSHWASIDTable[BIT(hwASIDBits)];
hw_asid_t armKSNextASID = hwASIDMin;

#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
/* The global, privileged, physically-mapped PD */
pde_t armKSGlobalPD[BIT(PD_सूचकांक_द्वयङ्काः)] ALIGN_BSS(BIT(se_L4_Page_Dir_द्वयङ्काः));

/* The global, privileged, page table. */
pte_t armKSGlobalPT[BIT(PT_सूचकांक_द्वयङ्काः)] ALIGN_BSS(BIT(se_L4_Page_सारणी_द्वयङ्काः));

#यदि_परिभाषितम् CONFIG_KERNEL_LOG_बफर
pte_t armKSGlobalLogPT[BIT(PT_सूचकांक_द्वयङ्काः)] ALIGN_BSS(BIT(se_L4_Page_सारणी_द्वयङ्काः));
#यदि_अन्तः /* CONFIG_KERNEL_LOG_BUFFER */

#अन्यथा
/* The global, hypervisor, level 1 page table */
pdeS1_t  armHSGlobalPGD[BIT(PGD_सूचकांक_द्वयङ्काः)] ALIGN_BSS(BIT(PGD_आकार_द्वयङ्काः));
/* The global, hypervisor, level 2 page table */
pdeS1_t  armHSGlobalPD[BIT(PT_सूचकांक_द्वयङ्काः)]   ALIGN_BSS(BIT(se_L4_Page_सारणी_द्वयङ्काः));
/* The global, hypervisor, level 3 page table */
pteS1_t  armHSGlobalPT[BIT(PT_सूचकांक_द्वयङ्काः)]   ALIGN_BSS(BIT(se_L4_Page_सारणी_द्वयङ्काः));
/* Global user space mappings, which are empty as there is no hared kernel region */
pde_t armUSGlobalPD[BIT(PD_सूचकांक_द्वयङ्काः)] ALIGN_BSS(BIT(se_L4_Page_Dir_द्वयङ्काः));;
/* Current VCPU */
ऊर्ध्वम्_STATE_DEFINE(vcpu_t, *armHSCurVCPU);
/* Whether the current loaded VCPU is enabled in the hardware or not */
ऊर्ध्वम्_STATE_DEFINE(bool_t, armHSVCPUActive);

#यदि_परिभाषितम् CONFIG_HAVE_FPU
/* Whether the hyper-mode kernel is allowed to execute FPU instructions */
ऊर्ध्वम्_STATE_DEFINE(bool_t, armHSFPUEnabled);
#यदि_अन्तः

#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#यदि_परिभाषितम् ARM_आधारः_CP14_SAVE_AND_RESTORE
/* Null state for the Debug coprocessor's break/watchpoint registers */
user_breakpoint_state_t armKSNullBreakpointState;
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_ARM_HAS_TLB_तालकम्
word_t arm_KSTlb_तालकम्_गणनम् = 0;
#यदि_अन्तः /* CONFIG_ARM_HAS_TLB_LOCK */
