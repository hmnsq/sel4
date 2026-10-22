/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/boot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/cspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/stack.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/cnode.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/untyped.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/api/invocation.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <armv/context_switch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/iospace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/vcpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/tlb.h>

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
ਗਿਣਤੀ mair_types {
    DEVICE_nGnRnE = 0,
    DEVICE_nGnRE = 1,
    DEVICE_GRE = 2,
    ਸਾਧਾਰਨ_NC = 3,
    ਸਾਧਾਰਨ = 4,
    ਸਾਧਾਰਨ_WT = 5
};

/* Stage-2 translation memory attributes */
ਗਿਣਤੀ mair_s2_types {
    S2_DEVICE_nGnRnE = 0,
    S2_DEVICE_nGnRE = 1,
    S2_DEVICE_nGRE  = 2,
    S2_DEVICE_GRE = 3,

    S2_ਸਾਧਾਰਨ_INNER_NC_OUTER_NC = 5,
    S2_ਸਾਧਾਰਨ_INNER_WTC_OUTER_NC = 6,
    S2_ਸਾਧਾਰਨ_INNER_WBC_OUTER_NC = 7,

    S2_ਸਾਧਾਰਨ_INNER_NC_OUTER_WTC = 9,
    S2_ਸਾਧਾਰਨ_INNER_WTC_OUTER_WTC = 10,
    S2_ਸਾਧਾਰਨ_INNER_WBC_OUTER_WTC = 11,

    S2_ਸਾਧਾਰਨ_INNER_NC_OUTER_WBC = 13,
    S2_ਸਾਧਾਰਨ_INNER_WTC_OUTER_WBC = 14,
    S2_ਸਾਧਾਰਨ_INNER_WBC_OUTER_WBC = 15,

    S2_ਸਾਧਾਰਨ = S2_ਸਾਧਾਰਨ_INNER_WBC_OUTER_WBC
};

/* Leif from Linaro said the big.LITTLE clusters should be treated as
 * inner shareable, and we believe so, although the Example B2-1 given in
 * ARM ARM DDI 0487B.b (ID092517) says otherwise.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SMP_SHARE   3

ਬਣਤਰ lookupPTSlot_ret {
    pte_t *pt_ਥਾਂ;
    ਸ਼ਬਦ_t pt_ਬਿੱਟ_ਖੱਬਾ;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ lookupPTSlot_ret lookupPTSlot_ret_t;

ਬਣਤਰ ਲੱਭੋ_VSpace_For_ASID_ret {
    exception_t ਸਥਿਤੀ;
    vspace_ਜੜ੍ਹ_t *vspace_ਜੜ੍ਹ;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਲੱਭੋ_VSpace_For_ASID_ret ਲੱਭੋ_VSpace_For_ASID_ret_t;

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

ਸਥਿਰ ਸ਼ਬਦ_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    ਚੋਣ (vm_rights) {
    ਹਾਲਤ VMKernelOnly:
        ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            ਵਾਪਸ 0;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ 0;
        }

    ਹਾਲਤ VMReadWrite:
        ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            ਵਾਪਸ 3;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ 1;
        }

    ਹਾਲਤ VMReadOnly:
        ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            ਵਾਪਸ 1;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ 3;
        }

    ਮੂਲ:
        fail("Invalid VM rights");
    }
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
ਸਥਿਰ ਸਤਰਅੰਦਰ CONST ਸ਼ਬਦ_t pte_get_AP(pte_t pte)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(pte_is_ਪੰਨਾ_type(pte));
    ਚੋਣ (pte_get_pte_type(pte)) {
    ਹਾਲਤ pte_pte_4k_ਪੰਨਾ:
        ਵਾਪਸ pte_pte_4k_ਪੰਨਾ_get_AP(pte);

    ਹਾਲਤ pte_pte_ਪੰਨਾ:
        ਵਾਪਸ pte_pte_ਪੰਨਾ_get_AP(pte);

    ਮੂਲ:
        ਵਾਪਸ 0;
    }
}

ਸਥਿਰ vm_rights_t CONST vmRightsFromPTE(pte_t pte)
{
    ਸ਼ਬਦ_t access_perms = pte_get_AP(pte);
    ਚੋਣ (access_perms) {
    ਹਾਲਤ 0:
    ਹਾਲਤ 2:
        ਵਾਪਸ VMKernelOnly;
    ਹਾਲਤ 1:
        ਵਾਪਸ VMReadWrite;
    ਹਾਲਤ 3:
        ਵਾਪਸ VMReadOnly;
    ਮੂਲ:
        fail("Invalid AP bit");
    }
}
#ਜੇ_ਅੰਤ

vm_rights_t CONST ਨਕਾਬ_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_ਨਕਾਬ)
{
    ਜੇ (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_ਨਕਾਬ)) {
        ਵਾਪਸ VMReadOnly;
    }
    ਜੇ (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_ਨਕਾਬ)) {
        ਜੇ (!seL4_CapRights_get_capAllowWrite(cap_rights_ਨਕਾਬ)) {
            ਵਾਪਸ VMReadOnly;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ VMReadWrite;
        }
    }
    ਜੇ (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_ਨਕਾਬ) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_ਨਕਾਬ)) {
        userError("Attempted to make unsupported write only mapping");
    }
    ਵਾਪਸ VMKernelOnly;
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
BOOT_ਕੋਡ ਖਾਲੀ map_kernel_ਚੌਖਟਾ(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_ਗੁਣ_t ਗੁਣ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(vaddr >= PPTR_ਸਿਖਰ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਸ਼ਬਦ_t uxn = vm_ਗੁਣ_get_arm_Execute_Never(ਗੁਣ);
#ਨਹੀਂਤਾਂ
    ਸ਼ਬਦ_t uxn = 1; /* unprivileged execute never */
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    ਸ਼ਬਦ_t attr_ਸੂਚਕਾਂਕ;
    ਸ਼ਬਦ_t shareable;
    ਜੇ (vm_ਗੁਣ_get_arm_ਪੰਨਾ_Cacheable(ਗੁਣ)) {
        attr_ਸੂਚਕਾਂਕ = ਸਾਧਾਰਨ;
        shareable = SMP_TERNARY(SMP_SHARE, 0);
    } ਨਹੀਂਤਾਂ {
        attr_ਸੂਚਕਾਂਕ = DEVICE_nGnRnE;
        shareable = 0;
    }
    armKSGlobalKernelPT[GET_KPT_ਸੂਚਕਾਂਕ(vaddr, KLVL_FRM_ARM_PT_LVL(3))] = pte_pte_4k_ਪੰਨਾ_new(uxn, paddr,
                                                                                            0, /* global */
                                                                                            1, /* access flag */
                                                                                            shareable,
                                                                                            APFromVMRights(vm_rights),
                                                                                            attr_ਸੂਚਕਾਂਕ);
}

/* Essentially, assert that the ELF, kernel devices, and log buffer are all in the same PUD.
   In the current code this is the top index. This is arbitrary, the important part
   is that they are all the same.
 */
compile_assert(elf_ਵਿੱਚ_kernel_mappings, GET_KPT_ਸੂਚਕਾਂਕ(KERNEL_ELF_ਅਧਾਰ,
                                                     KLVL_FRM_ARM_PT_LVL(1)) == ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1);
compile_assert(kdev_ਵਿੱਚ_kernel_mappings, GET_KPT_ਸੂਚਕਾਂਕ(KDEV_ਅਧਾਰ,
                                                      KLVL_FRM_ARM_PT_LVL(1)) == ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1);
compile_assert(log_buf_ਵਿੱਚ_kernel_mappings, GET_KPT_ਸੂਚਕਾਂਕ(KS_LOG_ਅਧਾਰ,
                                                         KLVL_FRM_ARM_PT_LVL(1)) == ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1);

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
BOOT_ਕੋਡ ਖਾਲੀ map_kernel_elf_image(pte_t kernel_mapping_pds[])
{
    /** This is not strictly necessary, but to handle a non-aligned KERNEL_ELF_PADDR_BASE
     *  we'd need to do a bit more logic and probably do what riscv64 does
     *  where the *actual* KERNEL_ELF_BASE is rounded up to the alignment.
     *  This is simpler and memory is cheap.
     **/
    compile_assert(elf_paddr_aligned, IS_ALIGNED(KERNEL_ELF_PADDR_ਅਧਾਰ_RAW, se_L4_Large_ਪੰਨਾ_ਬਿੱਟ));
    compile_assert(elf_ਅਧਾਰ_aligned, IS_ALIGNED(KERNEL_ELF_ਅਧਾਰ, se_L4_Large_ਪੰਨਾ_ਬਿੱਟ));

    /* Please don't overlap with KDEV_BASE */
    ਪ੍ਰਤਿਜ੍ਞਾ(KERNEL_ELF_ਸਿਖਰ <= KDEV_ਅਧਾਰ);
    /* Please don't overlap with KS_LOG_PPTR */
    ਪ੍ਰਤਿਜ੍ਞਾ(KERNEL_ELF_ਸਿਖਰ <= KS_LOG_PPTR);

    ਅਚਲ ਸ਼ਬਦ_t pd_ਸ਼ੁਰੂ = GET_KPT_ਸੂਚਕਾਂਕ(KERNEL_ELF_ਅਧਾਰ, KLVL_FRM_ARM_PT_LVL(2));
    /* pd_end and ELF_TOP are both exclusive, so handle that correctly */
    ਅਚਲ ਸ਼ਬਦ_t pd_ਸਿਰਾ   = GET_KPT_ਸੂਚਕਾਂਕ(KERNEL_ELF_ਸਿਖਰ - 1, KLVL_FRM_ARM_PT_LVL(2)) + 1;

    paddr_t paddr = KERNEL_ELF_PADDR_ਅਧਾਰ;
    ਅਣਵਰਤਿਆ pptr_t vaddr = KERNEL_ELF_ਅਧਾਰ;

    ਲਈ (ਸ਼ਬਦ_t pd = pd_ਸ਼ੁਰੂ; pd < pd_ਸਿਰਾ; pd++) {
        ਪ੍ਰਤਿਜ੍ਞਾ(pd == GET_KPT_ਸੂਚਕਾਂਕ(vaddr, KLVL_FRM_ARM_PT_LVL(2)));

        kernel_mapping_pds[pd] = pte_pte_ਪੰਨਾ_new(
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
                                     /* XN */ 0,
#ਨਹੀਂਤਾਂ
                                     /* UXN */ 1,
#ਜੇ_ਅੰਤ
                                     /* page_base_address */ paddr,
                                     /* global (nG) */ 0,
                                     /* access flag (AF) */ 1,
                                     /* shareability (SH) */ SMP_TERNARY(SMP_SHARE, 0),
                                     /* AP */ APFromVMRights(VMKernelOnly),
                                     /* AttrIndx */ ਸਾਧਾਰਨ
                                 );

        paddr += ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Large_ਪੰਨਾ_ਬਿੱਟ);
        vaddr += ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Large_ਪੰਨਾ_ਬਿੱਟ);
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
/* verify that the physical memory window as at the second entry of the PGD */
compile_assert(physical_ਖਿੜਕੀ_pgd_location,
               GET_KPT_ਸੂਚਕਾਂਕ(PPTR_ਅਧਾਰ, KLVL_FRM_ARM_PT_LVL(0)) == 1);
#ਨਹੀਂਤਾਂ
/* verify that the physical memory window as at the last entry of the PGD */
compile_assert(physical_ਖਿੜਕੀ_pgd_location,
               GET_KPT_ਸੂਚਕਾਂਕ(PPTR_ਅਧਾਰ, KLVL_FRM_ARM_PT_LVL(0)) == ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1);
#ਜੇ_ਅੰਤ
/* make sure that the PPTR_BASE and PPTR_TOP are huge paged aligned */
compile_assert(physical_ਖਿੜਕੀ_ਅਧਾਰ_aligned, IS_ALIGNED(PPTR_ਅਧਾਰ, se_L4_Huge_ਪੰਨਾ_ਬਿੱਟ));
compile_assert(physical_ਖਿੜਕੀ_ਸਿਖਰ_aligned, IS_ALIGNED(PPTR_ਸਿਖਰ, se_L4_Huge_ਪੰਨਾ_ਬਿੱਟ));

/* the PPTR_BASE starts from the bottom of the kernel PUD */
compile_assert(physical_ਖਿੜਕੀ_bottom_kernel_region,
               GET_KPT_ਸੂਚਕਾਂਕ(PPTR_ਅਧਾਰ, KLVL_FRM_ARM_PT_LVL(1)) == 0);
/* the PPTR_TOP then ends at the second-to-last entry of the PUD
    note: because this is the exclusive value the assert is one higher than
          you would expect.
*/
compile_assert(physical_ਖਿੜਕੀ_ਸਿਖਰ_kernel_region,
               GET_KPT_ਸੂਚਕਾਂਕ(PPTR_ਸਿਖਰ, KLVL_FRM_ARM_PT_LVL(1)) == ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1);

BOOT_ਕੋਡ ਖਾਲੀ map_kernel_physical_ਖਿੜਕੀ(ਖਾਲੀ)
{
    paddr_t paddr;
    pptr_t vaddr;
    ਸ਼ਬਦ_t idx;

    /* place all PDs except the last one in PUD */
    ਲਈ (idx = GET_KPT_ਸੂਚਕਾਂਕ(PPTR_ਅਧਾਰ, KLVL_FRM_ARM_PT_LVL(1)); idx < GET_KPT_ਸੂਚਕਾਂਕ(PPTR_ਸਿਖਰ, KLVL_FRM_ARM_PT_LVL(1));
         idx++) {
        armKSGlobalKernelPUD[idx] = pte_pte_ਸਾਰਣੀ_new(
                                        addrFromKPPtr(&armKSGlobalKernelPDs[idx][0])
                                    );
    }

    /* map the kernel window using large pages */
    vaddr = PPTR_ਅਧਾਰ;
    ਲਈ (paddr = PADDR_ਅਧਾਰ; paddr < PADDR_ਸਿਖਰ; paddr += ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Large_ਪੰਨਾ_ਬਿੱਟ)) {
        armKSGlobalKernelPDs[GET_KPT_ਸੂਚਕਾਂਕ(vaddr, KLVL_FRM_ARM_PT_LVL(1))][GET_KPT_ਸੂਚਕਾਂਕ(vaddr,
                                                                                         KLVL_FRM_ARM_PT_LVL(2))] = pte_pte_ਪੰਨਾ_new(
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
                                                                                                                        1, // XN
#ਨਹੀਂਤਾਂ
                                                                                                                        1, // UXN
#ਜੇ_ਅੰਤ
                                                                                                                        paddr,
                                                                                                                        0,                        /* global */
                                                                                                                        1,                        /* access flag */
                                                                                                                        SMP_TERNARY(SMP_SHARE, 0),        /* Inner-shareable if SMP enabled, otherwise unshared */
                                                                                                                        0,                        /* VMKernelOnly */
                                                                                                                        ਸਾਧਾਰਨ
                                                                                                                    );
        vaddr += ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Large_ਪੰਨਾ_ਬਿੱਟ);
    }
}

BOOT_ਕੋਡ ਖਾਲੀ map_kernel_ਖਿੜਕੀ(ਖਾਲੀ)
{
    /* place the PUD into the PGD */
    armKSGlobalKernelPGD[GET_KPT_ਸੂਚਕਾਂਕ(PPTR_ਅਧਾਰ, KLVL_FRM_ARM_PT_LVL(0))] =
        pte_pte_ਸਾਰਣੀ_new(addrFromKPPtr(armKSGlobalKernelPUD));

    map_kernel_physical_ਖਿੜਕੀ();

    ਅਚਲ ਸ਼ਬਦ_t elf_pud_idx = GET_KPT_ਸੂਚਕਾਂਕ(KERNEL_ELF_ਅਧਾਰ, KLVL_FRM_ARM_PT_LVL(1));
    pte_t *kernel_mapping_pds = armKSGlobalKernelPDs[elf_pud_idx];

    /* place the kernel mapping PDs in the kernel mapping PUD */
    armKSGlobalKernelPUD[elf_pud_idx] = pte_pte_ਸਾਰਣੀ_new(addrFromKPPtr(kernel_mapping_pds));

    /* map the kernel ELF */
    map_kernel_elf_image(kernel_mapping_pds);

    /* put the PT into the PD for device window. It would be nice if
       we could pass kernel_mapping_pds[511] to map_kernel_devices(), but it is
       shared between AArch64 and AArch32, so... */
    kernel_mapping_pds[ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1]
        = pte_pte_ਸਾਰਣੀ_new(addrFromKPPtr(armKSGlobalKernelPT));

    map_kernel_devices();
}

/* When the hypervisor support is enabled, the stage-2 translation table format
 * is used for applications.
 * The global bit is always 0.
 * The memory attributes use the S2 translation values.
 */
ਸਥਿਰ BOOT_ਕੋਡ ਖਾਲੀ map_it_ਚੌਖਟਾ_cap(cap_t vspace_cap, cap_t ਚੌਖਟਾ_cap, bool_t executable)
{
    vspace_ਜੜ੍ਹ_t *vspace_ਜੜ੍ਹ_2 = VSPACE_PTR(pptr_ਦਾ_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt;

    vptr_t vptr = cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(ਚੌਖਟਾ_cap);
    ਖਾਲੀ *pptr = (ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(ਚੌਖਟਾ_cap);

    ਪ੍ਰਤਿਜ੍ਞਾ(cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(ਚੌਖਟਾ_cap) != 0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AARCH64_VSPACE_S2_ਸ਼ੁਰੂ_L1
    pud = vspace_ਜੜ੍ਹ_2;
#ਨਹੀਂਤਾਂ
    vspace_ਜੜ੍ਹ_2 += GET_UPT_ਸੂਚਕਾਂਕ(vptr, ULVL_FRM_ARM_PT_LVL(0));
    ਪ੍ਰਤਿਜ੍ਞਾ(pte_pte_ਸਾਰਣੀ_ptr_get_present(vspace_ਜੜ੍ਹ_2));
    pud = paddr_ਮੰਜ਼ਿਲ_pptr(pte_pte_ਸਾਰਣੀ_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(vspace_ਜੜ੍ਹ_2));
#ਜੇ_ਅੰਤ
    pud += GET_UPT_ਸੂਚਕਾਂਕ(vptr, ULVL_FRM_ARM_PT_LVL(1));
    ਪ੍ਰਤਿਜ੍ਞਾ(pte_pte_ਸਾਰਣੀ_ptr_get_present(pud));
    pd = paddr_ਮੰਜ਼ਿਲ_pptr(pte_pte_ਸਾਰਣੀ_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(pud));
    pd += GET_UPT_ਸੂਚਕਾਂਕ(vptr, ULVL_FRM_ARM_PT_LVL(2));
    ਪ੍ਰਤਿਜ੍ਞਾ(pte_pte_ਸਾਰਣੀ_ptr_get_present(pd));
    pt = paddr_ਮੰਜ਼ਿਲ_pptr(pte_pte_ਸਾਰਣੀ_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(pd));
    *(pt + GET_UPT_ਸੂਚਕਾਂਕ(vptr, ULVL_FRM_ARM_PT_LVL(3))) = pte_pte_4k_ਪੰਨਾ_new(
                                                              !executable,                    /* unprivileged execute never */
                                                              pptr_ਮੰਜ਼ਿਲ_paddr(pptr),            /* page_base_address    */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              0,
#ਨਹੀਂਤਾਂ
                                                              1,                              /* not global */
#ਜੇ_ਅੰਤ
                                                              1,                              /* access flag */
                                                              SMP_TERNARY(SMP_SHARE, 0),              /* Inner-shareable if SMP enabled, otherwise unshared */
                                                              APFromVMRights(VMReadWrite),
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              S2_ਸਾਧਾਰਨ
#ਨਹੀਂਤਾਂ
                                                              ਸਾਧਾਰਨ
#ਜੇ_ਅੰਤ
                                                          );
}

ਸਥਿਰ BOOT_ਕੋਡ cap_t create_it_ਚੌਖਟਾ_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    vm_ਪੰਨਾ_ਮਾਪ_t ਚੌਖਟਾ_ਮਾਪ_2;
    ਜੇ (use_large) {
        ਚੌਖਟਾ_ਮਾਪ_2 = ARMLarge_ਪੰਨਾ;
    } ਨਹੀਂਤਾਂ {
        ਚੌਖਟਾ_ਮਾਪ_2 = ARMSmall_ਪੰਨਾ;
    }
    ਵਾਪਸ
        cap_ਚੌਖਟਾ_cap_new(
            asid,                          /* capFMappedASID */
            pptr,                          /* capFBasePtr */
            ਚੌਖਟਾ_ਮਾਪ_2,                    /* capFSize */
            vptr,                          /* capFMappedAddress */
            ਸ਼ਬਦ_From_VMRights(VMReadWrite), /* capFVMRights */
            false                          /* capFIsDevice */
        );
}

ਸਥਿਰ BOOT_ਕੋਡ ਖਾਲੀ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    vspace_ਜੜ੍ਹ_t *vspace_ਜੜ੍ਹ_2 = VSPACE_PTR(pptr_ਦਾ_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt = PT_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(pt_cap);

    ਪ੍ਰਤਿਜ੍ਞਾ(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(pt_cap));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AARCH64_VSPACE_S2_ਸ਼ੁਰੂ_L1
    pud = vspace_ਜੜ੍ਹ_2;
#ਨਹੀਂਤਾਂ
    vspace_ਜੜ੍ਹ_2 += GET_UPT_ਸੂਚਕਾਂਕ(vptr, ULVL_FRM_ARM_PT_LVL(0));
    ਪ੍ਰਤਿਜ੍ਞਾ(pte_pte_ਸਾਰਣੀ_ptr_get_present(vspace_ਜੜ੍ਹ_2));
    pud = paddr_ਮੰਜ਼ਿਲ_pptr(pte_pte_ਸਾਰਣੀ_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(vspace_ਜੜ੍ਹ_2));
#ਜੇ_ਅੰਤ
    pud += GET_UPT_ਸੂਚਕਾਂਕ(vptr, ULVL_FRM_ARM_PT_LVL(1));
    ਪ੍ਰਤਿਜ੍ਞਾ(pte_pte_ਸਾਰਣੀ_ptr_get_present(pud));
    pd = paddr_ਮੰਜ਼ਿਲ_pptr(pte_pte_ਸਾਰਣੀ_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(pud));
    *(pd + GET_UPT_ਸੂਚਕਾਂਕ(vptr, ULVL_FRM_ARM_PT_LVL(2))) = pte_pte_ਸਾਰਣੀ_new(
                                                              pptr_ਮੰਜ਼ਿਲ_paddr(pt)
                                                          );
}

ਸਥਿਰ BOOT_ਕੋਡ cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pt_cap(vspace_cap, cap);
    ਵਾਪਸ cap;
}

ਸਥਿਰ BOOT_ਕੋਡ ਖਾਲੀ map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    vspace_ਜੜ੍ਹ_t *vspace_ਜੜ੍ਹ_2 = VSPACE_PTR(pptr_ਦਾ_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd = PT_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(pd_cap));
    vptr_t vptr = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(pd_cap);

    ਪ੍ਰਤਿਜ੍ਞਾ(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(pd_cap));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AARCH64_VSPACE_S2_ਸ਼ੁਰੂ_L1
    pud = vspace_ਜੜ੍ਹ_2;
#ਨਹੀਂਤਾਂ
    vspace_ਜੜ੍ਹ_2 += GET_UPT_ਸੂਚਕਾਂਕ(vptr, ULVL_FRM_ARM_PT_LVL(0));
    ਪ੍ਰਤਿਜ੍ਞਾ(pte_pte_ਸਾਰਣੀ_ptr_get_present(vspace_ਜੜ੍ਹ_2));
    pud = paddr_ਮੰਜ਼ਿਲ_pptr(pte_pte_ਸਾਰਣੀ_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(vspace_ਜੜ੍ਹ_2));
#ਜੇ_ਅੰਤ
    *(pud + GET_UPT_ਸੂਚਕਾਂਕ(vptr, ULVL_FRM_ARM_PT_LVL(1))) = pte_pte_ਸਾਰਣੀ_new(
                                                               pptr_ਮੰਜ਼ਿਲ_paddr(pd)
                                                           );
}

ਸਥਿਰ BOOT_ਕੋਡ cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pd_cap(vspace_cap, cap);
    ਵਾਪਸ cap;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ AARCH64_VSPACE_S2_ਸ਼ੁਰੂ_L1
ਸਥਿਰ BOOT_ਕੋਡ ਖਾਲੀ map_it_pud_cap(cap_t vspace_cap, cap_t pud_cap)
{
    pte_t *pgd = PT_PTR(pptr_ਦਾ_cap(vspace_cap));
    pte_t *pud = PT_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(pud_cap));
    vptr_t vptr = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(pud_cap);

    ਪ੍ਰਤਿਜ੍ਞਾ(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(pud_cap));

    *(pgd + GET_UPT_ਸੂਚਕਾਂਕ(vptr, ULVL_FRM_ARM_PT_LVL(0))) = pte_pte_ਸਾਰਣੀ_new(
                                                               pptr_ਮੰਜ਼ਿਲ_paddr(pud));
}

ਸਥਿਰ BOOT_ਕੋਡ cap_t create_it_pud_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_new(
              asid,               /* capPTMappedASID */
              pptr,               /* capPTBasePtr */
              1,                  /* capPTIsMapped */
              vptr                /* capPTMappedAddress */
          );
    map_it_pud_cap(vspace_cap, cap);
    ਵਾਪਸ cap;
}
#ਜੇ_ਅੰਤ /* AARCH64_VSPACE_S2_START_L1 */
BOOT_ਕੋਡ ਸ਼ਬਦ_t arch_get_n_paging(v_region_t it_v_reg)
{
    ਵਾਪਸ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ AARCH64_VSPACE_S2_ਸ਼ੁਰੂ_L1
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_ਬਿੱਟ(ULVL_FRM_ARM_PT_LVL(0))) +
#ਜੇ_ਅੰਤ
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_ਬਿੱਟ(ULVL_FRM_ARM_PT_LVL(1))) +
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_ਬਿੱਟ(ULVL_FRM_ARM_PT_LVL(2)));
}

BOOT_ਕੋਡ cap_t create_it_ਪਤਾ_space(cap_t ਜੜ੍ਹ_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_ਥਾਂ_Pos ਥਾਂ_pos_before;
    se_L4_ਥਾਂ_Pos ਥਾਂ_pos_after;

    /* create the PGD */
    vspace_cap = cap_vspace_cap_new(
                     IT_ASID,           /* capVSMappedASID */
                     rootserver.vspace, /* capVSBasePtr    */
                     1                  /* capVSIsMapped   */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_SMMU
                     , CB_INVALID       /* capVSMappedCB   */
#ਜੇ_ਅੰਤ
                 );
    ਥਾਂ_pos_before = ndks_boot.ਥਾਂ_pos_cur;
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ AARCH64_VSPACE_S2_ਸ਼ੁਰੂ_L1
    /* Create any PUDs needed for the user land image */
    ਲਈ (vptr = ਗੇੜਾ_ਥੱਲੇ(it_v_reg.ਸ਼ੁਰੂ, GET_ULVL_PGSIZE_ਬਿੱਟ(ULVL_FRM_ARM_PT_LVL(0)));
         vptr < it_v_reg.ਸਿਰਾ;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(0))) {
        ਜੇ (!provide_cap(ਜੜ੍ਹ_cnode_cap, create_it_pud_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            ਵਾਪਸ cap_null_cap_new();
        }
    }
#ਜੇ_ਅੰਤ
    /* Create any PDs needed for the user land image */
    ਲਈ (vptr = ਗੇੜਾ_ਥੱਲੇ(it_v_reg.ਸ਼ੁਰੂ, GET_ULVL_PGSIZE_ਬਿੱਟ(ULVL_FRM_ARM_PT_LVL(1)));
         vptr < it_v_reg.ਸਿਰਾ;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))) {
        ਜੇ (!provide_cap(ਜੜ੍ਹ_cnode_cap, create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            ਵਾਪਸ cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    ਲਈ (vptr = ਗੇੜਾ_ਥੱਲੇ(it_v_reg.ਸ਼ੁਰੂ, GET_ULVL_PGSIZE_ਬਿੱਟ(ULVL_FRM_ARM_PT_LVL(2)));
         vptr < it_v_reg.ਸਿਰਾ;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))) {
        ਜੇ (!provide_cap(ਜੜ੍ਹ_cnode_cap, create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            ਵਾਪਸ cap_null_cap_new();
        }
    }

    ਥਾਂ_pos_after = ndks_boot.ਥਾਂ_pos_cur;
    ndks_boot.bi_ਚੌਖਟਾ->userImagePaging = (se_L4_ਥਾਂ_Region) {
        ਥਾਂ_pos_before, ਥਾਂ_pos_after
    };
    ਵਾਪਸ vspace_cap;
}

BOOT_ਕੋਡ cap_t create_unmapped_it_ਚੌਖਟਾ_cap(pptr_t pptr, bool_t use_large)
{
    ਵਾਪਸ create_it_ਚੌਖਟਾ_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_ਕੋਡ cap_t create_mapped_it_ਚੌਖਟਾ_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_ਚੌਖਟਾ_cap(pptr, vptr, asid, use_large);
    map_it_ਚੌਖਟਾ_cap(pd_cap, cap, executable);
    ਵਾਪਸ cap;
}

BOOT_ਕੋਡ ਖਾਲੀ activate_kernel_vspace(ਖਾਲੀ)
{
    cleanInvalidateL1Caches();
    ਸੈੱਟ_Current_Kernel_VSpace_ਜੜ੍ਹ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalKernelPGD)));

    /* Prevent elf-loader address translation to fill up TLB */
    ਸੈੱਟ_Current_User_VSpace_ਜੜ੍ਹ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));

    invalidateLocalTLB();
    ਤਾਲਾ_TLBEntry(KERNEL_ELF_ਅਧਾਰ);
}

BOOT_ਕੋਡ ਖਾਲੀ write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_ਦਾ_cap(it_ap_cap));
    asid_map_t asid_map = asid_map_asid_map_vspace_new(
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_SMMU
                              /* bind_cb: Number of bound context banks */
                              0,
#ਜੇ_ਅੰਤ
                              /* vspace_root: reference to vspace root page table object */
                              (ਸ਼ਬਦ_t)cap_vspace_cap_get_capVSBasePtr(it_vspace_cap)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
                              /* stored_hw_vmid, stored_vmid_valid: Assigned hardware VMID for TLB. */
                              , 0, false
#ਜੇ_ਅੰਤ
                          );
    ap->array[ASID_ਨੀਵਾਂ(IT_ASID)] = asid_map;
    armKSASIDTable[ASID_ਉੱਚਾ(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

asid_map_t ਲੱਭੋ_Map_For_ASID(asid_t asid)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ਉੱਚਾ(asid)];
    ਜੇ (!poolPtr) {
        ਵਾਪਸ asid_map_asid_map_none_new();
    }

    ਵਾਪਸ poolPtr->array[ASID_ਨੀਵਾਂ(asid)];
}

ਸਥਿਰ ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_VSpace_For_ASID(asid_t asid)
{
    ਲੱਭੋ_VSpace_For_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = ਲੱਭੋ_Map_For_ASID(asid);
    ਜੇ (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_ਜੜ੍ਹ_new();

        ret.vspace_ਜੜ੍ਹ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }

    ret.vspace_ਜੜ੍ਹ = (vspace_ਜੜ੍ਹ_t *)asid_map_asid_map_vspace_get_vspace_ਜੜ੍ਹ(asid_map);
    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ਵਾਪਸ ret;
}

ਸ਼ਬਦ_t *ਸ਼ੁੱਧ lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    ਸ਼ਬਦ_t w_ਬਫਰ_Ptr;
    cap_t ਬਫਰ_Cap;
    vm_rights_t vm_rights;

    w_ਬਫਰ_Ptr = thread->tcbIPCBuffer;
    ਬਫਰ_Cap = TCB_PTR_CTE_PTR(thread, tcb_ਬਫਰ)->cap;

    ਜੇ (unlikely(cap_get_capType(ਬਫਰ_Cap) != cap_ਚੌਖਟਾ_cap)) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (unlikely(cap_ਚੌਖਟਾ_cap_get_cap_FIs_Device(ਬਫਰ_Cap))) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    vm_rights = cap_ਚੌਖਟਾ_cap_get_cap_FVMRights(ਬਫਰ_Cap);
    ਜੇ (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        ਸ਼ਬਦ_t ਅਧਾਰ_Ptr;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਪੰਨਾ_ਬਿੱਟ_2;

        ਅਧਾਰ_Ptr = cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(ਬਫਰ_Cap);
        ਪੰਨਾ_ਬਿੱਟ_2 = ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(cap_ਚੌਖਟਾ_cap_get_cap_FSize(ਬਫਰ_Cap));
        ਵਾਪਸ (ਸ਼ਬਦ_t *)(ਅਧਾਰ_Ptr + (w_ਬਫਰ_Ptr & ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_2)));
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਨਿਰਾ;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    ਜੇ (cap_get_capType(cap) != cap_ਚੌਖਟਾ_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (unlikely(cap_ਚੌਖਟਾ_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (!IS_ALIGNED(vptr, se_L4_IPCBuffer_ਮਾਪ_ਬਿੱਟ)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (ਪੂਰਨ)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ lookupPTSlot_ret_t lookupPTSlot(vspace_ਜੜ੍ਹ_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    ਸ਼ਬਦ_t level = UPT_ਪੱਧਰ - 1;
    pte_t *pt = vspace;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_ਬਿੱਟ_ਖੱਬਾ = PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ * level + se_L4_ਪੰਨਾ_ਬਿੱਟ;
    ret.pt_ਥਾਂ = pt + ((vptr >> ret.pt_ਬਿੱਟ_ਖੱਬਾ) & ਨਕਾਬ(se_L4_VSpace_ਸੂਚਕਾਂਕ_ਬਿੱਟ));

    ਜਦੋਂ (pte_pte_ਸਾਰਣੀ_ptr_get_present(ret.pt_ਥਾਂ) && likely(level > 0)) {
        level--;
        ret.pt_ਬਿੱਟ_ਖੱਬਾ -= PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
        pt = paddr_ਮੰਜ਼ਿਲ_pptr(pte_pte_ਸਾਰਣੀ_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(ret.pt_ਥਾਂ));
        ret.pt_ਥਾਂ = pt + ((vptr >> ret.pt_ਬਿੱਟ_ਖੱਬਾ) & ਨਕਾਬ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ));
    }

    ਵਾਪਸ ret;
}

/* Note that if the hypervisor support is enabled, the user page tables use
 * stage-2 translation format. Otherwise, they follow the stage-1 translation format.
 */
ਸਥਿਰ pte_t make_User_ਪੰਨਾ_PTE(paddr_t paddr, vm_rights_t vm_rights, bool_t cap_read, vm_ਗੁਣ_t ਗੁਣ,
                             vm_ਪੰਨਾ_ਮਾਪ_t ਪੰਨਾ_ਮਾਪ)
{
    bool_t nonexecutable = vm_ਗੁਣ_get_arm_Execute_Never(ਗੁਣ) || !cap_read;
    ਸ਼ਬਦ_t cacheable = vm_ਗੁਣ_get_arm_ਪੰਨਾ_Cacheable(ਗੁਣ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਸ਼ਬਦ_t nG = 0; /* not global */
    ਸ਼ਬਦ_t attridx = cacheable ? S2_ਸਾਧਾਰਨ : S2_DEVICE_nGnRnE;
#ਨਹੀਂਤਾਂ
    ਸ਼ਬਦ_t nG = 1; /* not global */
    ਸ਼ਬਦ_t attridx = cacheable ? ਸਾਧਾਰਨ : DEVICE_nGnRnE;
#ਜੇ_ਅੰਤ

    /* Inner-shareable if SMP enabled, otherwise unshared (ignored for devices) */
    ਸ਼ਬਦ_t shareable = cacheable ? SMP_TERNARY(SMP_SHARE, 0) : 0;

    ਜੇ (ਪੰਨਾ_ਮਾਪ == ARMSmall_ਪੰਨਾ) {
        ਵਾਪਸ pte_pte_4k_ਪੰਨਾ_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                   shareable, APFromVMRights(vm_rights), attridx);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ pte_pte_ਪੰਨਾ_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                shareable, APFromVMRights(vm_rights), attridx);
    }
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    ਚੋਣ (vm_faultType) {
    ਹਾਲਤ ARMDataAbort: {
        ਸ਼ਬਦ_t addr, fault;

        addr = getFAR();
        fault = getDFSR();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* use the IPA */
        ਜੇ (ARCH_ਗੰਢ_STATE(armHSVCPUActive)) {
            addr = GET_PAR_ADDR(ਪਤਾ_Translate_S1(addr)) | (addr & ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ));
        }
#ਜੇ_ਅੰਤ
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        ਵਾਪਸ EXCEPTION_FAULT;
    }

    ਹਾਲਤ ARMPrefetchAbort: {
        ਸ਼ਬਦ_t pc, fault;

        pc = getRestartPC(thread);
        fault = getIFSR();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        ਜੇ (ARCH_ਗੰਢ_STATE(armHSVCPUActive)) {
            pc = GET_PAR_ADDR(ਪਤਾ_Translate_S1(pc)) | (pc & ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ));
        }
#ਜੇ_ਅੰਤ
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        ਵਾਪਸ EXCEPTION_FAULT;
    }

    ਮੂਲ:
        fail("Invalid VM fault type");
    }
}

bool_t CONST is_VTable_ਜੜ੍ਹ(cap_t cap)
{
    ਵਾਪਸ cap_get_capType(cap) == cap_vspace_cap;
}

bool_t CONST is_Valid_Native_ਜੜ੍ਹ(cap_t cap)
{
    ਵਾਪਸ is_VTable_ਜੜ੍ਹ(cap) &&
           cap_vspace_cap_get_capVSIsMapped(cap);
}

bool_t CONST is_Valid_VTable_ਜੜ੍ਹ(cap_t cap)
{
    ਵਾਪਸ is_Valid_Native_ਜੜ੍ਹ(cap);
}

ਖਾਲੀ ਸੈੱਟ_VMRoot(tcb_t *tcb)
{
    cap_t thread_ਜੜ੍ਹ;
    asid_t asid;
    vspace_ਜੜ੍ਹ_t *vspace_ਜੜ੍ਹ_2;
    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret;

    thread_ਜੜ੍ਹ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    /* ASID/VMID 0 is never allocated to a real VSpace (see findFreeHWASID()
     * for the hypervisor case), so the empty global VSpace can be installed
     * under it without flushing the TLB. */
    ਜੇ (!is_Valid_Native_ਜੜ੍ਹ(thread_ਜੜ੍ਹ)) {
        ਸੈੱਟ_Current_User_VSpace_ਜੜ੍ਹ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        ਵਾਪਸ;
    }

    vspace_ਜੜ੍ਹ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_ਜੜ੍ਹ));
    asid = cap_vspace_cap_get_capVSMappedASID(thread_ਜੜ੍ਹ);
    ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
    ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE || ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ != vspace_ਜੜ੍ਹ_2)) {
        ਸੈੱਟ_Current_User_VSpace_ਜੜ੍ਹ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        ਵਾਪਸ;
    }

    armv_contextSwitch(vspace_ਜੜ੍ਹ_2, asid);
}

ਸਥਿਰ bool_t ਸੈੱਟ_VMRoot_For_Flush(vspace_ਜੜ੍ਹ_t *vspace, asid_t asid)
{
    cap_t thread_ਜੜ੍ਹ;

    thread_ਜੜ੍ਹ = TCB_PTR_CTE_PTR(ਗੰਢ_STATE(ksCurThread), tcbVTable)->cap;

    ਜੇ (cap_get_capType(thread_ਜੜ੍ਹ) == cap_vspace_cap &&
        cap_vspace_cap_get_capVSIsMapped(thread_ਜੜ੍ਹ) &&
        VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_ਜੜ੍ਹ)) == vspace) {
        ਵਾਪਸ false;
    }

    armv_contextSwitch(vspace, asid);
    ਵਾਪਸ true;
}


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT

ਸਥਿਰ ਸਤਰਅੰਦਰ asid_pool_t *getPoolPtr(asid_t asid)
{
    ਵਾਪਸ armKSASIDTable[ASID_ਉੱਚਾ(asid)];
}

ਸਥਿਰ ਸਤਰਅੰਦਰ asid_map_t getASIDMap(asid_pool_t *poolPtr, asid_t asid)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(poolPtr != ਨਿਰਾ);
    ਵਾਪਸ poolPtr->array[ASID_ਨੀਵਾਂ(asid)];
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ਸੈੱਟ_ASIDMap(asid_pool_t *poolPtr, asid_t asid, asid_map_t asid_map)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(poolPtr != ਨਿਰਾ);
    poolPtr->array[ASID_ਨੀਵਾਂ(asid)] = asid_map;
}

ਸਥਿਰ ਖਾਲੀ invalidateASID(asid_t asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    ਪ੍ਰਤਿਜ੍ਞਾ(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_ਸੈੱਟ_stored_hw_vmid(asid_map, 0);
    asid_map = asid_map_asid_map_vspace_ਸੈੱਟ_stored_vmid_valid(asid_map, false);

    ਸੈੱਟ_ASIDMap(poolPtr, asid, asid_map);
}

ਸਥਿਰ ਖਾਲੀ storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    ਪ੍ਰਤਿਜ੍ਞਾ(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_ਸੈੱਟ_stored_hw_vmid(asid_map, hw_asid);
    asid_map = asid_map_asid_map_vspace_ਸੈੱਟ_stored_vmid_valid(asid_map, true);

    ਸੈੱਟ_ASIDMap(poolPtr, asid, asid_map);
    armKSHWASIDTable[hw_asid] = asid;
}

ਸਥਿਰ hw_asid_t ਲੱਭੋ_Free_HWASID(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t hw_asid_ਵਿੱਥ;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    ਲਈ (hw_asid_ਵਿੱਥ = 0;
         hw_asid_ਵਿੱਥ <= (ਸ਼ਬਦ_t)((hw_asid_t) - 1);
         hw_asid_ਵਿੱਥ++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_ਵਿੱਥ);
        ਜੇ (hw_asid != hwASIDReserved && armKSHWASIDTable[hw_asid] == asidInvalid) {
            ਵਾਪਸ hw_asid;
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
    ਜੇ (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    ਵਾਪਸ hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = ਲੱਭੋ_Map_For_ASID(asid);
    ਜੇ (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        ਵਾਪਸ asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    } ਨਹੀਂਤਾਂ {
        hw_asid_t new_hw_asid;

        new_hw_asid = ਲੱਭੋ_Free_HWASID();
        storeHWASID(asid, new_hw_asid);
        ਵਾਪਸ new_hw_asid;
    }
}

ਸਥਿਰ ਖਾਲੀ invalidateASIDEntry(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = ਲੱਭੋ_Map_For_ASID(asid);
    ਜੇ (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        armKSHWASIDTable[asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_SMMU
ਸਥਿਰ ਸ਼ਬਦ_t getASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ਉੱਚਾ(asid)];
    ਪ੍ਰਤਿਜ੍ਞਾ(asidPool);

    asid_map_t asid_map = asidPool->array[ASID_ਨੀਵਾਂ(asid)];
    ਪ੍ਰਤਿਜ੍ਞਾ(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    ਵਾਪਸ asid_map_asid_map_vspace_get_bind_cb(asid_map);
}

ਖਾਲੀ increaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ਉੱਚਾ(asid)];
    ਪ੍ਰਤਿਜ੍ਞਾ(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_ਨੀਵਾਂ(asid)];
    ਪ੍ਰਤਿਜ੍ਞਾ(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_ਸੈੱਟ_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) + 1);
}

ਖਾਲੀ decreaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ਉੱਚਾ(asid)];
    ਪ੍ਰਤਿਜ੍ਞਾ(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_ਨੀਵਾਂ(asid)];
    ਪ੍ਰਤਿਜ੍ਞਾ(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_ਸੈੱਟ_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) - 1);
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ invalidateTLBByASID(asid_t asid)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_SMMU
    ਸ਼ਬਦ_t bind_cb = getASIDBindCB(asid);
    ਜੇ (unlikely(bind_cb)) {
        invalidateSMMUTLBByASID(asid, bind_cb);
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = ਲੱਭੋ_Map_For_ASID(asid);
    ਜੇ (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        ਵਾਪਸ;
    }
    invalidateTranslationASID(asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map));
#ਨਹੀਂਤਾਂ
    invalidateTranslationASID(asid);
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ invalidateTLBByASIDVA(asid_t asid, vptr_t vaddr)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_SMMU
    ਸ਼ਬਦ_t bind_cb = getASIDBindCB(asid);
    ਜੇ (unlikely(bind_cb)) {
        invalidateSMMUTLBByASIDVA(asid, vaddr, bind_cb);
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = ਲੱਭੋ_Map_For_ASID(asid);
    ਜੇ (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        ਵਾਪਸ;
    }
    uint64_t hw_asid = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    invalidateTranslationSingle((hw_asid << 48) | vaddr >> se_L4_ਪੰਨਾ_ਬਿੱਟ);
#ਨਹੀਂਤਾਂ
    invalidateTranslationSingle((asid << 48) | vaddr >> se_L4_ਪੰਨਾ_ਬਿੱਟ);
#ਜੇ_ਅੰਤ
}


ਖਾਲੀ unmap_ਪੰਨਾ_ਸਾਰਣੀ(asid_t asid, vptr_t vptr, pte_t *ਨਿਸ਼ਾਨਾ_pt)
{
    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
    ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
        /* nothing to do */
        ਵਾਪਸ;
    }
    pte_t *pt_ਥਾਂ = ਨਿਰਾ;
    pte_t *pt = (pte_t *)ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ;

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < UPT_ਪੱਧਰ - 1 && pt != ਨਿਸ਼ਾਨਾ_pt; i++) {
        pt_ਥਾਂ = pt + GET_UPT_ਸੂਚਕਾਂਕ(vptr, i);
        ਜੇ (unlikely(!pte_pte_ਸਾਰਣੀ_ptr_get_present(pt_ਥਾਂ))) {
            /* couldn't find it */
            ਵਾਪਸ;
        }
        pt = paddr_ਮੰਜ਼ਿਲ_pptr(pte_pte_ਸਾਰਣੀ_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(pt_ਥਾਂ));
    }

    ਜੇ (pt != ਨਿਸ਼ਾਨਾ_pt) {
        /* didn't find it */
        ਵਾਪਸ;
    }
    /* If we found a pt then ptSlot won't be null */
    ਪ੍ਰਤਿਜ੍ਞਾ(pt_ਥਾਂ != ਨਿਰਾ);
    *pt_ਥਾਂ = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)pt_ਥਾਂ, pptr_ਮੰਜ਼ਿਲ_paddr(pt_ਥਾਂ));
    invalidateTLBByASID(asid);
}

ਖਾਲੀ unmap_ਪੰਨਾ(vm_ਪੰਨਾ_ਮਾਪ_t ਪੰਨਾ_ਮਾਪ, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret;
    lookupPTSlot_ret_t  lu_ret;
    pte_t pte;

    ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
    ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ;
    }

    lu_ret = lookupPTSlot(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vptr);
    ਜੇ (unlikely(lu_ret.pt_ਬਿੱਟ_ਖੱਬਾ != ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ਪੰਨਾ_ਮਾਪ))) {
        /* Do nothing if the wrong size object was returned */
        ਵਾਪਸ;
    }

    pte = *(lu_ret.pt_ਥਾਂ);
    ਜੇ (!pte_is_ਪੰਨਾ_type(pte)) {
        /* Do nothing if no page is present */
        ਵਾਪਸ;
    }

    ਜੇ (pte_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(pte) != pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)pptr)) {
        /* Do nothing if the mapped page is not the same physical frame */
        ਵਾਪਸ;
    }

    *(lu_ret.pt_ਥਾਂ) = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)lu_ret.pt_ਥਾਂ, pptr_ਮੰਜ਼ਿਲ_paddr(lu_ret.pt_ਥਾਂ));
    ਪ੍ਰਤਿਜ੍ਞਾ(asid < ਦੋਆਧਾਰੀ_ਅੰਕ(16));
    invalidateTLBByASIDVA(asid, vptr);
}

ਖਾਲੀ deleteASID(asid_t asid, vspace_ਜੜ੍ਹ_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ਉੱਚਾ(asid)];

    ਜੇ (poolPtr != ਨਿਰਾ) {
        asid_map_t asid_map = poolPtr->array[ASID_ਨੀਵਾਂ(asid)];
        ਜੇ (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_ਜੜ੍ਹ_t *)asid_map_asid_map_vspace_get_vspace_ਜੜ੍ਹ(asid_map) == vspace) {
            invalidateTLBByASID(asid);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
            invalidateASIDEntry(asid);
#ਜੇ_ਅੰਤ
            poolPtr->array[ASID_ਨੀਵਾਂ(asid)] = asid_map_asid_map_none_new();
            ਸੈੱਟ_VMRoot(ਗੰਢ_STATE(ksCurThread));
        }
    }
}

ਖਾਲੀ deleteASIDPool(asid_t asid_ਅਧਾਰ, asid_pool_t *pool)
{
    ਸ਼ਬਦ_t ਵਿੱਥ;

    ਪ੍ਰਤਿਜ੍ਞਾ((asid_ਅਧਾਰ & ਨਕਾਬ(asid_ਨੀਵਾਂ_ਬਿੱਟ)) == 0);

    ਜੇ (armKSASIDTable[ASID_ਉੱਚਾ(asid_ਅਧਾਰ)] == pool) {
        ਲਈ (ਵਿੱਥ = 0; ਵਿੱਥ < ਦੋਆਧਾਰੀ_ਅੰਕ(asid_ਨੀਵਾਂ_ਬਿੱਟ); ਵਿੱਥ++) {
            asid_map_t asid_map = pool->array[ਵਿੱਥ];
            ਜੇ (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                invalidateTLBByASID(asid_ਅਧਾਰ + ਵਿੱਥ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
                invalidateASIDEntry(asid_ਅਧਾਰ + ਵਿੱਥ);
#ਜੇ_ਅੰਤ
            }
        }
        armKSASIDTable[ASID_ਉੱਚਾ(asid_ਅਧਾਰ)] = ਨਿਰਾ;
        ਸੈੱਟ_VMRoot(ਗੰਢ_STATE(ksCurThread));
    }
}

ਸਥਿਰ ਖਾਲੀ doFlush(ਸ਼ਬਦ_t invLabel, vptr_t ਸ਼ੁਰੂ, vptr_t ਸਿਰਾ, paddr_t pstart)
{
    ਚੋਣ (invLabel) {
    ਹਾਲਤ ARMVSpace_Clean_ਅੰਕੜੇ:
    ਹਾਲਤ ARMPage_Clean_ਅੰਕੜੇ:
        cleanCacheRange_RAM(ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);
        ਰੋਕੋ;

    ਹਾਲਤ ARMVSpace_Invalidate_ਅੰਕੜੇ:
    ਹਾਲਤ ARMPage_Invalidate_ਅੰਕੜੇ:
        invalidateCacheRange_RAM(ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);
        ਰੋਕੋ;

    ਹਾਲਤ ARMVSpace_Clean_Invalidate_ਅੰਕੜੇ:
    ਹਾਲਤ ARMPage_Clean_Invalidate_ਅੰਕੜੇ:
        cleanInvalidateCacheRange_RAM(ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);
        ਰੋਕੋ;

    ਹਾਲਤ ARMVSpaceUnify_Instruction:
    ਹਾਲਤ ARMPageUnify_Instruction:
        /* First clean data lines to point of unification... */
        cleanCacheRange_PoU(ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);
        /* ... then wait for the completion of invalidations... */
        dsb();
        /* ... and ensure new instructions come from fresh cache lines. */
        isb();
        ਰੋਕੋ;
    ਮੂਲ:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

ਸਥਿਰ exception_t performVSpaceFlush(ਸ਼ਬਦ_t invLabel, vspace_ਜੜ੍ਹ_t *vspace_ਜੜ੍ਹ_2, asid_t asid,
                                      vptr_t ਸ਼ੁਰੂ, vptr_t ਸਿਰਾ, paddr_t pstart)
{

    ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        ਸ਼ਬਦ_t ਮਾਪ = ਸਿਰਾ - ਸ਼ੁਰੂ;
        ਸ਼ੁਰੂ = (vptr_t)paddr_ਮੰਜ਼ਿਲ_pptr(pstart);
        ਸਿਰਾ = ਸ਼ੁਰੂ + ਮਾਪ;
        ਜੇ (ਸ਼ੁਰੂ < ਸਿਰਾ) {
            doFlush(invLabel, ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);
        }
    } ਨਹੀਂਤਾਂ {
        bool_t ਜੜ੍ਹ_switched;

        /* Flush if given a non zero range */
        ਜੇ (ਸ਼ੁਰੂ < ਸਿਰਾ) {
            ਜੜ੍ਹ_switched = ਸੈੱਟ_VMRoot_For_Flush(vspace_ਜੜ੍ਹ_2, asid);
            doFlush(invLabel, ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);
            ਜੇ (ਜੜ੍ਹ_switched) {
                ਸੈੱਟ_VMRoot(ਗੰਢ_STATE(ksCurThread));
            }
        }
    }
    ਵਾਪਸ EXCEPTION_NONE;
}


ਸਥਿਰ exception_t perform_ਪੰਨਾ_ਸਾਰਣੀ_Invocation_Map(cap_t cap, cte_t *ct_ਥਾਂ, pte_t pte, pte_t *pt_ਥਾਂ)
{
    ct_ਥਾਂ->cap = cap;
    *pt_ਥਾਂ = pte;
    cleanByVA_PoU((vptr_t)pt_ਥਾਂ, pptr_ਮੰਜ਼ਿਲ_paddr(pt_ਥਾਂ));

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_ਪੰਨਾ_ਸਾਰਣੀ_Invocation_Unmap(cap_t cap, cte_t *ct_ਥਾਂ)
{
    ਜੇ (cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PT_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap));
        unmap_ਪੰਨਾ_ਸਾਰਣੀ(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(cap),
                       cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(cap), pt);
        clear_ਸਮ੍ਰਿਤੀ_PT((ਖਾਲੀ *)pt, cap_get_cap_ਮਾਪ_ਬਿੱਟ(cap));
    }

    cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ptr_ਸੈੱਟ_cap_PTIs_Mapped(&(ct_ਥਾਂ->cap), 0);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_ਪੰਨਾ_Invocation_Map(asid_t asid, cap_t cap, cte_t *ct_ਥਾਂ,
                                            pte_t pte, pte_t *pt_ਥਾਂ)
{
    bool_t tlbflush_required = pte_ptr_get_valid(pt_ਥਾਂ);

    ct_ਥਾਂ->cap = cap;
    *pt_ਥਾਂ = pte;

    cleanByVA_PoU((vptr_t)pt_ਥਾਂ, pptr_ਮੰਜ਼ਿਲ_paddr(pt_ਥਾਂ));
    ਜੇ (unlikely(tlbflush_required)) {
        ਪ੍ਰਤਿਜ੍ਞਾ(asid < ਦੋਆਧਾਰੀ_ਅੰਕ(16));
        invalidateTLBByASIDVA(asid, cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap));
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_ਪੰਨਾ_Invocation_Unmap(cap_t cap, cte_t *ct_ਥਾਂ)
{
    ਜੇ (cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap) != 0) {

        unmap_ਪੰਨਾ(cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap),
                  cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap),
                  cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap),
                  cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap));
    }

    cap_t ਥਾਂ_Cap = ct_ਥਾਂ->cap;
    ਥਾਂ_Cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ਪਤਾ(ਥਾਂ_Cap, 0);
    ਥਾਂ_Cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ASID(ਥਾਂ_Cap, asidInvalid);
    ct_ਥਾਂ->cap = ਥਾਂ_Cap;


    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_ਪੰਨਾ_Flush(ਸ਼ਬਦ_t invLabel, vspace_ਜੜ੍ਹ_t *vspace_ਜੜ੍ਹ_2, asid_t asid,
                                    vptr_t ਸ਼ੁਰੂ, vptr_t ਸਿਰਾ, paddr_t pstart)
{
    ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* We flush the cache with kernel virtual addresses since
         * the user virtual addresses are not valid in EL2.
         * Switching VMRoot is not required.
         */
        ਸ਼ਬਦ_t ਮਾਪ = ਸਿਰਾ - ਸ਼ੁਰੂ;
        ਸ਼ੁਰੂ = (vptr_t)paddr_ਮੰਜ਼ਿਲ_pptr(pstart);
        ਸਿਰਾ = ਸ਼ੁਰੂ + ਮਾਪ;

        ਜੇ (ਸ਼ੁਰੂ < ਸਿਰਾ) {
            doFlush(invLabel, ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);
        }
    } ਨਹੀਂਤਾਂ {
        bool_t ਜੜ੍ਹ_switched;

        ਜੇ (ਸ਼ੁਰੂ < ਸਿਰਾ) {
            ਜੜ੍ਹ_switched = ਸੈੱਟ_VMRoot_For_Flush(vspace_ਜੜ੍ਹ_2, asid);
            doFlush(invLabel, ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);
            ਜੇ (ਜੜ੍ਹ_switched) {
                ਸੈੱਟ_VMRoot(ਗੰਢ_STATE(ksCurThread));
            }
        }
    }
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_ਪੰਨਾ_Get_ਪਤਾ(pptr_t ਅਧਾਰ_ptr, bool_t call)
{
    paddr_t ਅਧਾਰ = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)ਅਧਾਰ_ptr);

    tcb_t *thread;
    thread = ਗੰਢ_STATE(ksCurThread);
    ਜੇ (call) {
        ਸ਼ਬਦ_t *ipc_ਬਫਰ = lookupIPCBuffer(true, thread);
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, badge_ਰਜਿਸਟਰ, 0);
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ = ਸੈੱਟ_MR(thread, ipc_ਬਫਰ, 0, ਅਧਾਰ);
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(
                        se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, ਲੰਮਾਈ)));
    }
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Running);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t performASIDControlInvocation(ਖਾਲੀ *ਚੌਖਟਾ, cte_t *ਥਾਂ,
                                                cte_t *ਮਾਪਾ, asid_t asid_ਅਧਾਰ)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_ਸੈੱਟ_cap_Free_ਸੂਚਕਾਂਕ(&(ਮਾਪਾ->cap),
                                         MAX_FREE_ਸੂਚਕਾਂਕ(cap_untyped_cap_get_cap_ਖੰਡ_ਮਾਪ(ਮਾਪਾ->cap)));

    memzero(ਚੌਖਟਾ, ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ASIDPool_ਬਿੱਟ));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ਪਾਓ(
        cap_asid_pool_cap_new(
            asid_ਅਧਾਰ,         /* capASIDBase  */
            ਸ਼ਬਦ_REF(ਚੌਖਟਾ)    /* capASIDPool  */
        ), ਮਾਪਾ, ਥਾਂ);

    ਪ੍ਰਤਿਜ੍ਞਾ((asid_ਅਧਾਰ & ਨਕਾਬ(asid_ਨੀਵਾਂ_ਬਿੱਟ)) == 0);
    armKSASIDTable[ASID_ਉੱਚਾ(asid_ਅਧਾਰ)] = (asid_pool_t *)ਚੌਖਟਾ;

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decode_ARMVSpace_ਜੜ੍ਹ_Invocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ,
                                                 cte_t *cte, cap_t cap, ਸ਼ਬਦ_t *ਬਫਰ)
{
    vptr_t ਸ਼ੁਰੂ, ਸਿਰਾ;
    paddr_t pstart;
    asid_t asid;
    vspace_ਜੜ੍ਹ_t *vspace_ਜੜ੍ਹ_2;
    lookupPTSlot_ret_t resolve_ret;
    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret;
    pte_t pte;

    ਚੋਣ (invLabel) {
    ਹਾਲਤ ARMVSpace_Clean_ਅੰਕੜੇ:
    ਹਾਲਤ ARMVSpace_Invalidate_ਅੰਕੜੇ:
    ਹਾਲਤ ARMVSpace_Clean_Invalidate_ਅੰਕੜੇ:
    ਹਾਲਤ ARMVSpaceUnify_Instruction:

        ਜੇ (ਲੰਮਾਈ < 2) {
            userError("VSpaceRoot Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸ਼ੁਰੂ = getSyscallArg(0, ਬਫਰ);
        ਸਿਰਾ =   getSyscallArg(1, ਬਫਰ);

        /* Check sanity of arguments */
        ਜੇ (ਸਿਰਾ <= ਸ਼ੁਰੂ) {
            userError("VSpaceRoot Flush: Invalid range.");
            current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
            current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 1;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        ਜੇ (ਸਿਰਾ > USER_ਸਿਖਰ) {
            userError("VSpaceRoot Flush: Exceed the user addressable region.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (unlikely(!is_Valid_Native_ਜੜ੍ਹ(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        /* Make sure that the supplied pgd is ok */
        vspace_ਜੜ੍ਹ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap));
        asid = cap_vspace_cap_get_capVSMappedASID(cap);

        ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
        ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
            userError("VSpaceRoot Flush: No VSpace for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (unlikely(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ != vspace_ਜੜ੍ਹ_2)) {
            userError("VSpaceRoot Flush: Invalid VSpace Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = lookupPTSlot(vspace_ਜੜ੍ਹ_2, ਸ਼ੁਰੂ);
        pte = *resolve_ret.pt_ਥਾਂ;

        /* Check that the returned slot is a page. */
        ਜੇ (!pte_is_ਪੰਨਾ_type(pte)) {

            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
            ਵਾਪਸ EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        ਜੇ (ਗੇੜਾ_ਥੱਲੇ(ਸ਼ੁਰੂ, resolve_ret.pt_ਬਿੱਟ_ਖੱਬਾ) != ਗੇੜਾ_ਥੱਲੇ(ਸਿਰਾ - 1, resolve_ret.pt_ਬਿੱਟ_ਖੱਬਾ)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = ਸ਼ੁਰੂ;
            current_syscall_error.rangeErrorMax = ਗੇੜਾ_ਥੱਲੇ(ਸ਼ੁਰੂ, resolve_ret.pt_ਬਿੱਟ_ਖੱਬਾ) +
                                                  ਨਕਾਬ(resolve_ret.pt_ਬਿੱਟ_ਖੱਬਾ);
            userError("VSpaceRoot Flush: cannot cross page boundary, valid range is [0x%lx..0x%lx)",
                      current_syscall_error.rangeErrorMin, current_syscall_error.rangeErrorMax);
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When in EL1, the mapping must be write-able for ARMVSpaceInvalidate_Data */
        ਜੇ (invLabel == ARMVSpace_Invalidate_ਅੰਕੜੇ && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMVSpaceInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
#ਜੇ_ਅੰਤ

        /* Calculate the physical start address. */
        paddr_t ਚੌਖਟਾ_ਅਧਾਰ_2 = pte_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(pte);

        pstart = ਚੌਖਟਾ_ਅਧਾਰ_2 + (ਸ਼ੁਰੂ & ਨਕਾਬ(resolve_ret.pt_ਬਿੱਟ_ਖੱਬਾ));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        ਜੇ (pstart < PADDR_ਅਧਾਰ || ((ਸਿਰਾ - ਸ਼ੁਰੂ) + pstart) > PADDR_ਸਿਖਰ) {
            userError("VSpaceRoot Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
#ਜੇ_ਅੰਤ

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performVSpaceFlush(invLabel, vspace_ਜੜ੍ਹ_2, asid, ਸ਼ੁਰੂ, ਸਿਰਾ - 1, pstart);

    ਮੂਲ:
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}


ਸਥਿਰ exception_t decode_ARMPage_ਸਾਰਣੀ_Invocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ,
                                                cte_t *cte, cap_t cap, ਸ਼ਬਦ_t *ਬਫਰ)
{
    cap_t vspace_ਜੜ੍ਹ_Cap;
    vspace_ਜੜ੍ਹ_t *vspace_ਜੜ੍ਹ_2;
    pte_t pte;
    asid_t asid;
    vptr_t vaddr;
    lookupPTSlot_ret_t pt_ਥਾਂ;
    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret;

    ਜੇ (invLabel == ARMPage_ਸਾਰਣੀ_Unmap) {
        ਜੇ (unlikely(!isFinalCapability(cte))) {
            current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_ਪੰਨਾ_ਸਾਰਣੀ_Invocation_Unmap(cap, cte);
    }

    ਜੇ (unlikely(invLabel != ARMPage_ਸਾਰਣੀ_Map)) {
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (unlikely(ਲੰਮਾਈ < 2 || current_extra_caps.excaprefs[0] == ਨਿਰਾ)) {
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (unlikely(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, ਬਫਰ);
    vspace_ਜੜ੍ਹ_Cap = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (unlikely(!is_Valid_Native_ਜੜ੍ਹ(vspace_ਜੜ੍ਹ_Cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    vspace_ਜੜ੍ਹ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_ਜੜ੍ਹ_Cap));
    asid = cap_vspace_cap_get_capVSMappedASID(vspace_ਜੜ੍ਹ_Cap);

    ਜੇ (unlikely(vaddr > USER_ਸਿਖਰ)) {
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
    ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (unlikely(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ != vspace_ਜੜ੍ਹ_2)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    pt_ਥਾਂ = lookupPTSlot(vspace_ਜੜ੍ਹ_2, vaddr);

    ਜੇ (unlikely(pt_ਥਾਂ.pt_ਬਿੱਟ_ਖੱਬਾ == se_L4_ਪੰਨਾ_ਬਿੱਟ || pte_ptr_get_valid(pt_ਥਾਂ.pt_ਥਾਂ))) {
        current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    pte = pte_pte_ਸਾਰਣੀ_new(pptr_ਮੰਜ਼ਿਲ_paddr(PTE_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap))));

    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_PTIs_Mapped(cap, 1);
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_PTMapped_ਪਤਾ(cap, (vaddr & ~ਨਕਾਬ(pt_ਥਾਂ.pt_ਬਿੱਟ_ਖੱਬਾ)));

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ perform_ਪੰਨਾ_ਸਾਰਣੀ_Invocation_Map(cap, cte, pte, pt_ਥਾਂ.pt_ਥਾਂ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool_t CONST checkVPAlignment(vm_ਪੰਨਾ_ਮਾਪ_t sz, ਸ਼ਬਦ_t w)
{
    ਵਾਪਸ (w & ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(sz))) == 0;
}

ਸਥਿਰ exception_t decodeARMFrameInvocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ,
                                            cte_t *cte, cap_t cap, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਚੋਣ (invLabel) {
    ਹਾਲਤ ARMPageMap: {
        vptr_t vaddr;
        paddr_t ਅਧਾਰ;
        cap_t vspace_ਜੜ੍ਹ_Cap;
        vspace_ਜੜ੍ਹ_t *vspace_ਜੜ੍ਹ_2;
        asid_t asid, ਚੌਖਟਾ_asid;
        vm_rights_t vmRights, capRights;
        vm_ਪੰਨਾ_ਮਾਪ_t ਚੌਖਟਾ_ਮਾਪ;
        vm_ਗੁਣ_t ਗੁਣ;
        ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret;

        ਜੇ (unlikely(ਲੰਮਾਈ < 3 || current_extra_caps.excaprefs[0] == ਨਿਰਾ)) {
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, ਬਫਰ);
        ਗੁਣ = vm_ਗੁਣ_From_ਸ਼ਬਦ(getSyscallArg(2, ਬਫਰ));
        vspace_ਜੜ੍ਹ_Cap = current_extra_caps.excaprefs[0]->cap;

        ਚੌਖਟਾ_ਮਾਪ = cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap);
        capRights = cap_ਚੌਖਟਾ_cap_get_cap_FVMRights(cap);
        vmRights = ਨਕਾਬ_VMRights(capRights, rights_From_ਸ਼ਬਦ(getSyscallArg(1, ਬਫਰ)));

        ਜੇ (unlikely(!is_Valid_Native_ਜੜ੍ਹ(vspace_ਜੜ੍ਹ_Cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        vspace_ਜੜ੍ਹ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_ਜੜ੍ਹ_Cap));
        asid = cap_vspace_cap_get_capVSMappedASID(vspace_ਜੜ੍ਹ_Cap);

        ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
        ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (unlikely(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ != vspace_ਜੜ੍ਹ_2)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (unlikely(!checkVPAlignment(ਚੌਖਟਾ_ਮਾਪ, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        /* In the case of remap, the cap should have a valid asid */
        ਚੌਖਟਾ_asid = cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap);

        ਜੇ (ਚੌਖਟਾ_asid != asidInvalid) {
            ਜੇ (ਚੌਖਟਾ_asid != asid) {
                userError("ARMPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;

            } ਨਹੀਂਤਾਂ ਜੇ (cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
                current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;
                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }
        } ਨਹੀਂਤਾਂ {
            ਜੇ (unlikely(vaddr + ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ਚੌਖਟਾ_ਮਾਪ)) - 1 > USER_ਸਿਖਰ)) {
                current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
                current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;
                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }
        }

        cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ASID(cap, asid);
        cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ਪਤਾ(cap, vaddr);

        ਅਧਾਰ = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap));

        lookupPTSlot_ret_t lu_ret = lookupPTSlot(vspace_ਜੜ੍ਹ_2, vaddr);
        ਜੇ (unlikely(lu_ret.pt_ਬਿੱਟ_ਖੱਬਾ != ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ਚੌਖਟਾ_ਮਾਪ))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_ਬਿੱਟ_ਖੱਬਾ);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        bool_t cap_read = (capRights == VMReadOnly || capRights == VMReadWrite);
        pte_t pte = make_User_ਪੰਨਾ_PTE(ਅਧਾਰ, vmRights, cap_read, ਗੁਣ, ਚੌਖਟਾ_ਮਾਪ);

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_ਪੰਨਾ_Invocation_Map(asid, cap, cte, pte, lu_ret.pt_ਥਾਂ);
    }

    ਹਾਲਤ ARMPageUnmap:
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_ਪੰਨਾ_Invocation_Unmap(cap, cte);

    ਹਾਲਤ ARMPage_Clean_ਅੰਕੜੇ:
    ਹਾਲਤ ARMPage_Invalidate_ਅੰਕੜੇ:
    ਹਾਲਤ ARMPage_Clean_Invalidate_ਅੰਕੜੇ:
    ਹਾਲਤ ARMPageUnify_Instruction: {
        vptr_t ਸ਼ੁਰੂ, ਸਿਰਾ;
        vptr_t vaddr;
        asid_t asid;
        ਸ਼ਬਦ_t ਪੰਨਾ_ਮਾਪ;
        ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret;

        ਜੇ (ਲੰਮਾਈ < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (unlikely(cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap) == 0)) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap);
        asid = cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap);

        ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
        ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
            userError("Page Flush: No PGD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸ਼ੁਰੂ = getSyscallArg(0, ਬਫਰ);
        ਸਿਰਾ =   getSyscallArg(1, ਬਫਰ);

        /* check that the range is sane */
        ਜੇ (ਸਿਰਾ <= ਸ਼ੁਰੂ) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
            current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 1;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        /* start and end are currently relative inside this page */
        ਪੰਨਾ_ਮਾਪ = ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap)));
        ਜੇ (ਸ਼ੁਰੂ >= ਪੰਨਾ_ਮਾਪ || ਸਿਰਾ > ਪੰਨਾ_ਮਾਪ) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
            current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸ਼ਬਦ_t pstart = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap)) + ਸ਼ੁਰੂ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        ਜੇ (pstart < PADDR_ਅਧਾਰ || ((ਸਿਰਾ - ਸ਼ੁਰੂ) + pstart) > PADDR_ਸਿਖਰ) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
#ਨਹੀਂਤਾਂ
        /* When in EL1, we are using the user page table for flushing and need to make sure
           the mapping info in the cap is not stale. */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vaddr);
        pte_t pte = *lu_ret.pt_ਥਾਂ;
        ਖਾਲੀ *ਅਧਾਰ_ptr = (ਖਾਲੀ *) cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap);
        ਜੇ (unlikely(lu_ret.pt_ਬਿੱਟ_ਖੱਬਾ != ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap)) ||
                     !pte_is_ਪੰਨਾ_type(pte) ||
                     pte_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(pte) != pptr_ਮੰਜ਼ਿਲ_paddr(ਅਧਾਰ_ptr))) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        /* When in EL1, the mapping must be writeable for DC IVAC */
        ਜੇ (invLabel == ARMPage_Invalidate_ਅੰਕੜੇ && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMPageInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
#ਜੇ_ਅੰਤ
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_ਪੰਨਾ_Flush(invLabel, ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, asid, vaddr + ਸ਼ੁਰੂ, vaddr + ਸਿਰਾ - 1,
                                pstart);
    }

    ਹਾਲਤ ARMPage_Get_ਪਤਾ:
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_ਪੰਨਾ_Get_ਪਤਾ(cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap), call);

    ਮੂਲ:
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_vspace_cap:
        ਵਾਪਸ decode_ARMVSpace_ਜੜ੍ਹ_Invocation(invLabel, ਲੰਮਾਈ, cte, cap, ਬਫਰ);
    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਵਾਪਸ decode_ARMPage_ਸਾਰਣੀ_Invocation(invLabel, ਲੰਮਾਈ, cte, cap, ਬਫਰ);

    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        ਵਾਪਸ decodeARMFrameInvocation(invLabel, ਲੰਮਾਈ, cte, cap, call, ਬਫਰ);

    ਹਾਲਤ cap_asid_control_cap: {
        ਸ਼ਬਦ_t i;
        asid_t asid_ਅਧਾਰ;
        ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ;
        cap_t untyped, ਜੜ੍ਹ;
        cte_t *ਮਾਪਾ_ਥਾਂ, *dest_ਥਾਂ;
        lookup_ਥਾਂ_ret_t lu_ret;
        ਖਾਲੀ *ਚੌਖਟਾ;
        exception_t ਸਥਿਤੀ;

        ਜੇ (unlikely(invLabel != ARMASIDControlMakePool)) {
            current_syscall_error.type = seL4_IllegalOperation;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (unlikely(ਲੰਮਾਈ < 2 ||
                     current_extra_caps.excaprefs[0] == ਨਿਰਾ ||
                     current_extra_caps.excaprefs[1] == ਨਿਰਾ)) {
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸੂਚਕਾਂਕ = getSyscallArg(0, ਬਫਰ);
        ਡੂੰਘਾਈ = getSyscallArg(1, ਬਫਰ);
        ਮਾਪਾ_ਥਾਂ = current_extra_caps.excaprefs[0];
        untyped = ਮਾਪਾ_ਥਾਂ->cap;
        ਜੜ੍ਹ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        ਲਈ (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        ਜੇ (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        asid_ਅਧਾਰ = i << asid_ਨੀਵਾਂ_ਬਿੱਟ;

        ਜੇ (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_ਖੰਡ_ਮਾਪ(untyped) != se_L4_ASIDPool_ਬਿੱਟ ||
                     cap_untyped_cap_get_capIsDevice(untyped))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸਥਿਤੀ = ensureNoChildren(ਮਾਪਾ_ਥਾਂ);
        ਜੇ (unlikely(ਸਥਿਤੀ != EXCEPTION_NONE)) {
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਚੌਖਟਾ = ਸ਼ਬਦ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_ਨਿਸ਼ਾਨਾ_ਥਾਂ(ਜੜ੍ਹ, ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ);
        ਜੇ (unlikely(lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
            ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
        }
        dest_ਥਾਂ = lu_ret.ਥਾਂ;

        ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(dest_ਥਾਂ);
        ਜੇ (unlikely(ਸਥਿਤੀ != EXCEPTION_NONE)) {
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performASIDControlInvocation(ਚੌਖਟਾ, dest_ਥਾਂ, ਮਾਪਾ_ਥਾਂ, asid_ਅਧਾਰ);
    }

    ਹਾਲਤ cap_asid_pool_cap: {
        cap_t vspaceCap;
        cte_t *vspace_Cap_ਥਾਂ;
        asid_pool_t *pool;
        ਸ਼ਬਦ_t i;
        asid_t asid;

        ਜੇ (unlikely(invLabel != ARMASIDPool_ਸੌਂਪੋ)) {
            current_syscall_error.type = seL4_IllegalOperation;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (unlikely(current_extra_caps.excaprefs[0] == ਨਿਰਾ)) {
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_ਥਾਂ = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_ਥਾਂ->cap;

        ਜੇ (unlikely(!is_VTable_ਜੜ੍ਹ(vspaceCap) || cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_ਉੱਚਾ(cap_asid_pool_cap_get_capASIDBase(cap))];

        ਜੇ (unlikely(!pool)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
            current_lookup_fault = lookup_fault_invalid_ਜੜ੍ਹ_new();

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        ਲਈ (i = 0; i < (1 << asid_ਨੀਵਾਂ_ਬਿੱਟ) && (asid + i == 0
                                               || (asid_map_get_type(pool->array[i]) != asid_map_asid_map_none)); i++);

        ਜੇ (unlikely(i == 1 << asid_ਨੀਵਾਂ_ਬਿੱਟ)) {
            current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performASIDPoolInvocation(asid, pool, vspace_Cap_ਥਾਂ);
    }

    ਮੂਲ:
        fail("Invalid ARM arch cap type");
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
ਖਾਲੀ kernelPrefetchAbort(ਸ਼ਬਦ_t pc) VISIBLE;
ਖਾਲੀ kernel_ਅੰਕੜੇ_Abort(ਸ਼ਬਦ_t pc) VISIBLE;

ਖਾਲੀ kernelPrefetchAbort(ਸ਼ਬਦ_t pc)
{
    ਲਿਖੋ("\n\nKERNEL PREFETCH ABORT!\n");
    ਲਿਖੋ("Faulting instruction: 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n", pc);
    ਲਿਖੋ("ESR (IFSR): 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n", getIFSR());
    halt();
}

ਖਾਲੀ kernel_ਅੰਕੜੇ_Abort(ਸ਼ਬਦ_t pc)
{
    ਲਿਖੋ("\n\nKERNEL DATA ABORT!\n");
    ਲਿਖੋ("Faulting instruction: 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n", pc);
    ਲਿਖੋ("FAR: 0x%"SEL4_PRIx_ਸ਼ਬਦ" ESR (DFSR): 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n",
           getFAR(), getDFSR());
    halt();
}
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ read_ਸ਼ਬਦ_From_VSpace_ret {
    exception_t ਸਥਿਤੀ;
    ਸ਼ਬਦ_t ਮੁੱਲ;
} read_ਸ਼ਬਦ_From_VSpace_ret_t;

ਸਥਿਰ read_ਸ਼ਬਦ_From_VSpace_ret_t read_ਸ਼ਬਦ_From_VSpace(vspace_ਜੜ੍ਹ_t *pd, ਸ਼ਬਦ_t vaddr)
{
    read_ਸ਼ਬਦ_From_VSpace_ret_t ret;
    ਸ਼ਬਦ_t ਵਿੱਥ;
    pptr_t kernel_vaddr;
    ਸ਼ਬਦ_t *ਮੁੱਲ;

    lookupPTSlot_ret_t lookup_ret = lookupPTSlot(pd, vaddr);

    /* Check that the returned slot is a page. */
    ਜੇ (!pte_ptr_get_valid(lookup_ret.pt_ਥਾਂ) ||
        (pte_pte_ਸਾਰਣੀ_ptr_get_present(lookup_ret.pt_ਥਾਂ) && lookup_ret.pt_ਬਿੱਟ_ਖੱਬਾ > ਪੰਨਾ_ਬਿੱਟ)) {
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }

    ਵਿੱਥ = vaddr & ਨਕਾਬ(lookup_ret.pt_ਬਿੱਟ_ਖੱਬਾ);
    kernel_vaddr = (ਸ਼ਬਦ_t)paddr_ਮੰਜ਼ਿਲ_pptr(pte_ਪੰਨਾ_ptr_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(lookup_ret.pt_ਥਾਂ));
    ਮੁੱਲ = (ਸ਼ਬਦ_t *)(kernel_vaddr + ਵਿੱਥ);

    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ret.ਮੁੱਲ = *ਮੁੱਲ;
    ਵਾਪਸ ret;
}

ਖਾਲੀ Arch_user_ਢੇਰ_Trace(tcb_t *tptr)
{
    cap_t thread_ਜੜ੍ਹ;
    vspace_ਜੜ੍ਹ_t *vspace_ਜੜ੍ਹ_2;
    ਸ਼ਬਦ_t sp;

    thread_ਜੜ੍ਹ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the vspace root */
    ਜੇ (cap_get_capType(thread_ਜੜ੍ਹ) != cap_vspace_cap) {
        ਲਿਖੋ("Invalid vspace\n");
        ਵਾਪਸ;
    }

    vspace_ਜੜ੍ਹ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_ਜੜ੍ਹ));
    sp = get_ਰਜਿਸਟਰ(tptr, SP_EL0);

    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    ਜੇ (!IS_ALIGNED(sp, se_L4_ਸ਼ਬਦ_ਮਾਪ_ਬਿੱਟ)) {
        ਲਿਖੋ("SP not aligned\n");
        ਵਾਪਸ;
    }

    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i = 0; i < CONFIG_USER_ਢੇਰ_TRACE_ਲੰਮਾਈ; i++) {
        ਸ਼ਬਦ_t ਪਤਾ = sp + (i * ਆਕਾਰ(ਸ਼ਬਦ_t));
        read_ਸ਼ਬਦ_From_VSpace_ret_t ਨਤੀਜਾ = read_ਸ਼ਬਦ_From_VSpace(vspace_ਜੜ੍ਹ_2,
                                                             ਪਤਾ);
        ਜੇ (ਨਤੀਜਾ.ਸਥਿਤੀ == EXCEPTION_NONE) {
            ਲਿਖੋ("0x%"SEL4_PRIx_ਸ਼ਬਦ": 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n",
                   ਪਤਾ, ਨਤੀਜਾ.ਮੁੱਲ);
        } ਨਹੀਂਤਾਂ {
            ਲਿਖੋ("0x%"SEL4_PRIx_ਸ਼ਬਦ": INVALID\n", ਪਤਾ);
        }
    }
}
#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */

#ਜੇ defined(CONFIG_KERNEL_LOG_ਬਫਰ)
exception_t benchmark_arch_map_log_ਬਫਰ(ਸ਼ਬਦ_t ਚੌਖਟਾ_cptr)
{
    lookup_Cap_And_ਥਾਂ_ret_t lu_ret;
    vm_ਪੰਨਾ_ਮਾਪ_t ਚੌਖਟਾ_ਮਾਪ;
    pptr_t  ਚੌਖਟਾ_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_ਥਾਂ(ਗੰਢ_STATE(ksCurThread), ਚੌਖਟਾ_cptr);

    ਜੇ (unlikely(lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", ਚੌਖਟਾ_cptr);
        current_fault = seL4_Fault_CapFault_new(ਚੌਖਟਾ_cptr, false);

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (cap_get_capType(lu_ret.cap) != cap_ਚੌਖਟਾ_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(ਚੌਖਟਾ_cptr, false);

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਚੌਖਟਾ_ਮਾਪ = cap_ਚੌਖਟਾ_cap_get_cap_FSize(lu_ret.cap);

    ਜੇ (ਚੌਖਟਾ_ਮਾਪ != ARMLarge_ਪੰਨਾ) {
        userError("Invalid frame size. The kernel expects 2M log buffer");
        current_fault = seL4_Fault_CapFault_new(ਚੌਖਟਾ_cptr, false);

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਚੌਖਟਾ_pptr = cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_ਬਫਰ = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *) ਚੌਖਟਾ_pptr);

    *armKSGlobalLogPTE = pte_pte_ਪੰਨਾ_new(
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
                             0, // XN
#ਨਹੀਂਤਾਂ
                             1, // UXN
#ਜੇ_ਅੰਤ
                             ks_User_Log_ਬਫਰ,
                             0,                         /* global */
                             1,                         /* access flag */
                             SMP_TERNARY(SMP_SHARE, 0), /* Inner-shareable if SMP enabled, otherwise unshared */
                             0,                         /* VMKernelOnly */
                             ਸਾਧਾਰਨ_WT);

    cleanByVA_PoU((vptr_t)armKSGlobalLogPTE, addrFromKPPtr(armKSGlobalLogPTE));
    invalidateTranslationSingle(KS_LOG_PPTR);
    ਵਾਪਸ EXCEPTION_NONE;
}
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_LOG_BUFFER */
