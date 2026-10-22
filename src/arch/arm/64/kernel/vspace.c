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
    عام_NC = 3,
    عام_2 = 4,
    عام_WT = 5
};

/* Stage-2 translation memory attributes */
تعداد mair_s2_types {
    S2_DEVICE_nGnRnE = 0,
    S2_DEVICE_nGnRE = 1,
    S2_DEVICE_nGRE  = 2,
    S2_DEVICE_GRE = 3,

    S2_عام_INNER_NC_OUTER_NC = 5,
    S2_عام_INNER_WTC_OUTER_NC = 6,
    S2_عام_INNER_WBC_OUTER_NC = 7,

    S2_عام_INNER_NC_OUTER_WTC = 9,
    S2_عام_INNER_WTC_OUTER_WTC = 10,
    S2_عام_INNER_WBC_OUTER_WTC = 11,

    S2_عام_INNER_NC_OUTER_WBC = 13,
    S2_عام_INNER_WTC_OUTER_WBC = 14,
    S2_عام_INNER_WBC_OUTER_WBC = 15,

    S2_عام = S2_عام_INNER_WBC_OUTER_WBC
};

/* Leif from Linaro said the big.LITTLE clusters should be treated as
 * inner shareable, and we believe so, although the Example B2-1 given in
 * ARM ARM DDI 0487B.b (ID092517) says otherwise.
 */

#تعریف_کرو SMP_SHARE   3

بنية lookupPTSlot_ret {
    pte_t *pt_تھاں;
    لفظ_t pt_بٹ_بایاں;
};
تعريفنوع بنية lookupPTSlot_ret lookupPTSlot_ret_t;

بنية لبھو_VSpace_For_ASID_ret {
    exception_t حالت;
    vspace_جڑ_t *vspace_جڑ;
};
تعريفنوع بنية لبھو_VSpace_For_ASID_ret لبھو_VSpace_For_ASID_ret_t;

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

ساكن لفظ_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    بدل (vm_rights) {
    حالة VMKernelOnly:
        إذا (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            أرجع 0;
        } وإلا {
            أرجع 0;
        }

    حالة VMReadWrite:
        إذا (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            أرجع 3;
        } وإلا {
            أرجع 1;
        }

    حالة VMReadOnly:
        إذا (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            أرجع 1;
        } وإلا {
            أرجع 3;
        }

    افتراضي:
        fail("Invalid VM rights");
    }
}

#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
ساكن مضمن CONST لفظ_t pte_get_AP(pte_t pte)
{
    أكد(pte_is_ورقہ_type(pte));
    بدل (pte_get_pte_type(pte)) {
    حالة pte_pte_4k_ورقہ:
        أرجع pte_pte_4k_ورقہ_get_AP(pte);

    حالة pte_pte_ورقہ:
        أرجع pte_pte_ورقہ_get_AP(pte);

    افتراضي:
        أرجع 0;
    }
}

ساكن vm_rights_t CONST vmRightsFromPTE(pte_t pte)
{
    لفظ_t access_perms = pte_get_AP(pte);
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
#جے_انت

vm_rights_t CONST نقاب_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_نقاب)
{
    إذا (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_نقاب)) {
        أرجع VMReadOnly;
    }
    إذا (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_نقاب)) {
        إذا (!seL4_CapRights_get_capAllowWrite(cap_rights_نقاب)) {
            أرجع VMReadOnly;
        } وإلا {
            أرجع VMReadWrite;
        }
    }
    إذا (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_نقاب) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_نقاب)) {
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
BOOT_کوڈ فراغ map_kernel_چوکھٹا(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_گن_t گن)
{
    أكد(vaddr >= PPTR_چوٹی);

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t uxn = vm_گن_get_arm_Execute_Never(گن);
#وإلا
    لفظ_t uxn = 1; /* unprivileged execute never */
#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    لفظ_t attr_اشاریہ;
    لفظ_t shareable;
    إذا (vm_گن_get_arm_ورقہ_Cacheable(گن)) {
        attr_اشاریہ = عام_2;
        shareable = SMP_TERNARY(SMP_SHARE, 0);
    } وإلا {
        attr_اشاریہ = DEVICE_nGnRnE;
        shareable = 0;
    }
    armKSGlobalKernelPT[GET_KPT_اشاریہ(vaddr, KLVL_FRM_ARM_PT_LVL(3))] = pte_pte_4k_ورقہ_new(uxn, paddr,
                                                                                            0, /* global */
                                                                                            1, /* access flag */
                                                                                            shareable,
                                                                                            APFromVMRights(vm_rights),
                                                                                            attr_اشاریہ);
}

/* Essentially, assert that the ELF, kernel devices, and log buffer are all in the same PUD.
   In the current code this is the top index. This is arbitrary, the important part
   is that they are all the same.
 */
compile_assert(elf_وچ_kernel_mappings, GET_KPT_اشاریہ(KERNEL_ELF_ادھار,
                                                     KLVL_FRM_ARM_PT_LVL(1)) == ثنائی_ہندسہ(PT_اشاریہ_بٹ) - 1);
compile_assert(kdev_وچ_kernel_mappings, GET_KPT_اشاریہ(KDEV_ادھار,
                                                      KLVL_FRM_ARM_PT_LVL(1)) == ثنائی_ہندسہ(PT_اشاریہ_بٹ) - 1);
compile_assert(log_buf_وچ_kernel_mappings, GET_KPT_اشاریہ(KS_LOG_ادھار,
                                                         KLVL_FRM_ARM_PT_LVL(1)) == ثنائی_ہندسہ(PT_اشاریہ_بٹ) - 1);

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
BOOT_کوڈ فراغ map_kernel_elf_image(pte_t kernel_mapping_pds[])
{
    /** This is not strictly necessary, but to handle a non-aligned KERNEL_ELF_PADDR_BASE
     *  we'd need to do a bit more logic and probably do what riscv64 does
     *  where the *actual* KERNEL_ELF_BASE is rounded up to the alignment.
     *  This is simpler and memory is cheap.
     **/
    compile_assert(elf_paddr_aligned, IS_ALIGNED(KERNEL_ELF_PADDR_ادھار_RAW, se_L4_Large_ورقہ_بٹ));
    compile_assert(elf_ادھار_aligned, IS_ALIGNED(KERNEL_ELF_ادھار, se_L4_Large_ورقہ_بٹ));

    /* Please don't overlap with KDEV_BASE */
    أكد(KERNEL_ELF_چوٹی <= KDEV_ادھار);
    /* Please don't overlap with KS_LOG_PPTR */
    أكد(KERNEL_ELF_چوٹی <= KS_LOG_PPTR);

    ثابت لفظ_t pd_شروع = GET_KPT_اشاریہ(KERNEL_ELF_ادھار, KLVL_FRM_ARM_PT_LVL(2));
    /* pd_end and ELF_TOP are both exclusive, so handle that correctly */
    ثابت لفظ_t pd_سرا   = GET_KPT_اشاریہ(KERNEL_ELF_چوٹی - 1, KLVL_FRM_ARM_PT_LVL(2)) + 1;

    paddr_t paddr = KERNEL_ELF_PADDR_ادھار;
    بن_ورتیا pptr_t vaddr = KERNEL_ELF_ادھار;

    لكل (لفظ_t pd = pd_شروع; pd < pd_سرا; pd++) {
        أكد(pd == GET_KPT_اشاریہ(vaddr, KLVL_FRM_ARM_PT_LVL(2)));

        kernel_mapping_pds[pd] = pte_pte_ورقہ_new(
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                                     /* XN */ 0,
#وإلا
                                     /* UXN */ 1,
#جے_انت
                                     /* page_base_address */ paddr,
                                     /* global (nG) */ 0,
                                     /* access flag (AF) */ 1,
                                     /* shareability (SH) */ SMP_TERNARY(SMP_SHARE, 0),
                                     /* AP */ APFromVMRights(VMKernelOnly),
                                     /* AttrIndx */ عام_2
                                 );

        paddr += ثنائی_ہندسہ(se_L4_Large_ورقہ_بٹ);
        vaddr += ثنائی_ہندسہ(se_L4_Large_ورقہ_بٹ);
    }
}

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
/* verify that the physical memory window as at the second entry of the PGD */
compile_assert(physical_کھڑکی_pgd_location,
               GET_KPT_اشاریہ(PPTR_ادھار, KLVL_FRM_ARM_PT_LVL(0)) == 1);
#وإلا
/* verify that the physical memory window as at the last entry of the PGD */
compile_assert(physical_کھڑکی_pgd_location,
               GET_KPT_اشاریہ(PPTR_ادھار, KLVL_FRM_ARM_PT_LVL(0)) == ثنائی_ہندسہ(PT_اشاریہ_بٹ) - 1);
#جے_انت
/* make sure that the PPTR_BASE and PPTR_TOP are huge paged aligned */
compile_assert(physical_کھڑکی_ادھار_aligned, IS_ALIGNED(PPTR_ادھار, se_L4_Huge_ورقہ_بٹ));
compile_assert(physical_کھڑکی_چوٹی_aligned, IS_ALIGNED(PPTR_چوٹی, se_L4_Huge_ورقہ_بٹ));

/* the PPTR_BASE starts from the bottom of the kernel PUD */
compile_assert(physical_کھڑکی_bottom_kernel_region,
               GET_KPT_اشاریہ(PPTR_ادھار, KLVL_FRM_ARM_PT_LVL(1)) == 0);
/* the PPTR_TOP then ends at the second-to-last entry of the PUD
    note: because this is the exclusive value the assert is one higher than
          you would expect.
*/
compile_assert(physical_کھڑکی_چوٹی_kernel_region,
               GET_KPT_اشاریہ(PPTR_چوٹی, KLVL_FRM_ARM_PT_LVL(1)) == ثنائی_ہندسہ(PT_اشاریہ_بٹ) - 1);

BOOT_کوڈ فراغ map_kernel_physical_کھڑکی(فراغ)
{
    paddr_t paddr;
    pptr_t vaddr;
    لفظ_t idx;

    /* place all PDs except the last one in PUD */
    لكل (idx = GET_KPT_اشاریہ(PPTR_ادھار, KLVL_FRM_ARM_PT_LVL(1)); idx < GET_KPT_اشاریہ(PPTR_چوٹی, KLVL_FRM_ARM_PT_LVL(1));
         idx++) {
        armKSGlobalKernelPUD[idx] = pte_pte_جدول_new(
                                        addrFromKPPtr(&armKSGlobalKernelPDs[idx][0])
                                    );
    }

    /* map the kernel window using large pages */
    vaddr = PPTR_ادھار;
    لكل (paddr = PADDR_ادھار; paddr < PADDR_چوٹی; paddr += ثنائی_ہندسہ(se_L4_Large_ورقہ_بٹ)) {
        armKSGlobalKernelPDs[GET_KPT_اشاریہ(vaddr, KLVL_FRM_ARM_PT_LVL(1))][GET_KPT_اشاریہ(vaddr,
                                                                                         KLVL_FRM_ARM_PT_LVL(2))] = pte_pte_ورقہ_new(
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                                                                                                                        1, // XN
#وإلا
                                                                                                                        1, // UXN
#جے_انت
                                                                                                                        paddr,
                                                                                                                        0,                        /* global */
                                                                                                                        1,                        /* access flag */
                                                                                                                        SMP_TERNARY(SMP_SHARE, 0),        /* Inner-shareable if SMP enabled, otherwise unshared */
                                                                                                                        0,                        /* VMKernelOnly */
                                                                                                                        عام_2
                                                                                                                    );
        vaddr += ثنائی_ہندسہ(se_L4_Large_ورقہ_بٹ);
    }
}

BOOT_کوڈ فراغ map_kernel_کھڑکی(فراغ)
{
    /* place the PUD into the PGD */
    armKSGlobalKernelPGD[GET_KPT_اشاریہ(PPTR_ادھار, KLVL_FRM_ARM_PT_LVL(0))] =
        pte_pte_جدول_new(addrFromKPPtr(armKSGlobalKernelPUD));

    map_kernel_physical_کھڑکی();

    ثابت لفظ_t elf_pud_idx = GET_KPT_اشاریہ(KERNEL_ELF_ادھار, KLVL_FRM_ARM_PT_LVL(1));
    pte_t *kernel_mapping_pds = armKSGlobalKernelPDs[elf_pud_idx];

    /* place the kernel mapping PDs in the kernel mapping PUD */
    armKSGlobalKernelPUD[elf_pud_idx] = pte_pte_جدول_new(addrFromKPPtr(kernel_mapping_pds));

    /* map the kernel ELF */
    map_kernel_elf_image(kernel_mapping_pds);

    /* put the PT into the PD for device window. It would be nice if
       we could pass kernel_mapping_pds[511] to map_kernel_devices(), but it is
       shared between AArch64 and AArch32, so... */
    kernel_mapping_pds[ثنائی_ہندسہ(PT_اشاریہ_بٹ) - 1]
        = pte_pte_جدول_new(addrFromKPPtr(armKSGlobalKernelPT));

    map_kernel_devices();
}

/* When the hypervisor support is enabled, the stage-2 translation table format
 * is used for applications.
 * The global bit is always 0.
 * The memory attributes use the S2 translation values.
 */
ساكن BOOT_کوڈ فراغ map_it_چوکھٹا_cap(cap_t vspace_cap, cap_t چوکھٹا_cap, bool_t executable)
{
    vspace_جڑ_t *vspace_جڑ_2 = VSPACE_PTR(pptr_دا_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt;

    vptr_t vptr = cap_چوکھٹا_cap_get_cap_FMapped_پتہ(چوکھٹا_cap);
    فراغ *pptr = (فراغ *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(چوکھٹا_cap);

    أكد(cap_چوکھٹا_cap_get_cap_FMapped_ASID(چوکھٹا_cap) != 0);

#جے_تعریف_شدہ AARCH64_VSPACE_S2_شروع_L1
    pud = vspace_جڑ_2;
#وإلا
    vspace_جڑ_2 += GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(0));
    أكد(pte_pte_جدول_ptr_get_present(vspace_جڑ_2));
    pud = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_ادھار_پتہ(vspace_جڑ_2));
#جے_انت
    pud += GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(1));
    أكد(pte_pte_جدول_ptr_get_present(pud));
    pd = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_ادھار_پتہ(pud));
    pd += GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(2));
    أكد(pte_pte_جدول_ptr_get_present(pd));
    pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_ادھار_پتہ(pd));
    *(pt + GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(3))) = pte_pte_4k_ورقہ_new(
                                                              !executable,                    /* unprivileged execute never */
                                                              pptr_منزل_paddr(pptr),            /* page_base_address    */
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              0,
#وإلا
                                                              1,                              /* not global */
#جے_انت
                                                              1,                              /* access flag */
                                                              SMP_TERNARY(SMP_SHARE, 0),              /* Inner-shareable if SMP enabled, otherwise unshared */
                                                              APFromVMRights(VMReadWrite),
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              S2_عام
#وإلا
                                                              عام_2
#جے_انت
                                                          );
}

ساكن BOOT_کوڈ cap_t create_it_چوکھٹا_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    vm_ورقہ_ناپ_t چوکھٹا_ناپ_2;
    إذا (use_large) {
        چوکھٹا_ناپ_2 = ARMLarge_ورقہ;
    } وإلا {
        چوکھٹا_ناپ_2 = ARMSmall_ورقہ;
    }
    أرجع
        cap_چوکھٹا_cap_new(
            asid,                          /* capFMappedASID */
            pptr,                          /* capFBasePtr */
            چوکھٹا_ناپ_2,                    /* capFSize */
            vptr,                          /* capFMappedAddress */
            لفظ_From_VMRights(VMReadWrite), /* capFVMRights */
            false                          /* capFIsDevice */
        );
}

ساكن BOOT_کوڈ فراغ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    vspace_جڑ_t *vspace_جڑ_2 = VSPACE_PTR(pptr_دا_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt = PT_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_ورقہ_جدول_cap_get_cap_PTMapped_پتہ(pt_cap);

    أكد(cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(pt_cap));

#جے_تعریف_شدہ AARCH64_VSPACE_S2_شروع_L1
    pud = vspace_جڑ_2;
#وإلا
    vspace_جڑ_2 += GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(0));
    أكد(pte_pte_جدول_ptr_get_present(vspace_جڑ_2));
    pud = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_ادھار_پتہ(vspace_جڑ_2));
#جے_انت
    pud += GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(1));
    أكد(pte_pte_جدول_ptr_get_present(pud));
    pd = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_ادھار_پتہ(pud));
    *(pd + GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(2))) = pte_pte_جدول_new(
                                                              pptr_منزل_paddr(pt)
                                                          );
}

ساكن BOOT_کوڈ cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ورقہ_جدول_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pt_cap(vspace_cap, cap);
    أرجع cap;
}

ساكن BOOT_کوڈ فراغ map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    vspace_جڑ_t *vspace_جڑ_2 = VSPACE_PTR(pptr_دا_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd = PT_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(pd_cap));
    vptr_t vptr = cap_ورقہ_جدول_cap_get_cap_PTMapped_پتہ(pd_cap);

    أكد(cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(pd_cap));

#جے_تعریف_شدہ AARCH64_VSPACE_S2_شروع_L1
    pud = vspace_جڑ_2;
#وإلا
    vspace_جڑ_2 += GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(0));
    أكد(pte_pte_جدول_ptr_get_present(vspace_جڑ_2));
    pud = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_ادھار_پتہ(vspace_جڑ_2));
#جے_انت
    *(pud + GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(1))) = pte_pte_جدول_new(
                                                               pptr_منزل_paddr(pd)
                                                           );
}

ساكن BOOT_کوڈ cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ورقہ_جدول_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pd_cap(vspace_cap, cap);
    أرجع cap;
}

#جے_بنا_تعریف AARCH64_VSPACE_S2_شروع_L1
ساكن BOOT_کوڈ فراغ map_it_pud_cap(cap_t vspace_cap, cap_t pud_cap)
{
    pte_t *pgd = PT_PTR(pptr_دا_cap(vspace_cap));
    pte_t *pud = PT_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(pud_cap));
    vptr_t vptr = cap_ورقہ_جدول_cap_get_cap_PTMapped_پتہ(pud_cap);

    أكد(cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(pud_cap));

    *(pgd + GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(0))) = pte_pte_جدول_new(
                                                               pptr_منزل_paddr(pud));
}

ساكن BOOT_کوڈ cap_t create_it_pud_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ورقہ_جدول_cap_new(
              asid,               /* capPTMappedASID */
              pptr,               /* capPTBasePtr */
              1,                  /* capPTIsMapped */
              vptr                /* capPTMappedAddress */
          );
    map_it_pud_cap(vspace_cap, cap);
    أرجع cap;
}
#جے_انت /* AARCH64_VSPACE_S2_START_L1 */
BOOT_کوڈ لفظ_t arch_get_n_paging(v_region_t it_v_reg)
{
    أرجع
#جے_بنا_تعریف AARCH64_VSPACE_S2_شروع_L1
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_بٹ(ULVL_FRM_ARM_PT_LVL(0))) +
#جے_انت
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_بٹ(ULVL_FRM_ARM_PT_LVL(1))) +
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_بٹ(ULVL_FRM_ARM_PT_LVL(2)));
}

BOOT_کوڈ cap_t create_it_پتہ_space(cap_t جڑ_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_تھاں_Pos تھاں_pos_before;
    se_L4_تھاں_Pos تھاں_pos_after;

    /* create the PGD */
    vspace_cap = cap_vspace_cap_new(
                     IT_ASID,           /* capVSMappedASID */
                     rootserver.vspace, /* capVSBasePtr    */
                     1                  /* capVSIsMapped   */
#جے_تعریف_شدہ CONFIG_ARM_SMMU
                     , CB_INVALID       /* capVSMappedCB   */
#جے_انت
                 );
    تھاں_pos_before = ndks_boot.تھاں_pos_cur;
    write_تھاں(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

#جے_بنا_تعریف AARCH64_VSPACE_S2_شروع_L1
    /* Create any PUDs needed for the user land image */
    لكل (vptr = گیڑا_تھلے(it_v_reg.شروع, GET_ULVL_PGSIZE_بٹ(ULVL_FRM_ARM_PT_LVL(0)));
         vptr < it_v_reg.سرا;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(0))) {
        إذا (!provide_cap(جڑ_cnode_cap, create_it_pud_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            أرجع cap_null_cap_new();
        }
    }
#جے_انت
    /* Create any PDs needed for the user land image */
    لكل (vptr = گیڑا_تھلے(it_v_reg.شروع, GET_ULVL_PGSIZE_بٹ(ULVL_FRM_ARM_PT_LVL(1)));
         vptr < it_v_reg.سرا;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))) {
        إذا (!provide_cap(جڑ_cnode_cap, create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            أرجع cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    لكل (vptr = گیڑا_تھلے(it_v_reg.شروع, GET_ULVL_PGSIZE_بٹ(ULVL_FRM_ARM_PT_LVL(2)));
         vptr < it_v_reg.سرا;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))) {
        إذا (!provide_cap(جڑ_cnode_cap, create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            أرجع cap_null_cap_new();
        }
    }

    تھاں_pos_after = ndks_boot.تھاں_pos_cur;
    ndks_boot.bi_چوکھٹا->userImagePaging = (se_L4_تھاں_Region) {
        تھاں_pos_before, تھاں_pos_after
    };
    أرجع vspace_cap;
}

BOOT_کوڈ cap_t create_unmapped_it_چوکھٹا_cap(pptr_t pptr, bool_t use_large)
{
    أرجع create_it_چوکھٹا_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_کوڈ cap_t create_mapped_it_چوکھٹا_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_چوکھٹا_cap(pptr, vptr, asid, use_large);
    map_it_چوکھٹا_cap(pd_cap, cap, executable);
    أرجع cap;
}

BOOT_کوڈ فراغ activate_kernel_vspace(فراغ)
{
    cleanInvalidateL1Caches();
    سیٹ_Current_Kernel_VSpace_جڑ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalKernelPGD)));

    /* Prevent elf-loader address translation to fill up TLB */
    سیٹ_Current_User_VSpace_جڑ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));

    invalidateLocalTLB();
    جندرا_TLBEntry(KERNEL_ELF_ادھار);
}

BOOT_کوڈ فراغ write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_دا_cap(it_ap_cap));
    asid_map_t asid_map = asid_map_asid_map_vspace_new(
#جے_تعریف_شدہ CONFIG_ARM_SMMU
                              /* bind_cb: Number of bound context banks */
                              0,
#جے_انت
                              /* vspace_root: reference to vspace root page table object */
                              (لفظ_t)cap_vspace_cap_get_capVSBasePtr(it_vspace_cap)
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                              /* stored_hw_vmid, stored_vmid_valid: Assigned hardware VMID for TLB. */
                              , 0, false
#جے_انت
                          );
    ap->array[ASID_کم(IT_ASID)] = asid_map;
    armKSASIDTable[ASID_بلند(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

asid_map_t لبھو_Map_For_ASID(asid_t asid)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_بلند(asid)];
    إذا (!poolPtr) {
        أرجع asid_map_asid_map_none_new();
    }

    أرجع poolPtr->array[ASID_کم(asid)];
}

ساكن لبھو_VSpace_For_ASID_ret_t لبھو_VSpace_For_ASID(asid_t asid)
{
    لبھو_VSpace_For_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = لبھو_Map_For_ASID(asid);
    إذا (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_جڑ_new();

        ret.vspace_جڑ = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.vspace_جڑ = (vspace_جڑ_t *)asid_map_asid_map_vspace_get_vspace_جڑ(asid_map);
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

لفظ_t *خالص lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    لفظ_t w_بفر_Ptr;
    cap_t بفر_Cap;
    vm_rights_t vm_rights;

    w_بفر_Ptr = thread->tcbIPCBuffer;
    بفر_Cap = TCB_PTR_CTE_PTR(thread, tcb_بفر)->cap;

    إذا (unlikely(cap_get_capType(بفر_Cap) != cap_چوکھٹا_cap)) {
        أرجع عدم;
    }
    إذا (unlikely(cap_چوکھٹا_cap_get_cap_FIs_Device(بفر_Cap))) {
        أرجع عدم;
    }

    vm_rights = cap_چوکھٹا_cap_get_cap_FVMRights(بفر_Cap);
    إذا (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        لفظ_t ادھار_Ptr;
        بنانشانی صحيح ورقہ_بٹ_2;

        ادھار_Ptr = cap_چوکھٹا_cap_get_cap_FBase_Ptr(بفر_Cap);
        ورقہ_بٹ_2 = ورقہ_بٹ_For_ناپ(cap_چوکھٹا_cap_get_cap_FSize(بفر_Cap));
        أرجع (لفظ_t *)(ادھار_Ptr + (w_بفر_Ptr & نقاب(ورقہ_بٹ_2)));
    } وإلا {
        أرجع عدم;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    إذا (cap_get_capType(cap) != cap_چوکھٹا_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(cap_چوکھٹا_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!IS_ALIGNED(vptr, se_L4_IPCBuffer_ناپ_بٹ)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (صحيح)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

ساكن lookupPTSlot_ret_t lookupPTSlot(vspace_جڑ_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    لفظ_t level = UPT_پدھراں - 1;
    pte_t *pt = vspace;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_بٹ_بایاں = PT_اشاریہ_بٹ * level + se_L4_ورقہ_بٹ;
    ret.pt_تھاں = pt + ((vptr >> ret.pt_بٹ_بایاں) & نقاب(se_L4_VSpace_اشاریہ_بٹ));

    طالما (pte_pte_جدول_ptr_get_present(ret.pt_تھاں) && likely(level > 0)) {
        level--;
        ret.pt_بٹ_بایاں -= PT_اشاریہ_بٹ;
        pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_ادھار_پتہ(ret.pt_تھاں));
        ret.pt_تھاں = pt + ((vptr >> ret.pt_بٹ_بایاں) & نقاب(PT_اشاریہ_بٹ));
    }

    أرجع ret;
}

/* Note that if the hypervisor support is enabled, the user page tables use
 * stage-2 translation format. Otherwise, they follow the stage-1 translation format.
 */
ساكن pte_t make_User_ورقہ_PTE(paddr_t paddr, vm_rights_t vm_rights, bool_t cap_read, vm_گن_t گن,
                             vm_ورقہ_ناپ_t ورقہ_ناپ)
{
    bool_t nonexecutable = vm_گن_get_arm_Execute_Never(گن) || !cap_read;
    لفظ_t cacheable = vm_گن_get_arm_ورقہ_Cacheable(گن);

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t nG = 0; /* not global */
    لفظ_t attridx = cacheable ? S2_عام : S2_DEVICE_nGnRnE;
#وإلا
    لفظ_t nG = 1; /* not global */
    لفظ_t attridx = cacheable ? عام_2 : DEVICE_nGnRnE;
#جے_انت

    /* Inner-shareable if SMP enabled, otherwise unshared (ignored for devices) */
    لفظ_t shareable = cacheable ? SMP_TERNARY(SMP_SHARE, 0) : 0;

    إذا (ورقہ_ناپ == ARMSmall_ورقہ) {
        أرجع pte_pte_4k_ورقہ_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                   shareable, APFromVMRights(vm_rights), attridx);
    } وإلا {
        أرجع pte_pte_ورقہ_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                shareable, APFromVMRights(vm_rights), attridx);
    }
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    بدل (vm_faultType) {
    حالة ARMDataAbort: {
        لفظ_t addr, fault;

        addr = getFAR();
        fault = getDFSR();

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* use the IPA */
        إذا (ARCH_گرہ_STATE(armHSVCPUActive)) {
            addr = GET_PAR_ADDR(پتہ_Translate_S1(addr)) | (addr & نقاب(ورقہ_بٹ));
        }
#جے_انت
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        أرجع EXCEPTION_FAULT;
    }

    حالة ARMPrefetchAbort: {
        لفظ_t pc, fault;

        pc = getRestartPC(thread);
        fault = getIFSR();

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (ARCH_گرہ_STATE(armHSVCPUActive)) {
            pc = GET_PAR_ADDR(پتہ_Translate_S1(pc)) | (pc & نقاب(ورقہ_بٹ));
        }
#جے_انت
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        أرجع EXCEPTION_FAULT;
    }

    افتراضي:
        fail("Invalid VM fault type");
    }
}

bool_t CONST is_VTable_جڑ(cap_t cap)
{
    أرجع cap_get_capType(cap) == cap_vspace_cap;
}

bool_t CONST is_Valid_Native_جڑ(cap_t cap)
{
    أرجع is_VTable_جڑ(cap) &&
           cap_vspace_cap_get_capVSIsMapped(cap);
}

bool_t CONST is_Valid_VTable_جڑ(cap_t cap)
{
    أرجع is_Valid_Native_جڑ(cap);
}

فراغ سیٹ_VMRoot(tcb_t *tcb)
{
    cap_t thread_جڑ;
    asid_t asid;
    vspace_جڑ_t *vspace_جڑ_2;
    لبھو_VSpace_For_ASID_ret_t لبھو_ret;

    thread_جڑ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    /* ASID/VMID 0 is never allocated to a real VSpace (see findFreeHWASID()
     * for the hypervisor case), so the empty global VSpace can be installed
     * under it without flushing the TLB. */
    إذا (!is_Valid_Native_جڑ(thread_جڑ)) {
        سیٹ_Current_User_VSpace_جڑ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        أرجع;
    }

    vspace_جڑ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_جڑ));
    asid = cap_vspace_cap_get_capVSMappedASID(thread_جڑ);
    لبھو_ret = لبھو_VSpace_For_ASID(asid);
    إذا (unlikely(لبھو_ret.حالت != EXCEPTION_NONE || لبھو_ret.vspace_جڑ != vspace_جڑ_2)) {
        سیٹ_Current_User_VSpace_جڑ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        أرجع;
    }

    armv_contextSwitch(vspace_جڑ_2, asid);
}

ساكن bool_t سیٹ_VMRoot_For_Flush(vspace_جڑ_t *vspace, asid_t asid)
{
    cap_t thread_جڑ;

    thread_جڑ = TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcbVTable)->cap;

    إذا (cap_get_capType(thread_جڑ) == cap_vspace_cap &&
        cap_vspace_cap_get_capVSIsMapped(thread_جڑ) &&
        VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_جڑ)) == vspace) {
        أرجع false;
    }

    armv_contextSwitch(vspace, asid);
    أرجع true;
}


#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT

ساكن مضمن asid_pool_t *getPoolPtr(asid_t asid)
{
    أرجع armKSASIDTable[ASID_بلند(asid)];
}

ساكن مضمن asid_map_t getASIDMap(asid_pool_t *poolPtr, asid_t asid)
{
    أكد(poolPtr != عدم);
    أرجع poolPtr->array[ASID_کم(asid)];
}

ساكن مضمن فراغ سیٹ_ASIDMap(asid_pool_t *poolPtr, asid_t asid, asid_map_t asid_map)
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

    asid_map = asid_map_asid_map_vspace_سیٹ_stored_hw_vmid(asid_map, 0);
    asid_map = asid_map_asid_map_vspace_سیٹ_stored_vmid_valid(asid_map, false);

    سیٹ_ASIDMap(poolPtr, asid, asid_map);
}

ساكن فراغ storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    أكد(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_سیٹ_stored_hw_vmid(asid_map, hw_asid);
    asid_map = asid_map_asid_map_vspace_سیٹ_stored_vmid_valid(asid_map, true);

    سیٹ_ASIDMap(poolPtr, asid, asid_map);
    armKSHWASIDTable[hw_asid] = asid;
}

ساكن hw_asid_t لبھو_Free_HWASID(فراغ)
{
    لفظ_t hw_asid_وتھ;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    لكل (hw_asid_وتھ = 0;
         hw_asid_وتھ <= (لفظ_t)((hw_asid_t) - 1);
         hw_asid_وتھ++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_وتھ);
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

    asid_map = لبھو_Map_For_ASID(asid);
    إذا (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        أرجع asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    } وإلا {
        hw_asid_t new_hw_asid;

        new_hw_asid = لبھو_Free_HWASID();
        storeHWASID(asid, new_hw_asid);
        أرجع new_hw_asid;
    }
}

ساكن فراغ invalidateASIDEntry(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = لبھو_Map_For_ASID(asid);
    إذا (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        armKSHWASIDTable[asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

#جے_انت

#جے_تعریف_شدہ CONFIG_ARM_SMMU
ساكن لفظ_t getASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_بلند(asid)];
    أكد(asidPool);

    asid_map_t asid_map = asidPool->array[ASID_کم(asid)];
    أكد(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    أرجع asid_map_asid_map_vspace_get_bind_cb(asid_map);
}

فراغ increaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_بلند(asid)];
    أكد(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_کم(asid)];
    أكد(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_سیٹ_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) + 1);
}

فراغ decreaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_بلند(asid)];
    أكد(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_کم(asid)];
    أكد(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_سیٹ_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) - 1);
}
#جے_انت

ساكن مضمن فراغ invalidateTLBByASID(asid_t asid)
{
#جے_تعریف_شدہ CONFIG_ARM_SMMU
    لفظ_t bind_cb = getASIDBindCB(asid);
    إذا (unlikely(bind_cb)) {
        invalidateSMMUTLBByASID(asid, bind_cb);
    }
#جے_انت
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = لبھو_Map_For_ASID(asid);
    إذا (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        أرجع;
    }
    invalidateTranslationASID(asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map));
#وإلا
    invalidateTranslationASID(asid);
#جے_انت
}

ساكن مضمن فراغ invalidateTLBByASIDVA(asid_t asid, vptr_t vaddr)
{
#جے_تعریف_شدہ CONFIG_ARM_SMMU
    لفظ_t bind_cb = getASIDBindCB(asid);
    إذا (unlikely(bind_cb)) {
        invalidateSMMUTLBByASIDVA(asid, vaddr, bind_cb);
    }
#جے_انت
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = لبھو_Map_For_ASID(asid);
    إذا (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        أرجع;
    }
    uint64_t hw_asid = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    invalidateTranslationSingle((hw_asid << 48) | vaddr >> se_L4_ورقہ_بٹ);
#وإلا
    invalidateTranslationSingle((asid << 48) | vaddr >> se_L4_ورقہ_بٹ);
#جے_انت
}


فراغ unmap_ورقہ_جدول(asid_t asid, vptr_t vptr, pte_t *نشانہ_pt)
{
    لبھو_VSpace_For_ASID_ret_t لبھو_ret = لبھو_VSpace_For_ASID(asid);
    إذا (unlikely(لبھو_ret.حالت != EXCEPTION_NONE)) {
        /* nothing to do */
        أرجع;
    }
    pte_t *pt_تھاں = عدم;
    pte_t *pt = (pte_t *)لبھو_ret.vspace_جڑ;

    لكل (لفظ_t i = 0; i < UPT_پدھراں - 1 && pt != نشانہ_pt; i++) {
        pt_تھاں = pt + GET_UPT_اشاریہ(vptr, i);
        إذا (unlikely(!pte_pte_جدول_ptr_get_present(pt_تھاں))) {
            /* couldn't find it */
            أرجع;
        }
        pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_ادھار_پتہ(pt_تھاں));
    }

    إذا (pt != نشانہ_pt) {
        /* didn't find it */
        أرجع;
    }
    /* If we found a pt then ptSlot won't be null */
    أكد(pt_تھاں != عدم);
    *pt_تھاں = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)pt_تھاں, pptr_منزل_paddr(pt_تھاں));
    invalidateTLBByASID(asid);
}

فراغ unmap_ورقہ(vm_ورقہ_ناپ_t ورقہ_ناپ, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    لبھو_VSpace_For_ASID_ret_t لبھو_ret;
    lookupPTSlot_ret_t  lu_ret;
    pte_t pte;

    لبھو_ret = لبھو_VSpace_For_ASID(asid);
    إذا (لبھو_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    lu_ret = lookupPTSlot(لبھو_ret.vspace_جڑ, vptr);
    إذا (unlikely(lu_ret.pt_بٹ_بایاں != ورقہ_بٹ_For_ناپ(ورقہ_ناپ))) {
        /* Do nothing if the wrong size object was returned */
        أرجع;
    }

    pte = *(lu_ret.pt_تھاں);
    إذا (!pte_is_ورقہ_type(pte)) {
        /* Do nothing if no page is present */
        أرجع;
    }

    إذا (pte_get_ورقہ_ادھار_پتہ(pte) != pptr_منزل_paddr((فراغ *)pptr)) {
        /* Do nothing if the mapped page is not the same physical frame */
        أرجع;
    }

    *(lu_ret.pt_تھاں) = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)lu_ret.pt_تھاں, pptr_منزل_paddr(lu_ret.pt_تھاں));
    أكد(asid < ثنائی_ہندسہ(16));
    invalidateTLBByASIDVA(asid, vptr);
}

فراغ deleteASID(asid_t asid, vspace_جڑ_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_بلند(asid)];

    إذا (poolPtr != عدم) {
        asid_map_t asid_map = poolPtr->array[ASID_کم(asid)];
        إذا (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_جڑ_t *)asid_map_asid_map_vspace_get_vspace_جڑ(asid_map) == vspace) {
            invalidateTLBByASID(asid);
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
            invalidateASIDEntry(asid);
#جے_انت
            poolPtr->array[ASID_کم(asid)] = asid_map_asid_map_none_new();
            سیٹ_VMRoot(گرہ_STATE(ksCurThread));
        }
    }
}

فراغ deleteASIDPool(asid_t asid_ادھار, asid_pool_t *pool)
{
    لفظ_t وتھ;

    أكد((asid_ادھار & نقاب(asid_کم_بٹ)) == 0);

    إذا (armKSASIDTable[ASID_بلند(asid_ادھار)] == pool) {
        لكل (وتھ = 0; وتھ < ثنائی_ہندسہ(asid_کم_بٹ); وتھ++) {
            asid_map_t asid_map = pool->array[وتھ];
            إذا (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                invalidateTLBByASID(asid_ادھار + وتھ);
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                invalidateASIDEntry(asid_ادھار + وتھ);
#جے_انت
            }
        }
        armKSASIDTable[ASID_بلند(asid_ادھار)] = عدم;
        سیٹ_VMRoot(گرہ_STATE(ksCurThread));
    }
}

ساكن فراغ doFlush(لفظ_t invLabel, vptr_t شروع, vptr_t سرا, paddr_t pstart)
{
    بدل (invLabel) {
    حالة ARMVSpace_Clean_انکڑے:
    حالة ARMPage_Clean_انکڑے:
        cleanCacheRange_RAM(شروع, سرا, pstart);
        اكسر;

    حالة ARMVSpace_Invalidate_انکڑے:
    حالة ARMPage_Invalidate_انکڑے:
        invalidateCacheRange_RAM(شروع, سرا, pstart);
        اكسر;

    حالة ARMVSpace_Clean_Invalidate_انکڑے:
    حالة ARMPage_Clean_Invalidate_انکڑے:
        cleanInvalidateCacheRange_RAM(شروع, سرا, pstart);
        اكسر;

    حالة ARMVSpaceUnify_Instruction:
    حالة ARMPageUnify_Instruction:
        /* First clean data lines to point of unification... */
        cleanCacheRange_PoU(شروع, سرا, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(شروع, سرا, pstart);
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

ساكن exception_t performVSpaceFlush(لفظ_t invLabel, vspace_جڑ_t *vspace_جڑ_2, asid_t asid,
                                      vptr_t شروع, vptr_t سرا, paddr_t pstart)
{

    إذا (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        لفظ_t ناپ = سرا - شروع;
        شروع = (vptr_t)paddr_منزل_pptr(pstart);
        سرا = شروع + ناپ;
        إذا (شروع < سرا) {
            doFlush(invLabel, شروع, سرا, pstart);
        }
    } وإلا {
        bool_t جڑ_switched;

        /* Flush if given a non zero range */
        إذا (شروع < سرا) {
            جڑ_switched = سیٹ_VMRoot_For_Flush(vspace_جڑ_2, asid);
            doFlush(invLabel, شروع, سرا, pstart);
            إذا (جڑ_switched) {
                سیٹ_VMRoot(گرہ_STATE(ksCurThread));
            }
        }
    }
    أرجع EXCEPTION_NONE;
}


ساكن exception_t perform_ورقہ_جدول_Invocation_Map(cap_t cap, cte_t *ct_تھاں, pte_t pte, pte_t *pt_تھاں)
{
    ct_تھاں->cap = cap;
    *pt_تھاں = pte;
    cleanByVA_PoU((vptr_t)pt_تھاں, pptr_منزل_paddr(pt_تھاں));

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_ورقہ_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_تھاں)
{
    إذا (cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PT_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_ورقہ_جدول(cap_ورقہ_جدول_cap_get_cap_PTMapped_ASID(cap),
                       cap_ورقہ_جدول_cap_get_cap_PTMapped_پتہ(cap), pt);
        clear_حافظہ_PT((فراغ *)pt, cap_get_cap_ناپ_بٹ(cap));
    }

    cap_ورقہ_جدول_cap_ptr_سیٹ_cap_PTIs_Mapped(&(ct_تھاں->cap), 0);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_ورقہ_Invocation_Map(asid_t asid, cap_t cap, cte_t *ct_تھاں,
                                            pte_t pte, pte_t *pt_تھاں)
{
    bool_t tlbflush_required = pte_ptr_get_valid(pt_تھاں);

    ct_تھاں->cap = cap;
    *pt_تھاں = pte;

    cleanByVA_PoU((vptr_t)pt_تھاں, pptr_منزل_paddr(pt_تھاں));
    إذا (unlikely(tlbflush_required)) {
        أكد(asid < ثنائی_ہندسہ(16));
        invalidateTLBByASIDVA(asid, cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap));
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_ورقہ_Invocation_Unmap(cap_t cap, cte_t *ct_تھاں)
{
    إذا (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap) != 0) {

        unmap_ورقہ(cap_چوکھٹا_cap_get_cap_FSize(cap),
                  cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                  cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                  cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap));
    }

    cap_t تھاں_Cap = ct_تھاں->cap;
    تھاں_Cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(تھاں_Cap, 0);
    تھاں_Cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(تھاں_Cap, asidInvalid);
    ct_تھاں->cap = تھاں_Cap;


    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_ورقہ_Flush(لفظ_t invLabel, vspace_جڑ_t *vspace_جڑ_2, asid_t asid,
                                    vptr_t شروع, vptr_t سرا, paddr_t pstart)
{
    إذا (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* We flush the cache with kernel virtual addresses since
         * the user virtual addresses are not valid in EL2.
         * Switching VMRoot is not required.
         */
        لفظ_t ناپ = سرا - شروع;
        شروع = (vptr_t)paddr_منزل_pptr(pstart);
        سرا = شروع + ناپ;

        إذا (شروع < سرا) {
            doFlush(invLabel, شروع, سرا, pstart);
        }
    } وإلا {
        bool_t جڑ_switched;

        إذا (شروع < سرا) {
            جڑ_switched = سیٹ_VMRoot_For_Flush(vspace_جڑ_2, asid);
            doFlush(invLabel, شروع, سرا, pstart);
            إذا (جڑ_switched) {
                سیٹ_VMRoot(گرہ_STATE(ksCurThread));
            }
        }
    }
    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_ورقہ_Get_پتہ(pptr_t ادھار_ptr, bool_t call)
{
    paddr_t ادھار = pptr_منزل_paddr((فراغ *)ادھار_ptr);

    tcb_t *thread;
    thread = گرہ_STATE(ksCurThread);
    إذا (call) {
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, thread);
        سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
        بنانشانی صحيح لمائی = سیٹ_MR(thread, ipc_بفر, 0, ادھار);
        سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_سنیہا_Info(
                        se_L4_سنیہا_Info_new(0, 0, 0, لمائی)));
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDControlInvocation(فراغ *چوکھٹا, cte_t *تھاں,
                                                cte_t *ماپا, asid_t asid_ادھار)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_سیٹ_cap_Free_اشاریہ(&(ماپا->cap),
                                         MAX_FREE_اشاریہ(cap_untyped_cap_get_cap_بلاک_ناپ(ماپا->cap)));

    memzero(چوکھٹا, ثنائی_ہندسہ(se_L4_ASIDPool_بٹ));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_داخل_کرو(
        cap_asid_pool_cap_new(
            asid_ادھار,         /* capASIDBase  */
            لفظ_REF(چوکھٹا)    /* capASIDPool  */
        ), ماپا, تھاں);

    أكد((asid_ادھار & نقاب(asid_کم_بٹ)) == 0);
    armKSASIDTable[ASID_بلند(asid_ادھار)] = (asid_pool_t *)چوکھٹا;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_ARMVSpace_جڑ_Invocation(لفظ_t invLabel, لفظ_t لمائی,
                                                 cte_t *cte, cap_t cap, لفظ_t *بفر)
{
    vptr_t شروع, سرا;
    paddr_t pstart;
    asid_t asid;
    vspace_جڑ_t *vspace_جڑ_2;
    lookupPTSlot_ret_t resolve_ret;
    لبھو_VSpace_For_ASID_ret_t لبھو_ret;
    pte_t pte;

    بدل (invLabel) {
    حالة ARMVSpace_Clean_انکڑے:
    حالة ARMVSpace_Invalidate_انکڑے:
    حالة ARMVSpace_Clean_Invalidate_انکڑے:
    حالة ARMVSpaceUnify_Instruction:

        إذا (لمائی < 2) {
            userError("VSpaceRoot Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شروع = getSyscallArg(0, بفر);
        سرا =   getSyscallArg(1, بفر);

        /* Check sanity of arguments */
        إذا (سرا <= شروع) {
            userError("VSpaceRoot Flush: Invalid range.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        إذا (سرا > USER_چوٹی) {
            userError("VSpaceRoot Flush: Exceed the user addressable region.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(!is_Valid_Native_جڑ(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Make sure that the supplied pgd is ok */
        vspace_جڑ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap));
        asid = cap_vspace_cap_get_capVSMappedASID(cap);

        لبھو_ret = لبھو_VSpace_For_ASID(asid);
        إذا (unlikely(لبھو_ret.حالت != EXCEPTION_NONE)) {
            userError("VSpaceRoot Flush: No VSpace for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(لبھو_ret.vspace_جڑ != vspace_جڑ_2)) {
            userError("VSpaceRoot Flush: Invalid VSpace Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = lookupPTSlot(vspace_جڑ_2, شروع);
        pte = *resolve_ret.pt_تھاں;

        /* Check that the returned slot is a page. */
        إذا (!pte_is_ورقہ_type(pte)) {

            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
            أرجع EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        إذا (گیڑا_تھلے(شروع, resolve_ret.pt_بٹ_بایاں) != گیڑا_تھلے(سرا - 1, resolve_ret.pt_بٹ_بایاں)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = شروع;
            current_syscall_error.rangeErrorMax = گیڑا_تھلے(شروع, resolve_ret.pt_بٹ_بایاں) +
                                                  نقاب(resolve_ret.pt_بٹ_بایاں);
            userError("VSpaceRoot Flush: cannot cross page boundary, valid range is [0x%lx..0x%lx)",
                      current_syscall_error.rangeErrorMin, current_syscall_error.rangeErrorMax);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When in EL1, the mapping must be write-able for ARMVSpaceInvalidate_Data */
        إذا (invLabel == ARMVSpace_Invalidate_انکڑے && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMVSpaceInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#جے_انت

        /* Calculate the physical start address. */
        paddr_t چوکھٹا_ادھار_2 = pte_get_ورقہ_ادھار_پتہ(pte);

        pstart = چوکھٹا_ادھار_2 + (شروع & نقاب(resolve_ret.pt_بٹ_بایاں));

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        إذا (pstart < PADDR_ادھار || ((سرا - شروع) + pstart) > PADDR_چوٹی) {
            userError("VSpaceRoot Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#جے_انت

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performVSpaceFlush(invLabel, vspace_جڑ_2, asid, شروع, سرا - 1, pstart);

    افتراضي:
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}


ساكن exception_t decode_ARMPage_جدول_Invocation(لفظ_t invLabel, لفظ_t لمائی,
                                                cte_t *cte, cap_t cap, لفظ_t *بفر)
{
    cap_t vspace_جڑ_Cap;
    vspace_جڑ_t *vspace_جڑ_2;
    pte_t pte;
    asid_t asid;
    vptr_t vaddr;
    lookupPTSlot_ret_t pt_تھاں;
    لبھو_VSpace_For_ASID_ret_t لبھو_ret;

    إذا (invLabel == ARMPage_جدول_Unmap) {
        إذا (unlikely(!isFinalCapability(cte))) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_ورقہ_جدول_Invocation_Unmap(cap, cte);
    }

    إذا (unlikely(invLabel != ARMPage_جدول_Map)) {
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(لمائی < 2 || current_extra_caps.excaprefs[0] == عدم)) {
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر);
    vspace_جڑ_Cap = current_extra_caps.excaprefs[0]->cap;

    إذا (unlikely(!is_Valid_Native_جڑ(vspace_جڑ_Cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vspace_جڑ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_جڑ_Cap));
    asid = cap_vspace_cap_get_capVSMappedASID(vspace_جڑ_Cap);

    إذا (unlikely(vaddr > USER_چوٹی)) {
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    لبھو_ret = لبھو_VSpace_For_ASID(asid);
    إذا (unlikely(لبھو_ret.حالت != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سروت = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(لبھو_ret.vspace_جڑ != vspace_جڑ_2)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pt_تھاں = lookupPTSlot(vspace_جڑ_2, vaddr);

    إذا (unlikely(pt_تھاں.pt_بٹ_بایاں == se_L4_ورقہ_بٹ || pte_ptr_get_valid(pt_تھاں.pt_تھاں))) {
        current_syscall_error.type = se_L4_Delete_پہلا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pte = pte_pte_جدول_new(pptr_منزل_paddr(PTE_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(cap))));

    cap = cap_ورقہ_جدول_cap_سیٹ_cap_PTIs_Mapped(cap, 1);
    cap = cap_ورقہ_جدول_cap_سیٹ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ورقہ_جدول_cap_سیٹ_cap_PTMapped_پتہ(cap, (vaddr & ~نقاب(pt_تھاں.pt_بٹ_بایاں)));

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_ورقہ_جدول_Invocation_Map(cap, cte, pte, pt_تھاں.pt_تھاں);
}

ساكن مضمن bool_t CONST checkVPAlignment(vm_ورقہ_ناپ_t sz, لفظ_t w)
{
    أرجع (w & نقاب(ورقہ_بٹ_For_ناپ(sz))) == 0;
}

ساكن exception_t decodeARMFrameInvocation(لفظ_t invLabel, لفظ_t لمائی,
                                            cte_t *cte, cap_t cap, bool_t call, لفظ_t *بفر)
{
    بدل (invLabel) {
    حالة ARMPageMap: {
        vptr_t vaddr;
        paddr_t ادھار;
        cap_t vspace_جڑ_Cap;
        vspace_جڑ_t *vspace_جڑ_2;
        asid_t asid, چوکھٹا_asid;
        vm_rights_t vmRights, capRights;
        vm_ورقہ_ناپ_t چوکھٹا_ناپ;
        vm_گن_t گن;
        لبھو_VSpace_For_ASID_ret_t لبھو_ret;

        إذا (unlikely(لمائی < 3 || current_extra_caps.excaprefs[0] == عدم)) {
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, بفر);
        گن = vm_گن_From_لفظ(getSyscallArg(2, بفر));
        vspace_جڑ_Cap = current_extra_caps.excaprefs[0]->cap;

        چوکھٹا_ناپ = cap_چوکھٹا_cap_get_cap_FSize(cap);
        capRights = cap_چوکھٹا_cap_get_cap_FVMRights(cap);
        vmRights = نقاب_VMRights(capRights, rights_From_لفظ(getSyscallArg(1, بفر)));

        إذا (unlikely(!is_Valid_Native_جڑ(vspace_جڑ_Cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_جڑ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_جڑ_Cap));
        asid = cap_vspace_cap_get_capVSMappedASID(vspace_جڑ_Cap);

        لبھو_ret = لبھو_VSpace_For_ASID(asid);
        إذا (unlikely(لبھو_ret.حالت != EXCEPTION_NONE)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(لبھو_ret.vspace_جڑ != vspace_جڑ_2)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(!checkVPAlignment(چوکھٹا_ناپ, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* In the case of remap, the cap should have a valid asid */
        چوکھٹا_asid = cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap);

        إذا (چوکھٹا_asid != asidInvalid) {
            إذا (چوکھٹا_asid != asid) {
                userError("ARMPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_نمبر = 1;
                أرجع EXCEPTION_SYSCALL_ERROR;

            } وإلا إذا (cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_نمبر = 0;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        } وإلا {
            إذا (unlikely(vaddr + ثنائی_ہندسہ(ورقہ_بٹ_For_ناپ(چوکھٹا_ناپ)) - 1 > USER_چوٹی)) {
                current_syscall_error.type = se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_نمبر = 0;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asid);
        cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(cap, vaddr);

        ادھار = pptr_منزل_paddr((فراغ *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap));

        lookupPTSlot_ret_t lu_ret = lookupPTSlot(vspace_جڑ_2, vaddr);
        إذا (unlikely(lu_ret.pt_بٹ_بایاں != ورقہ_بٹ_For_ناپ(چوکھٹا_ناپ))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_بٹ_بایاں);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        bool_t cap_read = (capRights == VMReadOnly || capRights == VMReadWrite);
        pte_t pte = make_User_ورقہ_PTE(ادھار, vmRights, cap_read, گن, چوکھٹا_ناپ);

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_ورقہ_Invocation_Map(asid, cap, cte, pte, lu_ret.pt_تھاں);
    }

    حالة ARMPageUnmap:
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_ورقہ_Invocation_Unmap(cap, cte);

    حالة ARMPage_Clean_انکڑے:
    حالة ARMPage_Invalidate_انکڑے:
    حالة ARMPage_Clean_Invalidate_انکڑے:
    حالة ARMPageUnify_Instruction: {
        vptr_t شروع, سرا;
        vptr_t vaddr;
        asid_t asid;
        لفظ_t ورقہ_ناپ;
        لبھو_VSpace_For_ASID_ret_t لبھو_ret;

        إذا (لمائی < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap) == 0)) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap);
        asid = cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap);

        لبھو_ret = لبھو_VSpace_For_ASID(asid);
        إذا (unlikely(لبھو_ret.حالت != EXCEPTION_NONE)) {
            userError("Page Flush: No PGD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شروع = getSyscallArg(0, بفر);
        سرا =   getSyscallArg(1, بفر);

        /* check that the range is sane */
        إذا (سرا <= شروع) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* start and end are currently relative inside this page */
        ورقہ_ناپ = ثنائی_ہندسہ(ورقہ_بٹ_For_ناپ(cap_چوکھٹا_cap_get_cap_FSize(cap)));
        إذا (شروع >= ورقہ_ناپ || سرا > ورقہ_ناپ) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        لفظ_t pstart = pptr_منزل_paddr((فراغ *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)) + شروع;
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        إذا (pstart < PADDR_ادھار || ((سرا - شروع) + pstart) > PADDR_چوٹی) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#وإلا
        /* When in EL1, we are using the user page table for flushing and need to make sure
           the mapping info in the cap is not stale. */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(لبھو_ret.vspace_جڑ, vaddr);
        pte_t pte = *lu_ret.pt_تھاں;
        فراغ *ادھار_ptr = (فراغ *) cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap);
        إذا (unlikely(lu_ret.pt_بٹ_بایاں != ورقہ_بٹ_For_ناپ(cap_چوکھٹا_cap_get_cap_FSize(cap)) ||
                     !pte_is_ورقہ_type(pte) ||
                     pte_get_ورقہ_ادھار_پتہ(pte) != pptr_منزل_paddr(ادھار_ptr))) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* When in EL1, the mapping must be writeable for DC IVAC */
        إذا (invLabel == ARMPage_Invalidate_انکڑے && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMPageInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#جے_انت
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_ورقہ_Flush(invLabel, لبھو_ret.vspace_جڑ, asid, vaddr + شروع, vaddr + سرا - 1,
                                pstart);
    }

    حالة ARMPage_Get_پتہ:
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_ورقہ_Get_پتہ(cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap), call);

    افتراضي:
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(لفظ_t invLabel, لفظ_t لمائی, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, لفظ_t *بفر)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_vspace_cap:
        أرجع decode_ARMVSpace_جڑ_Invocation(invLabel, لمائی, cte, cap, بفر);
    حالة cap_ورقہ_جدول_cap:
        أرجع decode_ARMPage_جدول_Invocation(invLabel, لمائی, cte, cap, بفر);

    حالة cap_چوکھٹا_cap:
        أرجع decodeARMFrameInvocation(invLabel, لمائی, cte, cap, call, بفر);

    حالة cap_asid_control_cap: {
        لفظ_t i;
        asid_t asid_ادھار;
        لفظ_t اشاریہ, ڈونگھائی;
        cap_t untyped, جڑ;
        cte_t *ماپا_تھاں, *dest_تھاں;
        lookup_تھاں_ret_t lu_ret;
        فراغ *چوکھٹا;
        exception_t حالت;

        إذا (unlikely(invLabel != ARMASIDControlMakePool)) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(لمائی < 2 ||
                     current_extra_caps.excaprefs[0] == عدم ||
                     current_extra_caps.excaprefs[1] == عدم)) {
            current_syscall_error.type = se_L4_Truncated_سنیہا;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        اشاریہ = getSyscallArg(0, بفر);
        ڈونگھائی = getSyscallArg(1, بفر);
        ماپا_تھاں = current_extra_caps.excaprefs[0];
        untyped = ماپا_تھاں->cap;
        جڑ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        لكل (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        إذا (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_پہلا;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_ادھار = i << asid_کم_بٹ;

        إذا (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_بلاک_ناپ(untyped) != se_L4_ASIDPool_بٹ ||
                     cap_untyped_cap_get_capIsDevice(untyped))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        حالت = ensureNoChildren(ماپا_تھاں);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            أرجع حالت;
        }

        چوکھٹا = لفظ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_نشانہ_تھاں(جڑ, اشاریہ, ڈونگھائی);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            أرجع lu_ret.حالت;
        }
        dest_تھاں = lu_ret.تھاں;

        حالت = ensure_خالی_تھاواں_تھاں(dest_تھاں);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            أرجع حالت;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDControlInvocation(چوکھٹا, dest_تھاں, ماپا_تھاں, asid_ادھار);
    }

    حالة cap_asid_pool_cap: {
        cap_t vspaceCap;
        cte_t *vspace_Cap_تھاں;
        asid_pool_t *pool;
        لفظ_t i;
        asid_t asid;

        إذا (unlikely(invLabel != ARMASIDPool_سونپو)) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
            current_syscall_error.type = se_L4_Truncated_سنیہا;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_تھاں = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_تھاں->cap;

        إذا (unlikely(!is_VTable_جڑ(vspaceCap) || cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_بلند(cap_asid_pool_cap_get_capASIDBase(cap))];

        إذا (unlikely(!pool)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = false;
            current_lookup_fault = lookup_fault_invalid_جڑ_new();

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        لكل (i = 0; i < (1 << asid_کم_بٹ) && (asid + i == 0
                                               || (asid_map_get_type(pool->array[i]) != asid_map_asid_map_none)); i++);

        إذا (unlikely(i == 1 << asid_کم_بٹ)) {
            current_syscall_error.type = se_L4_Delete_پہلا;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDPoolInvocation(asid, pool, vspace_Cap_تھاں);
    }

    افتراضي:
        fail("Invalid ARM arch cap type");
    }
}

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
فراغ kernelPrefetchAbort(لفظ_t pc) VISIBLE;
فراغ kernel_انکڑے_Abort(لفظ_t pc) VISIBLE;

فراغ kernelPrefetchAbort(لفظ_t pc)
{
    لکھو("\n\nKERNEL PREFETCH ABORT!\n");
    لکھو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکھو("ESR (IFSR): 0x%"SEL4_PRIx_لفظ"\n", getIFSR());
    halt();
}

فراغ kernel_انکڑے_Abort(لفظ_t pc)
{
    لکھو("\n\nKERNEL DATA ABORT!\n");
    لکھو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکھو("FAR: 0x%"SEL4_PRIx_لفظ" ESR (DFSR): 0x%"SEL4_PRIx_لفظ"\n",
           getFAR(), getDFSR());
    halt();
}
#جے_انت /* CONFIG_DEBUG_BUILD */

#جے_تعریف_شدہ CONFIG_PRINTING
تعريفنوع بنية read_لفظ_From_VSpace_ret {
    exception_t حالت;
    لفظ_t قدر;
} read_لفظ_From_VSpace_ret_t;

ساكن read_لفظ_From_VSpace_ret_t read_لفظ_From_VSpace(vspace_جڑ_t *pd, لفظ_t vaddr)
{
    read_لفظ_From_VSpace_ret_t ret;
    لفظ_t وتھ;
    pptr_t kernel_vaddr;
    لفظ_t *قدر;

    lookupPTSlot_ret_t lookup_ret = lookupPTSlot(pd, vaddr);

    /* Check that the returned slot is a page. */
    إذا (!pte_ptr_get_valid(lookup_ret.pt_تھاں) ||
        (pte_pte_جدول_ptr_get_present(lookup_ret.pt_تھاں) && lookup_ret.pt_بٹ_بایاں > ورقہ_بٹ)) {
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    وتھ = vaddr & نقاب(lookup_ret.pt_بٹ_بایاں);
    kernel_vaddr = (لفظ_t)paddr_منزل_pptr(pte_ورقہ_ptr_get_ورقہ_ادھار_پتہ(lookup_ret.pt_تھاں));
    قدر = (لفظ_t *)(kernel_vaddr + وتھ);

    ret.حالت = EXCEPTION_NONE;
    ret.قدر = *قدر;
    أرجع ret;
}

فراغ Arch_user_ڈھیر_Trace(tcb_t *tptr)
{
    cap_t thread_جڑ;
    vspace_جڑ_t *vspace_جڑ_2;
    لفظ_t sp;

    thread_جڑ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the vspace root */
    إذا (cap_get_capType(thread_جڑ) != cap_vspace_cap) {
        لکھو("Invalid vspace\n");
        أرجع;
    }

    vspace_جڑ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_جڑ));
    sp = get_رجسٹر(tptr, SP_EL0);

    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    إذا (!IS_ALIGNED(sp, se_L4_لفظ_ناپ_بٹ)) {
        لکھو("SP not aligned\n");
        أرجع;
    }

    لكل (بنانشانی صحيح i = 0; i < CONFIG_USER_ڈھیر_TRACE_لمائی; i++) {
        لفظ_t پتہ = sp + (i * حجم(لفظ_t));
        read_لفظ_From_VSpace_ret_t نتیجہ = read_لفظ_From_VSpace(vspace_جڑ_2,
                                                             پتہ);
        إذا (نتیجہ.حالت == EXCEPTION_NONE) {
            لکھو("0x%"SEL4_PRIx_لفظ": 0x%"SEL4_PRIx_لفظ"\n",
                   پتہ, نتیجہ.قدر);
        } وإلا {
            لکھو("0x%"SEL4_PRIx_لفظ": INVALID\n", پتہ);
        }
    }
}
#جے_انت /* CONFIG_PRINTING */

#إذا defined(CONFIG_KERNEL_LOG_بفر)
exception_t benchmark_arch_map_log_بفر(لفظ_t چوکھٹا_cptr)
{
    lookup_Cap_And_تھاں_ret_t lu_ret;
    vm_ورقہ_ناپ_t چوکھٹا_ناپ;
    pptr_t  چوکھٹا_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_تھاں(گرہ_STATE(ksCurThread), چوکھٹا_cptr);

    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", چوکھٹا_cptr);
        current_fault = seL4_Fault_CapFault_new(چوکھٹا_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(lu_ret.cap) != cap_چوکھٹا_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(چوکھٹا_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکھٹا_ناپ = cap_چوکھٹا_cap_get_cap_FSize(lu_ret.cap);

    إذا (چوکھٹا_ناپ != ARMLarge_ورقہ) {
        userError("Invalid frame size. The kernel expects 2M log buffer");
        current_fault = seL4_Fault_CapFault_new(چوکھٹا_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکھٹا_pptr = cap_چوکھٹا_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_بفر = pptr_منزل_paddr((فراغ *) چوکھٹا_pptr);

    *armKSGlobalLogPTE = pte_pte_ورقہ_new(
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                             0, // XN
#وإلا
                             1, // UXN
#جے_انت
                             ks_User_Log_بفر,
                             0,                         /* global */
                             1,                         /* access flag */
                             SMP_TERNARY(SMP_SHARE, 0), /* Inner-shareable if SMP enabled, otherwise unshared */
                             0,                         /* VMKernelOnly */
                             عام_WT);

    cleanByVA_PoU((vptr_t)armKSGlobalLogPTE, addrFromKPPtr(armKSGlobalLogPTE));
    invalidateTranslationSingle(KS_LOG_PPTR);
    أرجع EXCEPTION_NONE;
}
#جے_انت /* CONFIG_KERNEL_LOG_BUFFER */
