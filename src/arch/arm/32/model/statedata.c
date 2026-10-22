/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <util.h>
#समावेश <api/types.h>
#समावेश <arch/types.h>
#समावेश <arch/model/statedata.h>
#समावेश <arch/object/structures.h>
#समावेश <arch/machine/debug_conf.h>
#समावेश <linker.h>
#समावेश <plat/machine/hardware.h>

/* The top level asid mapping table */
asid_pool_t *armKSASIDTable[nASIDPools];

/* The hardware ASID to virtual ASID mapping table */
asid_t armKSHWASIDTable[BIT(hwASIDBits)];
hw_asid_t armKSNextASID = hwASIDMin;

#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
/* The global, privileged, physically-mapped PD */
pde_t armKSGlobalPD[BIT(PD_सूचकांक_BITS)] ALIGN_BSS(BIT(seL4_PageDirBits));

/* The global, privileged, page table. */
pte_t armKSGlobalPT[BIT(PT_सूचकांक_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));

#अगर_घोषित CONFIG_KERNEL_LOG_बफर
pte_t armKSGlobalLogPT[BIT(PT_सूचकांक_BITS)] ALIGN_BSS(BIT(seL4_PageTableBits));
#पूर्ण_अगर /* CONFIG_KERNEL_LOG_BUFFER */

#अन्यथा
/* The global, hypervisor, level 1 page table */
pdeS1_t  armHSGlobalPGD[BIT(PGD_सूचकांक_BITS)] ALIGN_BSS(BIT(PGD_आकार_BITS));
/* The global, hypervisor, level 2 page table */
pdeS1_t  armHSGlobalPD[BIT(PT_सूचकांक_BITS)]   ALIGN_BSS(BIT(seL4_PageTableBits));
/* The global, hypervisor, level 3 page table */
pteS1_t  armHSGlobalPT[BIT(PT_सूचकांक_BITS)]   ALIGN_BSS(BIT(seL4_PageTableBits));
/* Global user space mappings, which are empty as there is no hared kernel region */
pde_t armUSGlobalPD[BIT(PD_सूचकांक_BITS)] ALIGN_BSS(BIT(seL4_PageDirBits));;
/* Current VCPU */
UP_STATE_DEFINE(vcpu_t, *armHSCurVCPU);
/* Whether the current loaded VCPU is enabled in the hardware or not */
UP_STATE_DEFINE(bool_t, armHSVCPUActive);

#अगर_घोषित CONFIG_HAVE_FPU
/* Whether the hyper-mode kernel is allowed to execute FPU instructions */
UP_STATE_DEFINE(bool_t, armHSFPUEnabled);
#पूर्ण_अगर

#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#अगर_घोषित ARM_BASE_CP14_SAVE_AND_RESTORE
/* Null state for the Debug coprocessor's break/watchpoint registers */
user_breakpoint_state_t armKSNullBreakpointState;
#पूर्ण_अगर

#अगर_घोषित CONFIG_ARM_HAS_TLB_LOCK
word_t arm_KSTlb_Lock_गिनती = 0;
#पूर्ण_अगर /* CONFIG_ARM_HAS_TLB_LOCK */
