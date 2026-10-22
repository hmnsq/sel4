/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <api/types.h>
#ಅನ್ತರ್ಭಾವ <arch/types.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/object/structures.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
#ಅನ್ತರ್ಭಾವ <arch/object/smmu.h>
#ಆದರೆ_ಅಂತ್ಯ


asid_pool_t *armKSASIDTable[nASIDPools];

/* AArch64 Memory map explanation:
 *
 * EL1 and EL2 kernel build vaddrspace layouts:
 *
 * On AArch64, the EL1 and EL2 builds of the kernel both use approx 512GiB of
 * virtual address space.
 *
 * The EL1 kernel resides within approx 512 GiB of virtual memory somewhere
 * within the canonical top (not necessarily the same as the actual top, but all
 * the unused high bits of kernel virtual addresses are set to 1) of every
 * user VSpace.
 *
 * The EL2 kernel resides within approx 512 GiB of virtual memory somewhere
 * within the canonical bottom (all the unused high bits are set to 0) of its
 * own separate virtual address space.
 *
 * Common Aarch64 address space layout:
 *
 * The reason why 512 GiB was chosen is because assuming a 48-bit virtual
 * address space using a 4KiB Translation Granule (and therefore, 4 levels of
 * page tables):
 *
 * One top-level page-structure entry maps 512 GiB, so by limiting ourselves to
 * 512 GiB, we only need to pre-allocate 1 level0 table (lvl0 is the top level),
 * 1 level1 table, 512 level2 tables (with each level2 entry mapping 2MiB), and
 * skip level3.
 *
 * We do maintain a single level3 table however, which is mapped into the last
 * entry in the last level2 table, such that the last 2MiB are mapped using
 * 4KiB pages instead of 2MiB pages. The reason for this last 2MiB being mapped
 * using small pages is because this last 2MiB is where the kernel maps all the
 * different devices it uses (see map_kernel_devices()). This implies that the
 * kernel can only have up to approx 512GiB of kernel untypeds.
 *
 * If you wish for your AArch64 platform to use more than 512 GiB of virtual
 * memory, you will need to change the number of pre-allocated page tables below
 * to be sufficient to contain the mapping you want. And don't forget to update
 * this comment here afterward.
 */

/* User vaddrspace layouts:
 *
 * For EL2:
 *
 * A plain-english explanation of the memory layout is that the
 * the user address spaces cover the address range from 0x0 to the maximum
 * IPA.
 *
 * So for a CPU that can generate 44 bits of IPA/PA (such as the TX1/TX2), user
 * vaddr spaces will cover 16TiB from 0x0 to 0x00000fff_ffffffff.
 *
 * Basically by making the guest physical address spaces 44 bits, the guest
 * kernels can access all of (what they think is) physical memory, while
 * allowing us to potentially trap accesses by the guests to physical memory
 * beyond what the processor can address.
 *
 * For EL1:
 *
 * The standard canonical-high and canonical-low split using TCR_EL1.TBI
 * applies.
 */

vspace_ಬೇರು_t armKSGlobalUserVSpace[ಬಿಟ್(se_L4_VSpace_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN_BSS(ಬಿಟ್(se_L4_VSpace_ಬಿಟ್ಟುಗಳು));
pte_t armKSGlobalKernelPGD[ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN_BSS(ಬಿಟ್(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು));

pte_t armKSGlobalKernelPUD[ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN_BSS(ಬಿಟ್(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು));
pte_t armKSGlobalKernelPDs[ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)][ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN_BSS(ಬಿಟ್(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು));
pte_t armKSGlobalKernelPT[ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN_BSS(ಬಿಟ್(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_LOG_ಬಫರ್
pte_t *armKSGlobalLogPTE = &armKSGlobalKernelPDs[ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1][ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 2];
compile_assert(log_pude_is_correct_preallocated_pude,
               GET_KPT_ಸೂಚ್ಯಂಕ(KS_LOG_PPTR, KLVL_FRM_ARM_PT_LVL(1)) == ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1);
compile_assert(log_pde_is_correct_preallocated_pde,
               GET_KPT_ಸೂಚ್ಯಂಕ(KS_LOG_PPTR, KLVL_FRM_ARM_PT_LVL(2)) == ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 2);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
ಮೇಲೆ_STATE_DEFINE(vcpu_t, *armHSCurVCPU);
ಮೇಲೆ_STATE_DEFINE(bool_t, armHSVCPUActive);

/* The hardware VMID to virtual ASID mapping table.
 * The ARMv8 supports 8-bit VMID which is used as logical ASID
 * when the kernel runs in EL2.
 */
asid_t armKSHWASIDTable[ಬಿಟ್(hwASIDBits)];
hw_asid_t armKSNextASID = hwASIDMin;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
/*recording the state of created SID caps*/
bool_t smmuStateSIDTable[SMMU_MAX_SID];
/* CNode containing the cb_cap that is assigned to sids*/
cte_t smmuStateSIDNode[ಬಿಟ್(SMMU_SID_CNODE_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(SMMU_SID_CNODE_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು + se_L4_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು));
compile_assert(smmu_State_SIDCNode_ಗಾತ್ರ, ಗಾತ್ರ(smmuStateSIDNode) >= ((SMMU_MAX_SID) * ಗಾತ್ರ(cte_t)));

/*recording the state of the created cb caps*/
bool_t smmuStateCBTable[SMMU_MAX_CB];
/* CNode containing the vspace root cap that is assigned to sids*/
cte_t smmuStateCBNode[ಬಿಟ್(SMMU_CB_CNODE_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(SMMU_CB_CNODE_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು + se_L4_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು));
compile_assert(smmu_State_CBCNode_ಗಾತ್ರ, ಗಾತ್ರ(smmuStateCBNode) >= ((SMMU_MAX_CB) * ಗಾತ್ರ(cte_t)));
/*recording the context bank to ASID relationship*/
asid_t smmu_State_CBAsid_ಕೋಷ್ಟಕ[SMMU_MAX_CB];
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ARM_ತಳ_CP14_SAVE_AND_RESTORE
/* Null state for the Debug coprocessor's break/watchpoint registers */
user_breakpoint_state_t armKSNullBreakpointState;
#ಆದರೆ_ಅಂತ್ಯ /* ARM_BASE_CP14_SAVE_AND_RESTORE */
