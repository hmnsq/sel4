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
asid_t armKSHWASIDTable[بټ(hwASIDBits)];
hw_asid_t armKSNextASID = hwASIDMin;

#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
/* The global, privileged, physically-mapped PD */
pde_t armKSGlobalPD[بټ(PD_شاخص_بټونه)] ALIGN_BSS(بټ(se_L4_پاڼه_Dir_بټونه));

/* The global, privileged, page table. */
pte_t armKSGlobalPT[بټ(PT_شاخص_بټونه)] ALIGN_BSS(بټ(se_L4_پاڼه_جدول_بټونه));

#که_تعریف_شوی CONFIG_KERNEL_LOG_بفر
pte_t armKSGlobalLogPT[بټ(PT_شاخص_بټونه)] ALIGN_BSS(بټ(se_L4_پاڼه_جدول_بټونه));
#د_که_پای /* CONFIG_KERNEL_LOG_BUFFER */

#وإلا
/* The global, hypervisor, level 1 page table */
pdeS1_t  armHSGlobalPGD[بټ(PGD_شاخص_بټونه)] ALIGN_BSS(بټ(PGD_اندازه_بټونه));
/* The global, hypervisor, level 2 page table */
pdeS1_t  armHSGlobalPD[بټ(PT_شاخص_بټونه)]   ALIGN_BSS(بټ(se_L4_پاڼه_جدول_بټونه));
/* The global, hypervisor, level 3 page table */
pteS1_t  armHSGlobalPT[بټ(PT_شاخص_بټونه)]   ALIGN_BSS(بټ(se_L4_پاڼه_جدول_بټونه));
/* Global user space mappings, which are empty as there is no hared kernel region */
pde_t armUSGlobalPD[بټ(PD_شاخص_بټونه)] ALIGN_BSS(بټ(se_L4_پاڼه_Dir_بټونه));;
/* Current VCPU */
پورته_STATE_DEFINE(vcpu_t, *armHSCurVCPU);
/* Whether the current loaded VCPU is enabled in the hardware or not */
پورته_STATE_DEFINE(bool_t, armHSVCPUActive);

#که_تعریف_شوی CONFIG_HAVE_FPU
/* Whether the hyper-mode kernel is allowed to execute FPU instructions */
پورته_STATE_DEFINE(bool_t, armHSFPUEnabled);
#د_که_پای

#د_که_پای /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#که_تعریف_شوی ARM_بنسټ_CP14_SAVE_AND_RESTORE
/* Null state for the Debug coprocessor's break/watchpoint registers */
user_breakpoint_state_t armKSNullBreakpointState;
#د_که_پای

#که_تعریف_شوی CONFIG_ARM_HAS_TLB_قلف
کلمه_t arm_KSTlb_قلف_شمېر = 0;
#د_که_پای /* CONFIG_ARM_HAS_TLB_LOCK */
