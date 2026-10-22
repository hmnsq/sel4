/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/debug_conf.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>

/* The top level asid mapping table */
asid_pool_t *armKSASIDTable[nASIDPools];

/* The hardware ASID to virtual ASID mapping table */
asid_t armKSHWASIDTable[ਦੋਆਧਾਰੀ_ਅੰਕ(hwASIDBits)];
hw_asid_t armKSNextASID = hwASIDMin;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
/* The global, privileged, physically-mapped PD */
pde_t armKSGlobalPD[ਦੋਆਧਾਰੀ_ਅੰਕ(PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN_BSS(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_Dir_ਬਿੱਟ));

/* The global, privileged, page table. */
pte_t armKSGlobalPT[ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN_BSS(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_LOG_ਬਫਰ
pte_t armKSGlobalLogPT[ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN_BSS(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ));
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_LOG_BUFFER */

#ਨਹੀਂਤਾਂ
/* The global, hypervisor, level 1 page table */
pdeS1_t  armHSGlobalPGD[ਦੋਆਧਾਰੀ_ਅੰਕ(PGD_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN_BSS(ਦੋਆਧਾਰੀ_ਅੰਕ(PGD_ਮਾਪ_ਬਿੱਟ));
/* The global, hypervisor, level 2 page table */
pdeS1_t  armHSGlobalPD[ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)]   ALIGN_BSS(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ));
/* The global, hypervisor, level 3 page table */
pteS1_t  armHSGlobalPT[ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)]   ALIGN_BSS(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ));
/* Global user space mappings, which are empty as there is no hared kernel region */
pde_t armUSGlobalPD[ਦੋਆਧਾਰੀ_ਅੰਕ(PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN_BSS(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_Dir_ਬਿੱਟ));;
/* Current VCPU */
ਉੱਪਰ_STATE_DEFINE(vcpu_t, *armHSCurVCPU);
/* Whether the current loaded VCPU is enabled in the hardware or not */
ਉੱਪਰ_STATE_DEFINE(bool_t, armHSVCPUActive);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
/* Whether the hyper-mode kernel is allowed to execute FPU instructions */
ਉੱਪਰ_STATE_DEFINE(bool_t, armHSFPUEnabled);
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ARM_ਅਧਾਰ_CP14_SAVE_AND_RESTORE
/* Null state for the Debug coprocessor's break/watchpoint registers */
user_breakpoint_state_t armKSNullBreakpointState;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HAS_TLB_ਤਾਲਾ
ਸ਼ਬਦ_t arm_KSTlb_ਤਾਲਾ_ਗਿਣਤੀ = 0;
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HAS_TLB_LOCK */
