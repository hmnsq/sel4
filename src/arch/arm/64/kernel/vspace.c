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
    نرمال_NC = 3,
    نرمال = 4,
    نرمال_WT = 5
};

/* Stage-2 translation memory attributes */
تعداد mair_s2_types {
    S2_DEVICE_nGnRnE = 0,
    S2_DEVICE_nGnRE = 1,
    S2_DEVICE_nGRE  = 2,
    S2_DEVICE_GRE = 3,

    S2_نرمال_INNER_NC_OUTER_NC = 5,
    S2_نرمال_INNER_WTC_OUTER_NC = 6,
    S2_نرمال_INNER_WBC_OUTER_NC = 7,

    S2_نرمال_INNER_NC_OUTER_WTC = 9,
    S2_نرمال_INNER_WTC_OUTER_WTC = 10,
    S2_نرمال_INNER_WBC_OUTER_WTC = 11,

    S2_نرمال_INNER_NC_OUTER_WBC = 13,
    S2_نرمال_INNER_WTC_OUTER_WBC = 14,
    S2_نرمال_INNER_WBC_OUTER_WBC = 15,

    S2_نرمال = S2_نرمال_INNER_WBC_OUTER_WBC
};

/* Leif from Linaro said the big.LITTLE clusters should be treated as
 * inner shareable, and we believe so, although the Example B2-1 given in
 * ARM ARM DDI 0487B.b (ID092517) says otherwise.
 */

#تعریف_کن SMP_SHARE   3

بنية lookupPTSlot_ret {
    pte_t *pt_جایگاه;
    کلمه_t pt_بیتها_چپ;
};
تعريفنوع بنية lookupPTSlot_ret lookupPTSlot_ret_t;

بنية پیدا_کن_VSpace_For_ASID_ret {
    exception_t وضعیت;
    vspace_ریشه_t *vspace_ریشه;
};
تعريفنوع بنية پیدا_کن_VSpace_For_ASID_ret پیدا_کن_VSpace_For_ASID_ret_t;

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
        إذا (config_مجموعه(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            أرجع 0;
        } وإلا {
            أرجع 0;
        }

    حالة VMReadWrite:
        إذا (config_مجموعه(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            أرجع 3;
        } وإلا {
            أرجع 1;
        }

    حالة VMReadOnly:
        إذا (config_مجموعه(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            أرجع 1;
        } وإلا {
            أرجع 3;
        }

    افتراضي:
        fail("Invalid VM rights");
    }
}

#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
ساكن مضمن CONST کلمه_t pte_get_AP(pte_t pte)
{
    أكد(pte_is_صفحه_type(pte));
    بدل (pte_get_pte_type(pte)) {
    حالة pte_pte_4k_صفحه:
        أرجع pte_pte_4k_صفحه_get_AP(pte);

    حالة pte_pte_صفحه:
        أرجع pte_pte_صفحه_get_AP(pte);

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
#پایان_اگر

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
BOOT_کد فراغ map_kernel_قاب(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_صفات_t صفات)
{
    أكد(vaddr >= PPTR_بالاترین);

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    کلمه_t uxn = vm_صفات_get_arm_Execute_Never(صفات);
#وإلا
    کلمه_t uxn = 1; /* unprivileged execute never */
#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    کلمه_t attr_شاخص;
    کلمه_t shareable;
    إذا (vm_صفات_get_arm_صفحه_Cacheable(صفات)) {
        attr_شاخص = نرمال;
        shareable = SMP_TERNARY(SMP_SHARE, 0);
    } وإلا {
        attr_شاخص = DEVICE_nGnRnE;
        shareable = 0;
    }
    armKSGlobalKernelPT[GET_KPT_شاخص(vaddr, KLVL_FRM_ARM_PT_LVL(3))] = pte_pte_4k_صفحه_new(uxn, paddr,
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
compile_assert(elf_درون_kernel_mappings, GET_KPT_شاخص(KERNEL_ELF_پایه,
                                                     KLVL_FRM_ARM_PT_LVL(1)) == بیت(PT_شاخص_بیتها) - 1);
compile_assert(kdev_درون_kernel_mappings, GET_KPT_شاخص(KDEV_پایه,
                                                      KLVL_FRM_ARM_PT_LVL(1)) == بیت(PT_شاخص_بیتها) - 1);
compile_assert(log_buf_درون_kernel_mappings, GET_KPT_شاخص(KS_LOG_پایه,
                                                         KLVL_FRM_ARM_PT_LVL(1)) == بیت(PT_شاخص_بیتها) - 1);

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
BOOT_کد فراغ map_kernel_elf_image(pte_t kernel_mapping_pds[])
{
    /** This is not strictly necessary, but to handle a non-aligned KERNEL_ELF_PADDR_BASE
     *  we'd need to do a bit more logic and probably do what riscv64 does
     *  where the *actual* KERNEL_ELF_BASE is rounded up to the alignment.
     *  This is simpler and memory is cheap.
     **/
    compile_assert(elf_paddr_aligned, IS_ALIGNED(KERNEL_ELF_PADDR_پایه_RAW, se_L4_Large_صفحه_بیتها));
    compile_assert(elf_پایه_aligned, IS_ALIGNED(KERNEL_ELF_پایه, se_L4_Large_صفحه_بیتها));

    /* Please don't overlap with KDEV_BASE */
    أكد(KERNEL_ELF_بالاترین <= KDEV_پایه);
    /* Please don't overlap with KS_LOG_PPTR */
    أكد(KERNEL_ELF_بالاترین <= KS_LOG_PPTR);

    ثابت کلمه_t pd_شروع = GET_KPT_شاخص(KERNEL_ELF_پایه, KLVL_FRM_ARM_PT_LVL(2));
    /* pd_end and ELF_TOP are both exclusive, so handle that correctly */
    ثابت کلمه_t pd_انتها   = GET_KPT_شاخص(KERNEL_ELF_بالاترین - 1, KLVL_FRM_ARM_PT_LVL(2)) + 1;

    paddr_t paddr = KERNEL_ELF_PADDR_پایه;
    استفادهنشده pptr_t vaddr = KERNEL_ELF_پایه;

    لكل (کلمه_t pd = pd_شروع; pd < pd_انتها; pd++) {
        أكد(pd == GET_KPT_شاخص(vaddr, KLVL_FRM_ARM_PT_LVL(2)));

        kernel_mapping_pds[pd] = pte_pte_صفحه_new(
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
                                     /* XN */ 0,
#وإلا
                                     /* UXN */ 1,
#پایان_اگر
                                     /* page_base_address */ paddr,
                                     /* global (nG) */ 0,
                                     /* access flag (AF) */ 1,
                                     /* shareability (SH) */ SMP_TERNARY(SMP_SHARE, 0),
                                     /* AP */ APFromVMRights(VMKernelOnly),
                                     /* AttrIndx */ نرمال
                                 );

        paddr += بیت(se_L4_Large_صفحه_بیتها);
        vaddr += بیت(se_L4_Large_صفحه_بیتها);
    }
}

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
/* verify that the physical memory window as at the second entry of the PGD */
compile_assert(physical_پنجره_pgd_location,
               GET_KPT_شاخص(PPTR_پایه, KLVL_FRM_ARM_PT_LVL(0)) == 1);
#وإلا
/* verify that the physical memory window as at the last entry of the PGD */
compile_assert(physical_پنجره_pgd_location,
               GET_KPT_شاخص(PPTR_پایه, KLVL_FRM_ARM_PT_LVL(0)) == بیت(PT_شاخص_بیتها) - 1);
#پایان_اگر
/* make sure that the PPTR_BASE and PPTR_TOP are huge paged aligned */
compile_assert(physical_پنجره_پایه_aligned, IS_ALIGNED(PPTR_پایه, se_L4_Huge_صفحه_بیتها));
compile_assert(physical_پنجره_بالاترین_aligned, IS_ALIGNED(PPTR_بالاترین, se_L4_Huge_صفحه_بیتها));

/* the PPTR_BASE starts from the bottom of the kernel PUD */
compile_assert(physical_پنجره_bottom_kernel_region,
               GET_KPT_شاخص(PPTR_پایه, KLVL_FRM_ARM_PT_LVL(1)) == 0);
/* the PPTR_TOP then ends at the second-to-last entry of the PUD
    note: because this is the exclusive value the assert is one higher than
          you would expect.
*/
compile_assert(physical_پنجره_بالاترین_kernel_region,
               GET_KPT_شاخص(PPTR_بالاترین, KLVL_FRM_ARM_PT_LVL(1)) == بیت(PT_شاخص_بیتها) - 1);

BOOT_کد فراغ map_kernel_physical_پنجره(فراغ)
{
    paddr_t paddr;
    pptr_t vaddr;
    کلمه_t idx;

    /* place all PDs except the last one in PUD */
    لكل (idx = GET_KPT_شاخص(PPTR_پایه, KLVL_FRM_ARM_PT_LVL(1)); idx < GET_KPT_شاخص(PPTR_بالاترین, KLVL_FRM_ARM_PT_LVL(1));
         idx++) {
        armKSGlobalKernelPUD[idx] = pte_pte_جدول_new(
                                        addrFromKPPtr(&armKSGlobalKernelPDs[idx][0])
                                    );
    }

    /* map the kernel window using large pages */
    vaddr = PPTR_پایه;
    لكل (paddr = PADDR_پایه; paddr < PADDR_بالاترین; paddr += بیت(se_L4_Large_صفحه_بیتها)) {
        armKSGlobalKernelPDs[GET_KPT_شاخص(vaddr, KLVL_FRM_ARM_PT_LVL(1))][GET_KPT_شاخص(vaddr,
                                                                                         KLVL_FRM_ARM_PT_LVL(2))] = pte_pte_صفحه_new(
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
                                                                                                                        1, // XN
#وإلا
                                                                                                                        1, // UXN
#پایان_اگر
                                                                                                                        paddr,
                                                                                                                        0,                        /* global */
                                                                                                                        1,                        /* access flag */
                                                                                                                        SMP_TERNARY(SMP_SHARE, 0),        /* Inner-shareable if SMP enabled, otherwise unshared */
                                                                                                                        0,                        /* VMKernelOnly */
                                                                                                                        نرمال
                                                                                                                    );
        vaddr += بیت(se_L4_Large_صفحه_بیتها);
    }
}

BOOT_کد فراغ map_kernel_پنجره(فراغ)
{
    /* place the PUD into the PGD */
    armKSGlobalKernelPGD[GET_KPT_شاخص(PPTR_پایه, KLVL_FRM_ARM_PT_LVL(0))] =
        pte_pte_جدول_new(addrFromKPPtr(armKSGlobalKernelPUD));

    map_kernel_physical_پنجره();

    ثابت کلمه_t elf_pud_idx = GET_KPT_شاخص(KERNEL_ELF_پایه, KLVL_FRM_ARM_PT_LVL(1));
    pte_t *kernel_mapping_pds = armKSGlobalKernelPDs[elf_pud_idx];

    /* place the kernel mapping PDs in the kernel mapping PUD */
    armKSGlobalKernelPUD[elf_pud_idx] = pte_pte_جدول_new(addrFromKPPtr(kernel_mapping_pds));

    /* map the kernel ELF */
    map_kernel_elf_image(kernel_mapping_pds);

    /* put the PT into the PD for device window. It would be nice if
       we could pass kernel_mapping_pds[511] to map_kernel_devices(), but it is
       shared between AArch64 and AArch32, so... */
    kernel_mapping_pds[بیت(PT_شاخص_بیتها) - 1]
        = pte_pte_جدول_new(addrFromKPPtr(armKSGlobalKernelPT));

    map_kernel_devices();
}

/* When the hypervisor support is enabled, the stage-2 translation table format
 * is used for applications.
 * The global bit is always 0.
 * The memory attributes use the S2 translation values.
 */
ساكن BOOT_کد فراغ map_it_قاب_cap(cap_t vspace_cap, cap_t قاب_cap, bool_t executable)
{
    vspace_ریشه_t *vspace_ریشه_2 = VSPACE_PTR(pptr_از_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt;

    vptr_t vptr = cap_قاب_cap_get_cap_FMapped_آدرس(قاب_cap);
    فراغ *pptr = (فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(قاب_cap);

    أكد(cap_قاب_cap_get_cap_FMapped_ASID(قاب_cap) != 0);

#اگر_تعریف_شده AARCH64_VSPACE_S2_شروع_L1
    pud = vspace_ریشه_2;
#وإلا
    vspace_ریشه_2 += GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(0));
    أكد(pte_pte_جدول_ptr_get_present(vspace_ریشه_2));
    pud = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_پایه_آدرس(vspace_ریشه_2));
#پایان_اگر
    pud += GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(1));
    أكد(pte_pte_جدول_ptr_get_present(pud));
    pd = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_پایه_آدرس(pud));
    pd += GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(2));
    أكد(pte_pte_جدول_ptr_get_present(pd));
    pt = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_پایه_آدرس(pd));
    *(pt + GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(3))) = pte_pte_4k_صفحه_new(
                                                              !executable,                    /* unprivileged execute never */
                                                              pptr_مقصد_paddr(pptr),            /* page_base_address    */
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              0,
#وإلا
                                                              1,                              /* not global */
#پایان_اگر
                                                              1,                              /* access flag */
                                                              SMP_TERNARY(SMP_SHARE, 0),              /* Inner-shareable if SMP enabled, otherwise unshared */
                                                              APFromVMRights(VMReadWrite),
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              S2_نرمال
#وإلا
                                                              نرمال
#پایان_اگر
                                                          );
}

ساكن BOOT_کد cap_t create_it_قاب_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    vm_صفحه_اندازه_t قاب_اندازه_2;
    إذا (use_large) {
        قاب_اندازه_2 = ARMLarge_صفحه;
    } وإلا {
        قاب_اندازه_2 = ARMSmall_صفحه;
    }
    أرجع
        cap_قاب_cap_new(
            asid,                          /* capFMappedASID */
            pptr,                          /* capFBasePtr */
            قاب_اندازه_2,                    /* capFSize */
            vptr,                          /* capFMappedAddress */
            کلمه_From_VMRights(VMReadWrite), /* capFVMRights */
            false                          /* capFIsDevice */
        );
}

ساكن BOOT_کد فراغ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    vspace_ریشه_t *vspace_ریشه_2 = VSPACE_PTR(pptr_از_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt = PT_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_صفحه_جدول_cap_get_cap_PTMapped_آدرس(pt_cap);

    أكد(cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(pt_cap));

#اگر_تعریف_شده AARCH64_VSPACE_S2_شروع_L1
    pud = vspace_ریشه_2;
#وإلا
    vspace_ریشه_2 += GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(0));
    أكد(pte_pte_جدول_ptr_get_present(vspace_ریشه_2));
    pud = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_پایه_آدرس(vspace_ریشه_2));
#پایان_اگر
    pud += GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(1));
    أكد(pte_pte_جدول_ptr_get_present(pud));
    pd = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_پایه_آدرس(pud));
    *(pd + GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(2))) = pte_pte_جدول_new(
                                                              pptr_مقصد_paddr(pt)
                                                          );
}

ساكن BOOT_کد cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحه_جدول_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pt_cap(vspace_cap, cap);
    أرجع cap;
}

ساكن BOOT_کد فراغ map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    vspace_ریشه_t *vspace_ریشه_2 = VSPACE_PTR(pptr_از_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd = PT_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(pd_cap));
    vptr_t vptr = cap_صفحه_جدول_cap_get_cap_PTMapped_آدرس(pd_cap);

    أكد(cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(pd_cap));

#اگر_تعریف_شده AARCH64_VSPACE_S2_شروع_L1
    pud = vspace_ریشه_2;
#وإلا
    vspace_ریشه_2 += GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(0));
    أكد(pte_pte_جدول_ptr_get_present(vspace_ریشه_2));
    pud = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_پایه_آدرس(vspace_ریشه_2));
#پایان_اگر
    *(pud + GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(1))) = pte_pte_جدول_new(
                                                               pptr_مقصد_paddr(pd)
                                                           );
}

ساكن BOOT_کد cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحه_جدول_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pd_cap(vspace_cap, cap);
    أرجع cap;
}

#اگر_تعریف_نشده AARCH64_VSPACE_S2_شروع_L1
ساكن BOOT_کد فراغ map_it_pud_cap(cap_t vspace_cap, cap_t pud_cap)
{
    pte_t *pgd = PT_PTR(pptr_از_cap(vspace_cap));
    pte_t *pud = PT_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(pud_cap));
    vptr_t vptr = cap_صفحه_جدول_cap_get_cap_PTMapped_آدرس(pud_cap);

    أكد(cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(pud_cap));

    *(pgd + GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(0))) = pte_pte_جدول_new(
                                                               pptr_مقصد_paddr(pud));
}

ساكن BOOT_کد cap_t create_it_pud_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحه_جدول_cap_new(
              asid,               /* capPTMappedASID */
              pptr,               /* capPTBasePtr */
              1,                  /* capPTIsMapped */
              vptr                /* capPTMappedAddress */
          );
    map_it_pud_cap(vspace_cap, cap);
    أرجع cap;
}
#پایان_اگر /* AARCH64_VSPACE_S2_START_L1 */
BOOT_کد کلمه_t arch_get_n_paging(v_region_t it_v_reg)
{
    أرجع
#اگر_تعریف_نشده AARCH64_VSPACE_S2_شروع_L1
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_بیتها(ULVL_FRM_ARM_PT_LVL(0))) +
#پایان_اگر
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_بیتها(ULVL_FRM_ARM_PT_LVL(1))) +
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_بیتها(ULVL_FRM_ARM_PT_LVL(2)));
}

BOOT_کد cap_t create_it_آدرس_space(cap_t ریشه_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_جایگاه_Pos جایگاه_pos_before;
    se_L4_جایگاه_Pos جایگاه_pos_after;

    /* create the PGD */
    vspace_cap = cap_vspace_cap_new(
                     IT_ASID,           /* capVSMappedASID */
                     rootserver.vspace, /* capVSBasePtr    */
                     1                  /* capVSIsMapped   */
#اگر_تعریف_شده CONFIG_ARM_SMMU
                     , CB_INVALID       /* capVSMappedCB   */
#پایان_اگر
                 );
    جایگاه_pos_before = ndks_boot.جایگاه_pos_cur;
    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

#اگر_تعریف_نشده AARCH64_VSPACE_S2_شروع_L1
    /* Create any PUDs needed for the user land image */
    لكل (vptr = دور_پایین(it_v_reg.شروع, GET_ULVL_PGSIZE_بیتها(ULVL_FRM_ARM_PT_LVL(0)));
         vptr < it_v_reg.انتها;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(0))) {
        إذا (!provide_cap(ریشه_cnode_cap, create_it_pud_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            أرجع cap_null_cap_new();
        }
    }
#پایان_اگر
    /* Create any PDs needed for the user land image */
    لكل (vptr = دور_پایین(it_v_reg.شروع, GET_ULVL_PGSIZE_بیتها(ULVL_FRM_ARM_PT_LVL(1)));
         vptr < it_v_reg.انتها;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))) {
        إذا (!provide_cap(ریشه_cnode_cap, create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            أرجع cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    لكل (vptr = دور_پایین(it_v_reg.شروع, GET_ULVL_PGSIZE_بیتها(ULVL_FRM_ARM_PT_LVL(2)));
         vptr < it_v_reg.انتها;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))) {
        إذا (!provide_cap(ریشه_cnode_cap, create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            أرجع cap_null_cap_new();
        }
    }

    جایگاه_pos_after = ndks_boot.جایگاه_pos_cur;
    ndks_boot.bi_قاب->userImagePaging = (se_L4_جایگاه_Region) {
        جایگاه_pos_before, جایگاه_pos_after
    };
    أرجع vspace_cap;
}

BOOT_کد cap_t create_unmapped_it_قاب_cap(pptr_t pptr, bool_t use_large)
{
    أرجع create_it_قاب_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_کد cap_t create_mapped_it_قاب_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_قاب_cap(pptr, vptr, asid, use_large);
    map_it_قاب_cap(pd_cap, cap, executable);
    أرجع cap;
}

BOOT_کد فراغ activate_kernel_vspace(فراغ)
{
    cleanInvalidateL1Caches();
    مجموعه_Current_Kernel_VSpace_ریشه(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalKernelPGD)));

    /* Prevent elf-loader address translation to fill up TLB */
    مجموعه_Current_User_VSpace_ریشه(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));

    invalidateLocalTLB();
    قفل_TLBEntry(KERNEL_ELF_پایه);
}

BOOT_کد فراغ write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_از_cap(it_ap_cap));
    asid_map_t asid_map = asid_map_asid_map_vspace_new(
#اگر_تعریف_شده CONFIG_ARM_SMMU
                              /* bind_cb: Number of bound context banks */
                              0,
#پایان_اگر
                              /* vspace_root: reference to vspace root page table object */
                              (کلمه_t)cap_vspace_cap_get_capVSBasePtr(it_vspace_cap)
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
                              /* stored_hw_vmid, stored_vmid_valid: Assigned hardware VMID for TLB. */
                              , 0, false
#پایان_اگر
                          );
    ap->array[ASID_کم(IT_ASID)] = asid_map;
    armKSASIDTable[ASID_بالا(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

asid_map_t پیدا_کن_Map_For_ASID(asid_t asid)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_بالا(asid)];
    إذا (!poolPtr) {
        أرجع asid_map_asid_map_none_new();
    }

    أرجع poolPtr->array[ASID_کم(asid)];
}

ساكن پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_VSpace_For_ASID(asid_t asid)
{
    پیدا_کن_VSpace_For_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = پیدا_کن_Map_For_ASID(asid);
    إذا (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_ریشه_new();

        ret.vspace_ریشه = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.vspace_ریشه = (vspace_ریشه_t *)asid_map_asid_map_vspace_get_vspace_ریشه(asid_map);
    ret.وضعیت = EXCEPTION_NONE;
    أرجع ret;
}

کلمه_t *خالص lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    کلمه_t w_میانگیر_Ptr;
    cap_t میانگیر_Cap;
    vm_rights_t vm_rights;

    w_میانگیر_Ptr = thread->tcbIPCBuffer;
    میانگیر_Cap = TCB_PTR_CTE_PTR(thread, tcb_میانگیر)->cap;

    إذا (unlikely(cap_get_capType(میانگیر_Cap) != cap_قاب_cap)) {
        أرجع عدم;
    }
    إذا (unlikely(cap_قاب_cap_get_cap_FIs_Device(میانگیر_Cap))) {
        أرجع عدم;
    }

    vm_rights = cap_قاب_cap_get_cap_FVMRights(میانگیر_Cap);
    إذا (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        کلمه_t پایه_Ptr;
        بیعلامت صحيح صفحه_بیتها_2;

        پایه_Ptr = cap_قاب_cap_get_cap_FBase_Ptr(میانگیر_Cap);
        صفحه_بیتها_2 = صفحه_بیتها_For_اندازه(cap_قاب_cap_get_cap_FSize(میانگیر_Cap));
        أرجع (کلمه_t *)(پایه_Ptr + (w_میانگیر_Ptr & ماسک(صفحه_بیتها_2)));
    } وإلا {
        أرجع عدم;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    إذا (cap_get_capType(cap) != cap_قاب_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(cap_قاب_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!IS_ALIGNED(vptr, se_L4_IPCBuffer_اندازه_بیتها)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (صحيح)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

ساكن lookupPTSlot_ret_t lookupPTSlot(vspace_ریشه_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    کلمه_t level = UPT_سطوح - 1;
    pte_t *pt = vspace;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_بیتها_چپ = PT_شاخص_بیتها * level + se_L4_صفحه_بیتها;
    ret.pt_جایگاه = pt + ((vptr >> ret.pt_بیتها_چپ) & ماسک(se_L4_VSpace_شاخص_بیتها));

    طالما (pte_pte_جدول_ptr_get_present(ret.pt_جایگاه) && likely(level > 0)) {
        level--;
        ret.pt_بیتها_چپ -= PT_شاخص_بیتها;
        pt = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_پایه_آدرس(ret.pt_جایگاه));
        ret.pt_جایگاه = pt + ((vptr >> ret.pt_بیتها_چپ) & ماسک(PT_شاخص_بیتها));
    }

    أرجع ret;
}

/* Note that if the hypervisor support is enabled, the user page tables use
 * stage-2 translation format. Otherwise, they follow the stage-1 translation format.
 */
ساكن pte_t make_User_صفحه_PTE(paddr_t paddr, vm_rights_t vm_rights, bool_t cap_read, vm_صفات_t صفات,
                             vm_صفحه_اندازه_t صفحه_اندازه)
{
    bool_t nonexecutable = vm_صفات_get_arm_Execute_Never(صفات) || !cap_read;
    کلمه_t cacheable = vm_صفات_get_arm_صفحه_Cacheable(صفات);

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    کلمه_t nG = 0; /* not global */
    کلمه_t attridx = cacheable ? S2_نرمال : S2_DEVICE_nGnRnE;
#وإلا
    کلمه_t nG = 1; /* not global */
    کلمه_t attridx = cacheable ? نرمال : DEVICE_nGnRnE;
#پایان_اگر

    /* Inner-shareable if SMP enabled, otherwise unshared (ignored for devices) */
    کلمه_t shareable = cacheable ? SMP_TERNARY(SMP_SHARE, 0) : 0;

    إذا (صفحه_اندازه == ARMSmall_صفحه) {
        أرجع pte_pte_4k_صفحه_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                   shareable, APFromVMRights(vm_rights), attridx);
    } وإلا {
        أرجع pte_pte_صفحه_new(nonexecutable, paddr, nG, 1 /* access flag */,
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

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        /* use the IPA */
        إذا (ARCH_گره_STATE(armHSVCPUActive)) {
            addr = GET_PAR_ADDR(آدرس_Translate_S1(addr)) | (addr & ماسک(صفحه_بیتها));
        }
#پایان_اگر
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        أرجع EXCEPTION_FAULT;
    }

    حالة ARMPrefetchAbort: {
        کلمه_t pc, fault;

        pc = getRestartPC(thread);
        fault = getIFSR();

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (ARCH_گره_STATE(armHSVCPUActive)) {
            pc = GET_PAR_ADDR(آدرس_Translate_S1(pc)) | (pc & ماسک(صفحه_بیتها));
        }
#پایان_اگر
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        أرجع EXCEPTION_FAULT;
    }

    افتراضي:
        fail("Invalid VM fault type");
    }
}

bool_t CONST is_VTable_ریشه(cap_t cap)
{
    أرجع cap_get_capType(cap) == cap_vspace_cap;
}

bool_t CONST is_Valid_Native_ریشه(cap_t cap)
{
    أرجع is_VTable_ریشه(cap) &&
           cap_vspace_cap_get_capVSIsMapped(cap);
}

bool_t CONST is_Valid_VTable_ریشه(cap_t cap)
{
    أرجع is_Valid_Native_ریشه(cap);
}

فراغ مجموعه_VMRoot(tcb_t *tcb)
{
    cap_t thread_ریشه;
    asid_t asid;
    vspace_ریشه_t *vspace_ریشه_2;
    پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret;

    thread_ریشه = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    /* ASID/VMID 0 is never allocated to a real VSpace (see findFreeHWASID()
     * for the hypervisor case), so the empty global VSpace can be installed
     * under it without flushing the TLB. */
    إذا (!is_Valid_Native_ریشه(thread_ریشه)) {
        مجموعه_Current_User_VSpace_ریشه(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        أرجع;
    }

    vspace_ریشه_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_ریشه));
    asid = cap_vspace_cap_get_capVSMappedASID(thread_ریشه);
    پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
    إذا (unlikely(پیدا_کن_ret.وضعیت != EXCEPTION_NONE || پیدا_کن_ret.vspace_ریشه != vspace_ریشه_2)) {
        مجموعه_Current_User_VSpace_ریشه(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        أرجع;
    }

    armv_contextSwitch(vspace_ریشه_2, asid);
}

ساكن bool_t مجموعه_VMRoot_For_Flush(vspace_ریشه_t *vspace, asid_t asid)
{
    cap_t thread_ریشه;

    thread_ریشه = TCB_PTR_CTE_PTR(گره_STATE(ksCurThread), tcbVTable)->cap;

    إذا (cap_get_capType(thread_ریشه) == cap_vspace_cap &&
        cap_vspace_cap_get_capVSIsMapped(thread_ریشه) &&
        VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_ریشه)) == vspace) {
        أرجع false;
    }

    armv_contextSwitch(vspace, asid);
    أرجع true;
}


#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT

ساكن مضمن asid_pool_t *getPoolPtr(asid_t asid)
{
    أرجع armKSASIDTable[ASID_بالا(asid)];
}

ساكن مضمن asid_map_t getASIDMap(asid_pool_t *poolPtr, asid_t asid)
{
    أكد(poolPtr != عدم);
    أرجع poolPtr->array[ASID_کم(asid)];
}

ساكن مضمن فراغ مجموعه_ASIDMap(asid_pool_t *poolPtr, asid_t asid, asid_map_t asid_map)
{
    أكد(poolPtr != عدم);
    poolPtr->array[ASID_کم(asid)] = asid_map;
}

ساكن فراغ invalidateASID(asid_t asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    أكد(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_مجموعه_stored_hw_vmid(asid_map, 0);
    asid_map = asid_map_asid_map_vspace_مجموعه_stored_vmid_valid(asid_map, false);

    مجموعه_ASIDMap(poolPtr, asid, asid_map);
}

ساكن فراغ storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    أكد(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_مجموعه_stored_hw_vmid(asid_map, hw_asid);
    asid_map = asid_map_asid_map_vspace_مجموعه_stored_vmid_valid(asid_map, true);

    مجموعه_ASIDMap(poolPtr, asid, asid_map);
    armKSHWASIDTable[hw_asid] = asid;
}

ساكن hw_asid_t پیدا_کن_Free_HWASID(فراغ)
{
    کلمه_t hw_asid_جابه_جایی;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    لكل (hw_asid_جابه_جایی = 0;
         hw_asid_جابه_جایی <= (کلمه_t)((hw_asid_t) - 1);
         hw_asid_جابه_جایی++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_جابه_جایی);
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

    asid_map = پیدا_کن_Map_For_ASID(asid);
    إذا (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        أرجع asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    } وإلا {
        hw_asid_t new_hw_asid;

        new_hw_asid = پیدا_کن_Free_HWASID();
        storeHWASID(asid, new_hw_asid);
        أرجع new_hw_asid;
    }
}

ساكن فراغ invalidateASIDEntry(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = پیدا_کن_Map_For_ASID(asid);
    إذا (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        armKSHWASIDTable[asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

#پایان_اگر

#اگر_تعریف_شده CONFIG_ARM_SMMU
ساكن کلمه_t getASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_بالا(asid)];
    أكد(asidPool);

    asid_map_t asid_map = asidPool->array[ASID_کم(asid)];
    أكد(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    أرجع asid_map_asid_map_vspace_get_bind_cb(asid_map);
}

فراغ increaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_بالا(asid)];
    أكد(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_کم(asid)];
    أكد(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_مجموعه_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) + 1);
}

فراغ decreaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_بالا(asid)];
    أكد(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_کم(asid)];
    أكد(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_مجموعه_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) - 1);
}
#پایان_اگر

ساكن مضمن فراغ invalidateTLBByASID(asid_t asid)
{
#اگر_تعریف_شده CONFIG_ARM_SMMU
    کلمه_t bind_cb = getASIDBindCB(asid);
    إذا (unlikely(bind_cb)) {
        invalidateSMMUTLBByASID(asid, bind_cb);
    }
#پایان_اگر
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = پیدا_کن_Map_For_ASID(asid);
    إذا (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        أرجع;
    }
    invalidateTranslationASID(asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map));
#وإلا
    invalidateTranslationASID(asid);
#پایان_اگر
}

ساكن مضمن فراغ invalidateTLBByASIDVA(asid_t asid, vptr_t vaddr)
{
#اگر_تعریف_شده CONFIG_ARM_SMMU
    کلمه_t bind_cb = getASIDBindCB(asid);
    إذا (unlikely(bind_cb)) {
        invalidateSMMUTLBByASIDVA(asid, vaddr, bind_cb);
    }
#پایان_اگر
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = پیدا_کن_Map_For_ASID(asid);
    إذا (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        أرجع;
    }
    uint64_t hw_asid = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    invalidateTranslationSingle((hw_asid << 48) | vaddr >> se_L4_صفحه_بیتها);
#وإلا
    invalidateTranslationSingle((asid << 48) | vaddr >> se_L4_صفحه_بیتها);
#پایان_اگر
}


فراغ unmap_صفحه_جدول(asid_t asid, vptr_t vptr, pte_t *هدف_pt)
{
    پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
    إذا (unlikely(پیدا_کن_ret.وضعیت != EXCEPTION_NONE)) {
        /* nothing to do */
        أرجع;
    }
    pte_t *pt_جایگاه = عدم;
    pte_t *pt = (pte_t *)پیدا_کن_ret.vspace_ریشه;

    لكل (کلمه_t i = 0; i < UPT_سطوح - 1 && pt != هدف_pt; i++) {
        pt_جایگاه = pt + GET_UPT_شاخص(vptr, i);
        إذا (unlikely(!pte_pte_جدول_ptr_get_present(pt_جایگاه))) {
            /* couldn't find it */
            أرجع;
        }
        pt = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_پایه_آدرس(pt_جایگاه));
    }

    إذا (pt != هدف_pt) {
        /* didn't find it */
        أرجع;
    }
    /* If we found a pt then ptSlot won't be null */
    أكد(pt_جایگاه != عدم);
    *pt_جایگاه = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)pt_جایگاه, pptr_مقصد_paddr(pt_جایگاه));
    invalidateTLBByASID(asid);
}

فراغ unmap_صفحه(vm_صفحه_اندازه_t صفحه_اندازه, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret;
    lookupPTSlot_ret_t  lu_ret;
    pte_t pte;

    پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
    إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE) {
        أرجع;
    }

    lu_ret = lookupPTSlot(پیدا_کن_ret.vspace_ریشه, vptr);
    إذا (unlikely(lu_ret.pt_بیتها_چپ != صفحه_بیتها_For_اندازه(صفحه_اندازه))) {
        /* Do nothing if the wrong size object was returned */
        أرجع;
    }

    pte = *(lu_ret.pt_جایگاه);
    إذا (!pte_is_صفحه_type(pte)) {
        /* Do nothing if no page is present */
        أرجع;
    }

    إذا (pte_get_صفحه_پایه_آدرس(pte) != pptr_مقصد_paddr((فراغ *)pptr)) {
        /* Do nothing if the mapped page is not the same physical frame */
        أرجع;
    }

    *(lu_ret.pt_جایگاه) = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)lu_ret.pt_جایگاه, pptr_مقصد_paddr(lu_ret.pt_جایگاه));
    أكد(asid < بیت(16));
    invalidateTLBByASIDVA(asid, vptr);
}

فراغ deleteASID(asid_t asid, vspace_ریشه_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_بالا(asid)];

    إذا (poolPtr != عدم) {
        asid_map_t asid_map = poolPtr->array[ASID_کم(asid)];
        إذا (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_ریشه_t *)asid_map_asid_map_vspace_get_vspace_ریشه(asid_map) == vspace) {
            invalidateTLBByASID(asid);
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
            invalidateASIDEntry(asid);
#پایان_اگر
            poolPtr->array[ASID_کم(asid)] = asid_map_asid_map_none_new();
            مجموعه_VMRoot(گره_STATE(ksCurThread));
        }
    }
}

فراغ deleteASIDPool(asid_t asid_پایه, asid_pool_t *pool)
{
    کلمه_t جابه_جایی;

    أكد((asid_پایه & ماسک(asid_کم_بیتها)) == 0);

    إذا (armKSASIDTable[ASID_بالا(asid_پایه)] == pool) {
        لكل (جابه_جایی = 0; جابه_جایی < بیت(asid_کم_بیتها); جابه_جایی++) {
            asid_map_t asid_map = pool->array[جابه_جایی];
            إذا (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                invalidateTLBByASID(asid_پایه + جابه_جایی);
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
                invalidateASIDEntry(asid_پایه + جابه_جایی);
#پایان_اگر
            }
        }
        armKSASIDTable[ASID_بالا(asid_پایه)] = عدم;
        مجموعه_VMRoot(گره_STATE(ksCurThread));
    }
}

ساكن فراغ doFlush(کلمه_t invLabel, vptr_t شروع, vptr_t انتها, paddr_t pstart)
{
    بدل (invLabel) {
    حالة ARMVSpace_Clean_داده:
    حالة ARMPage_Clean_داده:
        cleanCacheRange_RAM(شروع, انتها, pstart);
        اكسر;

    حالة ARMVSpace_Invalidate_داده:
    حالة ARMPage_Invalidate_داده:
        invalidateCacheRange_RAM(شروع, انتها, pstart);
        اكسر;

    حالة ARMVSpace_Clean_Invalidate_داده:
    حالة ARMPage_Clean_Invalidate_داده:
        cleanInvalidateCacheRange_RAM(شروع, انتها, pstart);
        اكسر;

    حالة ARMVSpaceUnify_Instruction:
    حالة ARMPageUnify_Instruction:
        /* First clean data lines to point of unification... */
        cleanCacheRange_PoU(شروع, انتها, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(شروع, انتها, pstart);
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

ساكن exception_t performVSpaceFlush(کلمه_t invLabel, vspace_ریشه_t *vspace_ریشه_2, asid_t asid,
                                      vptr_t شروع, vptr_t انتها, paddr_t pstart)
{

    إذا (config_مجموعه(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        کلمه_t اندازه = انتها - شروع;
        شروع = (vptr_t)paddr_مقصد_pptr(pstart);
        انتها = شروع + اندازه;
        إذا (شروع < انتها) {
            doFlush(invLabel, شروع, انتها, pstart);
        }
    } وإلا {
        bool_t ریشه_switched;

        /* Flush if given a non zero range */
        إذا (شروع < انتها) {
            ریشه_switched = مجموعه_VMRoot_For_Flush(vspace_ریشه_2, asid);
            doFlush(invLabel, شروع, انتها, pstart);
            إذا (ریشه_switched) {
                مجموعه_VMRoot(گره_STATE(ksCurThread));
            }
        }
    }
    أرجع EXCEPTION_NONE;
}


ساكن exception_t perform_صفحه_جدول_Invocation_Map(cap_t cap, cte_t *ct_جایگاه, pte_t pte, pte_t *pt_جایگاه)
{
    ct_جایگاه->cap = cap;
    *pt_جایگاه = pte;
    cleanByVA_PoU((vptr_t)pt_جایگاه, pptr_مقصد_paddr(pt_جایگاه));

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحه_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_جایگاه)
{
    إذا (cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PT_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_صفحه_جدول(cap_صفحه_جدول_cap_get_cap_PTMapped_ASID(cap),
                       cap_صفحه_جدول_cap_get_cap_PTMapped_آدرس(cap), pt);
        clear_حافظه_PT((فراغ *)pt, cap_get_cap_اندازه_بیتها(cap));
    }

    cap_صفحه_جدول_cap_ptr_مجموعه_cap_PTIs_Mapped(&(ct_جایگاه->cap), 0);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحه_Invocation_Map(asid_t asid, cap_t cap, cte_t *ct_جایگاه,
                                            pte_t pte, pte_t *pt_جایگاه)
{
    bool_t tlbflush_required = pte_ptr_get_valid(pt_جایگاه);

    ct_جایگاه->cap = cap;
    *pt_جایگاه = pte;

    cleanByVA_PoU((vptr_t)pt_جایگاه, pptr_مقصد_paddr(pt_جایگاه));
    إذا (unlikely(tlbflush_required)) {
        أكد(asid < بیت(16));
        invalidateTLBByASIDVA(asid, cap_قاب_cap_get_cap_FMapped_آدرس(cap));
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحه_Invocation_Unmap(cap_t cap, cte_t *ct_جایگاه)
{
    إذا (cap_قاب_cap_get_cap_FMapped_ASID(cap) != 0) {

        unmap_صفحه(cap_قاب_cap_get_cap_FSize(cap),
                  cap_قاب_cap_get_cap_FMapped_ASID(cap),
                  cap_قاب_cap_get_cap_FMapped_آدرس(cap),
                  cap_قاب_cap_get_cap_FBase_Ptr(cap));
    }

    cap_t جایگاه_Cap = ct_جایگاه->cap;
    جایگاه_Cap = cap_قاب_cap_مجموعه_cap_FMapped_آدرس(جایگاه_Cap, 0);
    جایگاه_Cap = cap_قاب_cap_مجموعه_cap_FMapped_ASID(جایگاه_Cap, asidInvalid);
    ct_جایگاه->cap = جایگاه_Cap;


    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحه_Flush(کلمه_t invLabel, vspace_ریشه_t *vspace_ریشه_2, asid_t asid,
                                    vptr_t شروع, vptr_t انتها, paddr_t pstart)
{
    إذا (config_مجموعه(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* We flush the cache with kernel virtual addresses since
         * the user virtual addresses are not valid in EL2.
         * Switching VMRoot is not required.
         */
        کلمه_t اندازه = انتها - شروع;
        شروع = (vptr_t)paddr_مقصد_pptr(pstart);
        انتها = شروع + اندازه;

        إذا (شروع < انتها) {
            doFlush(invLabel, شروع, انتها, pstart);
        }
    } وإلا {
        bool_t ریشه_switched;

        إذا (شروع < انتها) {
            ریشه_switched = مجموعه_VMRoot_For_Flush(vspace_ریشه_2, asid);
            doFlush(invLabel, شروع, انتها, pstart);
            إذا (ریشه_switched) {
                مجموعه_VMRoot(گره_STATE(ksCurThread));
            }
        }
    }
    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحه_Get_آدرس(pptr_t پایه_ptr, bool_t call)
{
    paddr_t پایه = pptr_مقصد_paddr((فراغ *)پایه_ptr);

    tcb_t *thread;
    thread = گره_STATE(ksCurThread);
    إذا (call) {
        کلمه_t *ipc_میانگیر = lookupIPCBuffer(true, thread);
        مجموعه_ثبات(thread, badge_ثبات, 0);
        بیعلامت صحيح درازی = مجموعه_MR(thread, ipc_میانگیر, 0, پایه);
        مجموعه_ثبات(thread, پیام_Info_ثبات, کلمه_From_پیام_Info(
                        se_L4_پیام_Info_new(0, 0, 0, درازی)));
    }
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDControlInvocation(فراغ *قاب, cte_t *جایگاه,
                                                cte_t *والد, asid_t asid_پایه)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_مجموعه_cap_Free_شاخص(&(والد->cap),
                                         MAX_FREE_شاخص(cap_untyped_cap_get_cap_بلوک_اندازه(والد->cap)));

    memzero(قاب, بیت(se_L4_ASIDPool_بیتها));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_درج(
        cap_asid_pool_cap_new(
            asid_پایه,         /* capASIDBase  */
            کلمه_REF(قاب)    /* capASIDPool  */
        ), والد, جایگاه);

    أكد((asid_پایه & ماسک(asid_کم_بیتها)) == 0);
    armKSASIDTable[ASID_بالا(asid_پایه)] = (asid_pool_t *)قاب;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_ARMVSpace_ریشه_Invocation(کلمه_t invLabel, کلمه_t درازی,
                                                 cte_t *cte, cap_t cap, کلمه_t *میانگیر)
{
    vptr_t شروع, انتها;
    paddr_t pstart;
    asid_t asid;
    vspace_ریشه_t *vspace_ریشه_2;
    lookupPTSlot_ret_t resolve_ret;
    پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret;
    pte_t pte;

    بدل (invLabel) {
    حالة ARMVSpace_Clean_داده:
    حالة ARMVSpace_Invalidate_داده:
    حالة ARMVSpace_Clean_Invalidate_داده:
    حالة ARMVSpaceUnify_Instruction:

        إذا (درازی < 2) {
            userError("VSpaceRoot Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شروع = getSyscallArg(0, میانگیر);
        انتها =   getSyscallArg(1, میانگیر);

        /* Check sanity of arguments */
        إذا (انتها <= شروع) {
            userError("VSpaceRoot Flush: Invalid range.");
            current_syscall_error.type = se_L4_Invalid_آرگومان;
            current_syscall_error.invalid_آرگومان_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        إذا (انتها > USER_بالاترین) {
            userError("VSpaceRoot Flush: Exceed the user addressable region.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(!is_Valid_Native_ریشه(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Make sure that the supplied pgd is ok */
        vspace_ریشه_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap));
        asid = cap_vspace_cap_get_capVSMappedASID(cap);

        پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
        إذا (unlikely(پیدا_کن_ret.وضعیت != EXCEPTION_NONE)) {
            userError("VSpaceRoot Flush: No VSpace for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(پیدا_کن_ret.vspace_ریشه != vspace_ریشه_2)) {
            userError("VSpaceRoot Flush: Invalid VSpace Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = lookupPTSlot(vspace_ریشه_2, شروع);
        pte = *resolve_ret.pt_جایگاه;

        /* Check that the returned slot is a page. */
        إذا (!pte_is_صفحه_type(pte)) {

            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
            أرجع EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        إذا (دور_پایین(شروع, resolve_ret.pt_بیتها_چپ) != دور_پایین(انتها - 1, resolve_ret.pt_بیتها_چپ)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = شروع;
            current_syscall_error.rangeErrorMax = دور_پایین(شروع, resolve_ret.pt_بیتها_چپ) +
                                                  ماسک(resolve_ret.pt_بیتها_چپ);
            userError("VSpaceRoot Flush: cannot cross page boundary, valid range is [0x%lx..0x%lx)",
                      current_syscall_error.rangeErrorMin, current_syscall_error.rangeErrorMax);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When in EL1, the mapping must be write-able for ARMVSpaceInvalidate_Data */
        إذا (invLabel == ARMVSpace_Invalidate_داده && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMVSpaceInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#پایان_اگر

        /* Calculate the physical start address. */
        paddr_t قاب_پایه_2 = pte_get_صفحه_پایه_آدرس(pte);

        pstart = قاب_پایه_2 + (شروع & ماسک(resolve_ret.pt_بیتها_چپ));

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        إذا (pstart < PADDR_پایه || ((انتها - شروع) + pstart) > PADDR_بالاترین) {
            userError("VSpaceRoot Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#پایان_اگر

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performVSpaceFlush(invLabel, vspace_ریشه_2, asid, شروع, انتها - 1, pstart);

    افتراضي:
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}


ساكن exception_t decode_ARMPage_جدول_Invocation(کلمه_t invLabel, کلمه_t درازی,
                                                cte_t *cte, cap_t cap, کلمه_t *میانگیر)
{
    cap_t vspace_ریشه_Cap;
    vspace_ریشه_t *vspace_ریشه_2;
    pte_t pte;
    asid_t asid;
    vptr_t vaddr;
    lookupPTSlot_ret_t pt_جایگاه;
    پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret;

    إذا (invLabel == ARMPage_جدول_Unmap) {
        إذا (unlikely(!isFinalCapability(cte))) {
            current_syscall_error.type = se_L4_Revoke_اولی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحه_جدول_Invocation_Unmap(cap, cte);
    }

    إذا (unlikely(invLabel != ARMPage_جدول_Map)) {
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(درازی < 2 || current_extra_caps.excaprefs[0] == عدم)) {
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, میانگیر);
    vspace_ریشه_Cap = current_extra_caps.excaprefs[0]->cap;

    إذا (unlikely(!is_Valid_Native_ریشه(vspace_ریشه_Cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vspace_ریشه_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_ریشه_Cap));
    asid = cap_vspace_cap_get_capVSMappedASID(vspace_ریشه_Cap);

    إذا (unlikely(vaddr > USER_بالاترین)) {
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
    إذا (unlikely(پیدا_کن_ret.وضعیت != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(پیدا_کن_ret.vspace_ریشه != vspace_ریشه_2)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pt_جایگاه = lookupPTSlot(vspace_ریشه_2, vaddr);

    إذا (unlikely(pt_جایگاه.pt_بیتها_چپ == se_L4_صفحه_بیتها || pte_ptr_get_valid(pt_جایگاه.pt_جایگاه))) {
        current_syscall_error.type = se_L4_Delete_اولی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pte = pte_pte_جدول_new(pptr_مقصد_paddr(PTE_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap))));

    cap = cap_صفحه_جدول_cap_مجموعه_cap_PTIs_Mapped(cap, 1);
    cap = cap_صفحه_جدول_cap_مجموعه_cap_PTMapped_ASID(cap, asid);
    cap = cap_صفحه_جدول_cap_مجموعه_cap_PTMapped_آدرس(cap, (vaddr & ~ماسک(pt_جایگاه.pt_بیتها_چپ)));

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_صفحه_جدول_Invocation_Map(cap, cte, pte, pt_جایگاه.pt_جایگاه);
}

ساكن مضمن bool_t CONST checkVPAlignment(vm_صفحه_اندازه_t sz, کلمه_t w)
{
    أرجع (w & ماسک(صفحه_بیتها_For_اندازه(sz))) == 0;
}

ساكن exception_t decodeARMFrameInvocation(کلمه_t invLabel, کلمه_t درازی,
                                            cte_t *cte, cap_t cap, bool_t call, کلمه_t *میانگیر)
{
    بدل (invLabel) {
    حالة ARMPageMap: {
        vptr_t vaddr;
        paddr_t پایه;
        cap_t vspace_ریشه_Cap;
        vspace_ریشه_t *vspace_ریشه_2;
        asid_t asid, قاب_asid;
        vm_rights_t vmRights, capRights;
        vm_صفحه_اندازه_t قاب_اندازه;
        vm_صفات_t صفات;
        پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret;

        إذا (unlikely(درازی < 3 || current_extra_caps.excaprefs[0] == عدم)) {
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, میانگیر);
        صفات = vm_صفات_From_کلمه(getSyscallArg(2, میانگیر));
        vspace_ریشه_Cap = current_extra_caps.excaprefs[0]->cap;

        قاب_اندازه = cap_قاب_cap_get_cap_FSize(cap);
        capRights = cap_قاب_cap_get_cap_FVMRights(cap);
        vmRights = ماسک_VMRights(capRights, rights_From_کلمه(getSyscallArg(1, میانگیر)));

        إذا (unlikely(!is_Valid_Native_ریشه(vspace_ریشه_Cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_ریشه_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_ریشه_Cap));
        asid = cap_vspace_cap_get_capVSMappedASID(vspace_ریشه_Cap);

        پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
        إذا (unlikely(پیدا_کن_ret.وضعیت != EXCEPTION_NONE)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(پیدا_کن_ret.vspace_ریشه != vspace_ریشه_2)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(!checkVPAlignment(قاب_اندازه, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* In the case of remap, the cap should have a valid asid */
        قاب_asid = cap_قاب_cap_get_cap_FMapped_ASID(cap);

        إذا (قاب_asid != asidInvalid) {
            إذا (قاب_asid != asid) {
                userError("ARMPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_عدد = 1;
                أرجع EXCEPTION_SYSCALL_ERROR;

            } وإلا إذا (cap_قاب_cap_get_cap_FMapped_آدرس(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_آرگومان;
                current_syscall_error.invalid_آرگومان_عدد = 0;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        } وإلا {
            إذا (unlikely(vaddr + بیت(صفحه_بیتها_For_اندازه(قاب_اندازه)) - 1 > USER_بالاترین)) {
                current_syscall_error.type = se_L4_Invalid_آرگومان;
                current_syscall_error.invalid_آرگومان_عدد = 0;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        cap = cap_قاب_cap_مجموعه_cap_FMapped_ASID(cap, asid);
        cap = cap_قاب_cap_مجموعه_cap_FMapped_آدرس(cap, vaddr);

        پایه = pptr_مقصد_paddr((فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(cap));

        lookupPTSlot_ret_t lu_ret = lookupPTSlot(vspace_ریشه_2, vaddr);
        إذا (unlikely(lu_ret.pt_بیتها_چپ != صفحه_بیتها_For_اندازه(قاب_اندازه))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_بیتها_چپ);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        bool_t cap_read = (capRights == VMReadOnly || capRights == VMReadWrite);
        pte_t pte = make_User_صفحه_PTE(پایه, vmRights, cap_read, صفات, قاب_اندازه);

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحه_Invocation_Map(asid, cap, cte, pte, lu_ret.pt_جایگاه);
    }

    حالة ARMPageUnmap:
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحه_Invocation_Unmap(cap, cte);

    حالة ARMPage_Clean_داده:
    حالة ARMPage_Invalidate_داده:
    حالة ARMPage_Clean_Invalidate_داده:
    حالة ARMPageUnify_Instruction: {
        vptr_t شروع, انتها;
        vptr_t vaddr;
        asid_t asid;
        کلمه_t صفحه_اندازه;
        پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret;

        إذا (درازی < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(cap_قاب_cap_get_cap_FMapped_ASID(cap) == 0)) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = cap_قاب_cap_get_cap_FMapped_آدرس(cap);
        asid = cap_قاب_cap_get_cap_FMapped_ASID(cap);

        پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
        إذا (unlikely(پیدا_کن_ret.وضعیت != EXCEPTION_NONE)) {
            userError("Page Flush: No PGD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شروع = getSyscallArg(0, میانگیر);
        انتها =   getSyscallArg(1, میانگیر);

        /* check that the range is sane */
        إذا (انتها <= شروع) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_آرگومان;
            current_syscall_error.invalid_آرگومان_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* start and end are currently relative inside this page */
        صفحه_اندازه = بیت(صفحه_بیتها_For_اندازه(cap_قاب_cap_get_cap_FSize(cap)));
        إذا (شروع >= صفحه_اندازه || انتها > صفحه_اندازه) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_آرگومان;
            current_syscall_error.invalid_آرگومان_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        کلمه_t pstart = pptr_مقصد_paddr((فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(cap)) + شروع;
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        إذا (pstart < PADDR_پایه || ((انتها - شروع) + pstart) > PADDR_بالاترین) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#وإلا
        /* When in EL1, we are using the user page table for flushing and need to make sure
           the mapping info in the cap is not stale. */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(پیدا_کن_ret.vspace_ریشه, vaddr);
        pte_t pte = *lu_ret.pt_جایگاه;
        فراغ *پایه_ptr = (فراغ *) cap_قاب_cap_get_cap_FBase_Ptr(cap);
        إذا (unlikely(lu_ret.pt_بیتها_چپ != صفحه_بیتها_For_اندازه(cap_قاب_cap_get_cap_FSize(cap)) ||
                     !pte_is_صفحه_type(pte) ||
                     pte_get_صفحه_پایه_آدرس(pte) != pptr_مقصد_paddr(پایه_ptr))) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* When in EL1, the mapping must be writeable for DC IVAC */
        إذا (invLabel == ARMPage_Invalidate_داده && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMPageInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#پایان_اگر
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحه_Flush(invLabel, پیدا_کن_ret.vspace_ریشه, asid, vaddr + شروع, vaddr + انتها - 1,
                                pstart);
    }

    حالة ARMPage_Get_آدرس:
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحه_Get_آدرس(cap_قاب_cap_get_cap_FBase_Ptr(cap), call);

    افتراضي:
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(کلمه_t invLabel, کلمه_t درازی, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, کلمه_t *میانگیر)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_vspace_cap:
        أرجع decode_ARMVSpace_ریشه_Invocation(invLabel, درازی, cte, cap, میانگیر);
    حالة cap_صفحه_جدول_cap:
        أرجع decode_ARMPage_جدول_Invocation(invLabel, درازی, cte, cap, میانگیر);

    حالة cap_قاب_cap:
        أرجع decodeARMFrameInvocation(invLabel, درازی, cte, cap, call, میانگیر);

    حالة cap_asid_control_cap: {
        کلمه_t i;
        asid_t asid_پایه;
        کلمه_t شاخص, عمق;
        cap_t untyped, ریشه;
        cte_t *والد_جایگاه, *dest_جایگاه;
        lookup_جایگاه_ret_t lu_ret;
        فراغ *قاب;
        exception_t وضعیت;

        إذا (unlikely(invLabel != ARMASIDControlMakePool)) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(درازی < 2 ||
                     current_extra_caps.excaprefs[0] == عدم ||
                     current_extra_caps.excaprefs[1] == عدم)) {
            current_syscall_error.type = se_L4_Truncated_پیام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شاخص = getSyscallArg(0, میانگیر);
        عمق = getSyscallArg(1, میانگیر);
        والد_جایگاه = current_extra_caps.excaprefs[0];
        untyped = والد_جایگاه->cap;
        ریشه = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        لكل (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        إذا (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_اولی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_پایه = i << asid_کم_بیتها;

        إذا (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_بلوک_اندازه(untyped) != se_L4_ASIDPool_بیتها ||
                     cap_untyped_cap_get_capIsDevice(untyped))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        وضعیت = ensureNoChildren(والد_جایگاه);
        إذا (unlikely(وضعیت != EXCEPTION_NONE)) {
            أرجع وضعیت;
        }

        قاب = کلمه_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_هدف_جایگاه(ریشه, شاخص, عمق);
        إذا (unlikely(lu_ret.وضعیت != EXCEPTION_NONE)) {
            أرجع lu_ret.وضعیت;
        }
        dest_جایگاه = lu_ret.جایگاه;

        وضعیت = ensure_خالی_جایگاه(dest_جایگاه);
        إذا (unlikely(وضعیت != EXCEPTION_NONE)) {
            أرجع وضعیت;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDControlInvocation(قاب, dest_جایگاه, والد_جایگاه, asid_پایه);
    }

    حالة cap_asid_pool_cap: {
        cap_t vspaceCap;
        cte_t *vspace_Cap_جایگاه;
        asid_pool_t *pool;
        کلمه_t i;
        asid_t asid;

        إذا (unlikely(invLabel != ARMASIDPool_تخصیص_بده)) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
            current_syscall_error.type = se_L4_Truncated_پیام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_جایگاه = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_جایگاه->cap;

        إذا (unlikely(!is_VTable_ریشه(vspaceCap) || cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_بالا(cap_asid_pool_cap_get_capASIDBase(cap))];

        إذا (unlikely(!pool)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = false;
            current_lookup_fault = lookup_fault_invalid_ریشه_new();

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        لكل (i = 0; i < (1 << asid_کم_بیتها) && (asid + i == 0
                                               || (asid_map_get_type(pool->array[i]) != asid_map_asid_map_none)); i++);

        إذا (unlikely(i == 1 << asid_کم_بیتها)) {
            current_syscall_error.type = se_L4_Delete_اولی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDPoolInvocation(asid, pool, vspace_Cap_جایگاه);
    }

    افتراضي:
        fail("Invalid ARM arch cap type");
    }
}

#اگر_تعریف_شده CONFIG_DEBUG_BUILD
فراغ kernelPrefetchAbort(کلمه_t pc) VISIBLE;
فراغ kernel_داده_Abort(کلمه_t pc) VISIBLE;

فراغ kernelPrefetchAbort(کلمه_t pc)
{
    چاپ_کن("\n\nKERNEL PREFETCH ABORT!\n");
    چاپ_کن("Faulting instruction: 0x%"SEL4_PRIx_کلمه"\n", pc);
    چاپ_کن("ESR (IFSR): 0x%"SEL4_PRIx_کلمه"\n", getIFSR());
    halt();
}

فراغ kernel_داده_Abort(کلمه_t pc)
{
    چاپ_کن("\n\nKERNEL DATA ABORT!\n");
    چاپ_کن("Faulting instruction: 0x%"SEL4_PRIx_کلمه"\n", pc);
    چاپ_کن("FAR: 0x%"SEL4_PRIx_کلمه" ESR (DFSR): 0x%"SEL4_PRIx_کلمه"\n",
           getFAR(), getDFSR());
    halt();
}
#پایان_اگر /* CONFIG_DEBUG_BUILD */

#اگر_تعریف_شده CONFIG_PRINTING
تعريفنوع بنية read_کلمه_From_VSpace_ret {
    exception_t وضعیت;
    کلمه_t مقدار;
} read_کلمه_From_VSpace_ret_t;

ساكن read_کلمه_From_VSpace_ret_t read_کلمه_From_VSpace(vspace_ریشه_t *pd, کلمه_t vaddr)
{
    read_کلمه_From_VSpace_ret_t ret;
    کلمه_t جابه_جایی;
    pptr_t kernel_vaddr;
    کلمه_t *مقدار;

    lookupPTSlot_ret_t lookup_ret = lookupPTSlot(pd, vaddr);

    /* Check that the returned slot is a page. */
    إذا (!pte_ptr_get_valid(lookup_ret.pt_جایگاه) ||
        (pte_pte_جدول_ptr_get_present(lookup_ret.pt_جایگاه) && lookup_ret.pt_بیتها_چپ > صفحه_بیتها)) {
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    جابه_جایی = vaddr & ماسک(lookup_ret.pt_بیتها_چپ);
    kernel_vaddr = (کلمه_t)paddr_مقصد_pptr(pte_صفحه_ptr_get_صفحه_پایه_آدرس(lookup_ret.pt_جایگاه));
    مقدار = (کلمه_t *)(kernel_vaddr + جابه_جایی);

    ret.وضعیت = EXCEPTION_NONE;
    ret.مقدار = *مقدار;
    أرجع ret;
}

فراغ Arch_user_پشته_Trace(tcb_t *tptr)
{
    cap_t thread_ریشه;
    vspace_ریشه_t *vspace_ریشه_2;
    کلمه_t sp;

    thread_ریشه = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the vspace root */
    إذا (cap_get_capType(thread_ریشه) != cap_vspace_cap) {
        چاپ_کن("Invalid vspace\n");
        أرجع;
    }

    vspace_ریشه_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_ریشه));
    sp = get_ثبات(tptr, SP_EL0);

    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    إذا (!IS_ALIGNED(sp, se_L4_کلمه_اندازه_بیتها)) {
        چاپ_کن("SP not aligned\n");
        أرجع;
    }

    لكل (بیعلامت صحيح i = 0; i < CONFIG_USER_پشته_TRACE_درازی; i++) {
        کلمه_t آدرس = sp + (i * حجم(کلمه_t));
        read_کلمه_From_VSpace_ret_t نتیجه = read_کلمه_From_VSpace(vspace_ریشه_2,
                                                             آدرس);
        إذا (نتیجه.وضعیت == EXCEPTION_NONE) {
            چاپ_کن("0x%"SEL4_PRIx_کلمه": 0x%"SEL4_PRIx_کلمه"\n",
                   آدرس, نتیجه.مقدار);
        } وإلا {
            چاپ_کن("0x%"SEL4_PRIx_کلمه": INVALID\n", آدرس);
        }
    }
}
#پایان_اگر /* CONFIG_PRINTING */

#إذا defined(CONFIG_KERNEL_LOG_میانگیر)
exception_t benchmark_arch_map_log_میانگیر(کلمه_t قاب_cptr)
{
    lookup_Cap_And_جایگاه_ret_t lu_ret;
    vm_صفحه_اندازه_t قاب_اندازه;
    pptr_t  قاب_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_جایگاه(گره_STATE(ksCurThread), قاب_cptr);

    إذا (unlikely(lu_ret.وضعیت != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", قاب_cptr);
        current_fault = seL4_Fault_CapFault_new(قاب_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(lu_ret.cap) != cap_قاب_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(قاب_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    قاب_اندازه = cap_قاب_cap_get_cap_FSize(lu_ret.cap);

    إذا (قاب_اندازه != ARMLarge_صفحه) {
        userError("Invalid frame size. The kernel expects 2M log buffer");
        current_fault = seL4_Fault_CapFault_new(قاب_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    قاب_pptr = cap_قاب_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_میانگیر = pptr_مقصد_paddr((فراغ *) قاب_pptr);

    *armKSGlobalLogPTE = pte_pte_صفحه_new(
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
                             0, // XN
#وإلا
                             1, // UXN
#پایان_اگر
                             ks_User_Log_میانگیر,
                             0,                         /* global */
                             1,                         /* access flag */
                             SMP_TERNARY(SMP_SHARE, 0), /* Inner-shareable if SMP enabled, otherwise unshared */
                             0,                         /* VMKernelOnly */
                             نرمال_WT);

    cleanByVA_PoU((vptr_t)armKSGlobalLogPTE, addrFromKPPtr(armKSGlobalLogPTE));
    invalidateTranslationSingle(KS_LOG_PPTR);
    أرجع EXCEPTION_NONE;
}
#پایان_اگر /* CONFIG_KERNEL_LOG_BUFFER */
