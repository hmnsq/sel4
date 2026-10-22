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
asid_t armKSHWASIDTable[بیت(hwASIDBits)];
hw_asid_t armKSNextASID = hwASIDMin;

#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
/* The global, privileged, physically-mapped PD */
pde_t armKSGlobalPD[بیت(PD_شاخص_بیتها)] ALIGN_BSS(بیت(se_L4_صفحه_Dir_بیتها));

/* The global, privileged, page table. */
pte_t armKSGlobalPT[بیت(PT_شاخص_بیتها)] ALIGN_BSS(بیت(se_L4_صفحه_جدول_بیتها));

#اگر_تعریف_شده CONFIG_KERNEL_LOG_میانگیر
pte_t armKSGlobalLogPT[بیت(PT_شاخص_بیتها)] ALIGN_BSS(بیت(se_L4_صفحه_جدول_بیتها));
#پایان_اگر /* CONFIG_KERNEL_LOG_BUFFER */

#وإلا
/* The global, hypervisor, level 1 page table */
pdeS1_t  armHSGlobalPGD[بیت(PGD_شاخص_بیتها)] ALIGN_BSS(بیت(PGD_اندازه_بیتها));
/* The global, hypervisor, level 2 page table */
pdeS1_t  armHSGlobalPD[بیت(PT_شاخص_بیتها)]   ALIGN_BSS(بیت(se_L4_صفحه_جدول_بیتها));
/* The global, hypervisor, level 3 page table */
pteS1_t  armHSGlobalPT[بیت(PT_شاخص_بیتها)]   ALIGN_BSS(بیت(se_L4_صفحه_جدول_بیتها));
/* Global user space mappings, which are empty as there is no hared kernel region */
pde_t armUSGlobalPD[بیت(PD_شاخص_بیتها)] ALIGN_BSS(بیت(se_L4_صفحه_Dir_بیتها));;
/* Current VCPU */
به_بالا_STATE_DEFINE(vcpu_t, *armHSCurVCPU);
/* Whether the current loaded VCPU is enabled in the hardware or not */
به_بالا_STATE_DEFINE(bool_t, armHSVCPUActive);

#اگر_تعریف_شده CONFIG_HAVE_FPU
/* Whether the hyper-mode kernel is allowed to execute FPU instructions */
به_بالا_STATE_DEFINE(bool_t, armHSFPUEnabled);
#پایان_اگر

#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#اگر_تعریف_شده ARM_پایه_CP14_SAVE_AND_RESTORE
/* Null state for the Debug coprocessor's break/watchpoint registers */
user_breakpoint_state_t armKSNullBreakpointState;
#پایان_اگر

#اگر_تعریف_شده CONFIG_ARM_HAS_TLB_قفل
کلمه_t arm_KSTlb_قفل_شمار = 0;
#پایان_اگر /* CONFIG_ARM_HAS_TLB_LOCK */
