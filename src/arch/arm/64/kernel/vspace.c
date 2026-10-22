/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <benchmark/benchmark.h>
#شامل <api/failures.h>
#شامل <api/syscall.h>
#شامل <kernel/boot.h>
#شامل <kernel/cspace.h>
#شامل <kernel/thread.h>
#شامل <kernel/stack.h>
#شامل <machine/io.h>
#شامل <machine/debug.h>
#شامل <model/statedata.h>
#شامل <object/cnode.h>
#شامل <object/untyped.h>
#شامل <arch/api/invocation.h>
#شامل <arch/kernel/vspace.h>
#شامل <linker.h>
#شامل <plat/machine/hardware.h>
#شامل <armv/context_switch.h>
#شامل <arch/object/iospace.h>
#شامل <arch/object/vcpu.h>
#شامل <arch/machine/tlb.h>

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
شمار mair_types {
    DEVICE_nGnRnE = 0,
    DEVICE_nGnRE = 1,
    DEVICE_GRE = 2,
    معمول_NC = 3,
    معمول = 4,
    معمول_WT = 5
};

/* Stage-2 translation memory attributes */
شمار mair_s2_types {
    S2_DEVICE_nGnRnE = 0,
    S2_DEVICE_nGnRE = 1,
    S2_DEVICE_nGRE  = 2,
    S2_DEVICE_GRE = 3,

    S2_معمول_INNER_NC_OUTER_NC = 5,
    S2_معمول_INNER_WTC_OUTER_NC = 6,
    S2_معمول_INNER_WBC_OUTER_NC = 7,

    S2_معمول_INNER_NC_OUTER_WTC = 9,
    S2_معمول_INNER_WTC_OUTER_WTC = 10,
    S2_معمول_INNER_WBC_OUTER_WTC = 11,

    S2_معمول_INNER_NC_OUTER_WBC = 13,
    S2_معمول_INNER_WTC_OUTER_WBC = 14,
    S2_معمول_INNER_WBC_OUTER_WBC = 15,

    S2_معمول = S2_معمول_INNER_WBC_OUTER_WBC
};

/* Leif from Linaro said the big.LITTLE clusters should be treated as
 * inner shareable, and we believe so, although the Example B2-1 given in
 * ARM ARM DDI 0487B.b (ID092517) says otherwise.
 */

#تعریف_کرو SMP_SHARE   3

ساخت lookupPTSlot_ret {
    pte_t *pt_خانہ;
    لفظ_t pt_بٹ_بایاں;
};
قسمتعریف ساخت lookupPTSlot_ret lookupPTSlot_ret_t;

ساخت ڈھونڈو_VSpace_For_ASID_ret {
    exception_t حالت_2;
    vspace_جڑ_t *vspace_جڑ;
};
قسمتعریف ساخت ڈھونڈو_VSpace_For_ASID_ret ڈھونڈو_VSpace_For_ASID_ret_t;

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

ساکن لفظ_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    انتخاب (vm_rights) {
    حالت VMKernelOnly:
        اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            واپس 0;
        } ورنہ {
            واپس 0;
        }

    حالت VMReadWrite:
        اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            واپس 3;
        } ورنہ {
            واپس 1;
        }

    حالت VMReadOnly:
        اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            واپس 1;
        } ورنہ {
            واپس 3;
        }

    طےشدہ:
        fail("Invalid VM rights");
    }
}

#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
ساکن اندرونی CONST لفظ_t pte_get_AP(pte_t pte)
{
    أكد(pte_is_صفحہ_type(pte));
    انتخاب (pte_get_pte_type(pte)) {
    حالت pte_pte_4k_صفحہ:
        واپس pte_pte_4k_صفحہ_get_AP(pte);

    حالت pte_pte_صفحہ:
        واپس pte_pte_صفحہ_get_AP(pte);

    طےشدہ:
        واپس 0;
    }
}

ساکن vm_rights_t CONST vmRightsFromPTE(pte_t pte)
{
    لفظ_t access_perms = pte_get_AP(pte);
    انتخاب (access_perms) {
    حالت 0:
    حالت 2:
        واپس VMKernelOnly;
    حالت 1:
        واپس VMReadWrite;
    حالت 3:
        واپس VMReadOnly;
    طےشدہ:
        fail("Invalid AP bit");
    }
}
#اگر_ختم

vm_rights_t CONST نقاب_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_نقاب)
{
    اگر (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_نقاب)) {
        واپس VMReadOnly;
    }
    اگر (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_نقاب)) {
        اگر (!seL4_CapRights_get_capAllowWrite(cap_rights_نقاب)) {
            واپس VMReadOnly;
        } ورنہ {
            واپس VMReadWrite;
        }
    }
    اگر (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_نقاب) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_نقاب)) {
        userError("Attempted to make unsupported write only mapping");
    }
    واپس VMKernelOnly;
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
BOOT_رمز خالی map_kernel_چوکھٹا(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_اوصاف_t اوصاف)
{
    أكد(vaddr >= PPTR_چوٹی);

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t uxn = vm_اوصاف_get_arm_Execute_Never(اوصاف);
#ورنہ
    لفظ_t uxn = 1; /* unprivileged execute never */
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    لفظ_t attr_اشاریہ;
    لفظ_t shareable;
    اگر (vm_اوصاف_get_arm_صفحہ_Cacheable(اوصاف)) {
        attr_اشاریہ = معمول;
        shareable = SMP_TERNARY(SMP_SHARE, 0);
    } ورنہ {
        attr_اشاریہ = DEVICE_nGnRnE;
        shareable = 0;
    }
    armKSGlobalKernelPT[GET_KPT_اشاریہ(vaddr, KLVL_FRM_ARM_PT_LVL(3))] = pte_pte_4k_صفحہ_new(uxn, paddr,
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
compile_assert(elf_اندر_kernel_mappings, GET_KPT_اشاریہ(KERNEL_ELF_بنیاد,
                                                     KLVL_FRM_ARM_PT_LVL(1)) == ثنائی_ہندسہ(PT_اشاریہ_بٹ) - 1);
compile_assert(kdev_اندر_kernel_mappings, GET_KPT_اشاریہ(KDEV_بنیاد,
                                                      KLVL_FRM_ARM_PT_LVL(1)) == ثنائی_ہندسہ(PT_اشاریہ_بٹ) - 1);
compile_assert(log_buf_اندر_kernel_mappings, GET_KPT_اشاریہ(KS_LOG_بنیاد,
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
BOOT_رمز خالی map_kernel_elf_image(pte_t kernel_mapping_pds[])
{
    /** This is not strictly necessary, but to handle a non-aligned KERNEL_ELF_PADDR_BASE
     *  we'd need to do a bit more logic and probably do what riscv64 does
     *  where the *actual* KERNEL_ELF_BASE is rounded up to the alignment.
     *  This is simpler and memory is cheap.
     **/
    compile_assert(elf_paddr_aligned, IS_ALIGNED(KERNEL_ELF_PADDR_بنیاد_RAW, se_L4_Large_صفحہ_بٹ));
    compile_assert(elf_بنیاد_aligned, IS_ALIGNED(KERNEL_ELF_بنیاد, se_L4_Large_صفحہ_بٹ));

    /* Please don't overlap with KDEV_BASE */
    أكد(KERNEL_ELF_چوٹی <= KDEV_بنیاد);
    /* Please don't overlap with KS_LOG_PPTR */
    أكد(KERNEL_ELF_چوٹی <= KS_LOG_PPTR);

    ثابت لفظ_t pd_آغاز = GET_KPT_اشاریہ(KERNEL_ELF_بنیاد, KLVL_FRM_ARM_PT_LVL(2));
    /* pd_end and ELF_TOP are both exclusive, so handle that correctly */
    ثابت لفظ_t pd_انتہا   = GET_KPT_اشاریہ(KERNEL_ELF_چوٹی - 1, KLVL_FRM_ARM_PT_LVL(2)) + 1;

    paddr_t paddr = KERNEL_ELF_PADDR_بنیاد;
    غیر_مستعمل pptr_t vaddr = KERNEL_ELF_بنیاد;

    برائے (لفظ_t pd = pd_آغاز; pd < pd_انتہا; pd++) {
        أكد(pd == GET_KPT_اشاریہ(vaddr, KLVL_FRM_ARM_PT_LVL(2)));

        kernel_mapping_pds[pd] = pte_pte_صفحہ_new(
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                                     /* XN */ 0,
#ورنہ
                                     /* UXN */ 1,
#اگر_ختم
                                     /* page_base_address */ paddr,
                                     /* global (nG) */ 0,
                                     /* access flag (AF) */ 1,
                                     /* shareability (SH) */ SMP_TERNARY(SMP_SHARE, 0),
                                     /* AP */ APFromVMRights(VMKernelOnly),
                                     /* AttrIndx */ معمول
                                 );

        paddr += ثنائی_ہندسہ(se_L4_Large_صفحہ_بٹ);
        vaddr += ثنائی_ہندسہ(se_L4_Large_صفحہ_بٹ);
    }
}

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
/* verify that the physical memory window as at the second entry of the PGD */
compile_assert(physical_کھڑکی_pgd_location,
               GET_KPT_اشاریہ(PPTR_بنیاد, KLVL_FRM_ARM_PT_LVL(0)) == 1);
#ورنہ
/* verify that the physical memory window as at the last entry of the PGD */
compile_assert(physical_کھڑکی_pgd_location,
               GET_KPT_اشاریہ(PPTR_بنیاد, KLVL_FRM_ARM_PT_LVL(0)) == ثنائی_ہندسہ(PT_اشاریہ_بٹ) - 1);
#اگر_ختم
/* make sure that the PPTR_BASE and PPTR_TOP are huge paged aligned */
compile_assert(physical_کھڑکی_بنیاد_aligned, IS_ALIGNED(PPTR_بنیاد, se_L4_Huge_صفحہ_بٹ));
compile_assert(physical_کھڑکی_چوٹی_aligned, IS_ALIGNED(PPTR_چوٹی, se_L4_Huge_صفحہ_بٹ));

/* the PPTR_BASE starts from the bottom of the kernel PUD */
compile_assert(physical_کھڑکی_bottom_kernel_region,
               GET_KPT_اشاریہ(PPTR_بنیاد, KLVL_FRM_ARM_PT_LVL(1)) == 0);
/* the PPTR_TOP then ends at the second-to-last entry of the PUD
    note: because this is the exclusive value the assert is one higher than
          you would expect.
*/
compile_assert(physical_کھڑکی_چوٹی_kernel_region,
               GET_KPT_اشاریہ(PPTR_چوٹی, KLVL_FRM_ARM_PT_LVL(1)) == ثنائی_ہندسہ(PT_اشاریہ_بٹ) - 1);

BOOT_رمز خالی map_kernel_physical_کھڑکی(خالی)
{
    paddr_t paddr;
    pptr_t vaddr;
    لفظ_t idx;

    /* place all PDs except the last one in PUD */
    برائے (idx = GET_KPT_اشاریہ(PPTR_بنیاد, KLVL_FRM_ARM_PT_LVL(1)); idx < GET_KPT_اشاریہ(PPTR_چوٹی, KLVL_FRM_ARM_PT_LVL(1));
         idx++) {
        armKSGlobalKernelPUD[idx] = pte_pte_جدول_new(
                                        addrFromKPPtr(&armKSGlobalKernelPDs[idx][0])
                                    );
    }

    /* map the kernel window using large pages */
    vaddr = PPTR_بنیاد;
    برائے (paddr = PADDR_بنیاد; paddr < PADDR_چوٹی; paddr += ثنائی_ہندسہ(se_L4_Large_صفحہ_بٹ)) {
        armKSGlobalKernelPDs[GET_KPT_اشاریہ(vaddr, KLVL_FRM_ARM_PT_LVL(1))][GET_KPT_اشاریہ(vaddr,
                                                                                         KLVL_FRM_ARM_PT_LVL(2))] = pte_pte_صفحہ_new(
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                                                                                                                        1, // XN
#ورنہ
                                                                                                                        1, // UXN
#اگر_ختم
                                                                                                                        paddr,
                                                                                                                        0,                        /* global */
                                                                                                                        1,                        /* access flag */
                                                                                                                        SMP_TERNARY(SMP_SHARE, 0),        /* Inner-shareable if SMP enabled, otherwise unshared */
                                                                                                                        0,                        /* VMKernelOnly */
                                                                                                                        معمول
                                                                                                                    );
        vaddr += ثنائی_ہندسہ(se_L4_Large_صفحہ_بٹ);
    }
}

BOOT_رمز خالی map_kernel_کھڑکی(خالی)
{
    /* place the PUD into the PGD */
    armKSGlobalKernelPGD[GET_KPT_اشاریہ(PPTR_بنیاد, KLVL_FRM_ARM_PT_LVL(0))] =
        pte_pte_جدول_new(addrFromKPPtr(armKSGlobalKernelPUD));

    map_kernel_physical_کھڑکی();

    ثابت لفظ_t elf_pud_idx = GET_KPT_اشاریہ(KERNEL_ELF_بنیاد, KLVL_FRM_ARM_PT_LVL(1));
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
ساکن BOOT_رمز خالی map_it_چوکھٹا_cap(cap_t vspace_cap, cap_t چوکھٹا_cap, bool_t executable)
{
    vspace_جڑ_t *vspace_جڑ_2 = VSPACE_PTR(pptr_کا_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt;

    vptr_t vptr = cap_چوکھٹا_cap_get_cap_FMapped_پتہ(چوکھٹا_cap);
    خالی *pptr = (خالی *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(چوکھٹا_cap);

    أكد(cap_چوکھٹا_cap_get_cap_FMapped_ASID(چوکھٹا_cap) != 0);

#اگر_تعریف_شدہ AARCH64_VSPACE_S2_آغاز_L1
    pud = vspace_جڑ_2;
#ورنہ
    vspace_جڑ_2 += GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(0));
    أكد(pte_pte_جدول_ptr_get_present(vspace_جڑ_2));
    pud = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_بنیاد_پتہ(vspace_جڑ_2));
#اگر_ختم
    pud += GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(1));
    أكد(pte_pte_جدول_ptr_get_present(pud));
    pd = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_بنیاد_پتہ(pud));
    pd += GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(2));
    أكد(pte_pte_جدول_ptr_get_present(pd));
    pt = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_بنیاد_پتہ(pd));
    *(pt + GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(3))) = pte_pte_4k_صفحہ_new(
                                                              !executable,                    /* unprivileged execute never */
                                                              pptr_مقصد_paddr(pptr),            /* page_base_address    */
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              0,
#ورنہ
                                                              1,                              /* not global */
#اگر_ختم
                                                              1,                              /* access flag */
                                                              SMP_TERNARY(SMP_SHARE, 0),              /* Inner-shareable if SMP enabled, otherwise unshared */
                                                              APFromVMRights(VMReadWrite),
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              S2_معمول
#ورنہ
                                                              معمول
#اگر_ختم
                                                          );
}

ساکن BOOT_رمز cap_t create_it_چوکھٹا_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    vm_صفحہ_سائز_t چوکھٹا_سائز_2;
    اگر (use_large) {
        چوکھٹا_سائز_2 = ARMLarge_صفحہ;
    } ورنہ {
        چوکھٹا_سائز_2 = ARMSmall_صفحہ;
    }
    واپس
        cap_چوکھٹا_cap_new(
            asid,                          /* capFMappedASID */
            pptr,                          /* capFBasePtr */
            چوکھٹا_سائز_2,                    /* capFSize */
            vptr,                          /* capFMappedAddress */
            لفظ_From_VMRights(VMReadWrite), /* capFVMRights */
            false                          /* capFIsDevice */
        );
}

ساکن BOOT_رمز خالی map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    vspace_جڑ_t *vspace_جڑ_2 = VSPACE_PTR(pptr_کا_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt = PT_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_صفحہ_جدول_cap_get_cap_PTMapped_پتہ(pt_cap);

    أكد(cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(pt_cap));

#اگر_تعریف_شدہ AARCH64_VSPACE_S2_آغاز_L1
    pud = vspace_جڑ_2;
#ورنہ
    vspace_جڑ_2 += GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(0));
    أكد(pte_pte_جدول_ptr_get_present(vspace_جڑ_2));
    pud = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_بنیاد_پتہ(vspace_جڑ_2));
#اگر_ختم
    pud += GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(1));
    أكد(pte_pte_جدول_ptr_get_present(pud));
    pd = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_بنیاد_پتہ(pud));
    *(pd + GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(2))) = pte_pte_جدول_new(
                                                              pptr_مقصد_paddr(pt)
                                                          );
}

ساکن BOOT_رمز cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحہ_جدول_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pt_cap(vspace_cap, cap);
    واپس cap;
}

ساکن BOOT_رمز خالی map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    vspace_جڑ_t *vspace_جڑ_2 = VSPACE_PTR(pptr_کا_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd = PT_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(pd_cap));
    vptr_t vptr = cap_صفحہ_جدول_cap_get_cap_PTMapped_پتہ(pd_cap);

    أكد(cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(pd_cap));

#اگر_تعریف_شدہ AARCH64_VSPACE_S2_آغاز_L1
    pud = vspace_جڑ_2;
#ورنہ
    vspace_جڑ_2 += GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(0));
    أكد(pte_pte_جدول_ptr_get_present(vspace_جڑ_2));
    pud = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_بنیاد_پتہ(vspace_جڑ_2));
#اگر_ختم
    *(pud + GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(1))) = pte_pte_جدول_new(
                                                               pptr_مقصد_paddr(pd)
                                                           );
}

ساکن BOOT_رمز cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحہ_جدول_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pd_cap(vspace_cap, cap);
    واپس cap;
}

#اگر_غیر_تعریف_شدہ AARCH64_VSPACE_S2_آغاز_L1
ساکن BOOT_رمز خالی map_it_pud_cap(cap_t vspace_cap, cap_t pud_cap)
{
    pte_t *pgd = PT_PTR(pptr_کا_cap(vspace_cap));
    pte_t *pud = PT_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(pud_cap));
    vptr_t vptr = cap_صفحہ_جدول_cap_get_cap_PTMapped_پتہ(pud_cap);

    أكد(cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(pud_cap));

    *(pgd + GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(0))) = pte_pte_جدول_new(
                                                               pptr_مقصد_paddr(pud));
}

ساکن BOOT_رمز cap_t create_it_pud_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحہ_جدول_cap_new(
              asid,               /* capPTMappedASID */
              pptr,               /* capPTBasePtr */
              1,                  /* capPTIsMapped */
              vptr                /* capPTMappedAddress */
          );
    map_it_pud_cap(vspace_cap, cap);
    واپس cap;
}
#اگر_ختم /* AARCH64_VSPACE_S2_START_L1 */
BOOT_رمز لفظ_t arch_get_n_paging(v_region_t it_v_reg)
{
    واپس
#اگر_غیر_تعریف_شدہ AARCH64_VSPACE_S2_آغاز_L1
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_بٹ(ULVL_FRM_ARM_PT_LVL(0))) +
#اگر_ختم
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_بٹ(ULVL_FRM_ARM_PT_LVL(1))) +
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_بٹ(ULVL_FRM_ARM_PT_LVL(2)));
}

BOOT_رمز cap_t create_it_پتہ_space(cap_t جڑ_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_خانہ_Pos خانہ_pos_before;
    se_L4_خانہ_Pos خانہ_pos_after;

    /* create the PGD */
    vspace_cap = cap_vspace_cap_new(
                     IT_ASID,           /* capVSMappedASID */
                     rootserver.vspace, /* capVSBasePtr    */
                     1                  /* capVSIsMapped   */
#اگر_تعریف_شدہ CONFIG_ARM_SMMU
                     , CB_INVALID       /* capVSMappedCB   */
#اگر_ختم
                 );
    خانہ_pos_before = ndks_boot.خانہ_pos_cur;
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

#اگر_غیر_تعریف_شدہ AARCH64_VSPACE_S2_آغاز_L1
    /* Create any PUDs needed for the user land image */
    برائے (vptr = باری_نیچے_کو(it_v_reg.آغاز, GET_ULVL_PGSIZE_بٹ(ULVL_FRM_ARM_PT_LVL(0)));
         vptr < it_v_reg.انتہا;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(0))) {
        اگر (!provide_cap(جڑ_cnode_cap, create_it_pud_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            واپس cap_null_cap_new();
        }
    }
#اگر_ختم
    /* Create any PDs needed for the user land image */
    برائے (vptr = باری_نیچے_کو(it_v_reg.آغاز, GET_ULVL_PGSIZE_بٹ(ULVL_FRM_ARM_PT_LVL(1)));
         vptr < it_v_reg.انتہا;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))) {
        اگر (!provide_cap(جڑ_cnode_cap, create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            واپس cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    برائے (vptr = باری_نیچے_کو(it_v_reg.آغاز, GET_ULVL_PGSIZE_بٹ(ULVL_FRM_ARM_PT_LVL(2)));
         vptr < it_v_reg.انتہا;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))) {
        اگر (!provide_cap(جڑ_cnode_cap, create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            واپس cap_null_cap_new();
        }
    }

    خانہ_pos_after = ndks_boot.خانہ_pos_cur;
    ndks_boot.bi_چوکھٹا->userImagePaging = (se_L4_خانہ_Region) {
        خانہ_pos_before, خانہ_pos_after
    };
    واپس vspace_cap;
}

BOOT_رمز cap_t create_unmapped_it_چوکھٹا_cap(pptr_t pptr, bool_t use_large)
{
    واپس create_it_چوکھٹا_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_رمز cap_t create_mapped_it_چوکھٹا_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_چوکھٹا_cap(pptr, vptr, asid, use_large);
    map_it_چوکھٹا_cap(pd_cap, cap, executable);
    واپس cap;
}

BOOT_رمز خالی activate_kernel_vspace(خالی)
{
    cleanInvalidateL1Caches();
    سیٹ_Current_Kernel_VSpace_جڑ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalKernelPGD)));

    /* Prevent elf-loader address translation to fill up TLB */
    سیٹ_Current_User_VSpace_جڑ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));

    invalidateLocalTLB();
    تالا_TLBEntry(KERNEL_ELF_بنیاد);
}

BOOT_رمز خالی write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_کا_cap(it_ap_cap));
    asid_map_t asid_map = asid_map_asid_map_vspace_new(
#اگر_تعریف_شدہ CONFIG_ARM_SMMU
                              /* bind_cb: Number of bound context banks */
                              0,
#اگر_ختم
                              /* vspace_root: reference to vspace root page table object */
                              (لفظ_t)cap_vspace_cap_get_capVSBasePtr(it_vspace_cap)
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                              /* stored_hw_vmid, stored_vmid_valid: Assigned hardware VMID for TLB. */
                              , 0, false
#اگر_ختم
                          );
    ap->array[ASID_کم(IT_ASID)] = asid_map;
    armKSASIDTable[ASID_بلند(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

asid_map_t ڈھونڈو_Map_For_ASID(asid_t asid)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_بلند(asid)];
    اگر (!poolPtr) {
        واپس asid_map_asid_map_none_new();
    }

    واپس poolPtr->array[ASID_کم(asid)];
}

ساکن ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_VSpace_For_ASID(asid_t asid)
{
    ڈھونڈو_VSpace_For_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = ڈھونڈو_Map_For_ASID(asid);
    اگر (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_جڑ_new();

        ret.vspace_جڑ = تہی;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    }

    ret.vspace_جڑ = (vspace_جڑ_t *)asid_map_asid_map_vspace_get_vspace_جڑ(asid_map);
    ret.حالت_2 = EXCEPTION_NONE;
    واپس ret;
}

لفظ_t *خالص lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    لفظ_t w_بفر_Ptr;
    cap_t بفر_Cap;
    vm_rights_t vm_rights;

    w_بفر_Ptr = thread->tcbIPCBuffer;
    بفر_Cap = TCB_PTR_CTE_PTR(thread, tcb_بفر)->cap;

    اگر (unlikely(cap_get_capType(بفر_Cap) != cap_چوکھٹا_cap)) {
        واپس تہی;
    }
    اگر (unlikely(cap_چوکھٹا_cap_get_cap_FIs_Device(بفر_Cap))) {
        واپس تہی;
    }

    vm_rights = cap_چوکھٹا_cap_get_cap_FVMRights(بفر_Cap);
    اگر (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        لفظ_t بنیاد_Ptr;
        غیرعلامتی صحیح صفحہ_بٹ_2;

        بنیاد_Ptr = cap_چوکھٹا_cap_get_cap_FBase_Ptr(بفر_Cap);
        صفحہ_بٹ_2 = صفحہ_بٹ_For_سائز(cap_چوکھٹا_cap_get_cap_FSize(بفر_Cap));
        واپس (لفظ_t *)(بنیاد_Ptr + (w_بفر_Ptr & نقاب(صفحہ_بٹ_2)));
    } ورنہ {
        واپس تہی;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    اگر (cap_get_capType(cap) != cap_چوکھٹا_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (unlikely(cap_چوکھٹا_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (!IS_ALIGNED(vptr, se_L4_IPCBuffer_سائز_بٹ)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (صحیح)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    واپس EXCEPTION_NONE;
}

ساکن lookupPTSlot_ret_t lookupPTSlot(vspace_جڑ_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    لفظ_t level = UPT_سطحیں - 1;
    pte_t *pt = vspace;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_بٹ_بایاں = PT_اشاریہ_بٹ * level + se_L4_صفحہ_بٹ;
    ret.pt_خانہ = pt + ((vptr >> ret.pt_بٹ_بایاں) & نقاب(se_L4_VSpace_اشاریہ_بٹ));

    جب (pte_pte_جدول_ptr_get_present(ret.pt_خانہ) && likely(level > 0)) {
        level--;
        ret.pt_بٹ_بایاں -= PT_اشاریہ_بٹ;
        pt = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_بنیاد_پتہ(ret.pt_خانہ));
        ret.pt_خانہ = pt + ((vptr >> ret.pt_بٹ_بایاں) & نقاب(PT_اشاریہ_بٹ));
    }

    واپس ret;
}

/* Note that if the hypervisor support is enabled, the user page tables use
 * stage-2 translation format. Otherwise, they follow the stage-1 translation format.
 */
ساکن pte_t make_User_صفحہ_PTE(paddr_t paddr, vm_rights_t vm_rights, bool_t cap_read, vm_اوصاف_t اوصاف,
                             vm_صفحہ_سائز_t صفحہ_سائز)
{
    bool_t nonexecutable = vm_اوصاف_get_arm_Execute_Never(اوصاف) || !cap_read;
    لفظ_t cacheable = vm_اوصاف_get_arm_صفحہ_Cacheable(اوصاف);

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t nG = 0; /* not global */
    لفظ_t attridx = cacheable ? S2_معمول : S2_DEVICE_nGnRnE;
#ورنہ
    لفظ_t nG = 1; /* not global */
    لفظ_t attridx = cacheable ? معمول : DEVICE_nGnRnE;
#اگر_ختم

    /* Inner-shareable if SMP enabled, otherwise unshared (ignored for devices) */
    لفظ_t shareable = cacheable ? SMP_TERNARY(SMP_SHARE, 0) : 0;

    اگر (صفحہ_سائز == ARMSmall_صفحہ) {
        واپس pte_pte_4k_صفحہ_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                   shareable, APFromVMRights(vm_rights), attridx);
    } ورنہ {
        واپس pte_pte_صفحہ_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                shareable, APFromVMRights(vm_rights), attridx);
    }
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    انتخاب (vm_faultType) {
    حالت ARMDataAbort: {
        لفظ_t addr, fault;

        addr = getFAR();
        fault = getDFSR();

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* use the IPA */
        اگر (ARCH_گرہ_STATE(armHSVCPUActive)) {
            addr = GET_PAR_ADDR(پتہ_Translate_S1(addr)) | (addr & نقاب(صفحہ_بٹ));
        }
#اگر_ختم
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        واپس EXCEPTION_FAULT;
    }

    حالت ARMPrefetchAbort: {
        لفظ_t pc, fault;

        pc = getRestartPC(thread);
        fault = getIFSR();

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        اگر (ARCH_گرہ_STATE(armHSVCPUActive)) {
            pc = GET_PAR_ADDR(پتہ_Translate_S1(pc)) | (pc & نقاب(صفحہ_بٹ));
        }
#اگر_ختم
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        واپس EXCEPTION_FAULT;
    }

    طےشدہ:
        fail("Invalid VM fault type");
    }
}

bool_t CONST is_VTable_جڑ(cap_t cap)
{
    واپس cap_get_capType(cap) == cap_vspace_cap;
}

bool_t CONST is_Valid_Native_جڑ(cap_t cap)
{
    واپس is_VTable_جڑ(cap) &&
           cap_vspace_cap_get_capVSIsMapped(cap);
}

bool_t CONST is_Valid_VTable_جڑ(cap_t cap)
{
    واپس is_Valid_Native_جڑ(cap);
}

خالی سیٹ_VMRoot(tcb_t *tcb)
{
    cap_t thread_جڑ;
    asid_t asid;
    vspace_جڑ_t *vspace_جڑ_2;
    ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret;

    thread_جڑ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    /* ASID/VMID 0 is never allocated to a real VSpace (see findFreeHWASID()
     * for the hypervisor case), so the empty global VSpace can be installed
     * under it without flushing the TLB. */
    اگر (!is_Valid_Native_جڑ(thread_جڑ)) {
        سیٹ_Current_User_VSpace_جڑ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        واپس;
    }

    vspace_جڑ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_جڑ));
    asid = cap_vspace_cap_get_capVSMappedASID(thread_جڑ);
    ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
    اگر (unlikely(ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE || ڈھونڈو_ret.vspace_جڑ != vspace_جڑ_2)) {
        سیٹ_Current_User_VSpace_جڑ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        واپس;
    }

    armv_contextSwitch(vspace_جڑ_2, asid);
}

ساکن bool_t سیٹ_VMRoot_For_Flush(vspace_جڑ_t *vspace, asid_t asid)
{
    cap_t thread_جڑ;

    thread_جڑ = TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcbVTable)->cap;

    اگر (cap_get_capType(thread_جڑ) == cap_vspace_cap &&
        cap_vspace_cap_get_capVSIsMapped(thread_جڑ) &&
        VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_جڑ)) == vspace) {
        واپس false;
    }

    armv_contextSwitch(vspace, asid);
    واپس true;
}


#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT

ساکن اندرونی asid_pool_t *getPoolPtr(asid_t asid)
{
    واپس armKSASIDTable[ASID_بلند(asid)];
}

ساکن اندرونی asid_map_t getASIDMap(asid_pool_t *poolPtr, asid_t asid)
{
    أكد(poolPtr != تہی);
    واپس poolPtr->array[ASID_کم(asid)];
}

ساکن اندرونی خالی سیٹ_ASIDMap(asid_pool_t *poolPtr, asid_t asid, asid_map_t asid_map)
{
    أكد(poolPtr != تہی);
    poolPtr->array[ASID_کم(asid)] = asid_map;
}

ساکن خالی invalidateASID(asid_t asid)
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

ساکن خالی storeHWASID(asid_t asid, hw_asid_t hw_asid)
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

ساکن hw_asid_t ڈھونڈو_Free_HWASID(خالی)
{
    لفظ_t hw_asid_انحراف;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    برائے (hw_asid_انحراف = 0;
         hw_asid_انحراف <= (لفظ_t)((hw_asid_t) - 1);
         hw_asid_انحراف++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_انحراف);
        اگر (hw_asid != hwASIDReserved && armKSHWASIDTable[hw_asid] == asidInvalid) {
            واپس hw_asid;
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
    اگر (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    واپس hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = ڈھونڈو_Map_For_ASID(asid);
    اگر (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        واپس asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    } ورنہ {
        hw_asid_t new_hw_asid;

        new_hw_asid = ڈھونڈو_Free_HWASID();
        storeHWASID(asid, new_hw_asid);
        واپس new_hw_asid;
    }
}

ساکن خالی invalidateASIDEntry(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = ڈھونڈو_Map_For_ASID(asid);
    اگر (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        armKSHWASIDTable[asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

#اگر_ختم

#اگر_تعریف_شدہ CONFIG_ARM_SMMU
ساکن لفظ_t getASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_بلند(asid)];
    أكد(asidPool);

    asid_map_t asid_map = asidPool->array[ASID_کم(asid)];
    أكد(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    واپس asid_map_asid_map_vspace_get_bind_cb(asid_map);
}

خالی increaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_بلند(asid)];
    أكد(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_کم(asid)];
    أكد(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_سیٹ_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) + 1);
}

خالی decreaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_بلند(asid)];
    أكد(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_کم(asid)];
    أكد(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_سیٹ_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) - 1);
}
#اگر_ختم

ساکن اندرونی خالی invalidateTLBByASID(asid_t asid)
{
#اگر_تعریف_شدہ CONFIG_ARM_SMMU
    لفظ_t bind_cb = getASIDBindCB(asid);
    اگر (unlikely(bind_cb)) {
        invalidateSMMUTLBByASID(asid, bind_cb);
    }
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = ڈھونڈو_Map_For_ASID(asid);
    اگر (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        واپس;
    }
    invalidateTranslationASID(asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map));
#ورنہ
    invalidateTranslationASID(asid);
#اگر_ختم
}

ساکن اندرونی خالی invalidateTLBByASIDVA(asid_t asid, vptr_t vaddr)
{
#اگر_تعریف_شدہ CONFIG_ARM_SMMU
    لفظ_t bind_cb = getASIDBindCB(asid);
    اگر (unlikely(bind_cb)) {
        invalidateSMMUTLBByASIDVA(asid, vaddr, bind_cb);
    }
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = ڈھونڈو_Map_For_ASID(asid);
    اگر (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        واپس;
    }
    uint64_t hw_asid = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    invalidateTranslationSingle((hw_asid << 48) | vaddr >> se_L4_صفحہ_بٹ);
#ورنہ
    invalidateTranslationSingle((asid << 48) | vaddr >> se_L4_صفحہ_بٹ);
#اگر_ختم
}


خالی unmap_صفحہ_جدول(asid_t asid, vptr_t vptr, pte_t *ہدف_pt)
{
    ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
    اگر (unlikely(ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE)) {
        /* nothing to do */
        واپس;
    }
    pte_t *pt_خانہ = تہی;
    pte_t *pt = (pte_t *)ڈھونڈو_ret.vspace_جڑ;

    برائے (لفظ_t i = 0; i < UPT_سطحیں - 1 && pt != ہدف_pt; i++) {
        pt_خانہ = pt + GET_UPT_اشاریہ(vptr, i);
        اگر (unlikely(!pte_pte_جدول_ptr_get_present(pt_خانہ))) {
            /* couldn't find it */
            واپس;
        }
        pt = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_بنیاد_پتہ(pt_خانہ));
    }

    اگر (pt != ہدف_pt) {
        /* didn't find it */
        واپس;
    }
    /* If we found a pt then ptSlot won't be null */
    أكد(pt_خانہ != تہی);
    *pt_خانہ = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)pt_خانہ, pptr_مقصد_paddr(pt_خانہ));
    invalidateTLBByASID(asid);
}

خالی unmap_صفحہ(vm_صفحہ_سائز_t صفحہ_سائز, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret;
    lookupPTSlot_ret_t  lu_ret;
    pte_t pte;

    ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
    اگر (ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE) {
        واپس;
    }

    lu_ret = lookupPTSlot(ڈھونڈو_ret.vspace_جڑ, vptr);
    اگر (unlikely(lu_ret.pt_بٹ_بایاں != صفحہ_بٹ_For_سائز(صفحہ_سائز))) {
        /* Do nothing if the wrong size object was returned */
        واپس;
    }

    pte = *(lu_ret.pt_خانہ);
    اگر (!pte_is_صفحہ_type(pte)) {
        /* Do nothing if no page is present */
        واپس;
    }

    اگر (pte_get_صفحہ_بنیاد_پتہ(pte) != pptr_مقصد_paddr((خالی *)pptr)) {
        /* Do nothing if the mapped page is not the same physical frame */
        واپس;
    }

    *(lu_ret.pt_خانہ) = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)lu_ret.pt_خانہ, pptr_مقصد_paddr(lu_ret.pt_خانہ));
    أكد(asid < ثنائی_ہندسہ(16));
    invalidateTLBByASIDVA(asid, vptr);
}

خالی deleteASID(asid_t asid, vspace_جڑ_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_بلند(asid)];

    اگر (poolPtr != تہی) {
        asid_map_t asid_map = poolPtr->array[ASID_کم(asid)];
        اگر (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_جڑ_t *)asid_map_asid_map_vspace_get_vspace_جڑ(asid_map) == vspace) {
            invalidateTLBByASID(asid);
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
            invalidateASIDEntry(asid);
#اگر_ختم
            poolPtr->array[ASID_کم(asid)] = asid_map_asid_map_none_new();
            سیٹ_VMRoot(گرہ_STATE(ksCurThread));
        }
    }
}

خالی deleteASIDPool(asid_t asid_بنیاد, asid_pool_t *pool)
{
    لفظ_t انحراف;

    أكد((asid_بنیاد & نقاب(asid_کم_بٹ)) == 0);

    اگر (armKSASIDTable[ASID_بلند(asid_بنیاد)] == pool) {
        برائے (انحراف = 0; انحراف < ثنائی_ہندسہ(asid_کم_بٹ); انحراف++) {
            asid_map_t asid_map = pool->array[انحراف];
            اگر (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                invalidateTLBByASID(asid_بنیاد + انحراف);
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                invalidateASIDEntry(asid_بنیاد + انحراف);
#اگر_ختم
            }
        }
        armKSASIDTable[ASID_بلند(asid_بنیاد)] = تہی;
        سیٹ_VMRoot(گرہ_STATE(ksCurThread));
    }
}

ساکن خالی doFlush(لفظ_t invLabel, vptr_t آغاز, vptr_t انتہا, paddr_t pstart)
{
    انتخاب (invLabel) {
    حالت ARMVSpace_Clean_ڈیٹا:
    حالت ARMPage_Clean_ڈیٹا:
        cleanCacheRange_RAM(آغاز, انتہا, pstart);
        توڑو;

    حالت ARMVSpace_Invalidate_ڈیٹا:
    حالت ARMPage_Invalidate_ڈیٹا:
        invalidateCacheRange_RAM(آغاز, انتہا, pstart);
        توڑو;

    حالت ARMVSpace_Clean_Invalidate_ڈیٹا:
    حالت ARMPage_Clean_Invalidate_ڈیٹا:
        cleanInvalidateCacheRange_RAM(آغاز, انتہا, pstart);
        توڑو;

    حالت ARMVSpaceUnify_Instruction:
    حالت ARMPageUnify_Instruction:
        /* First clean data lines to point of unification... */
        cleanCacheRange_PoU(آغاز, انتہا, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(آغاز, انتہا, pstart);
        /* ... then wait for the completion of invalidations... */
        dsb();
        /* ... and ensure new instructions come from fresh cache lines. */
        isb();
        توڑو;
    طےشدہ:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

ساکن exception_t performVSpaceFlush(لفظ_t invLabel, vspace_جڑ_t *vspace_جڑ_2, asid_t asid,
                                      vptr_t آغاز, vptr_t انتہا, paddr_t pstart)
{

    اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        لفظ_t سائز = انتہا - آغاز;
        آغاز = (vptr_t)paddr_مقصد_pptr(pstart);
        انتہا = آغاز + سائز;
        اگر (آغاز < انتہا) {
            doFlush(invLabel, آغاز, انتہا, pstart);
        }
    } ورنہ {
        bool_t جڑ_switched;

        /* Flush if given a non zero range */
        اگر (آغاز < انتہا) {
            جڑ_switched = سیٹ_VMRoot_For_Flush(vspace_جڑ_2, asid);
            doFlush(invLabel, آغاز, انتہا, pstart);
            اگر (جڑ_switched) {
                سیٹ_VMRoot(گرہ_STATE(ksCurThread));
            }
        }
    }
    واپس EXCEPTION_NONE;
}


ساکن exception_t perform_صفحہ_جدول_Invocation_Map(cap_t cap, cte_t *ct_خانہ, pte_t pte, pte_t *pt_خانہ)
{
    ct_خانہ->cap = cap;
    *pt_خانہ = pte;
    cleanByVA_PoU((vptr_t)pt_خانہ, pptr_مقصد_paddr(pt_خانہ));

    واپس EXCEPTION_NONE;
}

ساکن exception_t perform_صفحہ_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_خانہ)
{
    اگر (cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PT_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_صفحہ_جدول(cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(cap),
                       cap_صفحہ_جدول_cap_get_cap_PTMapped_پتہ(cap), pt);
        clear_حافظہ_PT((خالی *)pt, cap_get_cap_سائز_بٹ(cap));
    }

    cap_صفحہ_جدول_cap_ptr_سیٹ_cap_PTIs_Mapped(&(ct_خانہ->cap), 0);
    واپس EXCEPTION_NONE;
}

ساکن exception_t perform_صفحہ_Invocation_Map(asid_t asid, cap_t cap, cte_t *ct_خانہ,
                                            pte_t pte, pte_t *pt_خانہ)
{
    bool_t tlbflush_required = pte_ptr_get_valid(pt_خانہ);

    ct_خانہ->cap = cap;
    *pt_خانہ = pte;

    cleanByVA_PoU((vptr_t)pt_خانہ, pptr_مقصد_paddr(pt_خانہ));
    اگر (unlikely(tlbflush_required)) {
        أكد(asid < ثنائی_ہندسہ(16));
        invalidateTLBByASIDVA(asid, cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap));
    }

    واپس EXCEPTION_NONE;
}

ساکن exception_t perform_صفحہ_Invocation_Unmap(cap_t cap, cte_t *ct_خانہ)
{
    اگر (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap) != 0) {

        unmap_صفحہ(cap_چوکھٹا_cap_get_cap_FSize(cap),
                  cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                  cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                  cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap));
    }

    cap_t خانہ_Cap = ct_خانہ->cap;
    خانہ_Cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(خانہ_Cap, 0);
    خانہ_Cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(خانہ_Cap, asidInvalid);
    ct_خانہ->cap = خانہ_Cap;


    واپس EXCEPTION_NONE;
}

ساکن exception_t perform_صفحہ_Flush(لفظ_t invLabel, vspace_جڑ_t *vspace_جڑ_2, asid_t asid,
                                    vptr_t آغاز, vptr_t انتہا, paddr_t pstart)
{
    اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* We flush the cache with kernel virtual addresses since
         * the user virtual addresses are not valid in EL2.
         * Switching VMRoot is not required.
         */
        لفظ_t سائز = انتہا - آغاز;
        آغاز = (vptr_t)paddr_مقصد_pptr(pstart);
        انتہا = آغاز + سائز;

        اگر (آغاز < انتہا) {
            doFlush(invLabel, آغاز, انتہا, pstart);
        }
    } ورنہ {
        bool_t جڑ_switched;

        اگر (آغاز < انتہا) {
            جڑ_switched = سیٹ_VMRoot_For_Flush(vspace_جڑ_2, asid);
            doFlush(invLabel, آغاز, انتہا, pstart);
            اگر (جڑ_switched) {
                سیٹ_VMRoot(گرہ_STATE(ksCurThread));
            }
        }
    }
    واپس EXCEPTION_NONE;
}

ساکن exception_t perform_صفحہ_Get_پتہ(pptr_t بنیاد_ptr, bool_t call)
{
    paddr_t بنیاد = pptr_مقصد_paddr((خالی *)بنیاد_ptr);

    tcb_t *thread;
    thread = گرہ_STATE(ksCurThread);
    اگر (call) {
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, thread);
        سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
        غیرعلامتی صحیح لمبائی = سیٹ_MR(thread, ipc_بفر, 0, بنیاد);
        سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, لمبائی)));
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
    واپس EXCEPTION_NONE;
}

ساکن exception_t performASIDControlInvocation(خالی *چوکھٹا, cte_t *خانہ,
                                                cte_t *والد, asid_t asid_بنیاد)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_سیٹ_cap_Free_اشاریہ(&(والد->cap),
                                         MAX_FREE_اشاریہ(cap_untyped_cap_get_cap_بلاک_سائز(والد->cap)));

    memzero(چوکھٹا, ثنائی_ہندسہ(se_L4_ASIDPool_بٹ));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_داخل_کرو(
        cap_asid_pool_cap_new(
            asid_بنیاد,         /* capASIDBase  */
            لفظ_REF(چوکھٹا)    /* capASIDPool  */
        ), والد, خانہ);

    أكد((asid_بنیاد & نقاب(asid_کم_بٹ)) == 0);
    armKSASIDTable[ASID_بلند(asid_بنیاد)] = (asid_pool_t *)چوکھٹا;

    واپس EXCEPTION_NONE;
}

ساکن exception_t decode_ARMVSpace_جڑ_Invocation(لفظ_t invLabel, لفظ_t لمبائی,
                                                 cte_t *cte, cap_t cap, لفظ_t *بفر)
{
    vptr_t آغاز, انتہا;
    paddr_t pstart;
    asid_t asid;
    vspace_جڑ_t *vspace_جڑ_2;
    lookupPTSlot_ret_t resolve_ret;
    ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret;
    pte_t pte;

    انتخاب (invLabel) {
    حالت ARMVSpace_Clean_ڈیٹا:
    حالت ARMVSpace_Invalidate_ڈیٹا:
    حالت ARMVSpace_Clean_Invalidate_ڈیٹا:
    حالت ARMVSpaceUnify_Instruction:

        اگر (لمبائی < 2) {
            userError("VSpaceRoot Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        آغاز = getSyscallArg(0, بفر);
        انتہا =   getSyscallArg(1, بفر);

        /* Check sanity of arguments */
        اگر (انتہا <= آغاز) {
            userError("VSpaceRoot Flush: Invalid range.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 1;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        اگر (انتہا > USER_چوٹی) {
            userError("VSpaceRoot Flush: Exceed the user addressable region.");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (unlikely(!is_Valid_Native_جڑ(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        /* Make sure that the supplied pgd is ok */
        vspace_جڑ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap));
        asid = cap_vspace_cap_get_capVSMappedASID(cap);

        ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
        اگر (unlikely(ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE)) {
            userError("VSpaceRoot Flush: No VSpace for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = false;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (unlikely(ڈھونڈو_ret.vspace_جڑ != vspace_جڑ_2)) {
            userError("VSpaceRoot Flush: Invalid VSpace Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = lookupPTSlot(vspace_جڑ_2, آغاز);
        pte = *resolve_ret.pt_خانہ;

        /* Check that the returned slot is a page. */
        اگر (!pte_is_صفحہ_type(pte)) {

            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
            واپس EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        اگر (باری_نیچے_کو(آغاز, resolve_ret.pt_بٹ_بایاں) != باری_نیچے_کو(انتہا - 1, resolve_ret.pt_بٹ_بایاں)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = آغاز;
            current_syscall_error.rangeErrorMax = باری_نیچے_کو(آغاز, resolve_ret.pt_بٹ_بایاں) +
                                                  نقاب(resolve_ret.pt_بٹ_بایاں);
            userError("VSpaceRoot Flush: cannot cross page boundary, valid range is [0x%lx..0x%lx)",
                      current_syscall_error.rangeErrorMin, current_syscall_error.rangeErrorMax);
            واپس EXCEPTION_SYSCALL_ERROR;
        }

#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When in EL1, the mapping must be write-able for ARMVSpaceInvalidate_Data */
        اگر (invLabel == ARMVSpace_Invalidate_ڈیٹا && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMVSpaceInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
#اگر_ختم

        /* Calculate the physical start address. */
        paddr_t چوکھٹا_بنیاد_2 = pte_get_صفحہ_بنیاد_پتہ(pte);

        pstart = چوکھٹا_بنیاد_2 + (آغاز & نقاب(resolve_ret.pt_بٹ_بایاں));

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        اگر (pstart < PADDR_بنیاد || ((انتہا - آغاز) + pstart) > PADDR_چوٹی) {
            userError("VSpaceRoot Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
#اگر_ختم

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس performVSpaceFlush(invLabel, vspace_جڑ_2, asid, آغاز, انتہا - 1, pstart);

    طےشدہ:
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
}


ساکن exception_t decode_ARMPage_جدول_Invocation(لفظ_t invLabel, لفظ_t لمبائی,
                                                cte_t *cte, cap_t cap, لفظ_t *بفر)
{
    cap_t vspace_جڑ_Cap;
    vspace_جڑ_t *vspace_جڑ_2;
    pte_t pte;
    asid_t asid;
    vptr_t vaddr;
    lookupPTSlot_ret_t pt_خانہ;
    ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret;

    اگر (invLabel == ARMPage_جدول_Unmap) {
        اگر (unlikely(!isFinalCapability(cte))) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس perform_صفحہ_جدول_Invocation_Unmap(cap, cte);
    }

    اگر (unlikely(invLabel != ARMPage_جدول_Map)) {
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (unlikely(لمبائی < 2 || current_extra_caps.excaprefs[0] == تہی)) {
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (unlikely(cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر);
    vspace_جڑ_Cap = current_extra_caps.excaprefs[0]->cap;

    اگر (unlikely(!is_Valid_Native_جڑ(vspace_جڑ_Cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    vspace_جڑ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_جڑ_Cap));
    asid = cap_vspace_cap_get_capVSMappedASID(vspace_جڑ_Cap);

    اگر (unlikely(vaddr > USER_چوٹی)) {
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
    اگر (unlikely(ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = false;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (unlikely(ڈھونڈو_ret.vspace_جڑ != vspace_جڑ_2)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    pt_خانہ = lookupPTSlot(vspace_جڑ_2, vaddr);

    اگر (unlikely(pt_خانہ.pt_بٹ_بایاں == se_L4_صفحہ_بٹ || pte_ptr_get_valid(pt_خانہ.pt_خانہ))) {
        current_syscall_error.type = se_L4_Delete_پہلا;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    pte = pte_pte_جدول_new(pptr_مقصد_paddr(PTE_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap))));

    cap = cap_صفحہ_جدول_cap_سیٹ_cap_PTIs_Mapped(cap, 1);
    cap = cap_صفحہ_جدول_cap_سیٹ_cap_PTMapped_ASID(cap, asid);
    cap = cap_صفحہ_جدول_cap_سیٹ_cap_PTMapped_پتہ(cap, (vaddr & ~نقاب(pt_خانہ.pt_بٹ_بایاں)));

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس perform_صفحہ_جدول_Invocation_Map(cap, cte, pte, pt_خانہ.pt_خانہ);
}

ساکن اندرونی bool_t CONST checkVPAlignment(vm_صفحہ_سائز_t sz, لفظ_t w)
{
    واپس (w & نقاب(صفحہ_بٹ_For_سائز(sz))) == 0;
}

ساکن exception_t decodeARMFrameInvocation(لفظ_t invLabel, لفظ_t لمبائی,
                                            cte_t *cte, cap_t cap, bool_t call, لفظ_t *بفر)
{
    انتخاب (invLabel) {
    حالت ARMPageMap: {
        vptr_t vaddr;
        paddr_t بنیاد;
        cap_t vspace_جڑ_Cap;
        vspace_جڑ_t *vspace_جڑ_2;
        asid_t asid, چوکھٹا_asid;
        vm_rights_t vmRights, capRights;
        vm_صفحہ_سائز_t چوکھٹا_سائز;
        vm_اوصاف_t اوصاف;
        ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret;

        اگر (unlikely(لمبائی < 3 || current_extra_caps.excaprefs[0] == تہی)) {
            current_syscall_error.type = se_L4_Truncated_پیغام;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, بفر);
        اوصاف = vm_اوصاف_From_لفظ(getSyscallArg(2, بفر));
        vspace_جڑ_Cap = current_extra_caps.excaprefs[0]->cap;

        چوکھٹا_سائز = cap_چوکھٹا_cap_get_cap_FSize(cap);
        capRights = cap_چوکھٹا_cap_get_cap_FVMRights(cap);
        vmRights = نقاب_VMRights(capRights, rights_From_لفظ(getSyscallArg(1, بفر)));

        اگر (unlikely(!is_Valid_Native_جڑ(vspace_جڑ_Cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        vspace_جڑ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_جڑ_Cap));
        asid = cap_vspace_cap_get_capVSMappedASID(vspace_جڑ_Cap);

        ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
        اگر (unlikely(ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = false;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (unlikely(ڈھونڈو_ret.vspace_جڑ != vspace_جڑ_2)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (unlikely(!checkVPAlignment(چوکھٹا_سائز, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        /* In the case of remap, the cap should have a valid asid */
        چوکھٹا_asid = cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap);

        اگر (چوکھٹا_asid != asidInvalid) {
            اگر (چوکھٹا_asid != asid) {
                userError("ARMPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_نمبر = 1;
                واپس EXCEPTION_SYSCALL_ERROR;

            } ورنہ اگر (cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_نمبر = 0;
                واپس EXCEPTION_SYSCALL_ERROR;
            }
        } ورنہ {
            اگر (unlikely(vaddr + ثنائی_ہندسہ(صفحہ_بٹ_For_سائز(چوکھٹا_سائز)) - 1 > USER_چوٹی)) {
                current_syscall_error.type = se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_نمبر = 0;
                واپس EXCEPTION_SYSCALL_ERROR;
            }
        }

        cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asid);
        cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(cap, vaddr);

        بنیاد = pptr_مقصد_paddr((خالی *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap));

        lookupPTSlot_ret_t lu_ret = lookupPTSlot(vspace_جڑ_2, vaddr);
        اگر (unlikely(lu_ret.pt_بٹ_بایاں != صفحہ_بٹ_For_سائز(چوکھٹا_سائز))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_بٹ_بایاں);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = false;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        bool_t cap_read = (capRights == VMReadOnly || capRights == VMReadWrite);
        pte_t pte = make_User_صفحہ_PTE(بنیاد, vmRights, cap_read, اوصاف, چوکھٹا_سائز);

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس perform_صفحہ_Invocation_Map(asid, cap, cte, pte, lu_ret.pt_خانہ);
    }

    حالت ARMPageUnmap:
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس perform_صفحہ_Invocation_Unmap(cap, cte);

    حالت ARMPage_Clean_ڈیٹا:
    حالت ARMPage_Invalidate_ڈیٹا:
    حالت ARMPage_Clean_Invalidate_ڈیٹا:
    حالت ARMPageUnify_Instruction: {
        vptr_t آغاز, انتہا;
        vptr_t vaddr;
        asid_t asid;
        لفظ_t صفحہ_سائز;
        ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret;

        اگر (لمبائی < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (unlikely(cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap) == 0)) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap);
        asid = cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap);

        ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
        اگر (unlikely(ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE)) {
            userError("Page Flush: No PGD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = false;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        آغاز = getSyscallArg(0, بفر);
        انتہا =   getSyscallArg(1, بفر);

        /* check that the range is sane */
        اگر (انتہا <= آغاز) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 1;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        /* start and end are currently relative inside this page */
        صفحہ_سائز = ثنائی_ہندسہ(صفحہ_بٹ_For_سائز(cap_چوکھٹا_cap_get_cap_FSize(cap)));
        اگر (آغاز >= صفحہ_سائز || انتہا > صفحہ_سائز) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        لفظ_t pstart = pptr_مقصد_paddr((خالی *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)) + آغاز;
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        اگر (pstart < PADDR_بنیاد || ((انتہا - آغاز) + pstart) > PADDR_چوٹی) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
#ورنہ
        /* When in EL1, we are using the user page table for flushing and need to make sure
           the mapping info in the cap is not stale. */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(ڈھونڈو_ret.vspace_جڑ, vaddr);
        pte_t pte = *lu_ret.pt_خانہ;
        خالی *بنیاد_ptr = (خالی *) cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap);
        اگر (unlikely(lu_ret.pt_بٹ_بایاں != صفحہ_بٹ_For_سائز(cap_چوکھٹا_cap_get_cap_FSize(cap)) ||
                     !pte_is_صفحہ_type(pte) ||
                     pte_get_صفحہ_بنیاد_پتہ(pte) != pptr_مقصد_paddr(بنیاد_ptr))) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        /* When in EL1, the mapping must be writeable for DC IVAC */
        اگر (invLabel == ARMPage_Invalidate_ڈیٹا && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMPageInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
#اگر_ختم
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس perform_صفحہ_Flush(invLabel, ڈھونڈو_ret.vspace_جڑ, asid, vaddr + آغاز, vaddr + انتہا - 1,
                                pstart);
    }

    حالت ARMPage_Get_پتہ:
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس perform_صفحہ_Get_پتہ(cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap), call);

    طےشدہ:
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(لفظ_t invLabel, لفظ_t لمبائی, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, لفظ_t *بفر)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_vspace_cap:
        واپس decode_ARMVSpace_جڑ_Invocation(invLabel, لمبائی, cte, cap, بفر);
    حالت cap_صفحہ_جدول_cap:
        واپس decode_ARMPage_جدول_Invocation(invLabel, لمبائی, cte, cap, بفر);

    حالت cap_چوکھٹا_cap:
        واپس decodeARMFrameInvocation(invLabel, لمبائی, cte, cap, call, بفر);

    حالت cap_asid_control_cap: {
        لفظ_t i;
        asid_t asid_بنیاد;
        لفظ_t اشاریہ, گہرائی;
        cap_t untyped, جڑ;
        cte_t *والد_خانہ, *dest_خانہ;
        lookup_خانہ_ret_t lu_ret;
        خالی *چوکھٹا;
        exception_t حالت_2;

        اگر (unlikely(invLabel != ARMASIDControlMakePool)) {
            current_syscall_error.type = seL4_IllegalOperation;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (unlikely(لمبائی < 2 ||
                     current_extra_caps.excaprefs[0] == تہی ||
                     current_extra_caps.excaprefs[1] == تہی)) {
            current_syscall_error.type = se_L4_Truncated_پیغام;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اشاریہ = getSyscallArg(0, بفر);
        گہرائی = getSyscallArg(1, بفر);
        والد_خانہ = current_extra_caps.excaprefs[0];
        untyped = والد_خانہ->cap;
        جڑ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        برائے (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        اگر (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_پہلا;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        asid_بنیاد = i << asid_کم_بٹ;

        اگر (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_بلاک_سائز(untyped) != se_L4_ASIDPool_بٹ ||
                     cap_untyped_cap_get_capIsDevice(untyped))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        حالت_2 = ensureNoChildren(والد_خانہ);
        اگر (unlikely(حالت_2 != EXCEPTION_NONE)) {
            واپس حالت_2;
        }

        چوکھٹا = لفظ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_ہدف_خانہ(جڑ, اشاریہ, گہرائی);
        اگر (unlikely(lu_ret.حالت_2 != EXCEPTION_NONE)) {
            واپس lu_ret.حالت_2;
        }
        dest_خانہ = lu_ret.خانہ;

        حالت_2 = ensure_خالی_خانے_خانہ(dest_خانہ);
        اگر (unlikely(حالت_2 != EXCEPTION_NONE)) {
            واپس حالت_2;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس performASIDControlInvocation(چوکھٹا, dest_خانہ, والد_خانہ, asid_بنیاد);
    }

    حالت cap_asid_pool_cap: {
        cap_t vspaceCap;
        cte_t *vspace_Cap_خانہ;
        asid_pool_t *pool;
        لفظ_t i;
        asid_t asid;

        اگر (unlikely(invLabel != ARMASIDPool_تفویض)) {
            current_syscall_error.type = seL4_IllegalOperation;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (unlikely(current_extra_caps.excaprefs[0] == تہی)) {
            current_syscall_error.type = se_L4_Truncated_پیغام;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_خانہ = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_خانہ->cap;

        اگر (unlikely(!is_VTable_جڑ(vspaceCap) || cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_بلند(cap_asid_pool_cap_get_capASIDBase(cap))];

        اگر (unlikely(!pool)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = false;
            current_lookup_fault = lookup_fault_invalid_جڑ_new();

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        برائے (i = 0; i < (1 << asid_کم_بٹ) && (asid + i == 0
                                               || (asid_map_get_type(pool->array[i]) != asid_map_asid_map_none)); i++);

        اگر (unlikely(i == 1 << asid_کم_بٹ)) {
            current_syscall_error.type = se_L4_Delete_پہلا;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس performASIDPoolInvocation(asid, pool, vspace_Cap_خانہ);
    }

    طےشدہ:
        fail("Invalid ARM arch cap type");
    }
}

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
خالی kernelPrefetchAbort(لفظ_t pc) VISIBLE;
خالی kernel_ڈیٹا_Abort(لفظ_t pc) VISIBLE;

خالی kernelPrefetchAbort(لفظ_t pc)
{
    لکھو("\n\nKERNEL PREFETCH ABORT!\n");
    لکھو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکھو("ESR (IFSR): 0x%"SEL4_PRIx_لفظ"\n", getIFSR());
    halt();
}

خالی kernel_ڈیٹا_Abort(لفظ_t pc)
{
    لکھو("\n\nKERNEL DATA ABORT!\n");
    لکھو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکھو("FAR: 0x%"SEL4_PRIx_لفظ" ESR (DFSR): 0x%"SEL4_PRIx_لفظ"\n",
           getFAR(), getDFSR());
    halt();
}
#اگر_ختم /* CONFIG_DEBUG_BUILD */

#اگر_تعریف_شدہ CONFIG_PRINTING
قسمتعریف ساخت read_لفظ_From_VSpace_ret {
    exception_t حالت_2;
    لفظ_t قدر;
} read_لفظ_From_VSpace_ret_t;

ساکن read_لفظ_From_VSpace_ret_t read_لفظ_From_VSpace(vspace_جڑ_t *pd, لفظ_t vaddr)
{
    read_لفظ_From_VSpace_ret_t ret;
    لفظ_t انحراف;
    pptr_t kernel_vaddr;
    لفظ_t *قدر;

    lookupPTSlot_ret_t lookup_ret = lookupPTSlot(pd, vaddr);

    /* Check that the returned slot is a page. */
    اگر (!pte_ptr_get_valid(lookup_ret.pt_خانہ) ||
        (pte_pte_جدول_ptr_get_present(lookup_ret.pt_خانہ) && lookup_ret.pt_بٹ_بایاں > صفحہ_بٹ)) {
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    }

    انحراف = vaddr & نقاب(lookup_ret.pt_بٹ_بایاں);
    kernel_vaddr = (لفظ_t)paddr_مقصد_pptr(pte_صفحہ_ptr_get_صفحہ_بنیاد_پتہ(lookup_ret.pt_خانہ));
    قدر = (لفظ_t *)(kernel_vaddr + انحراف);

    ret.حالت_2 = EXCEPTION_NONE;
    ret.قدر = *قدر;
    واپس ret;
}

خالی Arch_user_ڈھیر_Trace(tcb_t *tptr)
{
    cap_t thread_جڑ;
    vspace_جڑ_t *vspace_جڑ_2;
    لفظ_t sp;

    thread_جڑ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the vspace root */
    اگر (cap_get_capType(thread_جڑ) != cap_vspace_cap) {
        لکھو("Invalid vspace\n");
        واپس;
    }

    vspace_جڑ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_جڑ));
    sp = get_رجسٹر(tptr, SP_EL0);

    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    اگر (!IS_ALIGNED(sp, se_L4_لفظ_سائز_بٹ)) {
        لکھو("SP not aligned\n");
        واپس;
    }

    برائے (غیرعلامتی صحیح i = 0; i < CONFIG_USER_ڈھیر_TRACE_لمبائی; i++) {
        لفظ_t پتہ = sp + (i * حجم(لفظ_t));
        read_لفظ_From_VSpace_ret_t نتیجہ = read_لفظ_From_VSpace(vspace_جڑ_2,
                                                             پتہ);
        اگر (نتیجہ.حالت_2 == EXCEPTION_NONE) {
            لکھو("0x%"SEL4_PRIx_لفظ": 0x%"SEL4_PRIx_لفظ"\n",
                   پتہ, نتیجہ.قدر);
        } ورنہ {
            لکھو("0x%"SEL4_PRIx_لفظ": INVALID\n", پتہ);
        }
    }
}
#اگر_ختم /* CONFIG_PRINTING */

#اگر defined(CONFIG_KERNEL_LOG_بفر)
exception_t benchmark_arch_map_log_بفر(لفظ_t چوکھٹا_cptr)
{
    lookup_Cap_And_خانہ_ret_t lu_ret;
    vm_صفحہ_سائز_t چوکھٹا_سائز;
    pptr_t  چوکھٹا_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_خانہ(گرہ_STATE(ksCurThread), چوکھٹا_cptr);

    اگر (unlikely(lu_ret.حالت_2 != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", چوکھٹا_cptr);
        current_fault = seL4_Fault_CapFault_new(چوکھٹا_cptr, false);

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (cap_get_capType(lu_ret.cap) != cap_چوکھٹا_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(چوکھٹا_cptr, false);

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    چوکھٹا_سائز = cap_چوکھٹا_cap_get_cap_FSize(lu_ret.cap);

    اگر (چوکھٹا_سائز != ARMLarge_صفحہ) {
        userError("Invalid frame size. The kernel expects 2M log buffer");
        current_fault = seL4_Fault_CapFault_new(چوکھٹا_cptr, false);

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    چوکھٹا_pptr = cap_چوکھٹا_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_بفر = pptr_مقصد_paddr((خالی *) چوکھٹا_pptr);

    *armKSGlobalLogPTE = pte_pte_صفحہ_new(
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                             0, // XN
#ورنہ
                             1, // UXN
#اگر_ختم
                             ks_User_Log_بفر,
                             0,                         /* global */
                             1,                         /* access flag */
                             SMP_TERNARY(SMP_SHARE, 0), /* Inner-shareable if SMP enabled, otherwise unshared */
                             0,                         /* VMKernelOnly */
                             معمول_WT);

    cleanByVA_PoU((vptr_t)armKSGlobalLogPTE, addrFromKPPtr(armKSGlobalLogPTE));
    invalidateTranslationSingle(KS_LOG_PPTR);
    واپس EXCEPTION_NONE;
}
#اگر_ختم /* CONFIG_KERNEL_LOG_BUFFER */
