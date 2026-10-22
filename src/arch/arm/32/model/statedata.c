/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <api/types.h>
#అన్తర్భావ <arch/types.h>
#అన్తర్భావ <arch/model/statedata.h>
#అన్తర్భావ <arch/object/structures.h>
#అన్తర్భావ <arch/machine/debug_conf.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/hardware.h>

/* The top level asid mapping table */
asid_pool_t *armKSASIDTable[nASIDPools];

/* The hardware ASID to virtual ASID mapping table */
asid_t armKSHWASIDTable[బిట్టు(hwASIDBits)];
hw_asid_t armKSNextASID = hwASIDMin;

#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
/* The global, privileged, physically-mapped PD */
pde_t armKSGlobalPD[బిట్టు(PD_సూచీ_బిట్లు)] ALIGN_BSS(బిట్టు(se_L4_పుట_Dir_బిట్లు));

/* The global, privileged, page table. */
pte_t armKSGlobalPT[బిట్టు(PT_సూచీ_బిట్లు)] ALIGN_BSS(బిట్టు(se_L4_పుట_పట్టిక_బిట్లు));

#నిర్వచితమైతే CONFIG_KERNEL_LOG_బఫరు
pte_t armKSGlobalLogPT[బిట్టు(PT_సూచీ_బిట్లు)] ALIGN_BSS(బిట్టు(se_L4_పుట_పట్టిక_బిట్లు));
#అయితే_ముగింపు /* CONFIG_KERNEL_LOG_BUFFER */

#లేకపోతే
/* The global, hypervisor, level 1 page table */
pdeS1_t  armHSGlobalPGD[బిట్టు(PGD_సూచీ_బిట్లు)] ALIGN_BSS(బిట్టు(PGD_కొలత_బిట్లు));
/* The global, hypervisor, level 2 page table */
pdeS1_t  armHSGlobalPD[బిట్టు(PT_సూచీ_బిట్లు)]   ALIGN_BSS(బిట్టు(se_L4_పుట_పట్టిక_బిట్లు));
/* The global, hypervisor, level 3 page table */
pteS1_t  armHSGlobalPT[బిట్టు(PT_సూచీ_బిట్లు)]   ALIGN_BSS(బిట్టు(se_L4_పుట_పట్టిక_బిట్లు));
/* Global user space mappings, which are empty as there is no hared kernel region */
pde_t armUSGlobalPD[బిట్టు(PD_సూచీ_బిట్లు)] ALIGN_BSS(బిట్టు(se_L4_పుట_Dir_బిట్లు));;
/* Current VCPU */
పైకి_STATE_DEFINE(vcpu_t, *armHSCurVCPU);
/* Whether the current loaded VCPU is enabled in the hardware or not */
పైకి_STATE_DEFINE(bool_t, armHSVCPUActive);

#నిర్వచితమైతే CONFIG_HAVE_FPU
/* Whether the hyper-mode kernel is allowed to execute FPU instructions */
పైకి_STATE_DEFINE(bool_t, armHSFPUEnabled);
#అయితే_ముగింపు

#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#నిర్వచితమైతే ARM_పునాది_CP14_SAVE_AND_RESTORE
/* Null state for the Debug coprocessor's break/watchpoint registers */
user_breakpoint_state_t armKSNullBreakpointState;
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_ARM_HAS_TLB_తాళం
మాట_t arm_KSTlb_తాళం_లెక్క = 0;
#అయితే_ముగింపు /* CONFIG_ARM_HAS_TLB_LOCK */
