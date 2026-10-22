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

#وصف_ڪريو SMP_SHARE   3

بنية lookupPTSlot_ret {
    pte_t *pt_جڳهه;
    لفظ_t pt_بٽ_کاٻو;
};
تعريفنوع بنية lookupPTSlot_ret lookupPTSlot_ret_t;

بنية ڳوليو_VSpace_For_ASID_ret {
    exception_t حالت;
    vspace_پاڙ_t *vspace_پاڙ;
};
تعريفنوع بنية ڳوليو_VSpace_For_ASID_ret ڳوليو_VSpace_For_ASID_ret_t;

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
        إذا (config_سيٽ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            أرجع 0;
        } وإلا {
            أرجع 0;
        }

    حالة VMReadWrite:
        إذا (config_سيٽ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            أرجع 3;
        } وإلا {
            أرجع 1;
        }

    حالة VMReadOnly:
        إذا (config_سيٽ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            أرجع 1;
        } وإلا {
            أرجع 3;
        }

    افتراضي:
        fail("Invalid VM rights");
    }
}

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
ساكن مضمن CONST لفظ_t pte_get_AP(pte_t pte)
{
    أكد(pte_is_صفحو_type(pte));
    بدل (pte_get_pte_type(pte)) {
    حالة pte_pte_4k_صفحو:
        أرجع pte_pte_4k_صفحو_get_AP(pte);

    حالة pte_pte_صفحو:
        أرجع pte_pte_صفحو_get_AP(pte);

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
#جيڪڏهن_پورو

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
BOOT_ڪوڊ فراغ map_kernel_چوکٺو(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_خاصيتون_t خاصيتون)
{
    أكد(vaddr >= PPTR_چوٽي);

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t uxn = vm_خاصيتون_get_arm_Execute_Never(خاصيتون);
#وإلا
    لفظ_t uxn = 1; /* unprivileged execute never */
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    لفظ_t attr_اشاريو;
    لفظ_t shareable;
    إذا (vm_خاصيتون_get_arm_صفحو_Cacheable(خاصيتون)) {
        attr_اشاريو = عام_2;
        shareable = SMP_TERNARY(SMP_SHARE, 0);
    } وإلا {
        attr_اشاريو = DEVICE_nGnRnE;
        shareable = 0;
    }
    armKSGlobalKernelPT[GET_KPT_اشاريو(vaddr, KLVL_FRM_ARM_PT_LVL(3))] = pte_pte_4k_صفحو_new(uxn, paddr,
                                                                                            0, /* global */
                                                                                            1, /* access flag */
                                                                                            shareable,
                                                                                            APFromVMRights(vm_rights),
                                                                                            attr_اشاريو);
}

/* Essentially, assert that the ELF, kernel devices, and log buffer are all in the same PUD.
   In the current code this is the top index. This is arbitrary, the important part
   is that they are all the same.
 */
compile_assert(elf_اندريون_kernel_mappings, GET_KPT_اشاريو(KERNEL_ELF_بنياد,
                                                     KLVL_FRM_ARM_PT_LVL(1)) == ٻٽو_انگ(PT_اشاريو_بٽ) - 1);
compile_assert(kdev_اندريون_kernel_mappings, GET_KPT_اشاريو(KDEV_بنياد,
                                                      KLVL_FRM_ARM_PT_LVL(1)) == ٻٽو_انگ(PT_اشاريو_بٽ) - 1);
compile_assert(log_buf_اندريون_kernel_mappings, GET_KPT_اشاريو(KS_LOG_بنياد,
                                                         KLVL_FRM_ARM_PT_LVL(1)) == ٻٽو_انگ(PT_اشاريو_بٽ) - 1);

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
BOOT_ڪوڊ فراغ map_kernel_elf_image(pte_t kernel_mapping_pds[])
{
    /** This is not strictly necessary, but to handle a non-aligned KERNEL_ELF_PADDR_BASE
     *  we'd need to do a bit more logic and probably do what riscv64 does
     *  where the *actual* KERNEL_ELF_BASE is rounded up to the alignment.
     *  This is simpler and memory is cheap.
     **/
    compile_assert(elf_paddr_aligned, IS_ALIGNED(KERNEL_ELF_PADDR_بنياد_RAW, se_L4_Large_صفحو_بٽ));
    compile_assert(elf_بنياد_aligned, IS_ALIGNED(KERNEL_ELF_بنياد, se_L4_Large_صفحو_بٽ));

    /* Please don't overlap with KDEV_BASE */
    أكد(KERNEL_ELF_چوٽي <= KDEV_بنياد);
    /* Please don't overlap with KS_LOG_PPTR */
    أكد(KERNEL_ELF_چوٽي <= KS_LOG_PPTR);

    ثابت لفظ_t pd_شروعات = GET_KPT_اشاريو(KERNEL_ELF_بنياد, KLVL_FRM_ARM_PT_LVL(2));
    /* pd_end and ELF_TOP are both exclusive, so handle that correctly */
    ثابت لفظ_t pd_پڇاڙي   = GET_KPT_اشاريو(KERNEL_ELF_چوٽي - 1, KLVL_FRM_ARM_PT_LVL(2)) + 1;

    paddr_t paddr = KERNEL_ELF_PADDR_بنياد;
    اڻ_استعمال_ٿيل pptr_t vaddr = KERNEL_ELF_بنياد;

    لكل (لفظ_t pd = pd_شروعات; pd < pd_پڇاڙي; pd++) {
        أكد(pd == GET_KPT_اشاريو(vaddr, KLVL_FRM_ARM_PT_LVL(2)));

        kernel_mapping_pds[pd] = pte_pte_صفحو_new(
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
                                     /* XN */ 0,
#وإلا
                                     /* UXN */ 1,
#جيڪڏهن_پورو
                                     /* page_base_address */ paddr,
                                     /* global (nG) */ 0,
                                     /* access flag (AF) */ 1,
                                     /* shareability (SH) */ SMP_TERNARY(SMP_SHARE, 0),
                                     /* AP */ APFromVMRights(VMKernelOnly),
                                     /* AttrIndx */ عام_2
                                 );

        paddr += ٻٽو_انگ(se_L4_Large_صفحو_بٽ);
        vaddr += ٻٽو_انگ(se_L4_Large_صفحو_بٽ);
    }
}

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
/* verify that the physical memory window as at the second entry of the PGD */
compile_assert(physical_دري_pgd_location,
               GET_KPT_اشاريو(PPTR_بنياد, KLVL_FRM_ARM_PT_LVL(0)) == 1);
#وإلا
/* verify that the physical memory window as at the last entry of the PGD */
compile_assert(physical_دري_pgd_location,
               GET_KPT_اشاريو(PPTR_بنياد, KLVL_FRM_ARM_PT_LVL(0)) == ٻٽو_انگ(PT_اشاريو_بٽ) - 1);
#جيڪڏهن_پورو
/* make sure that the PPTR_BASE and PPTR_TOP are huge paged aligned */
compile_assert(physical_دري_بنياد_aligned, IS_ALIGNED(PPTR_بنياد, se_L4_Huge_صفحو_بٽ));
compile_assert(physical_دري_چوٽي_aligned, IS_ALIGNED(PPTR_چوٽي, se_L4_Huge_صفحو_بٽ));

/* the PPTR_BASE starts from the bottom of the kernel PUD */
compile_assert(physical_دري_bottom_kernel_region,
               GET_KPT_اشاريو(PPTR_بنياد, KLVL_FRM_ARM_PT_LVL(1)) == 0);
/* the PPTR_TOP then ends at the second-to-last entry of the PUD
    note: because this is the exclusive value the assert is one higher than
          you would expect.
*/
compile_assert(physical_دري_چوٽي_kernel_region,
               GET_KPT_اشاريو(PPTR_چوٽي, KLVL_FRM_ARM_PT_LVL(1)) == ٻٽو_انگ(PT_اشاريو_بٽ) - 1);

BOOT_ڪوڊ فراغ map_kernel_physical_دري(فراغ)
{
    paddr_t paddr;
    pptr_t vaddr;
    لفظ_t idx;

    /* place all PDs except the last one in PUD */
    لكل (idx = GET_KPT_اشاريو(PPTR_بنياد, KLVL_FRM_ARM_PT_LVL(1)); idx < GET_KPT_اشاريو(PPTR_چوٽي, KLVL_FRM_ARM_PT_LVL(1));
         idx++) {
        armKSGlobalKernelPUD[idx] = pte_pte_جدول_new(
                                        addrFromKPPtr(&armKSGlobalKernelPDs[idx][0])
                                    );
    }

    /* map the kernel window using large pages */
    vaddr = PPTR_بنياد;
    لكل (paddr = PADDR_بنياد; paddr < PADDR_چوٽي; paddr += ٻٽو_انگ(se_L4_Large_صفحو_بٽ)) {
        armKSGlobalKernelPDs[GET_KPT_اشاريو(vaddr, KLVL_FRM_ARM_PT_LVL(1))][GET_KPT_اشاريو(vaddr,
                                                                                         KLVL_FRM_ARM_PT_LVL(2))] = pte_pte_صفحو_new(
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
                                                                                                                        1, // XN
#وإلا
                                                                                                                        1, // UXN
#جيڪڏهن_پورو
                                                                                                                        paddr,
                                                                                                                        0,                        /* global */
                                                                                                                        1,                        /* access flag */
                                                                                                                        SMP_TERNARY(SMP_SHARE, 0),        /* Inner-shareable if SMP enabled, otherwise unshared */
                                                                                                                        0,                        /* VMKernelOnly */
                                                                                                                        عام_2
                                                                                                                    );
        vaddr += ٻٽو_انگ(se_L4_Large_صفحو_بٽ);
    }
}

BOOT_ڪوڊ فراغ map_kernel_دري(فراغ)
{
    /* place the PUD into the PGD */
    armKSGlobalKernelPGD[GET_KPT_اشاريو(PPTR_بنياد, KLVL_FRM_ARM_PT_LVL(0))] =
        pte_pte_جدول_new(addrFromKPPtr(armKSGlobalKernelPUD));

    map_kernel_physical_دري();

    ثابت لفظ_t elf_pud_idx = GET_KPT_اشاريو(KERNEL_ELF_بنياد, KLVL_FRM_ARM_PT_LVL(1));
    pte_t *kernel_mapping_pds = armKSGlobalKernelPDs[elf_pud_idx];

    /* place the kernel mapping PDs in the kernel mapping PUD */
    armKSGlobalKernelPUD[elf_pud_idx] = pte_pte_جدول_new(addrFromKPPtr(kernel_mapping_pds));

    /* map the kernel ELF */
    map_kernel_elf_image(kernel_mapping_pds);

    /* put the PT into the PD for device window. It would be nice if
       we could pass kernel_mapping_pds[511] to map_kernel_devices(), but it is
       shared between AArch64 and AArch32, so... */
    kernel_mapping_pds[ٻٽو_انگ(PT_اشاريو_بٽ) - 1]
        = pte_pte_جدول_new(addrFromKPPtr(armKSGlobalKernelPT));

    map_kernel_devices();
}

/* When the hypervisor support is enabled, the stage-2 translation table format
 * is used for applications.
 * The global bit is always 0.
 * The memory attributes use the S2 translation values.
 */
ساكن BOOT_ڪوڊ فراغ map_it_چوکٺو_cap(cap_t vspace_cap, cap_t چوکٺو_cap, bool_t executable)
{
    vspace_پاڙ_t *vspace_پاڙ_2 = VSPACE_PTR(pptr_جو_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt;

    vptr_t vptr = cap_چوکٺو_cap_get_cap_FMapped_پتو(چوکٺو_cap);
    فراغ *pptr = (فراغ *)cap_چوکٺو_cap_get_cap_FBase_Ptr(چوکٺو_cap);

    أكد(cap_چوکٺو_cap_get_cap_FMapped_ASID(چوکٺو_cap) != 0);

#جيڪڏهن_وصف_ٿيل AARCH64_VSPACE_S2_شروعات_L1
    pud = vspace_پاڙ_2;
#وإلا
    vspace_پاڙ_2 += GET_UPT_اشاريو(vptr, ULVL_FRM_ARM_PT_LVL(0));
    أكد(pte_pte_جدول_ptr_get_present(vspace_پاڙ_2));
    pud = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنياد_پتو(vspace_پاڙ_2));
#جيڪڏهن_پورو
    pud += GET_UPT_اشاريو(vptr, ULVL_FRM_ARM_PT_LVL(1));
    أكد(pte_pte_جدول_ptr_get_present(pud));
    pd = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنياد_پتو(pud));
    pd += GET_UPT_اشاريو(vptr, ULVL_FRM_ARM_PT_LVL(2));
    أكد(pte_pte_جدول_ptr_get_present(pd));
    pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنياد_پتو(pd));
    *(pt + GET_UPT_اشاريو(vptr, ULVL_FRM_ARM_PT_LVL(3))) = pte_pte_4k_صفحو_new(
                                                              !executable,                    /* unprivileged execute never */
                                                              pptr_منزل_paddr(pptr),            /* page_base_address    */
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              0,
#وإلا
                                                              1,                              /* not global */
#جيڪڏهن_پورو
                                                              1,                              /* access flag */
                                                              SMP_TERNARY(SMP_SHARE, 0),              /* Inner-shareable if SMP enabled, otherwise unshared */
                                                              APFromVMRights(VMReadWrite),
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              S2_عام
#وإلا
                                                              عام_2
#جيڪڏهن_پورو
                                                          );
}

ساكن BOOT_ڪوڊ cap_t create_it_چوکٺو_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    vm_صفحو_سائيز_t چوکٺو_سائيز_2;
    إذا (use_large) {
        چوکٺو_سائيز_2 = ARMLarge_صفحو;
    } وإلا {
        چوکٺو_سائيز_2 = ARMSmall_صفحو;
    }
    أرجع
        cap_چوکٺو_cap_new(
            asid,                          /* capFMappedASID */
            pptr,                          /* capFBasePtr */
            چوکٺو_سائيز_2,                    /* capFSize */
            vptr,                          /* capFMappedAddress */
            لفظ_From_VMRights(VMReadWrite), /* capFVMRights */
            false                          /* capFIsDevice */
        );
}

ساكن BOOT_ڪوڊ فراغ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    vspace_پاڙ_t *vspace_پاڙ_2 = VSPACE_PTR(pptr_جو_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt = PT_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_صفحو_جدول_cap_get_cap_PTMapped_پتو(pt_cap);

    أكد(cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(pt_cap));

#جيڪڏهن_وصف_ٿيل AARCH64_VSPACE_S2_شروعات_L1
    pud = vspace_پاڙ_2;
#وإلا
    vspace_پاڙ_2 += GET_UPT_اشاريو(vptr, ULVL_FRM_ARM_PT_LVL(0));
    أكد(pte_pte_جدول_ptr_get_present(vspace_پاڙ_2));
    pud = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنياد_پتو(vspace_پاڙ_2));
#جيڪڏهن_پورو
    pud += GET_UPT_اشاريو(vptr, ULVL_FRM_ARM_PT_LVL(1));
    أكد(pte_pte_جدول_ptr_get_present(pud));
    pd = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنياد_پتو(pud));
    *(pd + GET_UPT_اشاريو(vptr, ULVL_FRM_ARM_PT_LVL(2))) = pte_pte_جدول_new(
                                                              pptr_منزل_paddr(pt)
                                                          );
}

ساكن BOOT_ڪوڊ cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحو_جدول_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pt_cap(vspace_cap, cap);
    أرجع cap;
}

ساكن BOOT_ڪوڊ فراغ map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    vspace_پاڙ_t *vspace_پاڙ_2 = VSPACE_PTR(pptr_جو_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd = PT_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(pd_cap));
    vptr_t vptr = cap_صفحو_جدول_cap_get_cap_PTMapped_پتو(pd_cap);

    أكد(cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(pd_cap));

#جيڪڏهن_وصف_ٿيل AARCH64_VSPACE_S2_شروعات_L1
    pud = vspace_پاڙ_2;
#وإلا
    vspace_پاڙ_2 += GET_UPT_اشاريو(vptr, ULVL_FRM_ARM_PT_LVL(0));
    أكد(pte_pte_جدول_ptr_get_present(vspace_پاڙ_2));
    pud = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنياد_پتو(vspace_پاڙ_2));
#جيڪڏهن_پورو
    *(pud + GET_UPT_اشاريو(vptr, ULVL_FRM_ARM_PT_LVL(1))) = pte_pte_جدول_new(
                                                               pptr_منزل_paddr(pd)
                                                           );
}

ساكن BOOT_ڪوڊ cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحو_جدول_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pd_cap(vspace_cap, cap);
    أرجع cap;
}

#جيڪڏهن_وصف_نه_ٿيل AARCH64_VSPACE_S2_شروعات_L1
ساكن BOOT_ڪوڊ فراغ map_it_pud_cap(cap_t vspace_cap, cap_t pud_cap)
{
    pte_t *pgd = PT_PTR(pptr_جو_cap(vspace_cap));
    pte_t *pud = PT_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(pud_cap));
    vptr_t vptr = cap_صفحو_جدول_cap_get_cap_PTMapped_پتو(pud_cap);

    أكد(cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(pud_cap));

    *(pgd + GET_UPT_اشاريو(vptr, ULVL_FRM_ARM_PT_LVL(0))) = pte_pte_جدول_new(
                                                               pptr_منزل_paddr(pud));
}

ساكن BOOT_ڪوڊ cap_t create_it_pud_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحو_جدول_cap_new(
              asid,               /* capPTMappedASID */
              pptr,               /* capPTBasePtr */
              1,                  /* capPTIsMapped */
              vptr                /* capPTMappedAddress */
          );
    map_it_pud_cap(vspace_cap, cap);
    أرجع cap;
}
#جيڪڏهن_پورو /* AARCH64_VSPACE_S2_START_L1 */
BOOT_ڪوڊ لفظ_t arch_get_n_paging(v_region_t it_v_reg)
{
    أرجع
#جيڪڏهن_وصف_نه_ٿيل AARCH64_VSPACE_S2_شروعات_L1
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_بٽ(ULVL_FRM_ARM_PT_LVL(0))) +
#جيڪڏهن_پورو
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_بٽ(ULVL_FRM_ARM_PT_LVL(1))) +
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_بٽ(ULVL_FRM_ARM_PT_LVL(2)));
}

BOOT_ڪوڊ cap_t create_it_پتو_space(cap_t پاڙ_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_جڳهه_Pos جڳهه_pos_before;
    se_L4_جڳهه_Pos جڳهه_pos_after;

    /* create the PGD */
    vspace_cap = cap_vspace_cap_new(
                     IT_ASID,           /* capVSMappedASID */
                     rootserver.vspace, /* capVSBasePtr    */
                     1                  /* capVSIsMapped   */
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_SMMU
                     , CB_INVALID       /* capVSMappedCB   */
#جيڪڏهن_پورو
                 );
    جڳهه_pos_before = ndks_boot.جڳهه_pos_cur;
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

#جيڪڏهن_وصف_نه_ٿيل AARCH64_VSPACE_S2_شروعات_L1
    /* Create any PUDs needed for the user land image */
    لكل (vptr = ڦيرو_هيٺ_ڏانهن(it_v_reg.شروعات, GET_ULVL_PGSIZE_بٽ(ULVL_FRM_ARM_PT_LVL(0)));
         vptr < it_v_reg.پڇاڙي;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(0))) {
        إذا (!provide_cap(پاڙ_cnode_cap, create_it_pud_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            أرجع cap_null_cap_new();
        }
    }
#جيڪڏهن_پورو
    /* Create any PDs needed for the user land image */
    لكل (vptr = ڦيرو_هيٺ_ڏانهن(it_v_reg.شروعات, GET_ULVL_PGSIZE_بٽ(ULVL_FRM_ARM_PT_LVL(1)));
         vptr < it_v_reg.پڇاڙي;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))) {
        إذا (!provide_cap(پاڙ_cnode_cap, create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            أرجع cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    لكل (vptr = ڦيرو_هيٺ_ڏانهن(it_v_reg.شروعات, GET_ULVL_PGSIZE_بٽ(ULVL_FRM_ARM_PT_LVL(2)));
         vptr < it_v_reg.پڇاڙي;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))) {
        إذا (!provide_cap(پاڙ_cnode_cap, create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            أرجع cap_null_cap_new();
        }
    }

    جڳهه_pos_after = ndks_boot.جڳهه_pos_cur;
    ndks_boot.bi_چوکٺو->userImagePaging = (se_L4_جڳهه_Region) {
        جڳهه_pos_before, جڳهه_pos_after
    };
    أرجع vspace_cap;
}

BOOT_ڪوڊ cap_t create_unmapped_it_چوکٺو_cap(pptr_t pptr, bool_t use_large)
{
    أرجع create_it_چوکٺو_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_ڪوڊ cap_t create_mapped_it_چوکٺو_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_چوکٺو_cap(pptr, vptr, asid, use_large);
    map_it_چوکٺو_cap(pd_cap, cap, executable);
    أرجع cap;
}

BOOT_ڪوڊ فراغ activate_kernel_vspace(فراغ)
{
    cleanInvalidateL1Caches();
    سيٽ_Current_Kernel_VSpace_پاڙ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalKernelPGD)));

    /* Prevent elf-loader address translation to fill up TLB */
    سيٽ_Current_User_VSpace_پاڙ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));

    invalidateLocalTLB();
    تالو_TLBEntry(KERNEL_ELF_بنياد);
}

BOOT_ڪوڊ فراغ write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_جو_cap(it_ap_cap));
    asid_map_t asid_map = asid_map_asid_map_vspace_new(
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_SMMU
                              /* bind_cb: Number of bound context banks */
                              0,
#جيڪڏهن_پورو
                              /* vspace_root: reference to vspace root page table object */
                              (لفظ_t)cap_vspace_cap_get_capVSBasePtr(it_vspace_cap)
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
                              /* stored_hw_vmid, stored_vmid_valid: Assigned hardware VMID for TLB. */
                              , 0, false
#جيڪڏهن_پورو
                          );
    ap->array[ASID_هيٺيون(IT_ASID)] = asid_map;
    armKSASIDTable[ASID_مٿاهون(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

asid_map_t ڳوليو_Map_For_ASID(asid_t asid)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_مٿاهون(asid)];
    إذا (!poolPtr) {
        أرجع asid_map_asid_map_none_new();
    }

    أرجع poolPtr->array[ASID_هيٺيون(asid)];
}

ساكن ڳوليو_VSpace_For_ASID_ret_t ڳوليو_VSpace_For_ASID(asid_t asid)
{
    ڳوليو_VSpace_For_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = ڳوليو_Map_For_ASID(asid);
    إذا (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_پاڙ_new();

        ret.vspace_پاڙ = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.vspace_پاڙ = (vspace_پاڙ_t *)asid_map_asid_map_vspace_get_vspace_پاڙ(asid_map);
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

    إذا (unlikely(cap_get_capType(بفر_Cap) != cap_چوکٺو_cap)) {
        أرجع عدم;
    }
    إذا (unlikely(cap_چوکٺو_cap_get_cap_FIs_Device(بفر_Cap))) {
        أرجع عدم;
    }

    vm_rights = cap_چوکٺو_cap_get_cap_FVMRights(بفر_Cap);
    إذا (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        لفظ_t بنياد_Ptr;
        بنانشان صحيح صفحو_بٽ_2;

        بنياد_Ptr = cap_چوکٺو_cap_get_cap_FBase_Ptr(بفر_Cap);
        صفحو_بٽ_2 = صفحو_بٽ_For_سائيز(cap_چوکٺو_cap_get_cap_FSize(بفر_Cap));
        أرجع (لفظ_t *)(بنياد_Ptr + (w_بفر_Ptr & نقاب(صفحو_بٽ_2)));
    } وإلا {
        أرجع عدم;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    إذا (cap_get_capType(cap) != cap_چوکٺو_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(cap_چوکٺو_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!IS_ALIGNED(vptr, se_L4_IPCBuffer_سائيز_بٽ)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (صحيح)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

ساكن lookupPTSlot_ret_t lookupPTSlot(vspace_پاڙ_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    لفظ_t level = UPT_سطحون - 1;
    pte_t *pt = vspace;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_بٽ_کاٻو = PT_اشاريو_بٽ * level + se_L4_صفحو_بٽ;
    ret.pt_جڳهه = pt + ((vptr >> ret.pt_بٽ_کاٻو) & نقاب(se_L4_VSpace_اشاريو_بٽ));

    طالما (pte_pte_جدول_ptr_get_present(ret.pt_جڳهه) && likely(level > 0)) {
        level--;
        ret.pt_بٽ_کاٻو -= PT_اشاريو_بٽ;
        pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنياد_پتو(ret.pt_جڳهه));
        ret.pt_جڳهه = pt + ((vptr >> ret.pt_بٽ_کاٻو) & نقاب(PT_اشاريو_بٽ));
    }

    أرجع ret;
}

/* Note that if the hypervisor support is enabled, the user page tables use
 * stage-2 translation format. Otherwise, they follow the stage-1 translation format.
 */
ساكن pte_t make_User_صفحو_PTE(paddr_t paddr, vm_rights_t vm_rights, bool_t cap_read, vm_خاصيتون_t خاصيتون,
                             vm_صفحو_سائيز_t صفحو_سائيز)
{
    bool_t nonexecutable = vm_خاصيتون_get_arm_Execute_Never(خاصيتون) || !cap_read;
    لفظ_t cacheable = vm_خاصيتون_get_arm_صفحو_Cacheable(خاصيتون);

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t nG = 0; /* not global */
    لفظ_t attridx = cacheable ? S2_عام : S2_DEVICE_nGnRnE;
#وإلا
    لفظ_t nG = 1; /* not global */
    لفظ_t attridx = cacheable ? عام_2 : DEVICE_nGnRnE;
#جيڪڏهن_پورو

    /* Inner-shareable if SMP enabled, otherwise unshared (ignored for devices) */
    لفظ_t shareable = cacheable ? SMP_TERNARY(SMP_SHARE, 0) : 0;

    إذا (صفحو_سائيز == ARMSmall_صفحو) {
        أرجع pte_pte_4k_صفحو_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                   shareable, APFromVMRights(vm_rights), attridx);
    } وإلا {
        أرجع pte_pte_صفحو_new(nonexecutable, paddr, nG, 1 /* access flag */,
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

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        /* use the IPA */
        إذا (ARCH_ڳنڍ_STATE(armHSVCPUActive)) {
            addr = GET_PAR_ADDR(پتو_Translate_S1(addr)) | (addr & نقاب(صفحو_بٽ));
        }
#جيڪڏهن_پورو
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        أرجع EXCEPTION_FAULT;
    }

    حالة ARMPrefetchAbort: {
        لفظ_t pc, fault;

        pc = getRestartPC(thread);
        fault = getIFSR();

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (ARCH_ڳنڍ_STATE(armHSVCPUActive)) {
            pc = GET_PAR_ADDR(پتو_Translate_S1(pc)) | (pc & نقاب(صفحو_بٽ));
        }
#جيڪڏهن_پورو
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        أرجع EXCEPTION_FAULT;
    }

    افتراضي:
        fail("Invalid VM fault type");
    }
}

bool_t CONST is_VTable_پاڙ(cap_t cap)
{
    أرجع cap_get_capType(cap) == cap_vspace_cap;
}

bool_t CONST is_Valid_Native_پاڙ(cap_t cap)
{
    أرجع is_VTable_پاڙ(cap) &&
           cap_vspace_cap_get_capVSIsMapped(cap);
}

bool_t CONST is_Valid_VTable_پاڙ(cap_t cap)
{
    أرجع is_Valid_Native_پاڙ(cap);
}

فراغ سيٽ_VMRoot(tcb_t *tcb)
{
    cap_t thread_پاڙ;
    asid_t asid;
    vspace_پاڙ_t *vspace_پاڙ_2;
    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret;

    thread_پاڙ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    /* ASID/VMID 0 is never allocated to a real VSpace (see findFreeHWASID()
     * for the hypervisor case), so the empty global VSpace can be installed
     * under it without flushing the TLB. */
    إذا (!is_Valid_Native_پاڙ(thread_پاڙ)) {
        سيٽ_Current_User_VSpace_پاڙ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        أرجع;
    }

    vspace_پاڙ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_پاڙ));
    asid = cap_vspace_cap_get_capVSMappedASID(thread_پاڙ);
    ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
    إذا (unlikely(ڳوليو_ret.حالت != EXCEPTION_NONE || ڳوليو_ret.vspace_پاڙ != vspace_پاڙ_2)) {
        سيٽ_Current_User_VSpace_پاڙ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        أرجع;
    }

    armv_contextSwitch(vspace_پاڙ_2, asid);
}

ساكن bool_t سيٽ_VMRoot_For_Flush(vspace_پاڙ_t *vspace, asid_t asid)
{
    cap_t thread_پاڙ;

    thread_پاڙ = TCB_PTR_CTE_PTR(ڳنڍ_STATE(ksCurThread), tcbVTable)->cap;

    إذا (cap_get_capType(thread_پاڙ) == cap_vspace_cap &&
        cap_vspace_cap_get_capVSIsMapped(thread_پاڙ) &&
        VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_پاڙ)) == vspace) {
        أرجع false;
    }

    armv_contextSwitch(vspace, asid);
    أرجع true;
}


#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT

ساكن مضمن asid_pool_t *getPoolPtr(asid_t asid)
{
    أرجع armKSASIDTable[ASID_مٿاهون(asid)];
}

ساكن مضمن asid_map_t getASIDMap(asid_pool_t *poolPtr, asid_t asid)
{
    أكد(poolPtr != عدم);
    أرجع poolPtr->array[ASID_هيٺيون(asid)];
}

ساكن مضمن فراغ سيٽ_ASIDMap(asid_pool_t *poolPtr, asid_t asid, asid_map_t asid_map)
{
    أكد(poolPtr != عدم);
    poolPtr->array[ASID_هيٺيون(asid)] = asid_map;
}

ساكن فراغ invalidateASID(asid_t asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    أكد(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_سيٽ_stored_hw_vmid(asid_map, 0);
    asid_map = asid_map_asid_map_vspace_سيٽ_stored_vmid_valid(asid_map, false);

    سيٽ_ASIDMap(poolPtr, asid, asid_map);
}

ساكن فراغ storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    أكد(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_سيٽ_stored_hw_vmid(asid_map, hw_asid);
    asid_map = asid_map_asid_map_vspace_سيٽ_stored_vmid_valid(asid_map, true);

    سيٽ_ASIDMap(poolPtr, asid, asid_map);
    armKSHWASIDTable[hw_asid] = asid;
}

ساكن hw_asid_t ڳوليو_Free_HWASID(فراغ)
{
    لفظ_t hw_asid_وٿي;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    لكل (hw_asid_وٿي = 0;
         hw_asid_وٿي <= (لفظ_t)((hw_asid_t) - 1);
         hw_asid_وٿي++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_وٿي);
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

    asid_map = ڳوليو_Map_For_ASID(asid);
    إذا (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        أرجع asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    } وإلا {
        hw_asid_t new_hw_asid;

        new_hw_asid = ڳوليو_Free_HWASID();
        storeHWASID(asid, new_hw_asid);
        أرجع new_hw_asid;
    }
}

ساكن فراغ invalidateASIDEntry(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = ڳوليو_Map_For_ASID(asid);
    إذا (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        armKSHWASIDTable[asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_SMMU
ساكن لفظ_t getASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_مٿاهون(asid)];
    أكد(asidPool);

    asid_map_t asid_map = asidPool->array[ASID_هيٺيون(asid)];
    أكد(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    أرجع asid_map_asid_map_vspace_get_bind_cb(asid_map);
}

فراغ increaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_مٿاهون(asid)];
    أكد(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_هيٺيون(asid)];
    أكد(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_سيٽ_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) + 1);
}

فراغ decreaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_مٿاهون(asid)];
    أكد(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_هيٺيون(asid)];
    أكد(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_سيٽ_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) - 1);
}
#جيڪڏهن_پورو

ساكن مضمن فراغ invalidateTLBByASID(asid_t asid)
{
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_SMMU
    لفظ_t bind_cb = getASIDBindCB(asid);
    إذا (unlikely(bind_cb)) {
        invalidateSMMUTLBByASID(asid, bind_cb);
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = ڳوليو_Map_For_ASID(asid);
    إذا (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        أرجع;
    }
    invalidateTranslationASID(asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map));
#وإلا
    invalidateTranslationASID(asid);
#جيڪڏهن_پورو
}

ساكن مضمن فراغ invalidateTLBByASIDVA(asid_t asid, vptr_t vaddr)
{
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_SMMU
    لفظ_t bind_cb = getASIDBindCB(asid);
    إذا (unlikely(bind_cb)) {
        invalidateSMMUTLBByASIDVA(asid, vaddr, bind_cb);
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = ڳوليو_Map_For_ASID(asid);
    إذا (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        أرجع;
    }
    uint64_t hw_asid = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    invalidateTranslationSingle((hw_asid << 48) | vaddr >> se_L4_صفحو_بٽ);
#وإلا
    invalidateTranslationSingle((asid << 48) | vaddr >> se_L4_صفحو_بٽ);
#جيڪڏهن_پورو
}


فراغ unmap_صفحو_جدول(asid_t asid, vptr_t vptr, pte_t *هدف_pt)
{
    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
    إذا (unlikely(ڳوليو_ret.حالت != EXCEPTION_NONE)) {
        /* nothing to do */
        أرجع;
    }
    pte_t *pt_جڳهه = عدم;
    pte_t *pt = (pte_t *)ڳوليو_ret.vspace_پاڙ;

    لكل (لفظ_t i = 0; i < UPT_سطحون - 1 && pt != هدف_pt; i++) {
        pt_جڳهه = pt + GET_UPT_اشاريو(vptr, i);
        إذا (unlikely(!pte_pte_جدول_ptr_get_present(pt_جڳهه))) {
            /* couldn't find it */
            أرجع;
        }
        pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنياد_پتو(pt_جڳهه));
    }

    إذا (pt != هدف_pt) {
        /* didn't find it */
        أرجع;
    }
    /* If we found a pt then ptSlot won't be null */
    أكد(pt_جڳهه != عدم);
    *pt_جڳهه = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)pt_جڳهه, pptr_منزل_paddr(pt_جڳهه));
    invalidateTLBByASID(asid);
}

فراغ unmap_صفحو(vm_صفحو_سائيز_t صفحو_سائيز, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret;
    lookupPTSlot_ret_t  lu_ret;
    pte_t pte;

    ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
    إذا (ڳوليو_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    lu_ret = lookupPTSlot(ڳوليو_ret.vspace_پاڙ, vptr);
    إذا (unlikely(lu_ret.pt_بٽ_کاٻو != صفحو_بٽ_For_سائيز(صفحو_سائيز))) {
        /* Do nothing if the wrong size object was returned */
        أرجع;
    }

    pte = *(lu_ret.pt_جڳهه);
    إذا (!pte_is_صفحو_type(pte)) {
        /* Do nothing if no page is present */
        أرجع;
    }

    إذا (pte_get_صفحو_بنياد_پتو(pte) != pptr_منزل_paddr((فراغ *)pptr)) {
        /* Do nothing if the mapped page is not the same physical frame */
        أرجع;
    }

    *(lu_ret.pt_جڳهه) = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)lu_ret.pt_جڳهه, pptr_منزل_paddr(lu_ret.pt_جڳهه));
    أكد(asid < ٻٽو_انگ(16));
    invalidateTLBByASIDVA(asid, vptr);
}

فراغ deleteASID(asid_t asid, vspace_پاڙ_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_مٿاهون(asid)];

    إذا (poolPtr != عدم) {
        asid_map_t asid_map = poolPtr->array[ASID_هيٺيون(asid)];
        إذا (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_پاڙ_t *)asid_map_asid_map_vspace_get_vspace_پاڙ(asid_map) == vspace) {
            invalidateTLBByASID(asid);
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
            invalidateASIDEntry(asid);
#جيڪڏهن_پورو
            poolPtr->array[ASID_هيٺيون(asid)] = asid_map_asid_map_none_new();
            سيٽ_VMRoot(ڳنڍ_STATE(ksCurThread));
        }
    }
}

فراغ deleteASIDPool(asid_t asid_بنياد, asid_pool_t *pool)
{
    لفظ_t وٿي;

    أكد((asid_بنياد & نقاب(asid_هيٺيون_بٽ)) == 0);

    إذا (armKSASIDTable[ASID_مٿاهون(asid_بنياد)] == pool) {
        لكل (وٿي = 0; وٿي < ٻٽو_انگ(asid_هيٺيون_بٽ); وٿي++) {
            asid_map_t asid_map = pool->array[وٿي];
            إذا (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                invalidateTLBByASID(asid_بنياد + وٿي);
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
                invalidateASIDEntry(asid_بنياد + وٿي);
#جيڪڏهن_پورو
            }
        }
        armKSASIDTable[ASID_مٿاهون(asid_بنياد)] = عدم;
        سيٽ_VMRoot(ڳنڍ_STATE(ksCurThread));
    }
}

ساكن فراغ doFlush(لفظ_t invLabel, vptr_t شروعات, vptr_t پڇاڙي, paddr_t pstart)
{
    بدل (invLabel) {
    حالة ARMVSpace_Clean_معلومات:
    حالة ARMPage_Clean_معلومات:
        cleanCacheRange_RAM(شروعات, پڇاڙي, pstart);
        اكسر;

    حالة ARMVSpace_Invalidate_معلومات:
    حالة ARMPage_Invalidate_معلومات:
        invalidateCacheRange_RAM(شروعات, پڇاڙي, pstart);
        اكسر;

    حالة ARMVSpace_Clean_Invalidate_معلومات:
    حالة ARMPage_Clean_Invalidate_معلومات:
        cleanInvalidateCacheRange_RAM(شروعات, پڇاڙي, pstart);
        اكسر;

    حالة ARMVSpaceUnify_Instruction:
    حالة ARMPageUnify_Instruction:
        /* First clean data lines to point of unification... */
        cleanCacheRange_PoU(شروعات, پڇاڙي, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(شروعات, پڇاڙي, pstart);
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

ساكن exception_t performVSpaceFlush(لفظ_t invLabel, vspace_پاڙ_t *vspace_پاڙ_2, asid_t asid,
                                      vptr_t شروعات, vptr_t پڇاڙي, paddr_t pstart)
{

    إذا (config_سيٽ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        لفظ_t سائيز = پڇاڙي - شروعات;
        شروعات = (vptr_t)paddr_منزل_pptr(pstart);
        پڇاڙي = شروعات + سائيز;
        إذا (شروعات < پڇاڙي) {
            doFlush(invLabel, شروعات, پڇاڙي, pstart);
        }
    } وإلا {
        bool_t پاڙ_switched;

        /* Flush if given a non zero range */
        إذا (شروعات < پڇاڙي) {
            پاڙ_switched = سيٽ_VMRoot_For_Flush(vspace_پاڙ_2, asid);
            doFlush(invLabel, شروعات, پڇاڙي, pstart);
            إذا (پاڙ_switched) {
                سيٽ_VMRoot(ڳنڍ_STATE(ksCurThread));
            }
        }
    }
    أرجع EXCEPTION_NONE;
}


ساكن exception_t perform_صفحو_جدول_Invocation_Map(cap_t cap, cte_t *ct_جڳهه, pte_t pte, pte_t *pt_جڳهه)
{
    ct_جڳهه->cap = cap;
    *pt_جڳهه = pte;
    cleanByVA_PoU((vptr_t)pt_جڳهه, pptr_منزل_paddr(pt_جڳهه));

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحو_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_جڳهه)
{
    إذا (cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PT_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_صفحو_جدول(cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(cap),
                       cap_صفحو_جدول_cap_get_cap_PTMapped_پتو(cap), pt);
        clear_يادگيري_PT((فراغ *)pt, cap_get_cap_سائيز_بٽ(cap));
    }

    cap_صفحو_جدول_cap_ptr_سيٽ_cap_PTIs_Mapped(&(ct_جڳهه->cap), 0);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحو_Invocation_Map(asid_t asid, cap_t cap, cte_t *ct_جڳهه,
                                            pte_t pte, pte_t *pt_جڳهه)
{
    bool_t tlbflush_required = pte_ptr_get_valid(pt_جڳهه);

    ct_جڳهه->cap = cap;
    *pt_جڳهه = pte;

    cleanByVA_PoU((vptr_t)pt_جڳهه, pptr_منزل_paddr(pt_جڳهه));
    إذا (unlikely(tlbflush_required)) {
        أكد(asid < ٻٽو_انگ(16));
        invalidateTLBByASIDVA(asid, cap_چوکٺو_cap_get_cap_FMapped_پتو(cap));
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحو_Invocation_Unmap(cap_t cap, cte_t *ct_جڳهه)
{
    إذا (cap_چوکٺو_cap_get_cap_FMapped_ASID(cap) != 0) {

        unmap_صفحو(cap_چوکٺو_cap_get_cap_FSize(cap),
                  cap_چوکٺو_cap_get_cap_FMapped_ASID(cap),
                  cap_چوکٺو_cap_get_cap_FMapped_پتو(cap),
                  cap_چوکٺو_cap_get_cap_FBase_Ptr(cap));
    }

    cap_t جڳهه_Cap = ct_جڳهه->cap;
    جڳهه_Cap = cap_چوکٺو_cap_سيٽ_cap_FMapped_پتو(جڳهه_Cap, 0);
    جڳهه_Cap = cap_چوکٺو_cap_سيٽ_cap_FMapped_ASID(جڳهه_Cap, asidInvalid);
    ct_جڳهه->cap = جڳهه_Cap;


    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحو_Flush(لفظ_t invLabel, vspace_پاڙ_t *vspace_پاڙ_2, asid_t asid,
                                    vptr_t شروعات, vptr_t پڇاڙي, paddr_t pstart)
{
    إذا (config_سيٽ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* We flush the cache with kernel virtual addresses since
         * the user virtual addresses are not valid in EL2.
         * Switching VMRoot is not required.
         */
        لفظ_t سائيز = پڇاڙي - شروعات;
        شروعات = (vptr_t)paddr_منزل_pptr(pstart);
        پڇاڙي = شروعات + سائيز;

        إذا (شروعات < پڇاڙي) {
            doFlush(invLabel, شروعات, پڇاڙي, pstart);
        }
    } وإلا {
        bool_t پاڙ_switched;

        إذا (شروعات < پڇاڙي) {
            پاڙ_switched = سيٽ_VMRoot_For_Flush(vspace_پاڙ_2, asid);
            doFlush(invLabel, شروعات, پڇاڙي, pstart);
            إذا (پاڙ_switched) {
                سيٽ_VMRoot(ڳنڍ_STATE(ksCurThread));
            }
        }
    }
    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحو_Get_پتو(pptr_t بنياد_ptr, bool_t call)
{
    paddr_t بنياد = pptr_منزل_paddr((فراغ *)بنياد_ptr);

    tcb_t *thread;
    thread = ڳنڍ_STATE(ksCurThread);
    إذا (call) {
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, thread);
        سيٽ_رجسٽر(thread, badge_رجسٽر, 0);
        بنانشان صحيح ڊيگهائي = سيٽ_MR(thread, ipc_بفر, 0, بنياد);
        سيٽ_رجسٽر(thread, نياپو_Info_رجسٽر, لفظ_From_پيغام_Info(
                        se_L4_پيغام_Info_new(0, 0, 0, ڊيگهائي)));
    }
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDControlInvocation(فراغ *چوکٺو, cte_t *جڳهه,
                                                cte_t *والد, asid_t asid_بنياد)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_سيٽ_cap_Free_اشاريو(&(والد->cap),
                                         MAX_FREE_اشاريو(cap_untyped_cap_get_cap_بلاڪ_سائيز(والد->cap)));

    memzero(چوکٺو, ٻٽو_انگ(se_L4_ASIDPool_بٽ));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_وجهو(
        cap_asid_pool_cap_new(
            asid_بنياد,         /* capASIDBase  */
            لفظ_REF(چوکٺو)    /* capASIDPool  */
        ), والد, جڳهه);

    أكد((asid_بنياد & نقاب(asid_هيٺيون_بٽ)) == 0);
    armKSASIDTable[ASID_مٿاهون(asid_بنياد)] = (asid_pool_t *)چوکٺو;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_ARMVSpace_پاڙ_Invocation(لفظ_t invLabel, لفظ_t ڊيگهائي,
                                                 cte_t *cte, cap_t cap, لفظ_t *بفر)
{
    vptr_t شروعات, پڇاڙي;
    paddr_t pstart;
    asid_t asid;
    vspace_پاڙ_t *vspace_پاڙ_2;
    lookupPTSlot_ret_t resolve_ret;
    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret;
    pte_t pte;

    بدل (invLabel) {
    حالة ARMVSpace_Clean_معلومات:
    حالة ARMVSpace_Invalidate_معلومات:
    حالة ARMVSpace_Clean_Invalidate_معلومات:
    حالة ARMVSpaceUnify_Instruction:

        إذا (ڊيگهائي < 2) {
            userError("VSpaceRoot Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پيغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شروعات = getSyscallArg(0, بفر);
        پڇاڙي =   getSyscallArg(1, بفر);

        /* Check sanity of arguments */
        إذا (پڇاڙي <= شروعات) {
            userError("VSpaceRoot Flush: Invalid range.");
            current_syscall_error.type = se_L4_Invalid_دليل;
            current_syscall_error.invalid_دليل_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        إذا (پڇاڙي > USER_چوٽي) {
            userError("VSpaceRoot Flush: Exceed the user addressable region.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(!is_Valid_Native_پاڙ(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Make sure that the supplied pgd is ok */
        vspace_پاڙ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap));
        asid = cap_vspace_cap_get_capVSMappedASID(cap);

        ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
        إذا (unlikely(ڳوليو_ret.حالت != EXCEPTION_NONE)) {
            userError("VSpaceRoot Flush: No VSpace for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(ڳوليو_ret.vspace_پاڙ != vspace_پاڙ_2)) {
            userError("VSpaceRoot Flush: Invalid VSpace Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = lookupPTSlot(vspace_پاڙ_2, شروعات);
        pte = *resolve_ret.pt_جڳهه;

        /* Check that the returned slot is a page. */
        إذا (!pte_is_صفحو_type(pte)) {

            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
            أرجع EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        إذا (ڦيرو_هيٺ_ڏانهن(شروعات, resolve_ret.pt_بٽ_کاٻو) != ڦيرو_هيٺ_ڏانهن(پڇاڙي - 1, resolve_ret.pt_بٽ_کاٻو)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = شروعات;
            current_syscall_error.rangeErrorMax = ڦيرو_هيٺ_ڏانهن(شروعات, resolve_ret.pt_بٽ_کاٻو) +
                                                  نقاب(resolve_ret.pt_بٽ_کاٻو);
            userError("VSpaceRoot Flush: cannot cross page boundary, valid range is [0x%lx..0x%lx)",
                      current_syscall_error.rangeErrorMin, current_syscall_error.rangeErrorMax);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When in EL1, the mapping must be write-able for ARMVSpaceInvalidate_Data */
        إذا (invLabel == ARMVSpace_Invalidate_معلومات && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMVSpaceInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#جيڪڏهن_پورو

        /* Calculate the physical start address. */
        paddr_t چوکٺو_بنياد_2 = pte_get_صفحو_بنياد_پتو(pte);

        pstart = چوکٺو_بنياد_2 + (شروعات & نقاب(resolve_ret.pt_بٽ_کاٻو));

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        إذا (pstart < PADDR_بنياد || ((پڇاڙي - شروعات) + pstart) > PADDR_چوٽي) {
            userError("VSpaceRoot Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#جيڪڏهن_پورو

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performVSpaceFlush(invLabel, vspace_پاڙ_2, asid, شروعات, پڇاڙي - 1, pstart);

    افتراضي:
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}


ساكن exception_t decode_ARMPage_جدول_Invocation(لفظ_t invLabel, لفظ_t ڊيگهائي,
                                                cte_t *cte, cap_t cap, لفظ_t *بفر)
{
    cap_t vspace_پاڙ_Cap;
    vspace_پاڙ_t *vspace_پاڙ_2;
    pte_t pte;
    asid_t asid;
    vptr_t vaddr;
    lookupPTSlot_ret_t pt_جڳهه;
    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret;

    إذا (invLabel == ARMPage_جدول_Unmap) {
        إذا (unlikely(!isFinalCapability(cte))) {
            current_syscall_error.type = se_L4_Revoke_پهريون;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحو_جدول_Invocation_Unmap(cap, cte);
    }

    إذا (unlikely(invLabel != ARMPage_جدول_Map)) {
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(ڊيگهائي < 2 || current_extra_caps.excaprefs[0] == عدم)) {
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر);
    vspace_پاڙ_Cap = current_extra_caps.excaprefs[0]->cap;

    إذا (unlikely(!is_Valid_Native_پاڙ(vspace_پاڙ_Cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vspace_پاڙ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_پاڙ_Cap));
    asid = cap_vspace_cap_get_capVSMappedASID(vspace_پاڙ_Cap);

    إذا (unlikely(vaddr > USER_چوٽي)) {
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
    إذا (unlikely(ڳوليو_ret.حالت != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ذريعو = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(ڳوليو_ret.vspace_پاڙ != vspace_پاڙ_2)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pt_جڳهه = lookupPTSlot(vspace_پاڙ_2, vaddr);

    إذا (unlikely(pt_جڳهه.pt_بٽ_کاٻو == se_L4_صفحو_بٽ || pte_ptr_get_valid(pt_جڳهه.pt_جڳهه))) {
        current_syscall_error.type = se_L4_Delete_پهريون;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pte = pte_pte_جدول_new(pptr_منزل_paddr(PTE_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(cap))));

    cap = cap_صفحو_جدول_cap_سيٽ_cap_PTIs_Mapped(cap, 1);
    cap = cap_صفحو_جدول_cap_سيٽ_cap_PTMapped_ASID(cap, asid);
    cap = cap_صفحو_جدول_cap_سيٽ_cap_PTMapped_پتو(cap, (vaddr & ~نقاب(pt_جڳهه.pt_بٽ_کاٻو)));

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_صفحو_جدول_Invocation_Map(cap, cte, pte, pt_جڳهه.pt_جڳهه);
}

ساكن مضمن bool_t CONST checkVPAlignment(vm_صفحو_سائيز_t sz, لفظ_t w)
{
    أرجع (w & نقاب(صفحو_بٽ_For_سائيز(sz))) == 0;
}

ساكن exception_t decodeARMFrameInvocation(لفظ_t invLabel, لفظ_t ڊيگهائي,
                                            cte_t *cte, cap_t cap, bool_t call, لفظ_t *بفر)
{
    بدل (invLabel) {
    حالة ARMPageMap: {
        vptr_t vaddr;
        paddr_t بنياد;
        cap_t vspace_پاڙ_Cap;
        vspace_پاڙ_t *vspace_پاڙ_2;
        asid_t asid, چوکٺو_asid;
        vm_rights_t vmRights, capRights;
        vm_صفحو_سائيز_t چوکٺو_سائيز;
        vm_خاصيتون_t خاصيتون;
        ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret;

        إذا (unlikely(ڊيگهائي < 3 || current_extra_caps.excaprefs[0] == عدم)) {
            current_syscall_error.type = se_L4_Truncated_پيغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, بفر);
        خاصيتون = vm_خاصيتون_From_لفظ(getSyscallArg(2, بفر));
        vspace_پاڙ_Cap = current_extra_caps.excaprefs[0]->cap;

        چوکٺو_سائيز = cap_چوکٺو_cap_get_cap_FSize(cap);
        capRights = cap_چوکٺو_cap_get_cap_FVMRights(cap);
        vmRights = نقاب_VMRights(capRights, rights_From_لفظ(getSyscallArg(1, بفر)));

        إذا (unlikely(!is_Valid_Native_پاڙ(vspace_پاڙ_Cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_پاڙ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_پاڙ_Cap));
        asid = cap_vspace_cap_get_capVSMappedASID(vspace_پاڙ_Cap);

        ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
        إذا (unlikely(ڳوليو_ret.حالت != EXCEPTION_NONE)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(ڳوليو_ret.vspace_پاڙ != vspace_پاڙ_2)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(!checkVPAlignment(چوکٺو_سائيز, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* In the case of remap, the cap should have a valid asid */
        چوکٺو_asid = cap_چوکٺو_cap_get_cap_FMapped_ASID(cap);

        إذا (چوکٺو_asid != asidInvalid) {
            إذا (چوکٺو_asid != asid) {
                userError("ARMPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_عدد = 1;
                أرجع EXCEPTION_SYSCALL_ERROR;

            } وإلا إذا (cap_چوکٺو_cap_get_cap_FMapped_پتو(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_دليل;
                current_syscall_error.invalid_دليل_عدد = 0;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        } وإلا {
            إذا (unlikely(vaddr + ٻٽو_انگ(صفحو_بٽ_For_سائيز(چوکٺو_سائيز)) - 1 > USER_چوٽي)) {
                current_syscall_error.type = se_L4_Invalid_دليل;
                current_syscall_error.invalid_دليل_عدد = 0;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        cap = cap_چوکٺو_cap_سيٽ_cap_FMapped_ASID(cap, asid);
        cap = cap_چوکٺو_cap_سيٽ_cap_FMapped_پتو(cap, vaddr);

        بنياد = pptr_منزل_paddr((فراغ *)cap_چوکٺو_cap_get_cap_FBase_Ptr(cap));

        lookupPTSlot_ret_t lu_ret = lookupPTSlot(vspace_پاڙ_2, vaddr);
        إذا (unlikely(lu_ret.pt_بٽ_کاٻو != صفحو_بٽ_For_سائيز(چوکٺو_سائيز))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_بٽ_کاٻو);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        bool_t cap_read = (capRights == VMReadOnly || capRights == VMReadWrite);
        pte_t pte = make_User_صفحو_PTE(بنياد, vmRights, cap_read, خاصيتون, چوکٺو_سائيز);

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحو_Invocation_Map(asid, cap, cte, pte, lu_ret.pt_جڳهه);
    }

    حالة ARMPageUnmap:
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحو_Invocation_Unmap(cap, cte);

    حالة ARMPage_Clean_معلومات:
    حالة ARMPage_Invalidate_معلومات:
    حالة ARMPage_Clean_Invalidate_معلومات:
    حالة ARMPageUnify_Instruction: {
        vptr_t شروعات, پڇاڙي;
        vptr_t vaddr;
        asid_t asid;
        لفظ_t صفحو_سائيز;
        ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret;

        إذا (ڊيگهائي < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پيغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(cap_چوکٺو_cap_get_cap_FMapped_ASID(cap) == 0)) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = cap_چوکٺو_cap_get_cap_FMapped_پتو(cap);
        asid = cap_چوکٺو_cap_get_cap_FMapped_ASID(cap);

        ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
        إذا (unlikely(ڳوليو_ret.حالت != EXCEPTION_NONE)) {
            userError("Page Flush: No PGD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شروعات = getSyscallArg(0, بفر);
        پڇاڙي =   getSyscallArg(1, بفر);

        /* check that the range is sane */
        إذا (پڇاڙي <= شروعات) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_دليل;
            current_syscall_error.invalid_دليل_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* start and end are currently relative inside this page */
        صفحو_سائيز = ٻٽو_انگ(صفحو_بٽ_For_سائيز(cap_چوکٺو_cap_get_cap_FSize(cap)));
        إذا (شروعات >= صفحو_سائيز || پڇاڙي > صفحو_سائيز) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_دليل;
            current_syscall_error.invalid_دليل_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        لفظ_t pstart = pptr_منزل_paddr((فراغ *)cap_چوکٺو_cap_get_cap_FBase_Ptr(cap)) + شروعات;
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        إذا (pstart < PADDR_بنياد || ((پڇاڙي - شروعات) + pstart) > PADDR_چوٽي) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#وإلا
        /* When in EL1, we are using the user page table for flushing and need to make sure
           the mapping info in the cap is not stale. */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(ڳوليو_ret.vspace_پاڙ, vaddr);
        pte_t pte = *lu_ret.pt_جڳهه;
        فراغ *بنياد_ptr = (فراغ *) cap_چوکٺو_cap_get_cap_FBase_Ptr(cap);
        إذا (unlikely(lu_ret.pt_بٽ_کاٻو != صفحو_بٽ_For_سائيز(cap_چوکٺو_cap_get_cap_FSize(cap)) ||
                     !pte_is_صفحو_type(pte) ||
                     pte_get_صفحو_بنياد_پتو(pte) != pptr_منزل_paddr(بنياد_ptr))) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* When in EL1, the mapping must be writeable for DC IVAC */
        إذا (invLabel == ARMPage_Invalidate_معلومات && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMPageInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#جيڪڏهن_پورو
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحو_Flush(invLabel, ڳوليو_ret.vspace_پاڙ, asid, vaddr + شروعات, vaddr + پڇاڙي - 1,
                                pstart);
    }

    حالة ARMPage_Get_پتو:
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحو_Get_پتو(cap_چوکٺو_cap_get_cap_FBase_Ptr(cap), call);

    افتراضي:
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(لفظ_t invLabel, لفظ_t ڊيگهائي, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, لفظ_t *بفر)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_vspace_cap:
        أرجع decode_ARMVSpace_پاڙ_Invocation(invLabel, ڊيگهائي, cte, cap, بفر);
    حالة cap_صفحو_جدول_cap:
        أرجع decode_ARMPage_جدول_Invocation(invLabel, ڊيگهائي, cte, cap, بفر);

    حالة cap_چوکٺو_cap:
        أرجع decodeARMFrameInvocation(invLabel, ڊيگهائي, cte, cap, call, بفر);

    حالة cap_asid_control_cap: {
        لفظ_t i;
        asid_t asid_بنياد;
        لفظ_t اشاريو, اونهائي;
        cap_t untyped, پاڙ;
        cte_t *والد_جڳهه, *dest_جڳهه;
        lookup_جڳهه_ret_t lu_ret;
        فراغ *چوکٺو;
        exception_t حالت;

        إذا (unlikely(invLabel != ARMASIDControlMakePool)) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(ڊيگهائي < 2 ||
                     current_extra_caps.excaprefs[0] == عدم ||
                     current_extra_caps.excaprefs[1] == عدم)) {
            current_syscall_error.type = se_L4_Truncated_پيغام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        اشاريو = getSyscallArg(0, بفر);
        اونهائي = getSyscallArg(1, بفر);
        والد_جڳهه = current_extra_caps.excaprefs[0];
        untyped = والد_جڳهه->cap;
        پاڙ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        لكل (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        إذا (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_پهريون;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_بنياد = i << asid_هيٺيون_بٽ;

        إذا (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_بلاڪ_سائيز(untyped) != se_L4_ASIDPool_بٽ ||
                     cap_untyped_cap_get_capIsDevice(untyped))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        حالت = ensureNoChildren(والد_جڳهه);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            أرجع حالت;
        }

        چوکٺو = لفظ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_هدف_جڳهه(پاڙ, اشاريو, اونهائي);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            أرجع lu_ret.حالت;
        }
        dest_جڳهه = lu_ret.جڳهه;

        حالت = ensure_خالي_جايون_جڳهه(dest_جڳهه);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            أرجع حالت;
        }

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDControlInvocation(چوکٺو, dest_جڳهه, والد_جڳهه, asid_بنياد);
    }

    حالة cap_asid_pool_cap: {
        cap_t vspaceCap;
        cte_t *vspace_Cap_جڳهه;
        asid_pool_t *pool;
        لفظ_t i;
        asid_t asid;

        إذا (unlikely(invLabel != ARMASIDPool_سونپيو)) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
            current_syscall_error.type = se_L4_Truncated_پيغام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_جڳهه = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_جڳهه->cap;

        إذا (unlikely(!is_VTable_پاڙ(vspaceCap) || cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_مٿاهون(cap_asid_pool_cap_get_capASIDBase(cap))];

        إذا (unlikely(!pool)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = false;
            current_lookup_fault = lookup_fault_invalid_پاڙ_new();

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        لكل (i = 0; i < (1 << asid_هيٺيون_بٽ) && (asid + i == 0
                                               || (asid_map_get_type(pool->array[i]) != asid_map_asid_map_none)); i++);

        إذا (unlikely(i == 1 << asid_هيٺيون_بٽ)) {
            current_syscall_error.type = se_L4_Delete_پهريون;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDPoolInvocation(asid, pool, vspace_Cap_جڳهه);
    }

    افتراضي:
        fail("Invalid ARM arch cap type");
    }
}

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
فراغ kernelPrefetchAbort(لفظ_t pc) VISIBLE;
فراغ kernel_معلومات_Abort(لفظ_t pc) VISIBLE;

فراغ kernelPrefetchAbort(لفظ_t pc)
{
    لکو("\n\nKERNEL PREFETCH ABORT!\n");
    لکو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکو("ESR (IFSR): 0x%"SEL4_PRIx_لفظ"\n", getIFSR());
    halt();
}

فراغ kernel_معلومات_Abort(لفظ_t pc)
{
    لکو("\n\nKERNEL DATA ABORT!\n");
    لکو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکو("FAR: 0x%"SEL4_PRIx_لفظ" ESR (DFSR): 0x%"SEL4_PRIx_لفظ"\n",
           getFAR(), getDFSR());
    halt();
}
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
تعريفنوع بنية read_لفظ_From_VSpace_ret {
    exception_t حالت;
    لفظ_t قدر;
} read_لفظ_From_VSpace_ret_t;

ساكن read_لفظ_From_VSpace_ret_t read_لفظ_From_VSpace(vspace_پاڙ_t *pd, لفظ_t vaddr)
{
    read_لفظ_From_VSpace_ret_t ret;
    لفظ_t وٿي;
    pptr_t kernel_vaddr;
    لفظ_t *قدر;

    lookupPTSlot_ret_t lookup_ret = lookupPTSlot(pd, vaddr);

    /* Check that the returned slot is a page. */
    إذا (!pte_ptr_get_valid(lookup_ret.pt_جڳهه) ||
        (pte_pte_جدول_ptr_get_present(lookup_ret.pt_جڳهه) && lookup_ret.pt_بٽ_کاٻو > صفحو_بٽ)) {
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    وٿي = vaddr & نقاب(lookup_ret.pt_بٽ_کاٻو);
    kernel_vaddr = (لفظ_t)paddr_منزل_pptr(pte_صفحو_ptr_get_صفحو_بنياد_پتو(lookup_ret.pt_جڳهه));
    قدر = (لفظ_t *)(kernel_vaddr + وٿي);

    ret.حالت = EXCEPTION_NONE;
    ret.قدر = *قدر;
    أرجع ret;
}

فراغ Arch_user_ڍير_Trace(tcb_t *tptr)
{
    cap_t thread_پاڙ;
    vspace_پاڙ_t *vspace_پاڙ_2;
    لفظ_t sp;

    thread_پاڙ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the vspace root */
    إذا (cap_get_capType(thread_پاڙ) != cap_vspace_cap) {
        لکو("Invalid vspace\n");
        أرجع;
    }

    vspace_پاڙ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_پاڙ));
    sp = get_رجسٽر(tptr, SP_EL0);

    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    إذا (!IS_ALIGNED(sp, se_L4_لفظ_سائيز_بٽ)) {
        لکو("SP not aligned\n");
        أرجع;
    }

    لكل (بنانشان صحيح i = 0; i < CONFIG_USER_ڍير_TRACE_ڊيگهائي; i++) {
        لفظ_t پتو = sp + (i * حجم(لفظ_t));
        read_لفظ_From_VSpace_ret_t نتيجو = read_لفظ_From_VSpace(vspace_پاڙ_2,
                                                             پتو);
        إذا (نتيجو.حالت == EXCEPTION_NONE) {
            لکو("0x%"SEL4_PRIx_لفظ": 0x%"SEL4_PRIx_لفظ"\n",
                   پتو, نتيجو.قدر);
        } وإلا {
            لکو("0x%"SEL4_PRIx_لفظ": INVALID\n", پتو);
        }
    }
}
#جيڪڏهن_پورو /* CONFIG_PRINTING */

#إذا defined(CONFIG_KERNEL_LOG_بفر)
exception_t benchmark_arch_map_log_بفر(لفظ_t چوکٺو_cptr)
{
    lookup_Cap_And_جڳهه_ret_t lu_ret;
    vm_صفحو_سائيز_t چوکٺو_سائيز;
    pptr_t  چوکٺو_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_جڳهه(ڳنڍ_STATE(ksCurThread), چوکٺو_cptr);

    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", چوکٺو_cptr);
        current_fault = seL4_Fault_CapFault_new(چوکٺو_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(lu_ret.cap) != cap_چوکٺو_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(چوکٺو_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکٺو_سائيز = cap_چوکٺو_cap_get_cap_FSize(lu_ret.cap);

    إذا (چوکٺو_سائيز != ARMLarge_صفحو) {
        userError("Invalid frame size. The kernel expects 2M log buffer");
        current_fault = seL4_Fault_CapFault_new(چوکٺو_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکٺو_pptr = cap_چوکٺو_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_بفر = pptr_منزل_paddr((فراغ *) چوکٺو_pptr);

    *armKSGlobalLogPTE = pte_pte_صفحو_new(
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
                             0, // XN
#وإلا
                             1, // UXN
#جيڪڏهن_پورو
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
#جيڪڏهن_پورو /* CONFIG_KERNEL_LOG_BUFFER */
