/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <api/types.h>
#ಅನ್ತರ್ಭಾವ <arch/types.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/object/structures.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/debug_conf.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>

/* The top level asid mapping table */
asid_pool_t *armKSASIDTable[nASIDPools];

/* The hardware ASID to virtual ASID mapping table */
asid_t armKSHWASIDTable[ಬಿಟ್(hwASIDBits)];
hw_asid_t armKSNextASID = hwASIDMin;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
/* The global, privileged, physically-mapped PD */
pde_t armKSGlobalPD[ಬಿಟ್(PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN_BSS(ಬಿಟ್(se_L4_ಪುಟ_Dir_ಬಿಟ್ಟುಗಳು));

/* The global, privileged, page table. */
pte_t armKSGlobalPT[ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN_BSS(ಬಿಟ್(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_LOG_ಬಫರ್
pte_t armKSGlobalLogPT[ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN_BSS(ಬಿಟ್(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು));
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_LOG_BUFFER */

#ಇಲ್ಲದಿದ್ದರೆ
/* The global, hypervisor, level 1 page table */
pdeS1_t  armHSGlobalPGD[ಬಿಟ್(PGD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN_BSS(ಬಿಟ್(PGD_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು));
/* The global, hypervisor, level 2 page table */
pdeS1_t  armHSGlobalPD[ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)]   ALIGN_BSS(ಬಿಟ್(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು));
/* The global, hypervisor, level 3 page table */
pteS1_t  armHSGlobalPT[ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)]   ALIGN_BSS(ಬಿಟ್(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು));
/* Global user space mappings, which are empty as there is no hared kernel region */
pde_t armUSGlobalPD[ಬಿಟ್(PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN_BSS(ಬಿಟ್(se_L4_ಪುಟ_Dir_ಬಿಟ್ಟುಗಳು));;
/* Current VCPU */
ಮೇಲೆ_STATE_DEFINE(vcpu_t, *armHSCurVCPU);
/* Whether the current loaded VCPU is enabled in the hardware or not */
ಮೇಲೆ_STATE_DEFINE(bool_t, armHSVCPUActive);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
/* Whether the hyper-mode kernel is allowed to execute FPU instructions */
ಮೇಲೆ_STATE_DEFINE(bool_t, armHSFPUEnabled);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ARM_ತಳ_CP14_SAVE_AND_RESTORE
/* Null state for the Debug coprocessor's break/watchpoint registers */
user_breakpoint_state_t armKSNullBreakpointState;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HAS_TLB_ಬೀಗ
ಶಬ್ದ_t arm_KSTlb_ಬೀಗ_ಎಣಿಕೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HAS_TLB_LOCK */
