/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <util.h>
#ضمن <api/types.h>
#ضمن <arch/types.h>
#ضمن <arch/model/statedata.h>
#ضمن <arch/object/structures.h>
#ضمن <arch/machine/debug_conf.h>
#ضمن <linker.h>
#ضمن <plat/machine/hardware.h>

/* The top level asid mapping table */
asid_pool_t *armKSASIDTable[nASIDPools];

/* The hardware ASID to virtual ASID mapping table */
asid_t armKSHWASIDTable[ٻٽو_انگ(hwASIDBits)];
hw_asid_t armKSNextASID = hwASIDMin;

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
/* The global, privileged, physically-mapped PD */
pde_t armKSGlobalPD[ٻٽو_انگ(PD_اشاريو_بٽ)] ALIGN_BSS(ٻٽو_انگ(se_L4_صفحو_Dir_بٽ));

/* The global, privileged, page table. */
pte_t armKSGlobalPT[ٻٽو_انگ(PT_اشاريو_بٽ)] ALIGN_BSS(ٻٽو_انگ(se_L4_صفحو_جدول_بٽ));

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_LOG_بفر
pte_t armKSGlobalLogPT[ٻٽو_انگ(PT_اشاريو_بٽ)] ALIGN_BSS(ٻٽو_انگ(se_L4_صفحو_جدول_بٽ));
#جيڪڏهن_پورو /* CONFIG_KERNEL_LOG_BUFFER */

#وإلا
/* The global, hypervisor, level 1 page table */
pdeS1_t  armHSGlobalPGD[ٻٽو_انگ(PGD_اشاريو_بٽ)] ALIGN_BSS(ٻٽو_انگ(PGD_سائيز_بٽ));
/* The global, hypervisor, level 2 page table */
pdeS1_t  armHSGlobalPD[ٻٽو_انگ(PT_اشاريو_بٽ)]   ALIGN_BSS(ٻٽو_انگ(se_L4_صفحو_جدول_بٽ));
/* The global, hypervisor, level 3 page table */
pteS1_t  armHSGlobalPT[ٻٽو_انگ(PT_اشاريو_بٽ)]   ALIGN_BSS(ٻٽو_انگ(se_L4_صفحو_جدول_بٽ));
/* Global user space mappings, which are empty as there is no hared kernel region */
pde_t armUSGlobalPD[ٻٽو_انگ(PD_اشاريو_بٽ)] ALIGN_BSS(ٻٽو_انگ(se_L4_صفحو_Dir_بٽ));;
/* Current VCPU */
مٿي_STATE_DEFINE(vcpu_t, *armHSCurVCPU);
/* Whether the current loaded VCPU is enabled in the hardware or not */
مٿي_STATE_DEFINE(bool_t, armHSVCPUActive);

#جيڪڏهن_وصف_ٿيل CONFIG_HAVE_FPU
/* Whether the hyper-mode kernel is allowed to execute FPU instructions */
مٿي_STATE_DEFINE(bool_t, armHSFPUEnabled);
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#جيڪڏهن_وصف_ٿيل ARM_بنياد_CP14_SAVE_AND_RESTORE
/* Null state for the Debug coprocessor's break/watchpoint registers */
user_breakpoint_state_t armKSNullBreakpointState;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HAS_TLB_تالو
لفظ_t arm_KSTlb_تالو_ڳڻپ = 0;
#جيڪڏهن_پورو /* CONFIG_ARM_HAS_TLB_LOCK */
