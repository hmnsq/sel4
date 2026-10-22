/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <types.h>
#ضمن <benchmark/benchmark.h>
#ضمن <api/failures.h>
#ضمن <api/syscall.h>
#ضمن <kernel/boot.h>
#ضمن <kernel/cspace.h>
#ضمن <kernel/thread.h>
#ضمن <kernel/stack.h>
#ضمن <machine/io.h>
#ضمن <machine/debug.h>
#ضمن <model/statedata.h>
#ضمن <object/cnode.h>
#ضمن <object/untyped.h>
#ضمن <arch/api/invocation.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <linker.h>
#ضمن <plat/machine/hardware.h>
#ضمن <armv/context_switch.h>
#ضمن <arch/object/iospace.h>
#ضمن <arch/object/vcpu.h>
#ضمن <arch/machine/tlb.h>

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
تعداد mair_types {
    DEVICE_nGnRnE = 0,
    DEVICE_nGnRE = 1,
    DEVICE_GRE = 2,
    نورمال_NC = 3,
    نورمال = 4,
    نورمال_WT = 5
};

/* Stage-2 translation memory attributes */
تعداد mair_s2_types {
    S2_DEVICE_nGnRnE = 0,
    S2_DEVICE_nGnRE = 1,
    S2_DEVICE_nGRE  = 2,
    S2_DEVICE_GRE = 3,

    S2_نورمال_INNER_NC_OUTER_NC = 5,
    S2_نورمال_INNER_WTC_OUTER_NC = 6,
    S2_نورمال_INNER_WBC_OUTER_NC = 7,

    S2_نورمال_INNER_NC_OUTER_WTC = 9,
    S2_نورمال_INNER_WTC_OUTER_WTC = 10,
    S2_نورمال_INNER_WBC_OUTER_WTC = 11,

    S2_نورمال_INNER_NC_OUTER_WBC = 13,
    S2_نورمال_INNER_WTC_OUTER_WBC = 14,
    S2_نورمال_INNER_WBC_OUTER_WBC = 15,

    S2_نورمال = S2_نورمال_INNER_WBC_OUTER_WBC
};

/* Leif from Linaro said the big.LITTLE clusters should be treated as
 * inner shareable, and we believe so, although the Example B2-1 given in
 * ARM ARM DDI 0487B.b (ID092517) says otherwise.
 */

#تعریف_کړه SMP_SHARE   3

بنية lookupPTSlot_ret {
    pte_t *pt_خونه;
    کلمه_t pt_بټونه_کیڼ;
};
تعريفنوع بنية lookupPTSlot_ret lookupPTSlot_ret_t;

بنية پیدا_کړه_VSpace_For_ASID_ret {
    exception_t حالت;
    vspace_ریښه_t *vspace_ریښه;
};
تعريفنوع بنية پیدا_کړه_VSpace_For_ASID_ret پیدا_کړه_VSpace_For_ASID_ret_t;

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

ساكن کلمه_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    بدل (vm_rights) {
    حالة VMKernelOnly:
        إذا (config_سېټ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            أرجع 0;
        } وإلا {
            أرجع 0;
        }

    حالة VMReadWrite:
        إذا (config_سېټ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            أرجع 3;
        } وإلا {
            أرجع 1;
        }

    حالة VMReadOnly:
        إذا (config_سېټ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            أرجع 1;
        } وإلا {
            أرجع 3;
        }

    افتراضي:
        fail("Invalid VM rights");
    }
}

#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
ساكن مضمن CONST کلمه_t pte_get_AP(pte_t pte)
{
    أكد(pte_is_پاڼه_type(pte));
    بدل (pte_get_pte_type(pte)) {
    حالة pte_pte_4k_پاڼه:
        أرجع pte_pte_4k_پاڼه_get_AP(pte);

    حالة pte_pte_پاڼه:
        أرجع pte_pte_پاڼه_get_AP(pte);

    افتراضي:
        أرجع 0;
    }
}

ساكن vm_rights_t CONST vmRightsFromPTE(pte_t pte)
{
    کلمه_t access_perms = pte_get_AP(pte);
    بدل (access_perms) {
    حالة 0:
    حالة 2:
        أرجع VMKernelOnly;
    حالة 1:
        أرجع VMReadWrite;
    حالة 3:
        أرجع VMReadOnly;
    افتراضي:
        fail("Invalid AP bit");
    }
}
#د_که_پای

vm_rights_t CONST ماسک_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_ماسک)
{
    إذا (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_ماسک)) {
        أرجع VMReadOnly;
    }
    إذا (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_ماسک)) {
        إذا (!seL4_CapRights_get_capAllowWrite(cap_rights_ماسک)) {
            أرجع VMReadOnly;
        } وإلا {
            أرجع VMReadWrite;
        }
    }
    إذا (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_ماسک) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_ماسک)) {
        userError("Attempted to make unsupported write only mapping");
    }
    أرجع VMKernelOnly;
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
BOOT_کوډ فراغ map_kernel_چوکاټ(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_ځانګړتیاوې_t ځانګړتیاوې)
{
    أكد(vaddr >= PPTR_سر);

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    کلمه_t uxn = vm_ځانګړتیاوې_get_arm_Execute_Never(ځانګړتیاوې);
#وإلا
    کلمه_t uxn = 1; /* unprivileged execute never */
#د_که_پای /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    کلمه_t attr_شاخص;
    کلمه_t shareable;
    إذا (vm_ځانګړتیاوې_get_arm_پاڼه_Cacheable(ځانګړتیاوې)) {
        attr_شاخص = نورمال;
        shareable = SMP_TERNARY(SMP_SHARE, 0);
    } وإلا {
        attr_شاخص = DEVICE_nGnRnE;
        shareable = 0;
    }
    armKSGlobalKernelPT[GET_KPT_شاخص(vaddr, KLVL_FRM_ARM_PT_LVL(3))] = pte_pte_4k_پاڼه_new(uxn, paddr,
                                                                                            0, /* global */
                                                                                            1, /* access flag */
                                                                                            shareable,
                                                                                            APFromVMRights(vm_rights),
                                                                                            attr_شاخص);
}

/* Essentially, assert that the ELF, kernel devices, and log buffer are all in the same PUD.
   In the current code this is the top index. This is arbitrary, the important part
   is that they are all the same.
 */
compile_assert(elf_دننه_kernel_mappings, GET_KPT_شاخص(KERNEL_ELF_بنسټ,
                                                     KLVL_FRM_ARM_PT_LVL(1)) == بټ(PT_شاخص_بټونه) - 1);
compile_assert(kdev_دننه_kernel_mappings, GET_KPT_شاخص(KDEV_بنسټ,
                                                      KLVL_FRM_ARM_PT_LVL(1)) == بټ(PT_شاخص_بټونه) - 1);
compile_assert(log_buf_دننه_kernel_mappings, GET_KPT_شاخص(KS_LOG_بنسټ,
                                                         KLVL_FRM_ARM_PT_LVL(1)) == بټ(PT_شاخص_بټونه) - 1);

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
BOOT_کوډ فراغ map_kernel_elf_image(pte_t kernel_mapping_pds[])
{
    /** This is not strictly necessary, but to handle a non-aligned KERNEL_ELF_PADDR_BASE
     *  we'd need to do a bit more logic and probably do what riscv64 does
     *  where the *actual* KERNEL_ELF_BASE is rounded up to the alignment.
     *  This is simpler and memory is cheap.
     **/
    compile_assert(elf_paddr_aligned, IS_ALIGNED(KERNEL_ELF_PADDR_بنسټ_RAW, se_L4_Large_پاڼه_بټونه));
    compile_assert(elf_بنسټ_aligned, IS_ALIGNED(KERNEL_ELF_بنسټ, se_L4_Large_پاڼه_بټونه));

    /* Please don't overlap with KDEV_BASE */
    أكد(KERNEL_ELF_سر <= KDEV_بنسټ);
    /* Please don't overlap with KS_LOG_PPTR */
    أكد(KERNEL_ELF_سر <= KS_LOG_PPTR);

    ثابت کلمه_t pd_پیل = GET_KPT_شاخص(KERNEL_ELF_بنسټ, KLVL_FRM_ARM_PT_LVL(2));
    /* pd_end and ELF_TOP are both exclusive, so handle that correctly */
    ثابت کلمه_t pd_پای_ټکی   = GET_KPT_شاخص(KERNEL_ELF_سر - 1, KLVL_FRM_ARM_PT_LVL(2)) + 1;

    paddr_t paddr = KERNEL_ELF_PADDR_بنسټ;
    ناکارول_شوی pptr_t vaddr = KERNEL_ELF_بنسټ;

    لكل (کلمه_t pd = pd_پیل; pd < pd_پای_ټکی; pd++) {
        أكد(pd == GET_KPT_شاخص(vaddr, KLVL_FRM_ARM_PT_LVL(2)));

        kernel_mapping_pds[pd] = pte_pte_پاڼه_new(
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
                                     /* XN */ 0,
#وإلا
                                     /* UXN */ 1,
#د_که_پای
                                     /* page_base_address */ paddr,
                                     /* global (nG) */ 0,
                                     /* access flag (AF) */ 1,
                                     /* shareability (SH) */ SMP_TERNARY(SMP_SHARE, 0),
                                     /* AP */ APFromVMRights(VMKernelOnly),
                                     /* AttrIndx */ نورمال
                                 );

        paddr += بټ(se_L4_Large_پاڼه_بټونه);
        vaddr += بټ(se_L4_Large_پاڼه_بټونه);
    }
}

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
/* verify that the physical memory window as at the second entry of the PGD */
compile_assert(physical_کړکۍ_pgd_location,
               GET_KPT_شاخص(PPTR_بنسټ, KLVL_FRM_ARM_PT_LVL(0)) == 1);
#وإلا
/* verify that the physical memory window as at the last entry of the PGD */
compile_assert(physical_کړکۍ_pgd_location,
               GET_KPT_شاخص(PPTR_بنسټ, KLVL_FRM_ARM_PT_LVL(0)) == بټ(PT_شاخص_بټونه) - 1);
#د_که_پای
/* make sure that the PPTR_BASE and PPTR_TOP are huge paged aligned */
compile_assert(physical_کړکۍ_بنسټ_aligned, IS_ALIGNED(PPTR_بنسټ, se_L4_Huge_پاڼه_بټونه));
compile_assert(physical_کړکۍ_سر_aligned, IS_ALIGNED(PPTR_سر, se_L4_Huge_پاڼه_بټونه));

/* the PPTR_BASE starts from the bottom of the kernel PUD */
compile_assert(physical_کړکۍ_bottom_kernel_region,
               GET_KPT_شاخص(PPTR_بنسټ, KLVL_FRM_ARM_PT_LVL(1)) == 0);
/* the PPTR_TOP then ends at the second-to-last entry of the PUD
    note: because this is the exclusive value the assert is one higher than
          you would expect.
*/
compile_assert(physical_کړکۍ_سر_kernel_region,
               GET_KPT_شاخص(PPTR_سر, KLVL_FRM_ARM_PT_LVL(1)) == بټ(PT_شاخص_بټونه) - 1);

BOOT_کوډ فراغ map_kernel_physical_کړکۍ(فراغ)
{
    paddr_t paddr;
    pptr_t vaddr;
    کلمه_t idx;

    /* place all PDs except the last one in PUD */
    لكل (idx = GET_KPT_شاخص(PPTR_بنسټ, KLVL_FRM_ARM_PT_LVL(1)); idx < GET_KPT_شاخص(PPTR_سر, KLVL_FRM_ARM_PT_LVL(1));
         idx++) {
        armKSGlobalKernelPUD[idx] = pte_pte_جدول_new(
                                        addrFromKPPtr(&armKSGlobalKernelPDs[idx][0])
                                    );
    }

    /* map the kernel window using large pages */
    vaddr = PPTR_بنسټ;
    لكل (paddr = PADDR_بنسټ; paddr < PADDR_سر; paddr += بټ(se_L4_Large_پاڼه_بټونه)) {
        armKSGlobalKernelPDs[GET_KPT_شاخص(vaddr, KLVL_FRM_ARM_PT_LVL(1))][GET_KPT_شاخص(vaddr,
                                                                                         KLVL_FRM_ARM_PT_LVL(2))] = pte_pte_پاڼه_new(
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
                                                                                                                        1, // XN
#وإلا
                                                                                                                        1, // UXN
#د_که_پای
                                                                                                                        paddr,
                                                                                                                        0,                        /* global */
                                                                                                                        1,                        /* access flag */
                                                                                                                        SMP_TERNARY(SMP_SHARE, 0),        /* Inner-shareable if SMP enabled, otherwise unshared */
                                                                                                                        0,                        /* VMKernelOnly */
                                                                                                                        نورمال
                                                                                                                    );
        vaddr += بټ(se_L4_Large_پاڼه_بټونه);
    }
}

BOOT_کوډ فراغ map_kernel_کړکۍ(فراغ)
{
    /* place the PUD into the PGD */
    armKSGlobalKernelPGD[GET_KPT_شاخص(PPTR_بنسټ, KLVL_FRM_ARM_PT_LVL(0))] =
        pte_pte_جدول_new(addrFromKPPtr(armKSGlobalKernelPUD));

    map_kernel_physical_کړکۍ();

    ثابت کلمه_t elf_pud_idx = GET_KPT_شاخص(KERNEL_ELF_بنسټ, KLVL_FRM_ARM_PT_LVL(1));
    pte_t *kernel_mapping_pds = armKSGlobalKernelPDs[elf_pud_idx];

    /* place the kernel mapping PDs in the kernel mapping PUD */
    armKSGlobalKernelPUD[elf_pud_idx] = pte_pte_جدول_new(addrFromKPPtr(kernel_mapping_pds));

    /* map the kernel ELF */
    map_kernel_elf_image(kernel_mapping_pds);

    /* put the PT into the PD for device window. It would be nice if
       we could pass kernel_mapping_pds[511] to map_kernel_devices(), but it is
       shared between AArch64 and AArch32, so... */
    kernel_mapping_pds[بټ(PT_شاخص_بټونه) - 1]
        = pte_pte_جدول_new(addrFromKPPtr(armKSGlobalKernelPT));

    map_kernel_devices();
}

/* When the hypervisor support is enabled, the stage-2 translation table format
 * is used for applications.
 * The global bit is always 0.
 * The memory attributes use the S2 translation values.
 */
ساكن BOOT_کوډ فراغ map_it_چوکاټ_cap(cap_t vspace_cap, cap_t چوکاټ_cap, bool_t executable)
{
    vspace_ریښه_t *vspace_ریښه_2 = VSPACE_PTR(pptr_د_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt;

    vptr_t vptr = cap_چوکاټ_cap_get_cap_FMapped_پته(چوکاټ_cap);
    فراغ *pptr = (فراغ *)cap_چوکاټ_cap_get_cap_FBase_Ptr(چوکاټ_cap);

    أكد(cap_چوکاټ_cap_get_cap_FMapped_ASID(چوکاټ_cap) != 0);

#که_تعریف_شوی AARCH64_VSPACE_S2_پیل_L1
    pud = vspace_ریښه_2;
#وإلا
    vspace_ریښه_2 += GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(0));
    أكد(pte_pte_جدول_ptr_get_present(vspace_ریښه_2));
    pud = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنسټ_پته(vspace_ریښه_2));
#د_که_پای
    pud += GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(1));
    أكد(pte_pte_جدول_ptr_get_present(pud));
    pd = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنسټ_پته(pud));
    pd += GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(2));
    أكد(pte_pte_جدول_ptr_get_present(pd));
    pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنسټ_پته(pd));
    *(pt + GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(3))) = pte_pte_4k_پاڼه_new(
                                                              !executable,                    /* unprivileged execute never */
                                                              pptr_منزل_paddr(pptr),            /* page_base_address    */
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              0,
#وإلا
                                                              1,                              /* not global */
#د_که_پای
                                                              1,                              /* access flag */
                                                              SMP_TERNARY(SMP_SHARE, 0),              /* Inner-shareable if SMP enabled, otherwise unshared */
                                                              APFromVMRights(VMReadWrite),
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              S2_نورمال
#وإلا
                                                              نورمال
#د_که_پای
                                                          );
}

ساكن BOOT_کوډ cap_t create_it_چوکاټ_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    vm_پاڼه_اندازه_t چوکاټ_اندازه_2;
    إذا (use_large) {
        چوکاټ_اندازه_2 = ARMLarge_پاڼه;
    } وإلا {
        چوکاټ_اندازه_2 = ARMSmall_پاڼه;
    }
    أرجع
        cap_چوکاټ_cap_new(
            asid,                          /* capFMappedASID */
            pptr,                          /* capFBasePtr */
            چوکاټ_اندازه_2,                    /* capFSize */
            vptr,                          /* capFMappedAddress */
            کلمه_From_VMRights(VMReadWrite), /* capFVMRights */
            false                          /* capFIsDevice */
        );
}

ساكن BOOT_کوډ فراغ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    vspace_ریښه_t *vspace_ریښه_2 = VSPACE_PTR(pptr_د_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt = PT_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_پاڼه_جدول_cap_get_cap_PTMapped_پته(pt_cap);

    أكد(cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(pt_cap));

#که_تعریف_شوی AARCH64_VSPACE_S2_پیل_L1
    pud = vspace_ریښه_2;
#وإلا
    vspace_ریښه_2 += GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(0));
    أكد(pte_pte_جدول_ptr_get_present(vspace_ریښه_2));
    pud = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنسټ_پته(vspace_ریښه_2));
#د_که_پای
    pud += GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(1));
    أكد(pte_pte_جدول_ptr_get_present(pud));
    pd = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنسټ_پته(pud));
    *(pd + GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(2))) = pte_pte_جدول_new(
                                                              pptr_منزل_paddr(pt)
                                                          );
}

ساكن BOOT_کوډ cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_پاڼه_جدول_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pt_cap(vspace_cap, cap);
    أرجع cap;
}

ساكن BOOT_کوډ فراغ map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    vspace_ریښه_t *vspace_ریښه_2 = VSPACE_PTR(pptr_د_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd = PT_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(pd_cap));
    vptr_t vptr = cap_پاڼه_جدول_cap_get_cap_PTMapped_پته(pd_cap);

    أكد(cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(pd_cap));

#که_تعریف_شوی AARCH64_VSPACE_S2_پیل_L1
    pud = vspace_ریښه_2;
#وإلا
    vspace_ریښه_2 += GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(0));
    أكد(pte_pte_جدول_ptr_get_present(vspace_ریښه_2));
    pud = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنسټ_پته(vspace_ریښه_2));
#د_که_پای
    *(pud + GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(1))) = pte_pte_جدول_new(
                                                               pptr_منزل_paddr(pd)
                                                           );
}

ساكن BOOT_کوډ cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_پاڼه_جدول_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pd_cap(vspace_cap, cap);
    أرجع cap;
}

#که_نه_تعریف_شوی AARCH64_VSPACE_S2_پیل_L1
ساكن BOOT_کوډ فراغ map_it_pud_cap(cap_t vspace_cap, cap_t pud_cap)
{
    pte_t *pgd = PT_PTR(pptr_د_cap(vspace_cap));
    pte_t *pud = PT_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(pud_cap));
    vptr_t vptr = cap_پاڼه_جدول_cap_get_cap_PTMapped_پته(pud_cap);

    أكد(cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(pud_cap));

    *(pgd + GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(0))) = pte_pte_جدول_new(
                                                               pptr_منزل_paddr(pud));
}

ساكن BOOT_کوډ cap_t create_it_pud_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_پاڼه_جدول_cap_new(
              asid,               /* capPTMappedASID */
              pptr,               /* capPTBasePtr */
              1,                  /* capPTIsMapped */
              vptr                /* capPTMappedAddress */
          );
    map_it_pud_cap(vspace_cap, cap);
    أرجع cap;
}
#د_که_پای /* AARCH64_VSPACE_S2_START_L1 */
BOOT_کوډ کلمه_t arch_get_n_paging(v_region_t it_v_reg)
{
    أرجع
#که_نه_تعریف_شوی AARCH64_VSPACE_S2_پیل_L1
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_بټونه(ULVL_FRM_ARM_PT_LVL(0))) +
#د_که_پای
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_بټونه(ULVL_FRM_ARM_PT_LVL(1))) +
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_بټونه(ULVL_FRM_ARM_PT_LVL(2)));
}

BOOT_کوډ cap_t create_it_پته_space(cap_t ریښه_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_خونه_Pos خونه_pos_before;
    se_L4_خونه_Pos خونه_pos_after;

    /* create the PGD */
    vspace_cap = cap_vspace_cap_new(
                     IT_ASID,           /* capVSMappedASID */
                     rootserver.vspace, /* capVSBasePtr    */
                     1                  /* capVSIsMapped   */
#که_تعریف_شوی CONFIG_ARM_SMMU
                     , CB_INVALID       /* capVSMappedCB   */
#د_که_پای
                 );
    خونه_pos_before = ndks_boot.خونه_pos_cur;
    write_خونه(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

#که_نه_تعریف_شوی AARCH64_VSPACE_S2_پیل_L1
    /* Create any PUDs needed for the user land image */
    لكل (vptr = پړاو_ښکته(it_v_reg.پیل, GET_ULVL_PGSIZE_بټونه(ULVL_FRM_ARM_PT_LVL(0)));
         vptr < it_v_reg.پای_ټکی;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(0))) {
        إذا (!provide_cap(ریښه_cnode_cap, create_it_pud_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            أرجع cap_null_cap_new();
        }
    }
#د_که_پای
    /* Create any PDs needed for the user land image */
    لكل (vptr = پړاو_ښکته(it_v_reg.پیل, GET_ULVL_PGSIZE_بټونه(ULVL_FRM_ARM_PT_LVL(1)));
         vptr < it_v_reg.پای_ټکی;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))) {
        إذا (!provide_cap(ریښه_cnode_cap, create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            أرجع cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    لكل (vptr = پړاو_ښکته(it_v_reg.پیل, GET_ULVL_PGSIZE_بټونه(ULVL_FRM_ARM_PT_LVL(2)));
         vptr < it_v_reg.پای_ټکی;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))) {
        إذا (!provide_cap(ریښه_cnode_cap, create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            أرجع cap_null_cap_new();
        }
    }

    خونه_pos_after = ndks_boot.خونه_pos_cur;
    ndks_boot.bi_چوکاټ->userImagePaging = (se_L4_خونه_Region) {
        خونه_pos_before, خونه_pos_after
    };
    أرجع vspace_cap;
}

BOOT_کوډ cap_t create_unmapped_it_چوکاټ_cap(pptr_t pptr, bool_t use_large)
{
    أرجع create_it_چوکاټ_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_کوډ cap_t create_mapped_it_چوکاټ_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_چوکاټ_cap(pptr, vptr, asid, use_large);
    map_it_چوکاټ_cap(pd_cap, cap, executable);
    أرجع cap;
}

BOOT_کوډ فراغ activate_kernel_vspace(فراغ)
{
    cleanInvalidateL1Caches();
    سېټ_Current_Kernel_VSpace_ریښه(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalKernelPGD)));

    /* Prevent elf-loader address translation to fill up TLB */
    سېټ_Current_User_VSpace_ریښه(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));

    invalidateLocalTLB();
    قلف_TLBEntry(KERNEL_ELF_بنسټ);
}

BOOT_کوډ فراغ write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_د_cap(it_ap_cap));
    asid_map_t asid_map = asid_map_asid_map_vspace_new(
#که_تعریف_شوی CONFIG_ARM_SMMU
                              /* bind_cb: Number of bound context banks */
                              0,
#د_که_پای
                              /* vspace_root: reference to vspace root page table object */
                              (کلمه_t)cap_vspace_cap_get_capVSBasePtr(it_vspace_cap)
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
                              /* stored_hw_vmid, stored_vmid_valid: Assigned hardware VMID for TLB. */
                              , 0, false
#د_که_پای
                          );
    ap->array[ASID_ټیټ(IT_ASID)] = asid_map;
    armKSASIDTable[ASID_لوړ(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

asid_map_t پیدا_کړه_Map_For_ASID(asid_t asid)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_لوړ(asid)];
    إذا (!poolPtr) {
        أرجع asid_map_asid_map_none_new();
    }

    أرجع poolPtr->array[ASID_ټیټ(asid)];
}

ساكن پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_VSpace_For_ASID(asid_t asid)
{
    پیدا_کړه_VSpace_For_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = پیدا_کړه_Map_For_ASID(asid);
    إذا (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_ریښه_new();

        ret.vspace_ریښه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.vspace_ریښه = (vspace_ریښه_t *)asid_map_asid_map_vspace_get_vspace_ریښه(asid_map);
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

کلمه_t *خالص lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    کلمه_t w_بفر_Ptr;
    cap_t بفر_Cap;
    vm_rights_t vm_rights;

    w_بفر_Ptr = thread->tcbIPCBuffer;
    بفر_Cap = TCB_PTR_CTE_PTR(thread, tcb_بفر)->cap;

    إذا (unlikely(cap_get_capType(بفر_Cap) != cap_چوکاټ_cap)) {
        أرجع عدم;
    }
    إذا (unlikely(cap_چوکاټ_cap_get_cap_FIs_Device(بفر_Cap))) {
        أرجع عدم;
    }

    vm_rights = cap_چوکاټ_cap_get_cap_FVMRights(بفر_Cap);
    إذا (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        کلمه_t بنسټ_Ptr;
        بینښه صحيح پاڼه_بټونه_2;

        بنسټ_Ptr = cap_چوکاټ_cap_get_cap_FBase_Ptr(بفر_Cap);
        پاڼه_بټونه_2 = پاڼه_بټونه_For_اندازه(cap_چوکاټ_cap_get_cap_FSize(بفر_Cap));
        أرجع (کلمه_t *)(بنسټ_Ptr + (w_بفر_Ptr & ماسک(پاڼه_بټونه_2)));
    } وإلا {
        أرجع عدم;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    إذا (cap_get_capType(cap) != cap_چوکاټ_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(cap_چوکاټ_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!IS_ALIGNED(vptr, se_L4_IPCBuffer_اندازه_بټونه)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (صحيح)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

ساكن lookupPTSlot_ret_t lookupPTSlot(vspace_ریښه_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    کلمه_t level = UPT_پوړونه - 1;
    pte_t *pt = vspace;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_بټونه_کیڼ = PT_شاخص_بټونه * level + se_L4_پاڼه_بټونه;
    ret.pt_خونه = pt + ((vptr >> ret.pt_بټونه_کیڼ) & ماسک(se_L4_VSpace_شاخص_بټونه));

    طالما (pte_pte_جدول_ptr_get_present(ret.pt_خونه) && likely(level > 0)) {
        level--;
        ret.pt_بټونه_کیڼ -= PT_شاخص_بټونه;
        pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنسټ_پته(ret.pt_خونه));
        ret.pt_خونه = pt + ((vptr >> ret.pt_بټونه_کیڼ) & ماسک(PT_شاخص_بټونه));
    }

    أرجع ret;
}

/* Note that if the hypervisor support is enabled, the user page tables use
 * stage-2 translation format. Otherwise, they follow the stage-1 translation format.
 */
ساكن pte_t make_User_پاڼه_PTE(paddr_t paddr, vm_rights_t vm_rights, bool_t cap_read, vm_ځانګړتیاوې_t ځانګړتیاوې,
                             vm_پاڼه_اندازه_t پاڼه_اندازه)
{
    bool_t nonexecutable = vm_ځانګړتیاوې_get_arm_Execute_Never(ځانګړتیاوې) || !cap_read;
    کلمه_t cacheable = vm_ځانګړتیاوې_get_arm_پاڼه_Cacheable(ځانګړتیاوې);

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    کلمه_t nG = 0; /* not global */
    کلمه_t attridx = cacheable ? S2_نورمال : S2_DEVICE_nGnRnE;
#وإلا
    کلمه_t nG = 1; /* not global */
    کلمه_t attridx = cacheable ? نورمال : DEVICE_nGnRnE;
#د_که_پای

    /* Inner-shareable if SMP enabled, otherwise unshared (ignored for devices) */
    کلمه_t shareable = cacheable ? SMP_TERNARY(SMP_SHARE, 0) : 0;

    إذا (پاڼه_اندازه == ARMSmall_پاڼه) {
        أرجع pte_pte_4k_پاڼه_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                   shareable, APFromVMRights(vm_rights), attridx);
    } وإلا {
        أرجع pte_pte_پاڼه_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                shareable, APFromVMRights(vm_rights), attridx);
    }
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    بدل (vm_faultType) {
    حالة ARMDataAbort: {
        کلمه_t addr, fault;

        addr = getFAR();
        fault = getDFSR();

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        /* use the IPA */
        إذا (ARCH_غوټه_STATE(armHSVCPUActive)) {
            addr = GET_PAR_ADDR(پته_Translate_S1(addr)) | (addr & ماسک(پاڼه_بټونه));
        }
#د_که_پای
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        أرجع EXCEPTION_FAULT;
    }

    حالة ARMPrefetchAbort: {
        کلمه_t pc, fault;

        pc = getRestartPC(thread);
        fault = getIFSR();

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (ARCH_غوټه_STATE(armHSVCPUActive)) {
            pc = GET_PAR_ADDR(پته_Translate_S1(pc)) | (pc & ماسک(پاڼه_بټونه));
        }
#د_که_پای
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        أرجع EXCEPTION_FAULT;
    }

    افتراضي:
        fail("Invalid VM fault type");
    }
}

bool_t CONST is_VTable_ریښه(cap_t cap)
{
    أرجع cap_get_capType(cap) == cap_vspace_cap;
}

bool_t CONST is_Valid_Native_ریښه(cap_t cap)
{
    أرجع is_VTable_ریښه(cap) &&
           cap_vspace_cap_get_capVSIsMapped(cap);
}

bool_t CONST is_Valid_VTable_ریښه(cap_t cap)
{
    أرجع is_Valid_Native_ریښه(cap);
}

فراغ سېټ_VMRoot(tcb_t *tcb)
{
    cap_t thread_ریښه;
    asid_t asid;
    vspace_ریښه_t *vspace_ریښه_2;
    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret;

    thread_ریښه = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    /* ASID/VMID 0 is never allocated to a real VSpace (see findFreeHWASID()
     * for the hypervisor case), so the empty global VSpace can be installed
     * under it without flushing the TLB. */
    إذا (!is_Valid_Native_ریښه(thread_ریښه)) {
        سېټ_Current_User_VSpace_ریښه(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        أرجع;
    }

    vspace_ریښه_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_ریښه));
    asid = cap_vspace_cap_get_capVSMappedASID(thread_ریښه);
    پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
    إذا (unlikely(پیدا_کړه_ret.حالت != EXCEPTION_NONE || پیدا_کړه_ret.vspace_ریښه != vspace_ریښه_2)) {
        سېټ_Current_User_VSpace_ریښه(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        أرجع;
    }

    armv_contextSwitch(vspace_ریښه_2, asid);
}

ساكن bool_t سېټ_VMRoot_For_Flush(vspace_ریښه_t *vspace, asid_t asid)
{
    cap_t thread_ریښه;

    thread_ریښه = TCB_PTR_CTE_PTR(غوټه_STATE(ksCurThread), tcbVTable)->cap;

    إذا (cap_get_capType(thread_ریښه) == cap_vspace_cap &&
        cap_vspace_cap_get_capVSIsMapped(thread_ریښه) &&
        VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_ریښه)) == vspace) {
        أرجع false;
    }

    armv_contextSwitch(vspace, asid);
    أرجع true;
}


#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT

ساكن مضمن asid_pool_t *getPoolPtr(asid_t asid)
{
    أرجع armKSASIDTable[ASID_لوړ(asid)];
}

ساكن مضمن asid_map_t getASIDMap(asid_pool_t *poolPtr, asid_t asid)
{
    أكد(poolPtr != عدم);
    أرجع poolPtr->array[ASID_ټیټ(asid)];
}

ساكن مضمن فراغ سېټ_ASIDMap(asid_pool_t *poolPtr, asid_t asid, asid_map_t asid_map)
{
    أكد(poolPtr != عدم);
    poolPtr->array[ASID_ټیټ(asid)] = asid_map;
}

ساكن فراغ invalidateASID(asid_t asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    أكد(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_سېټ_stored_hw_vmid(asid_map, 0);
    asid_map = asid_map_asid_map_vspace_سېټ_stored_vmid_valid(asid_map, false);

    سېټ_ASIDMap(poolPtr, asid, asid_map);
}

ساكن فراغ storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    أكد(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_سېټ_stored_hw_vmid(asid_map, hw_asid);
    asid_map = asid_map_asid_map_vspace_سېټ_stored_vmid_valid(asid_map, true);

    سېټ_ASIDMap(poolPtr, asid, asid_map);
    armKSHWASIDTable[hw_asid] = asid;
}

ساكن hw_asid_t پیدا_کړه_Free_HWASID(فراغ)
{
    کلمه_t hw_asid_دننه_واټن;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    لكل (hw_asid_دننه_واټن = 0;
         hw_asid_دننه_واټن <= (کلمه_t)((hw_asid_t) - 1);
         hw_asid_دننه_واټن++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_دننه_واټن);
        إذا (hw_asid != hwASIDReserved && armKSHWASIDTable[hw_asid] == asidInvalid) {
            أرجع hw_asid;
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
    إذا (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    أرجع hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = پیدا_کړه_Map_For_ASID(asid);
    إذا (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        أرجع asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    } وإلا {
        hw_asid_t new_hw_asid;

        new_hw_asid = پیدا_کړه_Free_HWASID();
        storeHWASID(asid, new_hw_asid);
        أرجع new_hw_asid;
    }
}

ساكن فراغ invalidateASIDEntry(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = پیدا_کړه_Map_For_ASID(asid);
    إذا (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        armKSHWASIDTable[asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

#د_که_پای

#که_تعریف_شوی CONFIG_ARM_SMMU
ساكن کلمه_t getASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_لوړ(asid)];
    أكد(asidPool);

    asid_map_t asid_map = asidPool->array[ASID_ټیټ(asid)];
    أكد(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    أرجع asid_map_asid_map_vspace_get_bind_cb(asid_map);
}

فراغ increaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_لوړ(asid)];
    أكد(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_ټیټ(asid)];
    أكد(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_سېټ_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) + 1);
}

فراغ decreaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_لوړ(asid)];
    أكد(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_ټیټ(asid)];
    أكد(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_سېټ_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) - 1);
}
#د_که_پای

ساكن مضمن فراغ invalidateTLBByASID(asid_t asid)
{
#که_تعریف_شوی CONFIG_ARM_SMMU
    کلمه_t bind_cb = getASIDBindCB(asid);
    إذا (unlikely(bind_cb)) {
        invalidateSMMUTLBByASID(asid, bind_cb);
    }
#د_که_پای
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = پیدا_کړه_Map_For_ASID(asid);
    إذا (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        أرجع;
    }
    invalidateTranslationASID(asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map));
#وإلا
    invalidateTranslationASID(asid);
#د_که_پای
}

ساكن مضمن فراغ invalidateTLBByASIDVA(asid_t asid, vptr_t vaddr)
{
#که_تعریف_شوی CONFIG_ARM_SMMU
    کلمه_t bind_cb = getASIDBindCB(asid);
    إذا (unlikely(bind_cb)) {
        invalidateSMMUTLBByASIDVA(asid, vaddr, bind_cb);
    }
#د_که_پای
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = پیدا_کړه_Map_For_ASID(asid);
    إذا (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        أرجع;
    }
    uint64_t hw_asid = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    invalidateTranslationSingle((hw_asid << 48) | vaddr >> se_L4_پاڼه_بټونه);
#وإلا
    invalidateTranslationSingle((asid << 48) | vaddr >> se_L4_پاڼه_بټونه);
#د_که_پای
}


فراغ unmap_پاڼه_جدول(asid_t asid, vptr_t vptr, pte_t *موخه_pt)
{
    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
    إذا (unlikely(پیدا_کړه_ret.حالت != EXCEPTION_NONE)) {
        /* nothing to do */
        أرجع;
    }
    pte_t *pt_خونه = عدم;
    pte_t *pt = (pte_t *)پیدا_کړه_ret.vspace_ریښه;

    لكل (کلمه_t i = 0; i < UPT_پوړونه - 1 && pt != موخه_pt; i++) {
        pt_خونه = pt + GET_UPT_شاخص(vptr, i);
        إذا (unlikely(!pte_pte_جدول_ptr_get_present(pt_خونه))) {
            /* couldn't find it */
            أرجع;
        }
        pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنسټ_پته(pt_خونه));
    }

    إذا (pt != موخه_pt) {
        /* didn't find it */
        أرجع;
    }
    /* If we found a pt then ptSlot won't be null */
    أكد(pt_خونه != عدم);
    *pt_خونه = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)pt_خونه, pptr_منزل_paddr(pt_خونه));
    invalidateTLBByASID(asid);
}

فراغ unmap_پاڼه(vm_پاڼه_اندازه_t پاڼه_اندازه, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret;
    lookupPTSlot_ret_t  lu_ret;
    pte_t pte;

    پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
    إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    lu_ret = lookupPTSlot(پیدا_کړه_ret.vspace_ریښه, vptr);
    إذا (unlikely(lu_ret.pt_بټونه_کیڼ != پاڼه_بټونه_For_اندازه(پاڼه_اندازه))) {
        /* Do nothing if the wrong size object was returned */
        أرجع;
    }

    pte = *(lu_ret.pt_خونه);
    إذا (!pte_is_پاڼه_type(pte)) {
        /* Do nothing if no page is present */
        أرجع;
    }

    إذا (pte_get_پاڼه_بنسټ_پته(pte) != pptr_منزل_paddr((فراغ *)pptr)) {
        /* Do nothing if the mapped page is not the same physical frame */
        أرجع;
    }

    *(lu_ret.pt_خونه) = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)lu_ret.pt_خونه, pptr_منزل_paddr(lu_ret.pt_خونه));
    أكد(asid < بټ(16));
    invalidateTLBByASIDVA(asid, vptr);
}

فراغ deleteASID(asid_t asid, vspace_ریښه_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_لوړ(asid)];

    إذا (poolPtr != عدم) {
        asid_map_t asid_map = poolPtr->array[ASID_ټیټ(asid)];
        إذا (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_ریښه_t *)asid_map_asid_map_vspace_get_vspace_ریښه(asid_map) == vspace) {
            invalidateTLBByASID(asid);
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
            invalidateASIDEntry(asid);
#د_که_پای
            poolPtr->array[ASID_ټیټ(asid)] = asid_map_asid_map_none_new();
            سېټ_VMRoot(غوټه_STATE(ksCurThread));
        }
    }
}

فراغ deleteASIDPool(asid_t asid_بنسټ, asid_pool_t *pool)
{
    کلمه_t دننه_واټن;

    أكد((asid_بنسټ & ماسک(asid_ټیټ_بټونه)) == 0);

    إذا (armKSASIDTable[ASID_لوړ(asid_بنسټ)] == pool) {
        لكل (دننه_واټن = 0; دننه_واټن < بټ(asid_ټیټ_بټونه); دننه_واټن++) {
            asid_map_t asid_map = pool->array[دننه_واټن];
            إذا (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                invalidateTLBByASID(asid_بنسټ + دننه_واټن);
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
                invalidateASIDEntry(asid_بنسټ + دننه_واټن);
#د_که_پای
            }
        }
        armKSASIDTable[ASID_لوړ(asid_بنسټ)] = عدم;
        سېټ_VMRoot(غوټه_STATE(ksCurThread));
    }
}

ساكن فراغ doFlush(کلمه_t invLabel, vptr_t پیل, vptr_t پای_ټکی, paddr_t pstart)
{
    بدل (invLabel) {
    حالة ARMVSpace_Clean_معلومات:
    حالة ARMPage_Clean_معلومات:
        cleanCacheRange_RAM(پیل, پای_ټکی, pstart);
        اكسر;

    حالة ARMVSpace_Invalidate_معلومات:
    حالة ARMPage_Invalidate_معلومات:
        invalidateCacheRange_RAM(پیل, پای_ټکی, pstart);
        اكسر;

    حالة ARMVSpace_Clean_Invalidate_معلومات:
    حالة ARMPage_Clean_Invalidate_معلومات:
        cleanInvalidateCacheRange_RAM(پیل, پای_ټکی, pstart);
        اكسر;

    حالة ARMVSpaceUnify_Instruction:
    حالة ARMPageUnify_Instruction:
        /* First clean data lines to point of unification... */
        cleanCacheRange_PoU(پیل, پای_ټکی, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(پیل, پای_ټکی, pstart);
        /* ... then wait for the completion of invalidations... */
        dsb();
        /* ... and ensure new instructions come from fresh cache lines. */
        isb();
        اكسر;
    افتراضي:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

ساكن exception_t performVSpaceFlush(کلمه_t invLabel, vspace_ریښه_t *vspace_ریښه_2, asid_t asid,
                                      vptr_t پیل, vptr_t پای_ټکی, paddr_t pstart)
{

    إذا (config_سېټ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        کلمه_t اندازه = پای_ټکی - پیل;
        پیل = (vptr_t)paddr_منزل_pptr(pstart);
        پای_ټکی = پیل + اندازه;
        إذا (پیل < پای_ټکی) {
            doFlush(invLabel, پیل, پای_ټکی, pstart);
        }
    } وإلا {
        bool_t ریښه_switched;

        /* Flush if given a non zero range */
        إذا (پیل < پای_ټکی) {
            ریښه_switched = سېټ_VMRoot_For_Flush(vspace_ریښه_2, asid);
            doFlush(invLabel, پیل, پای_ټکی, pstart);
            إذا (ریښه_switched) {
                سېټ_VMRoot(غوټه_STATE(ksCurThread));
            }
        }
    }
    أرجع EXCEPTION_NONE;
}


ساكن exception_t perform_پاڼه_جدول_Invocation_Map(cap_t cap, cte_t *ct_خونه, pte_t pte, pte_t *pt_خونه)
{
    ct_خونه->cap = cap;
    *pt_خونه = pte;
    cleanByVA_PoU((vptr_t)pt_خونه, pptr_منزل_paddr(pt_خونه));

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_پاڼه_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_خونه)
{
    إذا (cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PT_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_پاڼه_جدول(cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(cap),
                       cap_پاڼه_جدول_cap_get_cap_PTMapped_پته(cap), pt);
        clear_حافظه_PT((فراغ *)pt, cap_get_cap_اندازه_بټونه(cap));
    }

    cap_پاڼه_جدول_cap_ptr_سېټ_cap_PTIs_Mapped(&(ct_خونه->cap), 0);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_پاڼه_Invocation_Map(asid_t asid, cap_t cap, cte_t *ct_خونه,
                                            pte_t pte, pte_t *pt_خونه)
{
    bool_t tlbflush_required = pte_ptr_get_valid(pt_خونه);

    ct_خونه->cap = cap;
    *pt_خونه = pte;

    cleanByVA_PoU((vptr_t)pt_خونه, pptr_منزل_paddr(pt_خونه));
    إذا (unlikely(tlbflush_required)) {
        أكد(asid < بټ(16));
        invalidateTLBByASIDVA(asid, cap_چوکاټ_cap_get_cap_FMapped_پته(cap));
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_پاڼه_Invocation_Unmap(cap_t cap, cte_t *ct_خونه)
{
    إذا (cap_چوکاټ_cap_get_cap_FMapped_ASID(cap) != 0) {

        unmap_پاڼه(cap_چوکاټ_cap_get_cap_FSize(cap),
                  cap_چوکاټ_cap_get_cap_FMapped_ASID(cap),
                  cap_چوکاټ_cap_get_cap_FMapped_پته(cap),
                  cap_چوکاټ_cap_get_cap_FBase_Ptr(cap));
    }

    cap_t خونه_Cap = ct_خونه->cap;
    خونه_Cap = cap_چوکاټ_cap_سېټ_cap_FMapped_پته(خونه_Cap, 0);
    خونه_Cap = cap_چوکاټ_cap_سېټ_cap_FMapped_ASID(خونه_Cap, asidInvalid);
    ct_خونه->cap = خونه_Cap;


    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_پاڼه_Flush(کلمه_t invLabel, vspace_ریښه_t *vspace_ریښه_2, asid_t asid,
                                    vptr_t پیل, vptr_t پای_ټکی, paddr_t pstart)
{
    إذا (config_سېټ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* We flush the cache with kernel virtual addresses since
         * the user virtual addresses are not valid in EL2.
         * Switching VMRoot is not required.
         */
        کلمه_t اندازه = پای_ټکی - پیل;
        پیل = (vptr_t)paddr_منزل_pptr(pstart);
        پای_ټکی = پیل + اندازه;

        إذا (پیل < پای_ټکی) {
            doFlush(invLabel, پیل, پای_ټکی, pstart);
        }
    } وإلا {
        bool_t ریښه_switched;

        إذا (پیل < پای_ټکی) {
            ریښه_switched = سېټ_VMRoot_For_Flush(vspace_ریښه_2, asid);
            doFlush(invLabel, پیل, پای_ټکی, pstart);
            إذا (ریښه_switched) {
                سېټ_VMRoot(غوټه_STATE(ksCurThread));
            }
        }
    }
    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_پاڼه_Get_پته(pptr_t بنسټ_ptr, bool_t call)
{
    paddr_t بنسټ = pptr_منزل_paddr((فراغ *)بنسټ_ptr);

    tcb_t *thread;
    thread = غوټه_STATE(ksCurThread);
    إذا (call) {
        کلمه_t *ipc_بفر = lookupIPCBuffer(true, thread);
        سېټ_ثبتوونکی(thread, badge_ثبتوونکی, 0);
        بینښه صحيح اوږدتیا = سېټ_MR(thread, ipc_بفر, 0, بنسټ);
        سېټ_ثبتوونکی(thread, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, اوږدتیا)));
    }
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDControlInvocation(فراغ *چوکاټ, cte_t *خونه,
                                                cte_t *پلار, asid_t asid_بنسټ)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_سېټ_cap_Free_شاخص(&(پلار->cap),
                                         MAX_FREE_شاخص(cap_untyped_cap_get_cap_بلاک_اندازه(پلار->cap)));

    memzero(چوکاټ, بټ(se_L4_ASIDPool_بټونه));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ورننباسه(
        cap_asid_pool_cap_new(
            asid_بنسټ,         /* capASIDBase  */
            کلمه_REF(چوکاټ)    /* capASIDPool  */
        ), پلار, خونه);

    أكد((asid_بنسټ & ماسک(asid_ټیټ_بټونه)) == 0);
    armKSASIDTable[ASID_لوړ(asid_بنسټ)] = (asid_pool_t *)چوکاټ;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_ARMVSpace_ریښه_Invocation(کلمه_t invLabel, کلمه_t اوږدتیا,
                                                 cte_t *cte, cap_t cap, کلمه_t *بفر)
{
    vptr_t پیل, پای_ټکی;
    paddr_t pstart;
    asid_t asid;
    vspace_ریښه_t *vspace_ریښه_2;
    lookupPTSlot_ret_t resolve_ret;
    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret;
    pte_t pte;

    بدل (invLabel) {
    حالة ARMVSpace_Clean_معلومات:
    حالة ARMVSpace_Invalidate_معلومات:
    حالة ARMVSpace_Clean_Invalidate_معلومات:
    حالة ARMVSpaceUnify_Instruction:

        إذا (اوږدتیا < 2) {
            userError("VSpaceRoot Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        پیل = getSyscallArg(0, بفر);
        پای_ټکی =   getSyscallArg(1, بفر);

        /* Check sanity of arguments */
        إذا (پای_ټکی <= پیل) {
            userError("VSpaceRoot Flush: Invalid range.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_شمېره = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        إذا (پای_ټکی > USER_سر) {
            userError("VSpaceRoot Flush: Exceed the user addressable region.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(!is_Valid_Native_ریښه(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Make sure that the supplied pgd is ok */
        vspace_ریښه_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap));
        asid = cap_vspace_cap_get_capVSMappedASID(cap);

        پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
        إذا (unlikely(پیدا_کړه_ret.حالت != EXCEPTION_NONE)) {
            userError("VSpaceRoot Flush: No VSpace for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(پیدا_کړه_ret.vspace_ریښه != vspace_ریښه_2)) {
            userError("VSpaceRoot Flush: Invalid VSpace Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = lookupPTSlot(vspace_ریښه_2, پیل);
        pte = *resolve_ret.pt_خونه;

        /* Check that the returned slot is a page. */
        إذا (!pte_is_پاڼه_type(pte)) {

            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
            أرجع EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        إذا (پړاو_ښکته(پیل, resolve_ret.pt_بټونه_کیڼ) != پړاو_ښکته(پای_ټکی - 1, resolve_ret.pt_بټونه_کیڼ)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = پیل;
            current_syscall_error.rangeErrorMax = پړاو_ښکته(پیل, resolve_ret.pt_بټونه_کیڼ) +
                                                  ماسک(resolve_ret.pt_بټونه_کیڼ);
            userError("VSpaceRoot Flush: cannot cross page boundary, valid range is [0x%lx..0x%lx)",
                      current_syscall_error.rangeErrorMin, current_syscall_error.rangeErrorMax);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When in EL1, the mapping must be write-able for ARMVSpaceInvalidate_Data */
        إذا (invLabel == ARMVSpace_Invalidate_معلومات && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMVSpaceInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#د_که_پای

        /* Calculate the physical start address. */
        paddr_t چوکاټ_بنسټ_2 = pte_get_پاڼه_بنسټ_پته(pte);

        pstart = چوکاټ_بنسټ_2 + (پیل & ماسک(resolve_ret.pt_بټونه_کیڼ));

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        إذا (pstart < PADDR_بنسټ || ((پای_ټکی - پیل) + pstart) > PADDR_سر) {
            userError("VSpaceRoot Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#د_که_پای

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performVSpaceFlush(invLabel, vspace_ریښه_2, asid, پیل, پای_ټکی - 1, pstart);

    افتراضي:
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}


ساكن exception_t decode_ARMPage_جدول_Invocation(کلمه_t invLabel, کلمه_t اوږدتیا,
                                                cte_t *cte, cap_t cap, کلمه_t *بفر)
{
    cap_t vspace_ریښه_Cap;
    vspace_ریښه_t *vspace_ریښه_2;
    pte_t pte;
    asid_t asid;
    vptr_t vaddr;
    lookupPTSlot_ret_t pt_خونه;
    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret;

    إذا (invLabel == ARMPage_جدول_Unmap) {
        إذا (unlikely(!isFinalCapability(cte))) {
            current_syscall_error.type = se_L4_Revoke_لومړی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_پاڼه_جدول_Invocation_Unmap(cap, cte);
    }

    إذا (unlikely(invLabel != ARMPage_جدول_Map)) {
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(اوږدتیا < 2 || current_extra_caps.excaprefs[0] == عدم)) {
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر);
    vspace_ریښه_Cap = current_extra_caps.excaprefs[0]->cap;

    إذا (unlikely(!is_Valid_Native_ریښه(vspace_ریښه_Cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vspace_ریښه_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_ریښه_Cap));
    asid = cap_vspace_cap_get_capVSMappedASID(vspace_ریښه_Cap);

    إذا (unlikely(vaddr > USER_سر)) {
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
    إذا (unlikely(پیدا_کړه_ret.حالت != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(پیدا_کړه_ret.vspace_ریښه != vspace_ریښه_2)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pt_خونه = lookupPTSlot(vspace_ریښه_2, vaddr);

    إذا (unlikely(pt_خونه.pt_بټونه_کیڼ == se_L4_پاڼه_بټونه || pte_ptr_get_valid(pt_خونه.pt_خونه))) {
        current_syscall_error.type = se_L4_Delete_لومړی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pte = pte_pte_جدول_new(pptr_منزل_paddr(PTE_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(cap))));

    cap = cap_پاڼه_جدول_cap_سېټ_cap_PTIs_Mapped(cap, 1);
    cap = cap_پاڼه_جدول_cap_سېټ_cap_PTMapped_ASID(cap, asid);
    cap = cap_پاڼه_جدول_cap_سېټ_cap_PTMapped_پته(cap, (vaddr & ~ماسک(pt_خونه.pt_بټونه_کیڼ)));

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_پاڼه_جدول_Invocation_Map(cap, cte, pte, pt_خونه.pt_خونه);
}

ساكن مضمن bool_t CONST checkVPAlignment(vm_پاڼه_اندازه_t sz, کلمه_t w)
{
    أرجع (w & ماسک(پاڼه_بټونه_For_اندازه(sz))) == 0;
}

ساكن exception_t decodeARMFrameInvocation(کلمه_t invLabel, کلمه_t اوږدتیا,
                                            cte_t *cte, cap_t cap, bool_t call, کلمه_t *بفر)
{
    بدل (invLabel) {
    حالة ARMPageMap: {
        vptr_t vaddr;
        paddr_t بنسټ;
        cap_t vspace_ریښه_Cap;
        vspace_ریښه_t *vspace_ریښه_2;
        asid_t asid, چوکاټ_asid;
        vm_rights_t vmRights, capRights;
        vm_پاڼه_اندازه_t چوکاټ_اندازه;
        vm_ځانګړتیاوې_t ځانګړتیاوې;
        پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret;

        إذا (unlikely(اوږدتیا < 3 || current_extra_caps.excaprefs[0] == عدم)) {
            current_syscall_error.type = se_L4_Truncated_پیغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, بفر);
        ځانګړتیاوې = vm_ځانګړتیاوې_From_کلمه(getSyscallArg(2, بفر));
        vspace_ریښه_Cap = current_extra_caps.excaprefs[0]->cap;

        چوکاټ_اندازه = cap_چوکاټ_cap_get_cap_FSize(cap);
        capRights = cap_چوکاټ_cap_get_cap_FVMRights(cap);
        vmRights = ماسک_VMRights(capRights, rights_From_کلمه(getSyscallArg(1, بفر)));

        إذا (unlikely(!is_Valid_Native_ریښه(vspace_ریښه_Cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_ریښه_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_ریښه_Cap));
        asid = cap_vspace_cap_get_capVSMappedASID(vspace_ریښه_Cap);

        پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
        إذا (unlikely(پیدا_کړه_ret.حالت != EXCEPTION_NONE)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(پیدا_کړه_ret.vspace_ریښه != vspace_ریښه_2)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(!checkVPAlignment(چوکاټ_اندازه, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* In the case of remap, the cap should have a valid asid */
        چوکاټ_asid = cap_چوکاټ_cap_get_cap_FMapped_ASID(cap);

        إذا (چوکاټ_asid != asidInvalid) {
            إذا (چوکاټ_asid != asid) {
                userError("ARMPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_شمېره = 1;
                أرجع EXCEPTION_SYSCALL_ERROR;

            } وإلا إذا (cap_چوکاټ_cap_get_cap_FMapped_پته(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_شمېره = 0;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        } وإلا {
            إذا (unlikely(vaddr + بټ(پاڼه_بټونه_For_اندازه(چوکاټ_اندازه)) - 1 > USER_سر)) {
                current_syscall_error.type = se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_شمېره = 0;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        cap = cap_چوکاټ_cap_سېټ_cap_FMapped_ASID(cap, asid);
        cap = cap_چوکاټ_cap_سېټ_cap_FMapped_پته(cap, vaddr);

        بنسټ = pptr_منزل_paddr((فراغ *)cap_چوکاټ_cap_get_cap_FBase_Ptr(cap));

        lookupPTSlot_ret_t lu_ret = lookupPTSlot(vspace_ریښه_2, vaddr);
        إذا (unlikely(lu_ret.pt_بټونه_کیڼ != پاڼه_بټونه_For_اندازه(چوکاټ_اندازه))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_بټونه_کیڼ);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        bool_t cap_read = (capRights == VMReadOnly || capRights == VMReadWrite);
        pte_t pte = make_User_پاڼه_PTE(بنسټ, vmRights, cap_read, ځانګړتیاوې, چوکاټ_اندازه);

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_پاڼه_Invocation_Map(asid, cap, cte, pte, lu_ret.pt_خونه);
    }

    حالة ARMPageUnmap:
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_پاڼه_Invocation_Unmap(cap, cte);

    حالة ARMPage_Clean_معلومات:
    حالة ARMPage_Invalidate_معلومات:
    حالة ARMPage_Clean_Invalidate_معلومات:
    حالة ARMPageUnify_Instruction: {
        vptr_t پیل, پای_ټکی;
        vptr_t vaddr;
        asid_t asid;
        کلمه_t پاڼه_اندازه;
        پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret;

        إذا (اوږدتیا < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(cap_چوکاټ_cap_get_cap_FMapped_ASID(cap) == 0)) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = cap_چوکاټ_cap_get_cap_FMapped_پته(cap);
        asid = cap_چوکاټ_cap_get_cap_FMapped_ASID(cap);

        پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
        إذا (unlikely(پیدا_کړه_ret.حالت != EXCEPTION_NONE)) {
            userError("Page Flush: No PGD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        پیل = getSyscallArg(0, بفر);
        پای_ټکی =   getSyscallArg(1, بفر);

        /* check that the range is sane */
        إذا (پای_ټکی <= پیل) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_شمېره = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* start and end are currently relative inside this page */
        پاڼه_اندازه = بټ(پاڼه_بټونه_For_اندازه(cap_چوکاټ_cap_get_cap_FSize(cap)));
        إذا (پیل >= پاڼه_اندازه || پای_ټکی > پاڼه_اندازه) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        کلمه_t pstart = pptr_منزل_paddr((فراغ *)cap_چوکاټ_cap_get_cap_FBase_Ptr(cap)) + پیل;
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        إذا (pstart < PADDR_بنسټ || ((پای_ټکی - پیل) + pstart) > PADDR_سر) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#وإلا
        /* When in EL1, we are using the user page table for flushing and need to make sure
           the mapping info in the cap is not stale. */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(پیدا_کړه_ret.vspace_ریښه, vaddr);
        pte_t pte = *lu_ret.pt_خونه;
        فراغ *بنسټ_ptr = (فراغ *) cap_چوکاټ_cap_get_cap_FBase_Ptr(cap);
        إذا (unlikely(lu_ret.pt_بټونه_کیڼ != پاڼه_بټونه_For_اندازه(cap_چوکاټ_cap_get_cap_FSize(cap)) ||
                     !pte_is_پاڼه_type(pte) ||
                     pte_get_پاڼه_بنسټ_پته(pte) != pptr_منزل_paddr(بنسټ_ptr))) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* When in EL1, the mapping must be writeable for DC IVAC */
        إذا (invLabel == ARMPage_Invalidate_معلومات && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMPageInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#د_که_پای
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_پاڼه_Flush(invLabel, پیدا_کړه_ret.vspace_ریښه, asid, vaddr + پیل, vaddr + پای_ټکی - 1,
                                pstart);
    }

    حالة ARMPage_Get_پته:
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_پاڼه_Get_پته(cap_چوکاټ_cap_get_cap_FBase_Ptr(cap), call);

    افتراضي:
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(کلمه_t invLabel, کلمه_t اوږدتیا, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, کلمه_t *بفر)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_vspace_cap:
        أرجع decode_ARMVSpace_ریښه_Invocation(invLabel, اوږدتیا, cte, cap, بفر);
    حالة cap_پاڼه_جدول_cap:
        أرجع decode_ARMPage_جدول_Invocation(invLabel, اوږدتیا, cte, cap, بفر);

    حالة cap_چوکاټ_cap:
        أرجع decodeARMFrameInvocation(invLabel, اوږدتیا, cte, cap, call, بفر);

    حالة cap_asid_control_cap: {
        کلمه_t i;
        asid_t asid_بنسټ;
        کلمه_t شاخص, ژوروالی;
        cap_t untyped, ریښه;
        cte_t *پلار_خونه, *dest_خونه;
        lookup_خونه_ret_t lu_ret;
        فراغ *چوکاټ;
        exception_t حالت;

        إذا (unlikely(invLabel != ARMASIDControlMakePool)) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(اوږدتیا < 2 ||
                     current_extra_caps.excaprefs[0] == عدم ||
                     current_extra_caps.excaprefs[1] == عدم)) {
            current_syscall_error.type = se_L4_Truncated_پیغام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شاخص = getSyscallArg(0, بفر);
        ژوروالی = getSyscallArg(1, بفر);
        پلار_خونه = current_extra_caps.excaprefs[0];
        untyped = پلار_خونه->cap;
        ریښه = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        لكل (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        إذا (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_لومړی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_بنسټ = i << asid_ټیټ_بټونه;

        إذا (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_بلاک_اندازه(untyped) != se_L4_ASIDPool_بټونه ||
                     cap_untyped_cap_get_capIsDevice(untyped))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        حالت = ensureNoChildren(پلار_خونه);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            أرجع حالت;
        }

        چوکاټ = کلمه_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_موخه_خونه(ریښه, شاخص, ژوروالی);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            أرجع lu_ret.حالت;
        }
        dest_خونه = lu_ret.خونه;

        حالت = ensure_تشه_خونه(dest_خونه);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            أرجع حالت;
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDControlInvocation(چوکاټ, dest_خونه, پلار_خونه, asid_بنسټ);
    }

    حالة cap_asid_pool_cap: {
        cap_t vspaceCap;
        cte_t *vspace_Cap_خونه;
        asid_pool_t *pool;
        کلمه_t i;
        asid_t asid;

        إذا (unlikely(invLabel != ARMASIDPool_ټاکل)) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
            current_syscall_error.type = se_L4_Truncated_پیغام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_خونه = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_خونه->cap;

        إذا (unlikely(!is_VTable_ریښه(vspaceCap) || cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_لوړ(cap_asid_pool_cap_get_capASIDBase(cap))];

        إذا (unlikely(!pool)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = false;
            current_lookup_fault = lookup_fault_invalid_ریښه_new();

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        لكل (i = 0; i < (1 << asid_ټیټ_بټونه) && (asid + i == 0
                                               || (asid_map_get_type(pool->array[i]) != asid_map_asid_map_none)); i++);

        إذا (unlikely(i == 1 << asid_ټیټ_بټونه)) {
            current_syscall_error.type = se_L4_Delete_لومړی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDPoolInvocation(asid, pool, vspace_Cap_خونه);
    }

    افتراضي:
        fail("Invalid ARM arch cap type");
    }
}

#که_تعریف_شوی CONFIG_DEBUG_BUILD
فراغ kernelPrefetchAbort(کلمه_t pc) VISIBLE;
فراغ kernel_معلومات_Abort(کلمه_t pc) VISIBLE;

فراغ kernelPrefetchAbort(کلمه_t pc)
{
    چاپ_کړه("\n\nKERNEL PREFETCH ABORT!\n");
    چاپ_کړه("Faulting instruction: 0x%"SEL4_PRIx_کلمه"\n", pc);
    چاپ_کړه("ESR (IFSR): 0x%"SEL4_PRIx_کلمه"\n", getIFSR());
    halt();
}

فراغ kernel_معلومات_Abort(کلمه_t pc)
{
    چاپ_کړه("\n\nKERNEL DATA ABORT!\n");
    چاپ_کړه("Faulting instruction: 0x%"SEL4_PRIx_کلمه"\n", pc);
    چاپ_کړه("FAR: 0x%"SEL4_PRIx_کلمه" ESR (DFSR): 0x%"SEL4_PRIx_کلمه"\n",
           getFAR(), getDFSR());
    halt();
}
#د_که_پای /* CONFIG_DEBUG_BUILD */

#که_تعریف_شوی CONFIG_PRINTING
تعريفنوع بنية read_کلمه_From_VSpace_ret {
    exception_t حالت;
    کلمه_t ارزښت;
} read_کلمه_From_VSpace_ret_t;

ساكن read_کلمه_From_VSpace_ret_t read_کلمه_From_VSpace(vspace_ریښه_t *pd, کلمه_t vaddr)
{
    read_کلمه_From_VSpace_ret_t ret;
    کلمه_t دننه_واټن;
    pptr_t kernel_vaddr;
    کلمه_t *ارزښت;

    lookupPTSlot_ret_t lookup_ret = lookupPTSlot(pd, vaddr);

    /* Check that the returned slot is a page. */
    إذا (!pte_ptr_get_valid(lookup_ret.pt_خونه) ||
        (pte_pte_جدول_ptr_get_present(lookup_ret.pt_خونه) && lookup_ret.pt_بټونه_کیڼ > پاڼه_بټونه)) {
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    دننه_واټن = vaddr & ماسک(lookup_ret.pt_بټونه_کیڼ);
    kernel_vaddr = (کلمه_t)paddr_منزل_pptr(pte_پاڼه_ptr_get_پاڼه_بنسټ_پته(lookup_ret.pt_خونه));
    ارزښت = (کلمه_t *)(kernel_vaddr + دننه_واټن);

    ret.حالت = EXCEPTION_NONE;
    ret.ارزښت = *ارزښت;
    أرجع ret;
}

فراغ Arch_user_ډېری_Trace(tcb_t *tptr)
{
    cap_t thread_ریښه;
    vspace_ریښه_t *vspace_ریښه_2;
    کلمه_t sp;

    thread_ریښه = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the vspace root */
    إذا (cap_get_capType(thread_ریښه) != cap_vspace_cap) {
        چاپ_کړه("Invalid vspace\n");
        أرجع;
    }

    vspace_ریښه_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_ریښه));
    sp = get_ثبتوونکی(tptr, SP_EL0);

    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    إذا (!IS_ALIGNED(sp, se_L4_کلمه_اندازه_بټونه)) {
        چاپ_کړه("SP not aligned\n");
        أرجع;
    }

    لكل (بینښه صحيح i = 0; i < CONFIG_USER_ډېری_TRACE_اوږدتیا; i++) {
        کلمه_t پته = sp + (i * حجم(کلمه_t));
        read_کلمه_From_VSpace_ret_t پایله = read_کلمه_From_VSpace(vspace_ریښه_2,
                                                             پته);
        إذا (پایله.حالت == EXCEPTION_NONE) {
            چاپ_کړه("0x%"SEL4_PRIx_کلمه": 0x%"SEL4_PRIx_کلمه"\n",
                   پته, پایله.ارزښت);
        } وإلا {
            چاپ_کړه("0x%"SEL4_PRIx_کلمه": INVALID\n", پته);
        }
    }
}
#د_که_پای /* CONFIG_PRINTING */

#إذا defined(CONFIG_KERNEL_LOG_بفر)
exception_t benchmark_arch_map_log_بفر(کلمه_t چوکاټ_cptr)
{
    lookup_Cap_And_خونه_ret_t lu_ret;
    vm_پاڼه_اندازه_t چوکاټ_اندازه;
    pptr_t  چوکاټ_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_خونه(غوټه_STATE(ksCurThread), چوکاټ_cptr);

    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", چوکاټ_cptr);
        current_fault = seL4_Fault_CapFault_new(چوکاټ_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(lu_ret.cap) != cap_چوکاټ_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(چوکاټ_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکاټ_اندازه = cap_چوکاټ_cap_get_cap_FSize(lu_ret.cap);

    إذا (چوکاټ_اندازه != ARMLarge_پاڼه) {
        userError("Invalid frame size. The kernel expects 2M log buffer");
        current_fault = seL4_Fault_CapFault_new(چوکاټ_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکاټ_pptr = cap_چوکاټ_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_بفر = pptr_منزل_paddr((فراغ *) چوکاټ_pptr);

    *armKSGlobalLogPTE = pte_pte_پاڼه_new(
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
                             0, // XN
#وإلا
                             1, // UXN
#د_که_پای
                             ks_User_Log_بفر,
                             0,                         /* global */
                             1,                         /* access flag */
                             SMP_TERNARY(SMP_SHARE, 0), /* Inner-shareable if SMP enabled, otherwise unshared */
                             0,                         /* VMKernelOnly */
                             نورمال_WT);

    cleanByVA_PoU((vptr_t)armKSGlobalLogPTE, addrFromKPPtr(armKSGlobalLogPTE));
    invalidateTranslationSingle(KS_LOG_PPTR);
    أرجع EXCEPTION_NONE;
}
#د_که_پای /* CONFIG_KERNEL_LOG_BUFFER */
