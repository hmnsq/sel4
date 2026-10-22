/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <api/types.h>
#అన్తర్భావ <arch/types.h>
#అన్తర్భావ <arch/model/statedata.h>
#అన్తర్భావ <arch/object/structures.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/hardware.h>

#నిర్వచితమైతే CONFIG_ARM_SMMU
#అన్తర్భావ <arch/object/smmu.h>
#అయితే_ముగింపు


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

vspace_మూలం_t armKSGlobalUserVSpace[బిట్టు(se_L4_VSpace_సూచీ_బిట్లు)] ALIGN_BSS(బిట్టు(se_L4_VSpace_బిట్లు));
pte_t armKSGlobalKernelPGD[బిట్టు(PT_సూచీ_బిట్లు)] ALIGN_BSS(బిట్టు(se_L4_పుట_పట్టిక_బిట్లు));

pte_t armKSGlobalKernelPUD[బిట్టు(PT_సూచీ_బిట్లు)] ALIGN_BSS(బిట్టు(se_L4_పుట_పట్టిక_బిట్లు));
pte_t armKSGlobalKernelPDs[బిట్టు(PT_సూచీ_బిట్లు)][బిట్టు(PT_సూచీ_బిట్లు)] ALIGN_BSS(బిట్టు(se_L4_పుట_పట్టిక_బిట్లు));
pte_t armKSGlobalKernelPT[బిట్టు(PT_సూచీ_బిట్లు)] ALIGN_BSS(బిట్టు(se_L4_పుట_పట్టిక_బిట్లు));

#నిర్వచితమైతే CONFIG_KERNEL_LOG_బఫరు
pte_t *armKSGlobalLogPTE = &armKSGlobalKernelPDs[బిట్టు(PT_సూచీ_బిట్లు) - 1][బిట్టు(PT_సూచీ_బిట్లు) - 2];
compile_assert(log_pude_is_correct_preallocated_pude,
               GET_KPT_సూచీ(KS_LOG_PPTR, KLVL_FRM_ARM_PT_LVL(1)) == బిట్టు(PT_సూచీ_బిట్లు) - 1);
compile_assert(log_pde_is_correct_preallocated_pde,
               GET_KPT_సూచీ(KS_LOG_PPTR, KLVL_FRM_ARM_PT_LVL(2)) == బిట్టు(PT_సూచీ_బిట్లు) - 2);
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
పైకి_STATE_DEFINE(vcpu_t, *armHSCurVCPU);
పైకి_STATE_DEFINE(bool_t, armHSVCPUActive);

/* The hardware VMID to virtual ASID mapping table.
 * The ARMv8 supports 8-bit VMID which is used as logical ASID
 * when the kernel runs in EL2.
 */
asid_t armKSHWASIDTable[బిట్టు(hwASIDBits)];
hw_asid_t armKSNextASID = hwASIDMin;
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_ARM_SMMU
/*recording the state of created SID caps*/
bool_t smmuStateSIDTable[SMMU_MAX_SID];
/* CNode containing the cb_cap that is assigned to sids*/
cte_t smmuStateSIDNode[బిట్టు(SMMU_SID_CNODE_స్థానం_బిట్లు)] ALIGN(బిట్టు(SMMU_SID_CNODE_స్థానం_బిట్లు + se_L4_స్థానం_బిట్లు));
compile_assert(smmu_State_SIDCNode_కొలత, పరిమాణం(smmuStateSIDNode) >= ((SMMU_MAX_SID) * పరిమాణం(cte_t)));

/*recording the state of the created cb caps*/
bool_t smmuStateCBTable[SMMU_MAX_CB];
/* CNode containing the vspace root cap that is assigned to sids*/
cte_t smmuStateCBNode[బిట్టు(SMMU_CB_CNODE_స్థానం_బిట్లు)] ALIGN(బిట్టు(SMMU_CB_CNODE_స్థానం_బిట్లు + se_L4_స్థానం_బిట్లు));
compile_assert(smmu_State_CBCNode_కొలత, పరిమాణం(smmuStateCBNode) >= ((SMMU_MAX_CB) * పరిమాణం(cte_t)));
/*recording the context bank to ASID relationship*/
asid_t smmu_State_CBAsid_పట్టిక[SMMU_MAX_CB];
#అయితే_ముగింపు

#నిర్వచితమైతే ARM_పునాది_CP14_SAVE_AND_RESTORE
/* Null state for the Debug coprocessor's break/watchpoint registers */
user_breakpoint_state_t armKSNullBreakpointState;
#అయితే_ముగింపు /* ARM_BASE_CP14_SAVE_AND_RESTORE */
