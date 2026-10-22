/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <kernel/stack.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <machine/debug.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <object/cnode.h>
#ಅನ್ತರ್ಭಾವ <object/untyped.h>
#ಅನ್ತರ್ಭಾವ <arch/api/invocation.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>
#ಅನ್ತರ್ಭಾವ <armv/context_switch.h>
#ಅನ್ತರ್ಭಾವ <arch/object/iospace.h>
#ಅನ್ತರ್ಭಾವ <arch/object/vcpu.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/tlb.h>

/*
 * Memory types are defined in Memory Attribute Indirection Register.
 *  - nGnRnE Device non-Gathering, non-Reordering, No Early write acknowledgement
 *  - nGnRE Unused Device non-Gathering, non-Reordering, Early write acknowledgement
 *  - GRE Unused Device Gathering, Reordering, Early write acknowledgement
 *  - NORMAL_NC Normal Memory, Inner/Outer non-cacheable
 *  - NORMAL Normal Memory, Inner/Outer Write-back non-transient, Write-allocate, Read-allocate
 *  - NORMAL_WT Normal Memory, Inner/Outer Write-through non-transient, No-Write-allocate, Read-allocate
 * Note: These should match with contents of MAIR_EL1 register!
 */
ಎಣಿಕೆ mair_types {
    DEVICE_nGnRnE = 0,
    DEVICE_nGnRE = 1,
    DEVICE_GRE = 2,
    ಸಾಮಾನ್ಯ_NC = 3,
    ಸಾಮಾನ್ಯ = 4,
    ಸಾಮಾನ್ಯ_WT = 5
};

/* Stage-2 translation memory attributes */
ಎಣಿಕೆ mair_s2_types {
    S2_DEVICE_nGnRnE = 0,
    S2_DEVICE_nGnRE = 1,
    S2_DEVICE_nGRE  = 2,
    S2_DEVICE_GRE = 3,

    S2_ಸಾಮಾನ್ಯ_INNER_NC_OUTER_NC = 5,
    S2_ಸಾಮಾನ್ಯ_INNER_WTC_OUTER_NC = 6,
    S2_ಸಾಮಾನ್ಯ_INNER_WBC_OUTER_NC = 7,

    S2_ಸಾಮಾನ್ಯ_INNER_NC_OUTER_WTC = 9,
    S2_ಸಾಮಾನ್ಯ_INNER_WTC_OUTER_WTC = 10,
    S2_ಸಾಮಾನ್ಯ_INNER_WBC_OUTER_WTC = 11,

    S2_ಸಾಮಾನ್ಯ_INNER_NC_OUTER_WBC = 13,
    S2_ಸಾಮಾನ್ಯ_INNER_WTC_OUTER_WBC = 14,
    S2_ಸಾಮಾನ್ಯ_INNER_WBC_OUTER_WBC = 15,

    S2_ಸಾಮಾನ್ಯ = S2_ಸಾಮಾನ್ಯ_INNER_WBC_OUTER_WBC
};

/* Leif from Linaro said the big.LITTLE clusters should be treated as
 * inner shareable, and we believe so, although the Example B2-1 given in
 * ARM ARM DDI 0487B.b (ID092517) says otherwise.
 */

#ವ್ಯಾಖ್ಯಾನಿಸು SMP_SHARE   3

ರಚನೆ lookupPTSlot_ret {
    pte_t *pt_ಸ್ಥಾನ;
    ಶಬ್ದ_t pt_ಬಿಟ್ಟುಗಳು_ಎಡ;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ lookupPTSlot_ret lookupPTSlot_ret_t;

ರಚನೆ ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret {
    exception_t ಸ್ಥಿತಿ_2;
    vspace_ಬೇರು_t *vspace_ಬೇರು;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t;

/* Stage-1 access permissions:
 * AP[2:1]  higher EL        EL0
 *   00       rw            None
 *   01       rw            rw
 *   10       r             None
 *   11       r             r
 *
 * Stage-2 access permissions:
 * S2AP    Access from Nonsecure EL1 or Non-secure EL0
 *  00                      None
 *  01                      r
 *  10                      w
 *  11                      rw
 *
 *  For VMs or native seL4 applications, if hypervisor support
 *  is enabled, we use the S2AP. The kernel itself running in
 *  EL2 still uses the Stage-1 AP format.
 */

ಸ್ಥಿರ ಶಬ್ದ_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    ಆಯ್ಕೆ (vm_rights) {
    ಸ್ಥಿತಿ VMKernelOnly:
        ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            ಹಿಂತಿರುಗಿ 0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ 0;
        }

    ಸ್ಥಿತಿ VMReadWrite:
        ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            ಹಿಂತಿರುಗಿ 3;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ 1;
        }

    ಸ್ಥಿತಿ VMReadOnly:
        ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            ಹಿಂತಿರುಗಿ 1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ 3;
        }

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid VM rights");
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
ಸ್ಥಿರ ಸಾಲೊಳಗೆ CONST ಶಬ್ದ_t pte_get_AP(pte_t pte)
{
    ಪ್ರತಿಜ್ಞಾ(pte_is_ಪುಟ_type(pte));
    ಆಯ್ಕೆ (pte_get_pte_type(pte)) {
    ಸ್ಥಿತಿ pte_pte_4k_ಪುಟ:
        ಹಿಂತಿರುಗಿ pte_pte_4k_ಪುಟ_get_AP(pte);

    ಸ್ಥಿತಿ pte_pte_ಪುಟ:
        ಹಿಂತಿರುಗಿ pte_pte_ಪುಟ_get_AP(pte);

    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ 0;
    }
}

ಸ್ಥಿರ vm_rights_t CONST vmRightsFromPTE(pte_t pte)
{
    ಶಬ್ದ_t access_perms = pte_get_AP(pte);
    ಆಯ್ಕೆ (access_perms) {
    ಸ್ಥಿತಿ 0:
    ಸ್ಥಿತಿ 2:
        ಹಿಂತಿರುಗಿ VMKernelOnly;
    ಸ್ಥಿತಿ 1:
        ಹಿಂತಿರುಗಿ VMReadWrite;
    ಸ್ಥಿತಿ 3:
        ಹಿಂತಿರುಗಿ VMReadOnly;
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid AP bit");
    }
}
#ಆದರೆ_ಅಂತ್ಯ

vm_rights_t CONST ಮುಖವಾಡ_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_ಮುಖವಾಡ)
{
    ಆದರೆ (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_ಮುಖವಾಡ)) {
        ಹಿಂತಿರುಗಿ VMReadOnly;
    }
    ಆದರೆ (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_ಮುಖವಾಡ)) {
        ಆದರೆ (!seL4_CapRights_get_capAllowWrite(cap_rights_ಮುಖವಾಡ)) {
            ಹಿಂತಿರುಗಿ VMReadOnly;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ VMReadWrite;
        }
    }
    ಆದರೆ (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_ಮುಖವಾಡ) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_ಮುಖವಾಡ)) {
        userError("Attempted to make unsupported write only mapping");
    }
    ಹಿಂತಿರುಗಿ VMKernelOnly;
}

/* ==================== BOOT CODE STARTS HERE ==================== */

/* The 54th bit is defined as UXN (unprivileged execute-never) for stage 1
 * of any translation regime for which stage 1 translation can support
 * two VA ranges. This field applies only to execution at EL0. A value
 * of 0 indicates that this control permits execution.
 *
 * The 54th bit is defined as XN (execute-never) for stage 1 of any translation
 * regime for which the stage 1 translation can support only a singe VA range or
 * stage 2 translation when ARMVv8.2-TTS2UXN is not implemented.
 * This field applies to execution at any exception level to which the stage of
 * translation applies. A value of 0 indicates that this control permits execution.
 *
 * When the kernel is running in EL2, the stage-1 translation only supports one
 * VA range so that the 54th bit is XN. Setting the bit to 0 allows execution.
 *
 */
BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_kernel_ಚೌಕಟ್ಟು(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_ಗುಣಲಕ್ಷಣಗಳು_t ಗುಣಲಕ್ಷಣಗಳು)
{
    ಪ್ರತಿಜ್ಞಾ(vaddr >= PPTR_ತುದಿ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಶಬ್ದ_t uxn = vm_ಗುಣಲಕ್ಷಣಗಳು_get_arm_Execute_Never(ಗುಣಲಕ್ಷಣಗಳು);
#ಇಲ್ಲದಿದ್ದರೆ
    ಶಬ್ದ_t uxn = 1; /* unprivileged execute never */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    ಶಬ್ದ_t attr_ಸೂಚ್ಯಂಕ;
    ಶಬ್ದ_t shareable;
    ಆದರೆ (vm_ಗುಣಲಕ್ಷಣಗಳು_get_arm_ಪುಟ_Cacheable(ಗುಣಲಕ್ಷಣಗಳು)) {
        attr_ಸೂಚ್ಯಂಕ = ಸಾಮಾನ್ಯ;
        shareable = SMP_TERNARY(SMP_SHARE, 0);
    } ಇಲ್ಲದಿದ್ದರೆ {
        attr_ಸೂಚ್ಯಂಕ = DEVICE_nGnRnE;
        shareable = 0;
    }
    armKSGlobalKernelPT[GET_KPT_ಸೂಚ್ಯಂಕ(vaddr, KLVL_FRM_ARM_PT_LVL(3))] = pte_pte_4k_ಪುಟ_new(uxn, paddr,
                                                                                            0, /* global */
                                                                                            1, /* access flag */
                                                                                            shareable,
                                                                                            APFromVMRights(vm_rights),
                                                                                            attr_ಸೂಚ್ಯಂಕ);
}

/* Essentially, assert that the ELF, kernel devices, and log buffer are all in the same PUD.
   In the current code this is the top index. This is arbitrary, the important part
   is that they are all the same.
 */
compile_assert(elf_ಒಳ_kernel_mappings, GET_KPT_ಸೂಚ್ಯಂಕ(KERNEL_ELF_ತಳ,
                                                     KLVL_FRM_ARM_PT_LVL(1)) == ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1);
compile_assert(kdev_ಒಳ_kernel_mappings, GET_KPT_ಸೂಚ್ಯಂಕ(KDEV_ತಳ,
                                                      KLVL_FRM_ARM_PT_LVL(1)) == ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1);
compile_assert(log_buf_ಒಳ_kernel_mappings, GET_KPT_ಸೂಚ್ಯಂಕ(KS_LOG_ತಳ,
                                                         KLVL_FRM_ARM_PT_LVL(1)) == ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1);

/**
 *  Per ARM ARM DDI 0487 (version L.b), in B2.11 "Mismatched memory attributes",
 *  when physical memory locations are accessed with mismatched attributes
 *  (caching, memory type (device/normal), shareability), the coherency of these
 *  memory locations may be lost, as well as various violations of read and
 *  write orderings or atomic operations. Other ARM documents refer to the
 *  situation in which two virtual addresses map to the same physical address as
 *  "Memory aliasing" (e.g. Document ID 102376 0200_01_en).
 *
 *  Currently, the memory attributes used in the physical memory window does
 *  match that of the kernel ELF mapping, so this is OK. At the same time,
 *  the current physical window mapping does use NORMAL memory covering memory
 *  that is not DRAM, which is questionable for newer processors with more
 *  speculative execution. (FIXME)
 *
 *  Hence, the kernel should never access kernel ELF memory through the physical
 *  memory window, i.e., never modify the result of `ptrFromPAddr()` when the
 *  physical address lies within the kernel ELF. (Reads are fine if they
 *  have been flushed appropriately per B2.11; this should be the case for
 *  any paging structures).
 *
 *  As an addendum, LWN Article "ARM's multiply-mapped memory mess" specifies
 *  that in ARMv6 specifications this kind of aliasing would be UNPREDICTABLE;
 *  the current ARMv8 A-Profile specifications no longer contain this wording,
 *  and instead specify the loss of coherency and other behaviours in B2.11.
 */
BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_kernel_elf_image(pte_t kernel_mapping_pds[])
{
    /** This is not strictly necessary, but to handle a non-aligned KERNEL_ELF_PADDR_BASE
     *  we'd need to do a bit more logic and probably do what riscv64 does
     *  where the *actual* KERNEL_ELF_BASE is rounded up to the alignment.
     *  This is simpler and memory is cheap.
     **/
    compile_assert(elf_paddr_aligned, IS_ALIGNED(KERNEL_ELF_PADDR_ತಳ_RAW, se_L4_Large_ಪುಟ_ಬಿಟ್ಟುಗಳು));
    compile_assert(elf_ತಳ_aligned, IS_ALIGNED(KERNEL_ELF_ತಳ, se_L4_Large_ಪುಟ_ಬಿಟ್ಟುಗಳು));

    /* Please don't overlap with KDEV_BASE */
    ಪ್ರತಿಜ್ಞಾ(KERNEL_ELF_ತುದಿ <= KDEV_ತಳ);
    /* Please don't overlap with KS_LOG_PPTR */
    ಪ್ರತಿಜ್ಞಾ(KERNEL_ELF_ತುದಿ <= KS_LOG_PPTR);

    ಸ್ಥಿರಾಂಕ ಶಬ್ದ_t pd_ಆರಂಭ = GET_KPT_ಸೂಚ್ಯಂಕ(KERNEL_ELF_ತಳ, KLVL_FRM_ARM_PT_LVL(2));
    /* pd_end and ELF_TOP are both exclusive, so handle that correctly */
    ಸ್ಥಿರಾಂಕ ಶಬ್ದ_t pd_ಕೊನೆ   = GET_KPT_ಸೂಚ್ಯಂಕ(KERNEL_ELF_ತುದಿ - 1, KLVL_FRM_ARM_PT_LVL(2)) + 1;

    paddr_t paddr = KERNEL_ELF_PADDR_ತಳ;
    ಬಳಸದ pptr_t vaddr = KERNEL_ELF_ತಳ;

    ಗಾಗಿ (ಶಬ್ದ_t pd = pd_ಆರಂಭ; pd < pd_ಕೊನೆ; pd++) {
        ಪ್ರತಿಜ್ಞಾ(pd == GET_KPT_ಸೂಚ್ಯಂಕ(vaddr, KLVL_FRM_ARM_PT_LVL(2)));

        kernel_mapping_pds[pd] = pte_pte_ಪುಟ_new(
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
                                     /* XN */ 0,
#ಇಲ್ಲದಿದ್ದರೆ
                                     /* UXN */ 1,
#ಆದರೆ_ಅಂತ್ಯ
                                     /* page_base_address */ paddr,
                                     /* global (nG) */ 0,
                                     /* access flag (AF) */ 1,
                                     /* shareability (SH) */ SMP_TERNARY(SMP_SHARE, 0),
                                     /* AP */ APFromVMRights(VMKernelOnly),
                                     /* AttrIndx */ ಸಾಮಾನ್ಯ
                                 );

        paddr += ಬಿಟ್(se_L4_Large_ಪುಟ_ಬಿಟ್ಟುಗಳು);
        vaddr += ಬಿಟ್(se_L4_Large_ಪುಟ_ಬಿಟ್ಟುಗಳು);
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
/* verify that the physical memory window as at the second entry of the PGD */
compile_assert(physical_ಕಿಟಕಿ_pgd_location,
               GET_KPT_ಸೂಚ್ಯಂಕ(PPTR_ತಳ, KLVL_FRM_ARM_PT_LVL(0)) == 1);
#ಇಲ್ಲದಿದ್ದರೆ
/* verify that the physical memory window as at the last entry of the PGD */
compile_assert(physical_ಕಿಟಕಿ_pgd_location,
               GET_KPT_ಸೂಚ್ಯಂಕ(PPTR_ತಳ, KLVL_FRM_ARM_PT_LVL(0)) == ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1);
#ಆದರೆ_ಅಂತ್ಯ
/* make sure that the PPTR_BASE and PPTR_TOP are huge paged aligned */
compile_assert(physical_ಕಿಟಕಿ_ತಳ_aligned, IS_ALIGNED(PPTR_ತಳ, se_L4_Huge_ಪುಟ_ಬಿಟ್ಟುಗಳು));
compile_assert(physical_ಕಿಟಕಿ_ತುದಿ_aligned, IS_ALIGNED(PPTR_ತುದಿ, se_L4_Huge_ಪುಟ_ಬಿಟ್ಟುಗಳು));

/* the PPTR_BASE starts from the bottom of the kernel PUD */
compile_assert(physical_ಕಿಟಕಿ_bottom_kernel_region,
               GET_KPT_ಸೂಚ್ಯಂಕ(PPTR_ತಳ, KLVL_FRM_ARM_PT_LVL(1)) == 0);
/* the PPTR_TOP then ends at the second-to-last entry of the PUD
    note: because this is the exclusive value the assert is one higher than
          you would expect.
*/
compile_assert(physical_ಕಿಟಕಿ_ತುದಿ_kernel_region,
               GET_KPT_ಸೂಚ್ಯಂಕ(PPTR_ತುದಿ, KLVL_FRM_ARM_PT_LVL(1)) == ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1);

BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_kernel_physical_ಕಿಟಕಿ(ಶೂನ್ಯ)
{
    paddr_t paddr;
    pptr_t vaddr;
    ಶಬ್ದ_t idx;

    /* place all PDs except the last one in PUD */
    ಗಾಗಿ (idx = GET_KPT_ಸೂಚ್ಯಂಕ(PPTR_ತಳ, KLVL_FRM_ARM_PT_LVL(1)); idx < GET_KPT_ಸೂಚ್ಯಂಕ(PPTR_ತುದಿ, KLVL_FRM_ARM_PT_LVL(1));
         idx++) {
        armKSGlobalKernelPUD[idx] = pte_pte_ಕೋಷ್ಟಕ_new(
                                        addrFromKPPtr(&armKSGlobalKernelPDs[idx][0])
                                    );
    }

    /* map the kernel window using large pages */
    vaddr = PPTR_ತಳ;
    ಗಾಗಿ (paddr = PADDR_ತಳ; paddr < PADDR_ತುದಿ; paddr += ಬಿಟ್(se_L4_Large_ಪುಟ_ಬಿಟ್ಟುಗಳು)) {
        armKSGlobalKernelPDs[GET_KPT_ಸೂಚ್ಯಂಕ(vaddr, KLVL_FRM_ARM_PT_LVL(1))][GET_KPT_ಸೂಚ್ಯಂಕ(vaddr,
                                                                                         KLVL_FRM_ARM_PT_LVL(2))] = pte_pte_ಪುಟ_new(
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
                                                                                                                        1, // XN
#ಇಲ್ಲದಿದ್ದರೆ
                                                                                                                        1, // UXN
#ಆದರೆ_ಅಂತ್ಯ
                                                                                                                        paddr,
                                                                                                                        0,                        /* global */
                                                                                                                        1,                        /* access flag */
                                                                                                                        SMP_TERNARY(SMP_SHARE, 0),        /* Inner-shareable if SMP enabled, otherwise unshared */
                                                                                                                        0,                        /* VMKernelOnly */
                                                                                                                        ಸಾಮಾನ್ಯ
                                                                                                                    );
        vaddr += ಬಿಟ್(se_L4_Large_ಪುಟ_ಬಿಟ್ಟುಗಳು);
    }
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_kernel_ಕಿಟಕಿ(ಶೂನ್ಯ)
{
    /* place the PUD into the PGD */
    armKSGlobalKernelPGD[GET_KPT_ಸೂಚ್ಯಂಕ(PPTR_ತಳ, KLVL_FRM_ARM_PT_LVL(0))] =
        pte_pte_ಕೋಷ್ಟಕ_new(addrFromKPPtr(armKSGlobalKernelPUD));

    map_kernel_physical_ಕಿಟಕಿ();

    ಸ್ಥಿರಾಂಕ ಶಬ್ದ_t elf_pud_idx = GET_KPT_ಸೂಚ್ಯಂಕ(KERNEL_ELF_ತಳ, KLVL_FRM_ARM_PT_LVL(1));
    pte_t *kernel_mapping_pds = armKSGlobalKernelPDs[elf_pud_idx];

    /* place the kernel mapping PDs in the kernel mapping PUD */
    armKSGlobalKernelPUD[elf_pud_idx] = pte_pte_ಕೋಷ್ಟಕ_new(addrFromKPPtr(kernel_mapping_pds));

    /* map the kernel ELF */
    map_kernel_elf_image(kernel_mapping_pds);

    /* put the PT into the PD for device window. It would be nice if
       we could pass kernel_mapping_pds[511] to map_kernel_devices(), but it is
       shared between AArch64 and AArch32, so... */
    kernel_mapping_pds[ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1]
        = pte_pte_ಕೋಷ್ಟಕ_new(addrFromKPPtr(armKSGlobalKernelPT));

    map_kernel_devices();
}

/* When the hypervisor support is enabled, the stage-2 translation table format
 * is used for applications.
 * The global bit is always 0.
 * The memory attributes use the S2 translation values.
 */
ಸ್ಥಿರ BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_it_ಚೌಕಟ್ಟು_cap(cap_t vspace_cap, cap_t ಚೌಕಟ್ಟು_cap, bool_t executable)
{
    vspace_ಬೇರು_t *vspace_ಬೇರು_2 = VSPACE_PTR(pptr_ದ_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt;

    vptr_t vptr = cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(ಚೌಕಟ್ಟು_cap);
    ಶೂನ್ಯ *pptr = (ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(ಚೌಕಟ್ಟು_cap);

    ಪ್ರತಿಜ್ಞಾ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(ಚೌಕಟ್ಟು_cap) != 0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AARCH64_VSPACE_S2_ಆರಂಭ_L1
    pud = vspace_ಬೇರು_2;
#ಇಲ್ಲದಿದ್ದರೆ
    vspace_ಬೇರು_2 += GET_UPT_ಸೂಚ್ಯಂಕ(vptr, ULVL_FRM_ARM_PT_LVL(0));
    ಪ್ರತಿಜ್ಞಾ(pte_pte_ಕೋಷ್ಟಕ_ptr_get_present(vspace_ಬೇರು_2));
    pud = paddr_ಗಮ್ಯ_pptr(pte_pte_ಕೋಷ್ಟಕ_ptr_get_pt_ತಳ_ವಿಳಾಸ(vspace_ಬೇರು_2));
#ಆದರೆ_ಅಂತ್ಯ
    pud += GET_UPT_ಸೂಚ್ಯಂಕ(vptr, ULVL_FRM_ARM_PT_LVL(1));
    ಪ್ರತಿಜ್ಞಾ(pte_pte_ಕೋಷ್ಟಕ_ptr_get_present(pud));
    pd = paddr_ಗಮ್ಯ_pptr(pte_pte_ಕೋಷ್ಟಕ_ptr_get_pt_ತಳ_ವಿಳಾಸ(pud));
    pd += GET_UPT_ಸೂಚ್ಯಂಕ(vptr, ULVL_FRM_ARM_PT_LVL(2));
    ಪ್ರತಿಜ್ಞಾ(pte_pte_ಕೋಷ್ಟಕ_ptr_get_present(pd));
    pt = paddr_ಗಮ್ಯ_pptr(pte_pte_ಕೋಷ್ಟಕ_ptr_get_pt_ತಳ_ವಿಳಾಸ(pd));
    *(pt + GET_UPT_ಸೂಚ್ಯಂಕ(vptr, ULVL_FRM_ARM_PT_LVL(3))) = pte_pte_4k_ಪುಟ_new(
                                                              !executable,                    /* unprivileged execute never */
                                                              pptr_ಗಮ್ಯ_paddr(pptr),            /* page_base_address    */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              0,
#ಇಲ್ಲದಿದ್ದರೆ
                                                              1,                              /* not global */
#ಆದರೆ_ಅಂತ್ಯ
                                                              1,                              /* access flag */
                                                              SMP_TERNARY(SMP_SHARE, 0),              /* Inner-shareable if SMP enabled, otherwise unshared */
                                                              APFromVMRights(VMReadWrite),
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              S2_ಸಾಮಾನ್ಯ
#ಇಲ್ಲದಿದ್ದರೆ
                                                              ಸಾಮಾನ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
                                                          );
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ cap_t create_it_ಚೌಕಟ್ಟು_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    vm_ಪುಟ_ಗಾತ್ರ_t ಚೌಕಟ್ಟು_ಗಾತ್ರ_2;
    ಆದರೆ (use_large) {
        ಚೌಕಟ್ಟು_ಗಾತ್ರ_2 = ARMLarge_ಪುಟ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಚೌಕಟ್ಟು_ಗಾತ್ರ_2 = ARMSmall_ಪುಟ;
    }
    ಹಿಂತಿರುಗಿ
        cap_ಚೌಕಟ್ಟು_cap_new(
            asid,                          /* capFMappedASID */
            pptr,                          /* capFBasePtr */
            ಚೌಕಟ್ಟು_ಗಾತ್ರ_2,                    /* capFSize */
            vptr,                          /* capFMappedAddress */
            ಶಬ್ದ_From_VMRights(VMReadWrite), /* capFVMRights */
            false                          /* capFIsDevice */
        );
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    vspace_ಬೇರು_t *vspace_ಬೇರು_2 = VSPACE_PTR(pptr_ದ_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt = PT_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(pt_cap);

    ಪ್ರತಿಜ್ಞಾ(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(pt_cap));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AARCH64_VSPACE_S2_ಆರಂಭ_L1
    pud = vspace_ಬೇರು_2;
#ಇಲ್ಲದಿದ್ದರೆ
    vspace_ಬೇರು_2 += GET_UPT_ಸೂಚ್ಯಂಕ(vptr, ULVL_FRM_ARM_PT_LVL(0));
    ಪ್ರತಿಜ್ಞಾ(pte_pte_ಕೋಷ್ಟಕ_ptr_get_present(vspace_ಬೇರು_2));
    pud = paddr_ಗಮ್ಯ_pptr(pte_pte_ಕೋಷ್ಟಕ_ptr_get_pt_ತಳ_ವಿಳಾಸ(vspace_ಬೇರು_2));
#ಆದರೆ_ಅಂತ್ಯ
    pud += GET_UPT_ಸೂಚ್ಯಂಕ(vptr, ULVL_FRM_ARM_PT_LVL(1));
    ಪ್ರತಿಜ್ಞಾ(pte_pte_ಕೋಷ್ಟಕ_ptr_get_present(pud));
    pd = paddr_ಗಮ್ಯ_pptr(pte_pte_ಕೋಷ್ಟಕ_ptr_get_pt_ತಳ_ವಿಳಾಸ(pud));
    *(pd + GET_UPT_ಸೂಚ್ಯಂಕ(vptr, ULVL_FRM_ARM_PT_LVL(2))) = pte_pte_ಕೋಷ್ಟಕ_new(
                                                              pptr_ಗಮ್ಯ_paddr(pt)
                                                          );
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pt_cap(vspace_cap, cap);
    ಹಿಂತಿರುಗಿ cap;
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    vspace_ಬೇರು_t *vspace_ಬೇರು_2 = VSPACE_PTR(pptr_ದ_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd = PT_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(pd_cap));
    vptr_t vptr = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(pd_cap);

    ಪ್ರತಿಜ್ಞಾ(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(pd_cap));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AARCH64_VSPACE_S2_ಆರಂಭ_L1
    pud = vspace_ಬೇರು_2;
#ಇಲ್ಲದಿದ್ದರೆ
    vspace_ಬೇರು_2 += GET_UPT_ಸೂಚ್ಯಂಕ(vptr, ULVL_FRM_ARM_PT_LVL(0));
    ಪ್ರತಿಜ್ಞಾ(pte_pte_ಕೋಷ್ಟಕ_ptr_get_present(vspace_ಬೇರು_2));
    pud = paddr_ಗಮ್ಯ_pptr(pte_pte_ಕೋಷ್ಟಕ_ptr_get_pt_ತಳ_ವಿಳಾಸ(vspace_ಬೇರು_2));
#ಆದರೆ_ಅಂತ್ಯ
    *(pud + GET_UPT_ಸೂಚ್ಯಂಕ(vptr, ULVL_FRM_ARM_PT_LVL(1))) = pte_pte_ಕೋಷ್ಟಕ_new(
                                                               pptr_ಗಮ್ಯ_paddr(pd)
                                                           );
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pd_cap(vspace_cap, cap);
    ಹಿಂತಿರುಗಿ cap;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ AARCH64_VSPACE_S2_ಆರಂಭ_L1
ಸ್ಥಿರ BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_it_pud_cap(cap_t vspace_cap, cap_t pud_cap)
{
    pte_t *pgd = PT_PTR(pptr_ದ_cap(vspace_cap));
    pte_t *pud = PT_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(pud_cap));
    vptr_t vptr = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(pud_cap);

    ಪ್ರತಿಜ್ಞಾ(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(pud_cap));

    *(pgd + GET_UPT_ಸೂಚ್ಯಂಕ(vptr, ULVL_FRM_ARM_PT_LVL(0))) = pte_pte_ಕೋಷ್ಟಕ_new(
                                                               pptr_ಗಮ್ಯ_paddr(pud));
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ cap_t create_it_pud_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_new(
              asid,               /* capPTMappedASID */
              pptr,               /* capPTBasePtr */
              1,                  /* capPTIsMapped */
              vptr                /* capPTMappedAddress */
          );
    map_it_pud_cap(vspace_cap, cap);
    ಹಿಂತಿರುಗಿ cap;
}
#ಆದರೆ_ಅಂತ್ಯ /* AARCH64_VSPACE_S2_START_L1 */
BOOT_ಸಂಕೇತ ಶಬ್ದ_t arch_get_n_paging(v_region_t it_v_reg)
{
    ಹಿಂತಿರುಗಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ AARCH64_VSPACE_S2_ಆರಂಭ_L1
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_ಬಿಟ್ಟುಗಳು(ULVL_FRM_ARM_PT_LVL(0))) +
#ಆದರೆ_ಅಂತ್ಯ
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_ಬಿಟ್ಟುಗಳು(ULVL_FRM_ARM_PT_LVL(1))) +
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_ಬಿಟ್ಟುಗಳು(ULVL_FRM_ARM_PT_LVL(2)));
}

BOOT_ಸಂಕೇತ cap_t create_it_ವಿಳಾಸ_space(cap_t ಬೇರು_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_ಸ್ಥಾನ_Pos ಸ್ಥಾನ_pos_before;
    se_L4_ಸ್ಥಾನ_Pos ಸ್ಥಾನ_pos_after;

    /* create the PGD */
    vspace_cap = cap_vspace_cap_new(
                     IT_ASID,           /* capVSMappedASID */
                     rootserver.vspace, /* capVSBasePtr    */
                     1                  /* capVSIsMapped   */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
                     , CB_INVALID       /* capVSMappedCB   */
#ಆದರೆ_ಅಂತ್ಯ
                 );
    ಸ್ಥಾನ_pos_before = ndks_boot.ಸ್ಥಾನ_pos_cur;
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ AARCH64_VSPACE_S2_ಆರಂಭ_L1
    /* Create any PUDs needed for the user land image */
    ಗಾಗಿ (vptr = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(it_v_reg.ಆರಂಭ, GET_ULVL_PGSIZE_ಬಿಟ್ಟುಗಳು(ULVL_FRM_ARM_PT_LVL(0)));
         vptr < it_v_reg.ಕೊನೆ;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(0))) {
        ಆದರೆ (!provide_cap(ಬೇರು_cnode_cap, create_it_pud_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            ಹಿಂತಿರುಗಿ cap_null_cap_new();
        }
    }
#ಆದರೆ_ಅಂತ್ಯ
    /* Create any PDs needed for the user land image */
    ಗಾಗಿ (vptr = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(it_v_reg.ಆರಂಭ, GET_ULVL_PGSIZE_ಬಿಟ್ಟುಗಳು(ULVL_FRM_ARM_PT_LVL(1)));
         vptr < it_v_reg.ಕೊನೆ;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))) {
        ಆದರೆ (!provide_cap(ಬೇರು_cnode_cap, create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            ಹಿಂತಿರುಗಿ cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    ಗಾಗಿ (vptr = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(it_v_reg.ಆರಂಭ, GET_ULVL_PGSIZE_ಬಿಟ್ಟುಗಳು(ULVL_FRM_ARM_PT_LVL(2)));
         vptr < it_v_reg.ಕೊನೆ;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))) {
        ಆದರೆ (!provide_cap(ಬೇರು_cnode_cap, create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            ಹಿಂತಿರುಗಿ cap_null_cap_new();
        }
    }

    ಸ್ಥಾನ_pos_after = ndks_boot.ಸ್ಥಾನ_pos_cur;
    ndks_boot.bi_ಚೌಕಟ್ಟು->userImagePaging = (se_L4_ಸ್ಥಾನ_Region) {
        ಸ್ಥಾನ_pos_before, ಸ್ಥಾನ_pos_after
    };
    ಹಿಂತಿರುಗಿ vspace_cap;
}

BOOT_ಸಂಕೇತ cap_t create_unmapped_it_ಚೌಕಟ್ಟು_cap(pptr_t pptr, bool_t use_large)
{
    ಹಿಂತಿರುಗಿ create_it_ಚೌಕಟ್ಟು_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_ಸಂಕೇತ cap_t create_mapped_it_ಚೌಕಟ್ಟು_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_ಚೌಕಟ್ಟು_cap(pptr, vptr, asid, use_large);
    map_it_ಚೌಕಟ್ಟು_cap(pd_cap, cap, executable);
    ಹಿಂತಿರುಗಿ cap;
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ activate_kernel_vspace(ಶೂನ್ಯ)
{
    cleanInvalidateL1Caches();
    ಗಣ_Current_Kernel_VSpace_ಬೇರು(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalKernelPGD)));

    /* Prevent elf-loader address translation to fill up TLB */
    ಗಣ_Current_User_VSpace_ಬೇರು(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));

    invalidateLocalTLB();
    ಬೀಗ_TLBEntry(KERNEL_ELF_ತಳ);
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_ದ_cap(it_ap_cap));
    asid_map_t asid_map = asid_map_asid_map_vspace_new(
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
                              /* bind_cb: Number of bound context banks */
                              0,
#ಆದರೆ_ಅಂತ್ಯ
                              /* vspace_root: reference to vspace root page table object */
                              (ಶಬ್ದ_t)cap_vspace_cap_get_capVSBasePtr(it_vspace_cap)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
                              /* stored_hw_vmid, stored_vmid_valid: Assigned hardware VMID for TLB. */
                              , 0, false
#ಆದರೆ_ಅಂತ್ಯ
                          );
    ap->array[ASID_ಕೆಳಗಿನ(IT_ASID)] = asid_map;
    armKSASIDTable[ASID_ಮೇಲಿನ(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

asid_map_t ಕಂಡುಹಿಡಿ_Map_For_ASID(asid_t asid)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಆದರೆ (!poolPtr) {
        ಹಿಂತಿರುಗಿ asid_map_asid_map_none_new();
    }

    ಹಿಂತಿರುಗಿ poolPtr->array[ASID_ಕೆಳಗಿನ(asid)];
}

ಸ್ಥಿರ ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid_t asid)
{
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = ಕಂಡುಹಿಡಿ_Map_For_ASID(asid);
    ಆದರೆ (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();

        ret.vspace_ಬೇರು = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }

    ret.vspace_ಬೇರು = (vspace_ಬೇರು_t *)asid_map_asid_map_vspace_get_vspace_ಬೇರು(asid_map);
    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ಹಿಂತಿರುಗಿ ret;
}

ಶಬ್ದ_t *ಶುದ್ಧ lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    ಶಬ್ದ_t w_ಬಫರ್_Ptr;
    cap_t ಬಫರ್_Cap;
    vm_rights_t vm_rights;

    w_ಬಫರ್_Ptr = thread->tcbIPCBuffer;
    ಬಫರ್_Cap = TCB_PTR_CTE_PTR(thread, tcb_ಬಫರ್)->cap;

    ಆದರೆ (unlikely(cap_get_capType(ಬಫರ್_Cap) != cap_ಚೌಕಟ್ಟು_cap)) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (unlikely(cap_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Device(ಬಫರ್_Cap))) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    vm_rights = cap_ಚೌಕಟ್ಟು_cap_get_cap_FVMRights(ಬಫರ್_Cap);
    ಆದರೆ (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        ಶಬ್ದ_t ತಳ_Ptr;
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಪುಟ_ಬಿಟ್ಟುಗಳು_2;

        ತಳ_Ptr = cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(ಬಫರ್_Cap);
        ಪುಟ_ಬಿಟ್ಟುಗಳು_2 = ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(ಬಫರ್_Cap));
        ಹಿಂತಿರುಗಿ (ಶಬ್ದ_t *)(ತಳ_Ptr + (w_ಬಫರ್_Ptr & ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_2)));
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    ಆದರೆ (cap_get_capType(cap) != cap_ಚೌಕಟ್ಟು_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (unlikely(cap_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (!IS_ALIGNED(vptr, se_L4_IPCBuffer_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (ಪೂರ್ಣಾಂಕ)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ lookupPTSlot_ret_t lookupPTSlot(vspace_ಬೇರು_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    ಶಬ್ದ_t level = UPT_ಮಟ್ಟಗಳು - 1;
    pte_t *pt = vspace;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ = PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು * level + se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು;
    ret.pt_ಸ್ಥಾನ = pt + ((vptr >> ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ) & ಮುಖವಾಡ(se_L4_VSpace_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು));

    ಆಗ (pte_pte_ಕೋಷ್ಟಕ_ptr_get_present(ret.pt_ಸ್ಥಾನ) && likely(level > 0)) {
        level--;
        ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ -= PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
        pt = paddr_ಗಮ್ಯ_pptr(pte_pte_ಕೋಷ್ಟಕ_ptr_get_pt_ತಳ_ವಿಳಾಸ(ret.pt_ಸ್ಥಾನ));
        ret.pt_ಸ್ಥಾನ = pt + ((vptr >> ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ) & ಮುಖವಾಡ(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು));
    }

    ಹಿಂತಿರುಗಿ ret;
}

/* Note that if the hypervisor support is enabled, the user page tables use
 * stage-2 translation format. Otherwise, they follow the stage-1 translation format.
 */
ಸ್ಥಿರ pte_t make_User_ಪುಟ_PTE(paddr_t paddr, vm_rights_t vm_rights, bool_t cap_read, vm_ಗುಣಲಕ್ಷಣಗಳು_t ಗುಣಲಕ್ಷಣಗಳು,
                             vm_ಪುಟ_ಗಾತ್ರ_t ಪುಟ_ಗಾತ್ರ)
{
    bool_t nonexecutable = vm_ಗುಣಲಕ್ಷಣಗಳು_get_arm_Execute_Never(ಗುಣಲಕ್ಷಣಗಳು) || !cap_read;
    ಶಬ್ದ_t cacheable = vm_ಗುಣಲಕ್ಷಣಗಳು_get_arm_ಪುಟ_Cacheable(ಗುಣಲಕ್ಷಣಗಳು);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಶಬ್ದ_t nG = 0; /* not global */
    ಶಬ್ದ_t attridx = cacheable ? S2_ಸಾಮಾನ್ಯ : S2_DEVICE_nGnRnE;
#ಇಲ್ಲದಿದ್ದರೆ
    ಶಬ್ದ_t nG = 1; /* not global */
    ಶಬ್ದ_t attridx = cacheable ? ಸಾಮಾನ್ಯ : DEVICE_nGnRnE;
#ಆದರೆ_ಅಂತ್ಯ

    /* Inner-shareable if SMP enabled, otherwise unshared (ignored for devices) */
    ಶಬ್ದ_t shareable = cacheable ? SMP_TERNARY(SMP_SHARE, 0) : 0;

    ಆದರೆ (ಪುಟ_ಗಾತ್ರ == ARMSmall_ಪುಟ) {
        ಹಿಂತಿರುಗಿ pte_pte_4k_ಪುಟ_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                   shareable, APFromVMRights(vm_rights), attridx);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ pte_pte_ಪುಟ_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                shareable, APFromVMRights(vm_rights), attridx);
    }
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    ಆಯ್ಕೆ (vm_faultType) {
    ಸ್ಥಿತಿ ARMDataAbort: {
        ಶಬ್ದ_t addr, fault;

        addr = getFAR();
        fault = getDFSR();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* use the IPA */
        ಆದರೆ (ARCH_ಗಂಟು_STATE(armHSVCPUActive)) {
            addr = GET_PAR_ADDR(ವಿಳಾಸ_Translate_S1(addr)) | (addr & ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು));
        }
#ಆದರೆ_ಅಂತ್ಯ
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        ಹಿಂತಿರುಗಿ EXCEPTION_FAULT;
    }

    ಸ್ಥಿತಿ ARMPrefetchAbort: {
        ಶಬ್ದ_t pc, fault;

        pc = getRestartPC(thread);
        fault = getIFSR();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        ಆದರೆ (ARCH_ಗಂಟು_STATE(armHSVCPUActive)) {
            pc = GET_PAR_ADDR(ವಿಳಾಸ_Translate_S1(pc)) | (pc & ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು));
        }
#ಆದರೆ_ಅಂತ್ಯ
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        ಹಿಂತಿರುಗಿ EXCEPTION_FAULT;
    }

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid VM fault type");
    }
}

bool_t CONST is_VTable_ಬೇರು(cap_t cap)
{
    ಹಿಂತಿರುಗಿ cap_get_capType(cap) == cap_vspace_cap;
}

bool_t CONST is_Valid_Native_ಬೇರು(cap_t cap)
{
    ಹಿಂತಿರುಗಿ is_VTable_ಬೇರು(cap) &&
           cap_vspace_cap_get_capVSIsMapped(cap);
}

bool_t CONST is_Valid_VTable_ಬೇರು(cap_t cap)
{
    ಹಿಂತಿರುಗಿ is_Valid_Native_ಬೇರು(cap);
}

ಶೂನ್ಯ ಗಣ_VMRoot(tcb_t *tcb)
{
    cap_t thread_ಬೇರು;
    asid_t asid;
    vspace_ಬೇರು_t *vspace_ಬೇರು_2;
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    /* ASID/VMID 0 is never allocated to a real VSpace (see findFreeHWASID()
     * for the hypervisor case), so the empty global VSpace can be installed
     * under it without flushing the TLB. */
    ಆದರೆ (!is_Valid_Native_ಬೇರು(thread_ಬೇರು)) {
        ಗಣ_Current_User_VSpace_ಬೇರು(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        ಹಿಂತಿರುಗಿ;
    }

    vspace_ಬೇರು_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_ಬೇರು));
    asid = cap_vspace_cap_get_capVSMappedASID(thread_ಬೇರು);
    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
    ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE || ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು != vspace_ಬೇರು_2)) {
        ಗಣ_Current_User_VSpace_ಬೇರು(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        ಹಿಂತಿರುಗಿ;
    }

    armv_contextSwitch(vspace_ಬೇರು_2, asid);
}

ಸ್ಥಿರ bool_t ಗಣ_VMRoot_For_Flush(vspace_ಬೇರು_t *vspace, asid_t asid)
{
    cap_t thread_ಬೇರು;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcbVTable)->cap;

    ಆದರೆ (cap_get_capType(thread_ಬೇರು) == cap_vspace_cap &&
        cap_vspace_cap_get_capVSIsMapped(thread_ಬೇರು) &&
        VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_ಬೇರು)) == vspace) {
        ಹಿಂತಿರುಗಿ false;
    }

    armv_contextSwitch(vspace, asid);
    ಹಿಂತಿರುಗಿ true;
}


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT

ಸ್ಥಿರ ಸಾಲೊಳಗೆ asid_pool_t *getPoolPtr(asid_t asid)
{
    ಹಿಂತಿರುಗಿ armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ asid_map_t getASIDMap(asid_pool_t *poolPtr, asid_t asid)
{
    ಪ್ರತಿಜ್ಞಾ(poolPtr != ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ poolPtr->array[ASID_ಕೆಳಗಿನ(asid)];
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಗಣ_ASIDMap(asid_pool_t *poolPtr, asid_t asid, asid_map_t asid_map)
{
    ಪ್ರತಿಜ್ಞಾ(poolPtr != ಖಾಲಿ);
    poolPtr->array[ASID_ಕೆಳಗಿನ(asid)] = asid_map;
}

ಸ್ಥಿರ ಶೂನ್ಯ invalidateASID(asid_t asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    ಪ್ರತಿಜ್ಞಾ(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_ಗಣ_stored_hw_vmid(asid_map, 0);
    asid_map = asid_map_asid_map_vspace_ಗಣ_stored_vmid_valid(asid_map, false);

    ಗಣ_ASIDMap(poolPtr, asid, asid_map);
}

ಸ್ಥಿರ ಶೂನ್ಯ storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    ಪ್ರತಿಜ್ಞಾ(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_ಗಣ_stored_hw_vmid(asid_map, hw_asid);
    asid_map = asid_map_asid_map_vspace_ಗಣ_stored_vmid_valid(asid_map, true);

    ಗಣ_ASIDMap(poolPtr, asid, asid_map);
    armKSHWASIDTable[hw_asid] = asid;
}

ಸ್ಥಿರ hw_asid_t ಕಂಡುಹಿಡಿ_Free_HWASID(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t hw_asid_ಸ್ಥಾನಾಂತರ;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    ಗಾಗಿ (hw_asid_ಸ್ಥಾನಾಂತರ = 0;
         hw_asid_ಸ್ಥಾನಾಂತರ <= (ಶಬ್ದ_t)((hw_asid_t) - 1);
         hw_asid_ಸ್ಥಾನಾಂತರ++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_ಸ್ಥಾನಾಂತರ);
        ಆದರೆ (hw_asid != hwASIDReserved && armKSHWASIDTable[hw_asid] == asidInvalid) {
            ಹಿಂತಿರುಗಿ hw_asid;
        }
    }
    /* If we've scanned the table without finding a free ASID */
    hw_asid = armKSNextASID;
    invalidateASID(armKSHWASIDTable[hw_asid]);

    /* Flush TLB */
    invalidateTranslationASID(hw_asid);
    armKSHWASIDTable[hw_asid] = asidInvalid;

    /* Increment the NextASID index, skipping the reserved VMID 0 on wrap */
    armKSNextASID++;
    ಆದರೆ (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    ಹಿಂತಿರುಗಿ hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = ಕಂಡುಹಿಡಿ_Map_For_ASID(asid);
    ಆದರೆ (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        ಹಿಂತಿರುಗಿ asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    } ಇಲ್ಲದಿದ್ದರೆ {
        hw_asid_t new_hw_asid;

        new_hw_asid = ಕಂಡುಹಿಡಿ_Free_HWASID();
        storeHWASID(asid, new_hw_asid);
        ಹಿಂತಿರುಗಿ new_hw_asid;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ invalidateASIDEntry(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = ಕಂಡುಹಿಡಿ_Map_For_ASID(asid);
    ಆದರೆ (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        armKSHWASIDTable[asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
ಸ್ಥಿರ ಶಬ್ದ_t getASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asidPool);

    asid_map_t asid_map = asidPool->array[ASID_ಕೆಳಗಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    ಹಿಂತಿರುಗಿ asid_map_asid_map_vspace_get_bind_cb(asid_map);
}

ಶೂನ್ಯ increaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_ಕೆಳಗಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_ಗಣ_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) + 1);
}

ಶೂನ್ಯ decreaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_ಕೆಳಗಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_ಗಣ_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) - 1);
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ invalidateTLBByASID(asid_t asid)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
    ಶಬ್ದ_t bind_cb = getASIDBindCB(asid);
    ಆದರೆ (unlikely(bind_cb)) {
        invalidateSMMUTLBByASID(asid, bind_cb);
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = ಕಂಡುಹಿಡಿ_Map_For_ASID(asid);
    ಆದರೆ (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        ಹಿಂತಿರುಗಿ;
    }
    invalidateTranslationASID(asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map));
#ಇಲ್ಲದಿದ್ದರೆ
    invalidateTranslationASID(asid);
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ invalidateTLBByASIDVA(asid_t asid, vptr_t vaddr)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
    ಶಬ್ದ_t bind_cb = getASIDBindCB(asid);
    ಆದರೆ (unlikely(bind_cb)) {
        invalidateSMMUTLBByASIDVA(asid, vaddr, bind_cb);
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = ಕಂಡುಹಿಡಿ_Map_For_ASID(asid);
    ಆದರೆ (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        ಹಿಂತಿರುಗಿ;
    }
    uint64_t hw_asid = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    invalidateTranslationSingle((hw_asid << 48) | vaddr >> se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು);
#ಇಲ್ಲದಿದ್ದರೆ
    invalidateTranslationSingle((asid << 48) | vaddr >> se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು);
#ಆದರೆ_ಅಂತ್ಯ
}


ಶೂನ್ಯ unmap_ಪುಟ_ಕೋಷ್ಟಕ(asid_t asid, vptr_t vptr, pte_t *ಗುರಿ_pt)
{
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
    ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
        /* nothing to do */
        ಹಿಂತಿರುಗಿ;
    }
    pte_t *pt_ಸ್ಥಾನ = ಖಾಲಿ;
    pte_t *pt = (pte_t *)ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು;

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < UPT_ಮಟ್ಟಗಳು - 1 && pt != ಗುರಿ_pt; i++) {
        pt_ಸ್ಥಾನ = pt + GET_UPT_ಸೂಚ್ಯಂಕ(vptr, i);
        ಆದರೆ (unlikely(!pte_pte_ಕೋಷ್ಟಕ_ptr_get_present(pt_ಸ್ಥಾನ))) {
            /* couldn't find it */
            ಹಿಂತಿರುಗಿ;
        }
        pt = paddr_ಗಮ್ಯ_pptr(pte_pte_ಕೋಷ್ಟಕ_ptr_get_pt_ತಳ_ವಿಳಾಸ(pt_ಸ್ಥಾನ));
    }

    ಆದರೆ (pt != ಗುರಿ_pt) {
        /* didn't find it */
        ಹಿಂತಿರುಗಿ;
    }
    /* If we found a pt then ptSlot won't be null */
    ಪ್ರತಿಜ್ಞಾ(pt_ಸ್ಥಾನ != ಖಾಲಿ);
    *pt_ಸ್ಥಾನ = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)pt_ಸ್ಥಾನ, pptr_ಗಮ್ಯ_paddr(pt_ಸ್ಥಾನ));
    invalidateTLBByASID(asid);
}

ಶೂನ್ಯ unmap_ಪುಟ(vm_ಪುಟ_ಗಾತ್ರ_t ಪುಟ_ಗಾತ್ರ, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;
    lookupPTSlot_ret_t  lu_ret;
    pte_t pte;

    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ;
    }

    lu_ret = lookupPTSlot(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vptr);
    ಆದರೆ (unlikely(lu_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ != ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ಪುಟ_ಗಾತ್ರ))) {
        /* Do nothing if the wrong size object was returned */
        ಹಿಂತಿರುಗಿ;
    }

    pte = *(lu_ret.pt_ಸ್ಥಾನ);
    ಆದರೆ (!pte_is_ಪುಟ_type(pte)) {
        /* Do nothing if no page is present */
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (pte_get_ಪುಟ_ತಳ_ವಿಳಾಸ(pte) != pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)pptr)) {
        /* Do nothing if the mapped page is not the same physical frame */
        ಹಿಂತಿರುಗಿ;
    }

    *(lu_ret.pt_ಸ್ಥಾನ) = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)lu_ret.pt_ಸ್ಥಾನ, pptr_ಗಮ್ಯ_paddr(lu_ret.pt_ಸ್ಥಾನ));
    ಪ್ರತಿಜ್ಞಾ(asid < ಬಿಟ್(16));
    invalidateTLBByASIDVA(asid, vptr);
}

ಶೂನ್ಯ deleteASID(asid_t asid, vspace_ಬೇರು_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];

    ಆದರೆ (poolPtr != ಖಾಲಿ) {
        asid_map_t asid_map = poolPtr->array[ASID_ಕೆಳಗಿನ(asid)];
        ಆದರೆ (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_ಬೇರು_t *)asid_map_asid_map_vspace_get_vspace_ಬೇರು(asid_map) == vspace) {
            invalidateTLBByASID(asid);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
            invalidateASIDEntry(asid);
#ಆದರೆ_ಅಂತ್ಯ
            poolPtr->array[ASID_ಕೆಳಗಿನ(asid)] = asid_map_asid_map_none_new();
            ಗಣ_VMRoot(ಗಂಟು_STATE(ksCurThread));
        }
    }
}

ಶೂನ್ಯ deleteASIDPool(asid_t asid_ತಳ, asid_pool_t *pool)
{
    ಶಬ್ದ_t ಸ್ಥಾನಾಂತರ;

    ಪ್ರತಿಜ್ಞಾ((asid_ತಳ & ಮುಖವಾಡ(asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು)) == 0);

    ಆದರೆ (armKSASIDTable[ASID_ಮೇಲಿನ(asid_ತಳ)] == pool) {
        ಗಾಗಿ (ಸ್ಥಾನಾಂತರ = 0; ಸ್ಥಾನಾಂತರ < ಬಿಟ್(asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು); ಸ್ಥಾನಾಂತರ++) {
            asid_map_t asid_map = pool->array[ಸ್ಥಾನಾಂತರ];
            ಆದರೆ (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                invalidateTLBByASID(asid_ತಳ + ಸ್ಥಾನಾಂತರ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
                invalidateASIDEntry(asid_ತಳ + ಸ್ಥಾನಾಂತರ);
#ಆದರೆ_ಅಂತ್ಯ
            }
        }
        armKSASIDTable[ASID_ಮೇಲಿನ(asid_ತಳ)] = ಖಾಲಿ;
        ಗಣ_VMRoot(ಗಂಟು_STATE(ksCurThread));
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ doFlush(ಶಬ್ದ_t invLabel, vptr_t ಆರಂಭ, vptr_t ಕೊನೆ, paddr_t pstart)
{
    ಆಯ್ಕೆ (invLabel) {
    ಸ್ಥಿತಿ ARMVSpace_Clean_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMPage_Clean_ದತ್ತಾಂಶ:
        cleanCacheRange_RAM(ಆರಂಭ, ಕೊನೆ, pstart);
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ ARMVSpace_Invalidate_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMPage_Invalidate_ದತ್ತಾಂಶ:
        invalidateCacheRange_RAM(ಆರಂಭ, ಕೊನೆ, pstart);
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ ARMVSpace_Clean_Invalidate_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMPage_Clean_Invalidate_ದತ್ತಾಂಶ:
        cleanInvalidateCacheRange_RAM(ಆರಂಭ, ಕೊನೆ, pstart);
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ ARMVSpaceUnify_Instruction:
    ಸ್ಥಿತಿ ARMPageUnify_Instruction:
        /* First clean data lines to point of unification... */
        cleanCacheRange_PoU(ಆರಂಭ, ಕೊನೆ, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(ಆರಂಭ, ಕೊನೆ, pstart);
        /* ... then wait for the completion of invalidations... */
        dsb();
        /* ... and ensure new instructions come from fresh cache lines. */
        isb();
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

ಸ್ಥಿರ exception_t performVSpaceFlush(ಶಬ್ದ_t invLabel, vspace_ಬೇರು_t *vspace_ಬೇರು_2, asid_t asid,
                                      vptr_t ಆರಂಭ, vptr_t ಕೊನೆ, paddr_t pstart)
{

    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        ಶಬ್ದ_t ಗಾತ್ರ_2 = ಕೊನೆ - ಆರಂಭ;
        ಆರಂಭ = (vptr_t)paddr_ಗಮ್ಯ_pptr(pstart);
        ಕೊನೆ = ಆರಂಭ + ಗಾತ್ರ_2;
        ಆದರೆ (ಆರಂಭ < ಕೊನೆ) {
            doFlush(invLabel, ಆರಂಭ, ಕೊನೆ, pstart);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        bool_t ಬೇರು_switched;

        /* Flush if given a non zero range */
        ಆದರೆ (ಆರಂಭ < ಕೊನೆ) {
            ಬೇರು_switched = ಗಣ_VMRoot_For_Flush(vspace_ಬೇರು_2, asid);
            doFlush(invLabel, ಆರಂಭ, ಕೊನೆ, pstart);
            ಆದರೆ (ಬೇರು_switched) {
                ಗಣ_VMRoot(ಗಂಟು_STATE(ksCurThread));
            }
        }
    }
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}


ಸ್ಥಿರ exception_t perform_ಪುಟ_ಕೋಷ್ಟಕ_Invocation_Map(cap_t cap, cte_t *ct_ಸ್ಥಾನ, pte_t pte, pte_t *pt_ಸ್ಥಾನ)
{
    ct_ಸ್ಥಾನ->cap = cap;
    *pt_ಸ್ಥಾನ = pte;
    cleanByVA_PoU((vptr_t)pt_ಸ್ಥಾನ, pptr_ಗಮ್ಯ_paddr(pt_ಸ್ಥಾನ));

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_ಪುಟ_ಕೋಷ್ಟಕ_Invocation_Unmap(cap_t cap, cte_t *ct_ಸ್ಥಾನ)
{
    ಆದರೆ (cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PT_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap));
        unmap_ಪುಟ_ಕೋಷ್ಟಕ(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(cap),
                       cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(cap), pt);
        clear_ಸ್ಮರಣೆ_PT((ಶೂನ್ಯ *)pt, cap_get_cap_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(cap));
    }

    cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ptr_ಗಣ_cap_PTIs_Mapped(&(ct_ಸ್ಥಾನ->cap), 0);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_ಪುಟ_Invocation_Map(asid_t asid, cap_t cap, cte_t *ct_ಸ್ಥಾನ,
                                            pte_t pte, pte_t *pt_ಸ್ಥಾನ)
{
    bool_t tlbflush_required = pte_ptr_get_valid(pt_ಸ್ಥಾನ);

    ct_ಸ್ಥಾನ->cap = cap;
    *pt_ಸ್ಥಾನ = pte;

    cleanByVA_PoU((vptr_t)pt_ಸ್ಥಾನ, pptr_ಗಮ್ಯ_paddr(pt_ಸ್ಥಾನ));
    ಆದರೆ (unlikely(tlbflush_required)) {
        ಪ್ರತಿಜ್ಞಾ(asid < ಬಿಟ್(16));
        invalidateTLBByASIDVA(asid, cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap));
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_ಪುಟ_Invocation_Unmap(cap_t cap, cte_t *ct_ಸ್ಥಾನ)
{
    ಆದರೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap) != 0) {

        unmap_ಪುಟ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap),
                  cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap),
                  cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap),
                  cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap));
    }

    cap_t ಸ್ಥಾನ_Cap = ct_ಸ್ಥಾನ->cap;
    ಸ್ಥಾನ_Cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ವಿಳಾಸ(ಸ್ಥಾನ_Cap, 0);
    ಸ್ಥಾನ_Cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ASID(ಸ್ಥಾನ_Cap, asidInvalid);
    ct_ಸ್ಥಾನ->cap = ಸ್ಥಾನ_Cap;


    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_ಪುಟ_Flush(ಶಬ್ದ_t invLabel, vspace_ಬೇರು_t *vspace_ಬೇರು_2, asid_t asid,
                                    vptr_t ಆರಂಭ, vptr_t ಕೊನೆ, paddr_t pstart)
{
    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* We flush the cache with kernel virtual addresses since
         * the user virtual addresses are not valid in EL2.
         * Switching VMRoot is not required.
         */
        ಶಬ್ದ_t ಗಾತ್ರ_2 = ಕೊನೆ - ಆರಂಭ;
        ಆರಂಭ = (vptr_t)paddr_ಗಮ್ಯ_pptr(pstart);
        ಕೊನೆ = ಆರಂಭ + ಗಾತ್ರ_2;

        ಆದರೆ (ಆರಂಭ < ಕೊನೆ) {
            doFlush(invLabel, ಆರಂಭ, ಕೊನೆ, pstart);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        bool_t ಬೇರು_switched;

        ಆದರೆ (ಆರಂಭ < ಕೊನೆ) {
            ಬೇರು_switched = ಗಣ_VMRoot_For_Flush(vspace_ಬೇರು_2, asid);
            doFlush(invLabel, ಆರಂಭ, ಕೊನೆ, pstart);
            ಆದರೆ (ಬೇರು_switched) {
                ಗಣ_VMRoot(ಗಂಟು_STATE(ksCurThread));
            }
        }
    }
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_ಪುಟ_Get_ವಿಳಾಸ(pptr_t ತಳ_ptr, bool_t call)
{
    paddr_t ತಳ = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)ತಳ_ptr);

    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);
    ಆದರೆ (call) {
        ಶಬ್ದ_t *ipc_ಬಫರ್ = lookupIPCBuffer(true, thread);
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, badge_ರಿಜಿಸ್ಟರ್, 0);
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ = ಗಣ_MR(thread, ipc_ಬಫರ್, 0, ತಳ);
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(
                        se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, ಉದ್ದಳತೆ)));
    }
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performASIDControlInvocation(ಶೂನ್ಯ *ಚೌಕಟ್ಟು, cte_t *ಸ್ಥಾನ,
                                                cte_t *ಪೋಷಕ, asid_t asid_ತಳ)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_ಗಣ_cap_Free_ಸೂಚ್ಯಂಕ(&(ಪೋಷಕ->cap),
                                         MAX_FREE_ಸೂಚ್ಯಂಕ(cap_untyped_cap_get_cap_ಖಂಡ_ಗಾತ್ರ(ಪೋಷಕ->cap)));

    memzero(ಚೌಕಟ್ಟು, ಬಿಟ್(se_L4_ASIDPool_ಬಿಟ್ಟುಗಳು));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ಸೇರಿಸು(
        cap_asid_pool_cap_new(
            asid_ತಳ,         /* capASIDBase  */
            ಶಬ್ದ_REF(ಚೌಕಟ್ಟು)    /* capASIDPool  */
        ), ಪೋಷಕ, ಸ್ಥಾನ);

    ಪ್ರತಿಜ್ಞಾ((asid_ತಳ & ಮುಖವಾಡ(asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು)) == 0);
    armKSASIDTable[ASID_ಮೇಲಿನ(asid_ತಳ)] = (asid_pool_t *)ಚೌಕಟ್ಟು;

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decode_ARMVSpace_ಬೇರು_Invocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ,
                                                 cte_t *cte, cap_t cap, ಶಬ್ದ_t *ಬಫರ್)
{
    vptr_t ಆರಂಭ, ಕೊನೆ;
    paddr_t pstart;
    asid_t asid;
    vspace_ಬೇರು_t *vspace_ಬೇರು_2;
    lookupPTSlot_ret_t resolve_ret;
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;
    pte_t pte;

    ಆಯ್ಕೆ (invLabel) {
    ಸ್ಥಿತಿ ARMVSpace_Clean_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMVSpace_Invalidate_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMVSpace_Clean_Invalidate_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMVSpaceUnify_Instruction:

        ಆದರೆ (ಉದ್ದಳತೆ < 2) {
            userError("VSpaceRoot Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆರಂಭ = getSyscallArg(0, ಬಫರ್);
        ಕೊನೆ =   getSyscallArg(1, ಬಫರ್);

        /* Check sanity of arguments */
        ಆದರೆ (ಕೊನೆ <= ಆರಂಭ) {
            userError("VSpaceRoot Flush: Invalid range.");
            current_syscall_error.type = se_L4_Invalid_ವಾದ;
            current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 1;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        ಆದರೆ (ಕೊನೆ > USER_ತುದಿ) {
            userError("VSpaceRoot Flush: Exceed the user addressable region.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (unlikely(!is_Valid_Native_ಬೇರು(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        /* Make sure that the supplied pgd is ok */
        vspace_ಬೇರು_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap));
        asid = cap_vspace_cap_get_capVSMappedASID(cap);

        ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
        ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            userError("VSpaceRoot Flush: No VSpace for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು != vspace_ಬೇರು_2)) {
            userError("VSpaceRoot Flush: Invalid VSpace Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = lookupPTSlot(vspace_ಬೇರು_2, ಆರಂಭ);
        pte = *resolve_ret.pt_ಸ್ಥಾನ;

        /* Check that the returned slot is a page. */
        ಆದರೆ (!pte_is_ಪುಟ_type(pte)) {

            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
            ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        ಆದರೆ (ಸುತ್ತು_ಕೆಳಕ್ಕೆ(ಆರಂಭ, resolve_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ) != ಸುತ್ತು_ಕೆಳಕ್ಕೆ(ಕೊನೆ - 1, resolve_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = ಆರಂಭ;
            current_syscall_error.rangeErrorMax = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(ಆರಂಭ, resolve_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ) +
                                                  ಮುಖವಾಡ(resolve_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ);
            userError("VSpaceRoot Flush: cannot cross page boundary, valid range is [0x%lx..0x%lx)",
                      current_syscall_error.rangeErrorMin, current_syscall_error.rangeErrorMax);
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When in EL1, the mapping must be write-able for ARMVSpaceInvalidate_Data */
        ಆದರೆ (invLabel == ARMVSpace_Invalidate_ದತ್ತಾಂಶ && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMVSpaceInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
#ಆದರೆ_ಅಂತ್ಯ

        /* Calculate the physical start address. */
        paddr_t ಚೌಕಟ್ಟು_ತಳ_2 = pte_get_ಪುಟ_ತಳ_ವಿಳಾಸ(pte);

        pstart = ಚೌಕಟ್ಟು_ತಳ_2 + (ಆರಂಭ & ಮುಖವಾಡ(resolve_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        ಆದರೆ (pstart < PADDR_ತಳ || ((ಕೊನೆ - ಆರಂಭ) + pstart) > PADDR_ತುದಿ) {
            userError("VSpaceRoot Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
#ಆದರೆ_ಅಂತ್ಯ

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performVSpaceFlush(invLabel, vspace_ಬೇರು_2, asid, ಆರಂಭ, ಕೊನೆ - 1, pstart);

    ಪೂರ್ವನಿಯೋಜಿತ:
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}


ಸ್ಥಿರ exception_t decode_ARMPage_ಕೋಷ್ಟಕ_Invocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ,
                                                cte_t *cte, cap_t cap, ಶಬ್ದ_t *ಬಫರ್)
{
    cap_t vspace_ಬೇರು_Cap;
    vspace_ಬೇರು_t *vspace_ಬೇರು_2;
    pte_t pte;
    asid_t asid;
    vptr_t vaddr;
    lookupPTSlot_ret_t pt_ಸ್ಥಾನ;
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;

    ಆದರೆ (invLabel == ARMPage_ಕೋಷ್ಟಕ_Unmap) {
        ಆದರೆ (unlikely(!isFinalCapability(cte))) {
            current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_ಪುಟ_ಕೋಷ್ಟಕ_Invocation_Unmap(cap, cte);
    }

    ಆದರೆ (unlikely(invLabel != ARMPage_ಕೋಷ್ಟಕ_Map)) {
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (unlikely(ಉದ್ದಳತೆ < 2 || current_extra_caps.excaprefs[0] == ಖಾಲಿ)) {
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (unlikely(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, ಬಫರ್);
    vspace_ಬೇರು_Cap = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (unlikely(!is_Valid_Native_ಬೇರು(vspace_ಬೇರು_Cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    vspace_ಬೇರು_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_ಬೇರು_Cap));
    asid = cap_vspace_cap_get_capVSMappedASID(vspace_ಬೇರು_Cap);

    ಆದರೆ (unlikely(vaddr > USER_ತುದಿ)) {
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
    ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು != vspace_ಬೇರು_2)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    pt_ಸ್ಥಾನ = lookupPTSlot(vspace_ಬೇರು_2, vaddr);

    ಆದರೆ (unlikely(pt_ಸ್ಥಾನ.pt_ಬಿಟ್ಟುಗಳು_ಎಡ == se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು || pte_ptr_get_valid(pt_ಸ್ಥಾನ.pt_ಸ್ಥಾನ))) {
        current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    pte = pte_pte_ಕೋಷ್ಟಕ_new(pptr_ಗಮ್ಯ_paddr(PTE_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap))));

    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_PTIs_Mapped(cap, 1);
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_PTMapped_ವಿಳಾಸ(cap, (vaddr & ~ಮುಖವಾಡ(pt_ಸ್ಥಾನ.pt_ಬಿಟ್ಟುಗಳು_ಎಡ)));

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ perform_ಪುಟ_ಕೋಷ್ಟಕ_Invocation_Map(cap, cte, pte, pt_ಸ್ಥಾನ.pt_ಸ್ಥಾನ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool_t CONST checkVPAlignment(vm_ಪುಟ_ಗಾತ್ರ_t sz, ಶಬ್ದ_t w)
{
    ಹಿಂತಿರುಗಿ (w & ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(sz))) == 0;
}

ಸ್ಥಿರ exception_t decodeARMFrameInvocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ,
                                            cte_t *cte, cap_t cap, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆಯ್ಕೆ (invLabel) {
    ಸ್ಥಿತಿ ARMPageMap: {
        vptr_t vaddr;
        paddr_t ತಳ;
        cap_t vspace_ಬೇರು_Cap;
        vspace_ಬೇರು_t *vspace_ಬೇರು_2;
        asid_t asid, ಚೌಕಟ್ಟು_asid;
        vm_rights_t vmRights, capRights;
        vm_ಪುಟ_ಗಾತ್ರ_t ಚೌಕಟ್ಟು_ಗಾತ್ರ;
        vm_ಗುಣಲಕ್ಷಣಗಳು_t ಗುಣಲಕ್ಷಣಗಳು;
        ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;

        ಆದರೆ (unlikely(ಉದ್ದಳತೆ < 3 || current_extra_caps.excaprefs[0] == ಖಾಲಿ)) {
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, ಬಫರ್);
        ಗುಣಲಕ್ಷಣಗಳು = vm_ಗುಣಲಕ್ಷಣಗಳು_From_ಶಬ್ದ(getSyscallArg(2, ಬಫರ್));
        vspace_ಬೇರು_Cap = current_extra_caps.excaprefs[0]->cap;

        ಚೌಕಟ್ಟು_ಗಾತ್ರ = cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap);
        capRights = cap_ಚೌಕಟ್ಟು_cap_get_cap_FVMRights(cap);
        vmRights = ಮುಖವಾಡ_VMRights(capRights, rights_From_ಶಬ್ದ(getSyscallArg(1, ಬಫರ್)));

        ಆದರೆ (unlikely(!is_Valid_Native_ಬೇರು(vspace_ಬೇರು_Cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        vspace_ಬೇರು_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_ಬೇರು_Cap));
        asid = cap_vspace_cap_get_capVSMappedASID(vspace_ಬೇರು_Cap);

        ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
        ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು != vspace_ಬೇರು_2)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (unlikely(!checkVPAlignment(ಚೌಕಟ್ಟು_ಗಾತ್ರ, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        /* In the case of remap, the cap should have a valid asid */
        ಚೌಕಟ್ಟು_asid = cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap);

        ಆದರೆ (ಚೌಕಟ್ಟು_asid != asidInvalid) {
            ಆದರೆ (ಚೌಕಟ್ಟು_asid != asid) {
                userError("ARMPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;

            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_ವಾದ;
                current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;
                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (unlikely(vaddr + ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ಚೌಕಟ್ಟು_ಗಾತ್ರ)) - 1 > USER_ತುದಿ)) {
                current_syscall_error.type = se_L4_Invalid_ವಾದ;
                current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;
                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }
        }

        cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ASID(cap, asid);
        cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ವಿಳಾಸ(cap, vaddr);

        ತಳ = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap));

        lookupPTSlot_ret_t lu_ret = lookupPTSlot(vspace_ಬೇರು_2, vaddr);
        ಆದರೆ (unlikely(lu_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ != ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ಚೌಕಟ್ಟು_ಗಾತ್ರ))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        bool_t cap_read = (capRights == VMReadOnly || capRights == VMReadWrite);
        pte_t pte = make_User_ಪುಟ_PTE(ತಳ, vmRights, cap_read, ಗುಣಲಕ್ಷಣಗಳು, ಚೌಕಟ್ಟು_ಗಾತ್ರ);

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_ಪುಟ_Invocation_Map(asid, cap, cte, pte, lu_ret.pt_ಸ್ಥಾನ);
    }

    ಸ್ಥಿತಿ ARMPageUnmap:
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_ಪುಟ_Invocation_Unmap(cap, cte);

    ಸ್ಥಿತಿ ARMPage_Clean_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMPage_Invalidate_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMPage_Clean_Invalidate_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMPageUnify_Instruction: {
        vptr_t ಆರಂಭ, ಕೊನೆ;
        vptr_t vaddr;
        asid_t asid;
        ಶಬ್ದ_t ಪುಟ_ಗಾತ್ರ;
        ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;

        ಆದರೆ (ಉದ್ದಳತೆ < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (unlikely(cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap) == 0)) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap);
        asid = cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap);

        ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
        ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            userError("Page Flush: No PGD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆರಂಭ = getSyscallArg(0, ಬಫರ್);
        ಕೊನೆ =   getSyscallArg(1, ಬಫರ್);

        /* check that the range is sane */
        ಆದರೆ (ಕೊನೆ <= ಆರಂಭ) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_ವಾದ;
            current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 1;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        /* start and end are currently relative inside this page */
        ಪುಟ_ಗಾತ್ರ = ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap)));
        ಆದರೆ (ಆರಂಭ >= ಪುಟ_ಗಾತ್ರ || ಕೊನೆ > ಪುಟ_ಗಾತ್ರ) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_ವಾದ;
            current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಶಬ್ದ_t pstart = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap)) + ಆರಂಭ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        ಆದರೆ (pstart < PADDR_ತಳ || ((ಕೊನೆ - ಆರಂಭ) + pstart) > PADDR_ತುದಿ) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
#ಇಲ್ಲದಿದ್ದರೆ
        /* When in EL1, we are using the user page table for flushing and need to make sure
           the mapping info in the cap is not stale. */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vaddr);
        pte_t pte = *lu_ret.pt_ಸ್ಥಾನ;
        ಶೂನ್ಯ *ತಳ_ptr = (ಶೂನ್ಯ *) cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap);
        ಆದರೆ (unlikely(lu_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ != ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap)) ||
                     !pte_is_ಪುಟ_type(pte) ||
                     pte_get_ಪುಟ_ತಳ_ವಿಳಾಸ(pte) != pptr_ಗಮ್ಯ_paddr(ತಳ_ptr))) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        /* When in EL1, the mapping must be writeable for DC IVAC */
        ಆದರೆ (invLabel == ARMPage_Invalidate_ದತ್ತಾಂಶ && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMPageInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
#ಆದರೆ_ಅಂತ್ಯ
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_ಪುಟ_Flush(invLabel, ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, asid, vaddr + ಆರಂಭ, vaddr + ಕೊನೆ - 1,
                                pstart);
    }

    ಸ್ಥಿತಿ ARMPage_Get_ವಿಳಾಸ:
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_ಪುಟ_Get_ವಿಳಾಸ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap), call);

    ಪೂರ್ವನಿಯೋಜಿತ:
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_vspace_cap:
        ಹಿಂತಿರುಗಿ decode_ARMVSpace_ಬೇರು_Invocation(invLabel, ಉದ್ದಳತೆ, cte, cap, ಬಫರ್);
    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಹಿಂತಿರುಗಿ decode_ARMPage_ಕೋಷ್ಟಕ_Invocation(invLabel, ಉದ್ದಳತೆ, cte, cap, ಬಫರ್);

    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        ಹಿಂತಿರುಗಿ decodeARMFrameInvocation(invLabel, ಉದ್ದಳತೆ, cte, cap, call, ಬಫರ್);

    ಸ್ಥಿತಿ cap_asid_control_cap: {
        ಶಬ್ದ_t i;
        asid_t asid_ತಳ;
        ಶಬ್ದ_t ಸೂಚ್ಯಂಕ, ಆಳ;
        cap_t untyped, ಬೇರು;
        cte_t *ಪೋಷಕ_ಸ್ಥಾನ, *dest_ಸ್ಥಾನ;
        lookup_ಸ್ಥಾನ_ret_t lu_ret;
        ಶೂನ್ಯ *ಚೌಕಟ್ಟು;
        exception_t ಸ್ಥಿತಿ_2;

        ಆದರೆ (unlikely(invLabel != ARMASIDControlMakePool)) {
            current_syscall_error.type = seL4_IllegalOperation;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (unlikely(ಉದ್ದಳತೆ < 2 ||
                     current_extra_caps.excaprefs[0] == ಖಾಲಿ ||
                     current_extra_caps.excaprefs[1] == ಖಾಲಿ)) {
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಸೂಚ್ಯಂಕ = getSyscallArg(0, ಬಫರ್);
        ಆಳ = getSyscallArg(1, ಬಫರ್);
        ಪೋಷಕ_ಸ್ಥಾನ = current_extra_caps.excaprefs[0];
        untyped = ಪೋಷಕ_ಸ್ಥಾನ->cap;
        ಬೇರು = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        ಗಾಗಿ (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        ಆದರೆ (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_ಮೊದಲ;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        asid_ತಳ = i << asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು;

        ಆದರೆ (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_ಖಂಡ_ಗಾತ್ರ(untyped) != se_L4_ASIDPool_ಬಿಟ್ಟುಗಳು ||
                     cap_untyped_cap_get_capIsDevice(untyped))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಸ್ಥಿತಿ_2 = ensureNoChildren(ಪೋಷಕ_ಸ್ಥಾನ);
        ಆದರೆ (unlikely(ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಚೌಕಟ್ಟು = ಶಬ್ದ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_ಗುರಿ_ಸ್ಥಾನ(ಬೇರು, ಸೂಚ್ಯಂಕ, ಆಳ);
        ಆದರೆ (unlikely(lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
        }
        dest_ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;

        ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(dest_ಸ್ಥಾನ);
        ಆದರೆ (unlikely(ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performASIDControlInvocation(ಚೌಕಟ್ಟು, dest_ಸ್ಥಾನ, ಪೋಷಕ_ಸ್ಥಾನ, asid_ತಳ);
    }

    ಸ್ಥಿತಿ cap_asid_pool_cap: {
        cap_t vspaceCap;
        cte_t *vspace_Cap_ಸ್ಥಾನ;
        asid_pool_t *pool;
        ಶಬ್ದ_t i;
        asid_t asid;

        ಆದರೆ (unlikely(invLabel != ARMASIDPool_ನಿಯೋಜಿಸು)) {
            current_syscall_error.type = seL4_IllegalOperation;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (unlikely(current_extra_caps.excaprefs[0] == ಖಾಲಿ)) {
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_ಸ್ಥಾನ = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_ಸ್ಥಾನ->cap;

        ಆದರೆ (unlikely(!is_VTable_ಬೇರು(vspaceCap) || cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_ಮೇಲಿನ(cap_asid_pool_cap_get_capASIDBase(cap))];

        ಆದರೆ (unlikely(!pool)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
            current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        ಗಾಗಿ (i = 0; i < (1 << asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು) && (asid + i == 0
                                               || (asid_map_get_type(pool->array[i]) != asid_map_asid_map_none)); i++);

        ಆದರೆ (unlikely(i == 1 << asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು)) {
            current_syscall_error.type = se_L4_Delete_ಮೊದಲ;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performASIDPoolInvocation(asid, pool, vspace_Cap_ಸ್ಥಾನ);
    }

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid ARM arch cap type");
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
ಶೂನ್ಯ kernelPrefetchAbort(ಶಬ್ದ_t pc) VISIBLE;
ಶೂನ್ಯ kernel_ದತ್ತಾಂಶ_Abort(ಶಬ್ದ_t pc) VISIBLE;

ಶೂನ್ಯ kernelPrefetchAbort(ಶಬ್ದ_t pc)
{
    ಬರೆ("\n\nKERNEL PREFETCH ABORT!\n");
    ಬರೆ("Faulting instruction: 0x%"SEL4_PRIx_ಶಬ್ದ"\n", pc);
    ಬರೆ("ESR (IFSR): 0x%"SEL4_PRIx_ಶಬ್ದ"\n", getIFSR());
    halt();
}

ಶೂನ್ಯ kernel_ದತ್ತಾಂಶ_Abort(ಶಬ್ದ_t pc)
{
    ಬರೆ("\n\nKERNEL DATA ABORT!\n");
    ಬರೆ("Faulting instruction: 0x%"SEL4_PRIx_ಶಬ್ದ"\n", pc);
    ಬರೆ("FAR: 0x%"SEL4_PRIx_ಶಬ್ದ" ESR (DFSR): 0x%"SEL4_PRIx_ಶಬ್ದ"\n",
           getFAR(), getDFSR());
    halt();
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ read_ಶಬ್ದ_From_VSpace_ret {
    exception_t ಸ್ಥಿತಿ_2;
    ಶಬ್ದ_t ಮೌಲ್ಯ;
} read_ಶಬ್ದ_From_VSpace_ret_t;

ಸ್ಥಿರ read_ಶಬ್ದ_From_VSpace_ret_t read_ಶಬ್ದ_From_VSpace(vspace_ಬೇರು_t *pd, ಶಬ್ದ_t vaddr)
{
    read_ಶಬ್ದ_From_VSpace_ret_t ret;
    ಶಬ್ದ_t ಸ್ಥಾನಾಂತರ;
    pptr_t kernel_vaddr;
    ಶಬ್ದ_t *ಮೌಲ್ಯ;

    lookupPTSlot_ret_t lookup_ret = lookupPTSlot(pd, vaddr);

    /* Check that the returned slot is a page. */
    ಆದರೆ (!pte_ptr_get_valid(lookup_ret.pt_ಸ್ಥಾನ) ||
        (pte_pte_ಕೋಷ್ಟಕ_ptr_get_present(lookup_ret.pt_ಸ್ಥಾನ) && lookup_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ > ಪುಟ_ಬಿಟ್ಟುಗಳು)) {
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }

    ಸ್ಥಾನಾಂತರ = vaddr & ಮುಖವಾಡ(lookup_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ);
    kernel_vaddr = (ಶಬ್ದ_t)paddr_ಗಮ್ಯ_pptr(pte_ಪುಟ_ptr_get_ಪುಟ_ತಳ_ವಿಳಾಸ(lookup_ret.pt_ಸ್ಥಾನ));
    ಮೌಲ್ಯ = (ಶಬ್ದ_t *)(kernel_vaddr + ಸ್ಥಾನಾಂತರ);

    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ret.ಮೌಲ್ಯ = *ಮೌಲ್ಯ;
    ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ Arch_user_ರಾಶಿ_Trace(tcb_t *tptr)
{
    cap_t thread_ಬೇರು;
    vspace_ಬೇರು_t *vspace_ಬೇರು_2;
    ಶಬ್ದ_t sp;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the vspace root */
    ಆದರೆ (cap_get_capType(thread_ಬೇರು) != cap_vspace_cap) {
        ಬರೆ("Invalid vspace\n");
        ಹಿಂತಿರುಗಿ;
    }

    vspace_ಬೇರು_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_ಬೇರು));
    sp = get_ರಿಜಿಸ್ಟರ್(tptr, SP_EL0);

    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    ಆದರೆ (!IS_ALIGNED(sp, se_L4_ಶಬ್ದ_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)) {
        ಬರೆ("SP not aligned\n");
        ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i = 0; i < CONFIG_USER_ರಾಶಿ_TRACE_ಉದ್ದಳತೆ; i++) {
        ಶಬ್ದ_t ವಿಳಾಸ = sp + (i * ಗಾತ್ರ(ಶಬ್ದ_t));
        read_ಶಬ್ದ_From_VSpace_ret_t ಫಲಿತಾಂಶ = read_ಶಬ್ದ_From_VSpace(vspace_ಬೇರು_2,
                                                             ವಿಳಾಸ);
        ಆದರೆ (ಫಲಿತಾಂಶ.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE) {
            ಬರೆ("0x%"SEL4_PRIx_ಶಬ್ದ": 0x%"SEL4_PRIx_ಶಬ್ದ"\n",
                   ವಿಳಾಸ, ಫಲಿತಾಂಶ.ಮೌಲ್ಯ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಬರೆ("0x%"SEL4_PRIx_ಶಬ್ದ": INVALID\n", ವಿಳಾಸ);
        }
    }
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */

#ಆದರೆ defined(CONFIG_KERNEL_LOG_ಬಫರ್)
exception_t benchmark_arch_map_log_ಬಫರ್(ಶಬ್ದ_t ಚೌಕಟ್ಟು_cptr)
{
    lookup_Cap_And_ಸ್ಥಾನ_ret_t lu_ret;
    vm_ಪುಟ_ಗಾತ್ರ_t ಚೌಕಟ್ಟು_ಗಾತ್ರ;
    pptr_t  ಚೌಕಟ್ಟು_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_ಸ್ಥಾನ(ಗಂಟು_STATE(ksCurThread), ಚೌಕಟ್ಟು_cptr);

    ಆದರೆ (unlikely(lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", ಚೌಕಟ್ಟು_cptr);
        current_fault = seL4_Fault_CapFault_new(ಚೌಕಟ್ಟು_cptr, false);

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (cap_get_capType(lu_ret.cap) != cap_ಚೌಕಟ್ಟು_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(ಚೌಕಟ್ಟು_cptr, false);

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಚೌಕಟ್ಟು_ಗಾತ್ರ = cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(lu_ret.cap);

    ಆದರೆ (ಚೌಕಟ್ಟು_ಗಾತ್ರ != ARMLarge_ಪುಟ) {
        userError("Invalid frame size. The kernel expects 2M log buffer");
        current_fault = seL4_Fault_CapFault_new(ಚೌಕಟ್ಟು_cptr, false);

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಚೌಕಟ್ಟು_pptr = cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_ಬಫರ್ = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *) ಚೌಕಟ್ಟು_pptr);

    *armKSGlobalLogPTE = pte_pte_ಪುಟ_new(
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
                             0, // XN
#ಇಲ್ಲದಿದ್ದರೆ
                             1, // UXN
#ಆದರೆ_ಅಂತ್ಯ
                             ks_User_Log_ಬಫರ್,
                             0,                         /* global */
                             1,                         /* access flag */
                             SMP_TERNARY(SMP_SHARE, 0), /* Inner-shareable if SMP enabled, otherwise unshared */
                             0,                         /* VMKernelOnly */
                             ಸಾಮಾನ್ಯ_WT);

    cleanByVA_PoU((vptr_t)armKSGlobalLogPTE, addrFromKPPtr(armKSGlobalLogPTE));
    invalidateTranslationSingle(KS_LOG_PPTR);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_LOG_BUFFER */
