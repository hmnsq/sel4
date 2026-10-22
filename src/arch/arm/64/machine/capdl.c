/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#اگر_تعریف_شده CONFIG_DEBUG_BUILD

#ضمن <arch/machine/capdl.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>

کلمه_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#اگر_تعریف_شده CONFIG_PRINTING

ساكن فراغ obj_قاب_print_attrs(vm_صفحه_اندازه_t قاب_اندازه, paddr_t قاب_پایه);
ساكن فراغ cap_قاب_print_attrs_pt(pte_t *pt_جایگاه);
ساكن فراغ cap_قاب_print_attrs_impl(کلمه_t SH, کلمه_t AP, کلمه_t NXN);
ساكن فراغ cap_قاب_print_attrs_vptr(کلمه_t vptr, cap_t vspace);

ساكن فراغ cap_قاب_print_attrs_vptr_2(کلمه_t vptr, vspace_ریشه_t *vspace_ریشه_2);

ساكن فراغ arm64_obj_pt_print_slots(pte_t *pd_جایگاه);
ساكن فراغ arm64_obj_pd_print_slots(pte_t *pud_جایگاه);
ساكن فراغ arm64_obj_pud_print_slots(فراغ *pgd_جایگاه_or_vspace);

ساكن فراغ arm64_cap_pt_print_slots(pte_t *pd_جایگاه, vptr_t vptr);
ساكن فراغ arm64_cap_pd_print_slots(pte_t *pud_جایگاه, vptr_t vptr);
ساكن فراغ arm64_cap_pud_print_slots(فراغ *pgd_جایگاه_or_vspace, vptr_t vptr);

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
/* use when only have access to pte of frames */
ساكن فراغ cap_قاب_print_attrs_pt(pte_t *pt_جایگاه)
{
    cap_قاب_print_attrs_impl(pte_pte_صفحه_ptr_get_SH(pt_جایگاه),
                               pte_pte_صفحه_ptr_get_AP(pt_جایگاه),
                               pte_pte_صفحه_ptr_get_UXN(pt_جایگاه));
}

ساكن فراغ cap_قاب_print_attrs_impl(کلمه_t SH, کلمه_t AP, کلمه_t NXN)
{
    چاپ_کن("(");

    /* rights */
    بدل (AP) {
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة 0b00:
        اكسر;
    حالة 0b01:
        چاپ_کن("R");
        اكسر;
    حالة 0b10:
        چاپ_کن("W");
        اكسر;
    حالة 0b11:
        چاپ_کن("RW");
        اكسر;
#وإلا
    حالة 0b00:
        اكسر;
    حالة 0b01:
        چاپ_کن("RW");
    حالة 0b10:
        اكسر;
    حالة 0b11:
        چاپ_کن("R");
#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    افتراضي:
        اكسر;
    }

    إذا (!NXN) {
        چاپ_کن("X");
    }

    /* Only has effect if SMP enabled */
    إذا (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        چاپ_کن(", uncached");
    }

    چاپ_کن(")\n");
}

/* use when only have access to vptr of frames */
ساكن فراغ cap_قاب_print_attrs_vptr_2(کلمه_t vptr, vspace_ریشه_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    إذا (!pte_ptr_get_valid(ret.pt_جایگاه) ||
        (pte_pte_جدول_ptr_get_present(ret.pt_جایگاه) && ret.pt_بیتها_چپ > صفحه_بیتها)) {
        أكد(0);
    }

    کلمه_t جدول_شاخص;
    بدل (ret.pt_بیتها_چپ) {

    حالة ARMHuge_صفحه:
        جدول_شاخص = GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(1));
        اكسر;
    حالة ARMLarge_صفحه:
        جدول_شاخص = GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(2));
        اكسر;
    حالة ARMSmall_صفحه:
        جدول_شاخص = GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(3));
        اكسر;
    افتراضي:
        أكد(0);

    }
    چاپ_کن("frame_%p_%04lu ", ret.pt_جایگاه, جدول_شاخص);
    cap_قاب_print_attrs_pt(ret.pt_جایگاه);
}

فراغ cap_قاب_print_attrs_vptr(کلمه_t vptr, cap_t vspace)
{
    cap_قاب_print_attrs_vptr_2(vptr, VSPACE_PTR(pptr_از_cap(vspace)));
}

/*
 * print object slots
 */
ساكن فراغ arm64_cap_pt_print_slots(pte_t *pd_جایگاه, vptr_t vptr)
{
    pte_t *pt = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_پایه_آدرس(pd_جایگاه));
    چاپ_کن("pt_%p_%04lu {\n", pd_جایگاه, GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    لكل (کلمه_t i = 0; i < بیت(PT_شاخص_بیتها); i ++) {
        pte_t *pt_جایگاه = pt + i;

        إذا (pte_4k_صفحه_ptr_get_present(pt_جایگاه)) {
            // print pte entries
            چاپ_کن("0x%lx: frame_%p_%04lu", i, pt_جایگاه, i);
            cap_قاب_print_attrs_pt(pt_جایگاه);
        }
    }
    چاپ_کن("}\n"); /* pt */
}

ساكن فراغ arm64_cap_pd_print_slots(pte_t *pud_جایگاه, vptr_t vptr)
{
    چاپ_کن("pd_%p_%04lu {\n", pud_جایگاه, GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_پایه_آدرس(pud_جایگاه));

    لكل (کلمه_t i = 0; i < بیت(PT_شاخص_بیتها); i++) {
        pte_t *pd_جایگاه = pd + i;

        بدل (pte_ptr_get_pte_type(pd_جایگاه)) {

        حالة pte_pte_صفحه:
            چاپ_کن("0x%lx: frame_%p_%04lu", i, pd_جایگاه, i);
            cap_قاب_print_attrs_pt(pd_جایگاه);
            اكسر;

        حالة pte_pte_جدول:
            چاپ_کن("0x%lx: pt_%p_%04lu\n", i, pd_جایگاه, i);
            اكسر;
        }
    }

    چاپ_کن("}\n"); /* pd */

    لكل (کلمه_t i = 0; i < بیت(PT_شاخص_بیتها); i++) {
        pte_t *pd_جایگاه = pd + i;
        إذا (pte_ptr_get_pte_type(pd_جایگاه) == pte_pte_جدول) {
            arm64_cap_pt_print_slots(pd_جایگاه, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

ساكن فراغ arm64_cap_pud_print_slots(فراغ *pgd_جایگاه_or_vspace, vptr_t vptr)
{
#اگر_تعریف_شده AARCH64_VSPACE_S2_شروع_L1
    pte_t *pud = pgd_جایگاه_or_vspace;
    کلمه_t شاخص_بیتها = se_L4_VSpace_شاخص_بیتها;
    چاپ_کن("%p_pd {\n", pgd_جایگاه_or_vspace);
#وإلا
    pte_t *pud = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_پایه_آدرس(pgd_جایگاه_or_vspace));
    کلمه_t شاخص_بیتها = se_L4_صفحه_جدول_شاخص_بیتها;
    چاپ_کن("pud_%p_%04lu {\n", pgd_جایگاه_or_vspace, GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#پایان_اگر

    لكل (کلمه_t i = 0; i < بیت(شاخص_بیتها); i++) {
        pte_t *pud_جایگاه = pud + i;
        إذا (pte_ptr_get_pte_type(pud_جایگاه) == pte_pte_جدول) {
            چاپ_کن("0x%lx: pd_%p_%04lu\n", i, pud_جایگاه, i);
        }
    }

    چاپ_کن("}\n"); /* pgd/pud */

    لكل (کلمه_t i = 0; i < بیت(شاخص_بیتها); i++) {
        pte_t *pud_جایگاه = pud + GET_UPT_شاخص(i, ULVL_FRM_ARM_PT_LVL(1));
        إذا (pte_ptr_get_pte_type(pud_جایگاه) == pte_pte_جدول) {
            arm64_cap_pd_print_slots(pud_جایگاه, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    إذا (is_VTable_ریشه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !دیده(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جمع_کن_مقصد_دیده(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_ریشه_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#اگر_تعریف_شده AARCH64_VSPACE_S2_شروع_L1
        arm64_cap_pud_print_slots(vspace, 0);
#وإلا
        چاپ_کن("%p_pd {\n", vspace);
        لكل (کلمه_t i = 0; i < PT_شاخص_بیتها; i++) {
            pte_t *pt_جایگاه = vspace + i;
            إذا (pte_pte_جدول_ptr_get_present(pt_جایگاه)) {
                چاپ_کن("0x%lx: pud_%p_%04lu\n", i, pt_جایگاه, i);
            }
        }
        چاپ_کن("}\n"); /* pd */

        لكل (کلمه_t i = 0; i < PT_شاخص_بیتها; i++) {
            pte_t *pt_جایگاه = vspace + i;
            إذا (pte_pte_جدول_ptr_get_present(pt_جایگاه)) {
                arm64_cap_pud_print_slots(pt_جایگاه, i * GET_ULVL_PGSIZE(0));
            }
        }
#پایان_اگر
    }
}

فراغ print_ipc_میانگیر_جایگاه(tcb_t *tcb)
{
    کلمه_t vptr = tcb->tcbIPCBuffer;
    چاپ_کن("ipc_buffer_slot: ");
    cap_قاب_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

فراغ print_cap_arch(cap_t cap)
{

    بدل (cap_get_capType(cap)) {
    حالة cap_صفحه_جدول_cap: {
        asid_t asid = cap_صفحه_جدول_cap_get_cap_PTMapped_ASID(cap);
        vptr_t vptr = cap_صفحه_جدول_cap_get_cap_PTMapped_آدرس(cap);
        pte_t *هدف_pt = PT_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap));

        پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
        pte_t *pt_جایگاه = عدم;
        pte_t *pt = (pte_t *)پیدا_کن_ret.vspace_ریشه;
        کلمه_t level;
        لكل (level = 0; level < UPT_سطوح - 1 && pt != هدف_pt; level++) {
            pt_جایگاه = pt + GET_UPT_شاخص(vptr, level);
            إذا (unlikely(!pte_pte_جدول_ptr_get_present(pt_جایگاه))) {
                /* couldn't find it */
                اكسر;
            }
            pt = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_پایه_آدرس(pt_جایگاه));
        }
        إذا (pt != هدف_pt) {
            /* didn't find it */
            اكسر;
        }


        إذا (asid) {
            چاپ_کن("pt_%p_%04lu (asid: %lu)\n",
                   هدف_pt, GET_UPT_شاخص(vptr, level), (دراز بیعلامت صحيح)asid);
        } وإلا {
            چاپ_کن("pt_%p_%04lu\n", هدف_pt, GET_UPT_شاخص(vptr, level));
        }
        اكسر;
    }
    حالة cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
        إذا (asid) {
            چاپ_کن("%p_pd (asid: %lu)\n",
                   پیدا_کن_ret.vspace_ریشه, (دراز بیعلامت صحيح)asid);
        } وإلا {
            چاپ_کن("%p_pd\n", پیدا_کن_ret.vspace_ریشه);
        }
        اكسر;
    }
    حالة cap_asid_control_cap: {
        /* only one in the system */
        چاپ_کن("asid_control\n");
        اكسر;
    }
    حالة cap_قاب_cap: {
        vptr_t vptr = cap_قاب_cap_get_cap_FMapped_آدرس(cap);
        پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(cap_قاب_cap_get_cap_FMapped_ASID(cap));
        أكد(پیدا_کن_ret.وضعیت == EXCEPTION_NONE);
        cap_قاب_print_attrs_vptr_2(vptr, پیدا_کن_ret.vspace_ریشه);
        اكسر;
    }
    حالة cap_asid_pool_cap: {
        چاپ_کن("%p_asid_pool\n", (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        اكسر;
    }
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        چاپ_کن("%p_vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#پایان_اگر

        /* ARM specific caps */
#اگر_تعریف_شده CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        چاپ_کن("%p_io_space\n", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        اكسر;
    }
#پایان_اگر
    افتراضي: {
        چاپ_کن("[unknown cap %lu]\n", (دراز بیعلامت صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ print_object_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_قاب_cap:
    حالة cap_صفحه_جدول_cap:
    حالة cap_vspace_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        چاپ_کن("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        اكسر;
    }
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        چاپ_کن("%p_vcpu = vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#پایان_اگر
        /* ARM specific objects */
#اگر_تعریف_شده CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        چاپ_کن("%p_io_space = io_space ", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        اكسر;
    }
#پایان_اگر
    افتراضي: {
        چاپ_کن("[unknown object %lu]\n", (دراز بیعلامت صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ obj_قاب_print_attrs(vm_صفحه_اندازه_t قاب_اندازه, paddr_t قاب_پایه)
{
    چاپ_کن("(");

    /* VM size */
    بدل (قاب_اندازه) {
    حالة ARMHuge_صفحه:
        چاپ_کن("1G");
        اكسر;
    حالة ARMLarge_صفحه:
        چاپ_کن("2M");
        اكسر;
    حالة ARMSmall_صفحه:
        چاپ_کن("4k");
        اكسر;
    }

    چاپ_کن(", paddr: 0x%p)\n", (فراغ *)قاب_پایه);
}

فراغ arm64_obj_pt_print_slots(pte_t *pd_جایگاه)
{
    pte_t *pt = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_پایه_آدرس(pd_جایگاه));

    لكل (کلمه_t i = 0; i < بیت(PT_شاخص_بیتها); i++) {
        pte_t *pt_جایگاه = pt + i;

        إذا (pte_4k_صفحه_ptr_get_present(pt_جایگاه)) {
            چاپ_کن("frame_%p_%04lu = frame ", pt_جایگاه, i);
            obj_قاب_print_attrs(ARMSmall_صفحه, pte_صفحه_ptr_get_صفحه_پایه_آدرس(pt_جایگاه));
        }
    }
}

فراغ arm64_obj_pd_print_slots(pte_t *pud_جایگاه)
{
    pte_t *pd = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_پایه_آدرس(pud_جایگاه));

    لكل (کلمه_t i = 0; i < بیت(PT_شاخص_بیتها); i++) {
        pte_t *pd_جایگاه = pd + i;

        إذا (pte_ptr_get_pte_type(pd_جایگاه) == pte_pte_صفحه) {
            چاپ_کن("frame_%p_%04lu = frame ", pd_جایگاه, i);
            obj_قاب_print_attrs(ARMLarge_صفحه, pte_صفحه_ptr_get_صفحه_پایه_آدرس(pd_جایگاه));
        }

        إذا (pte_ptr_get_pte_type(pd_جایگاه) == pte_pte_جدول) {
            چاپ_کن("pt_%p_%04lu = pt\n", pd_جایگاه, i);
            arm64_obj_pt_print_slots(pd_جایگاه);
        }
    }
}

فراغ arm64_obj_pud_print_slots(فراغ *pgd_جایگاه_or_vspace)
{
    pte_t *pud = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_پایه_آدرس(pgd_جایگاه_or_vspace));
#اگر_تعریف_شده AARCH64_VSPACE_S2_شروع_L1
    کلمه_t شاخص_بیتها = se_L4_VSpace_شاخص_بیتها;
#وإلا
    کلمه_t شاخص_بیتها = se_L4_صفحه_جدول_شاخص_بیتها;
#پایان_اگر
    لكل (کلمه_t i = 0; i < بیت(شاخص_بیتها); i++) {
        pte_t *pud_جایگاه = pud + i;

        بدل (pte_ptr_get_pte_type(pud_جایگاه)) {
        حالة pte_pte_صفحه:
            چاپ_کن("frame_%p_%04lu = frame ", pud_جایگاه, i);
            obj_قاب_print_attrs(ARMHuge_صفحه, pte_صفحه_ptr_get_صفحه_پایه_آدرس(pud_جایگاه));
            اكسر;

        حالة pte_pte_جدول: {
            چاپ_کن("pd_%p_%04lu = pd\n", pud_جایگاه, i);
            arm64_obj_pd_print_slots(pud_جایگاه);

        }
        }
    }
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    إذا (is_VTable_ریشه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !دیده(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جمع_کن_مقصد_دیده(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_ریشه_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#اگر_تعریف_شده AARCH64_VSPACE_S2_شروع_L1
        چاپ_کن("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#وإلا
        چاپ_کن("%p_pd = pgd\n", vspace);
        لكل (کلمه_t i = 0; i < PT_شاخص_بیتها; i++) {
            pte_t *pt_جایگاه = vspace + i;
            إذا (pte_pte_جدول_ptr_get_present(pt_جایگاه)) {
                چاپ_کن("pud_%p_%04lu = pud\n", pt_جایگاه, i);
                arm64_obj_pud_print_slots(pt_جایگاه);
            }
        }
#پایان_اگر
    }
}

#پایان_اگر /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
{
    چاپ_کن("arch aarch64\n");
    چاپ_کن("objects {\n");
#اگر_تعریف_شده CONFIG_PRINTING
    print_objects();
#پایان_اگر
    چاپ_کن("}\n");

    چاپ_کن("caps {\n");

    /* reset the seen list */
    reset_دیده_list();

#اگر_تعریف_شده CONFIG_PRINTING
    print_caps();
    چاپ_کن("}\n");

    obj_irq_print_maps();
#پایان_اگر /* CONFIG_PRINTING */
}

#پایان_اگر /* CONFIG_DEBUG_BUILD */
