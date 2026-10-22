/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <util.h>
#شامل <api/types.h>
#شامل <arch/types.h>
#شامل <arch/model/statedata.h>
#شامل <arch/object/structures.h>
#شامل <arch/machine/debug_conf.h>
#شامل <linker.h>
#شامل <plat/machine/hardware.h>

/* The top level asid mapping table */
asid_pool_t *armKSASIDTable[nASIDPools];

/* The hardware ASID to virtual ASID mapping table */
asid_t armKSHWASIDTable[ثنائی_ہندسہ(hwASIDBits)];
hw_asid_t armKSNextASID = hwASIDMin;

#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
/* The global, privileged, physically-mapped PD */
pde_t armKSGlobalPD[ثنائی_ہندسہ(PD_اشاریہ_بٹ)] ALIGN_BSS(ثنائی_ہندسہ(se_L4_صفحہ_Dir_بٹ));

/* The global, privileged, page table. */
pte_t armKSGlobalPT[ثنائی_ہندسہ(PT_اشاریہ_بٹ)] ALIGN_BSS(ثنائی_ہندسہ(se_L4_صفحہ_جدول_بٹ));

#اگر_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
pte_t armKSGlobalLogPT[ثنائی_ہندسہ(PT_اشاریہ_بٹ)] ALIGN_BSS(ثنائی_ہندسہ(se_L4_صفحہ_جدول_بٹ));
#اگر_ختم /* CONFIG_KERNEL_LOG_BUFFER */

#ورنہ
/* The global, hypervisor, level 1 page table */
pdeS1_t  armHSGlobalPGD[ثنائی_ہندسہ(PGD_اشاریہ_بٹ)] ALIGN_BSS(ثنائی_ہندسہ(PGD_سائز_بٹ));
/* The global, hypervisor, level 2 page table */
pdeS1_t  armHSGlobalPD[ثنائی_ہندسہ(PT_اشاریہ_بٹ)]   ALIGN_BSS(ثنائی_ہندسہ(se_L4_صفحہ_جدول_بٹ));
/* The global, hypervisor, level 3 page table */
pteS1_t  armHSGlobalPT[ثنائی_ہندسہ(PT_اشاریہ_بٹ)]   ALIGN_BSS(ثنائی_ہندسہ(se_L4_صفحہ_جدول_بٹ));
/* Global user space mappings, which are empty as there is no hared kernel region */
pde_t armUSGlobalPD[ثنائی_ہندسہ(PD_اشاریہ_بٹ)] ALIGN_BSS(ثنائی_ہندسہ(se_L4_صفحہ_Dir_بٹ));;
/* Current VCPU */
اوپر_STATE_DEFINE(vcpu_t, *armHSCurVCPU);
/* Whether the current loaded VCPU is enabled in the hardware or not */
اوپر_STATE_DEFINE(bool_t, armHSVCPUActive);

#اگر_تعریف_شدہ CONFIG_HAVE_FPU
/* Whether the hyper-mode kernel is allowed to execute FPU instructions */
اوپر_STATE_DEFINE(bool_t, armHSFPUEnabled);
#اگر_ختم

#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#اگر_تعریف_شدہ ARM_بنیاد_CP14_SAVE_AND_RESTORE
/* Null state for the Debug coprocessor's break/watchpoint registers */
user_breakpoint_state_t armKSNullBreakpointState;
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_ARM_HAS_TLB_تالا
لفظ_t arm_KSTlb_تالا_گنتی = 0;
#اگر_ختم /* CONFIG_ARM_HAS_TLB_LOCK */
