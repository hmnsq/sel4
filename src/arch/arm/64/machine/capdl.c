/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#که_تعریف_شوی CONFIG_DEBUG_BUILD

#ضمن <arch/machine/capdl.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>

کلمه_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#که_تعریف_شوی CONFIG_PRINTING

ساكن فراغ obj_چوکاټ_print_attrs(vm_پاڼه_اندازه_t چوکاټ_اندازه, paddr_t چوکاټ_بنسټ);
ساكن فراغ cap_چوکاټ_print_attrs_pt(pte_t *pt_خونه);
ساكن فراغ cap_چوکاټ_print_attrs_impl(کلمه_t SH, کلمه_t AP, کلمه_t NXN);
ساكن فراغ cap_چوکاټ_print_attrs_vptr(کلمه_t vptr, cap_t vspace);

ساكن فراغ cap_چوکاټ_print_attrs_vptr_2(کلمه_t vptr, vspace_ریښه_t *vspace_ریښه_2);

ساكن فراغ arm64_obj_pt_print_slots(pte_t *pd_خونه);
ساكن فراغ arm64_obj_pd_print_slots(pte_t *pud_خونه);
ساكن فراغ arm64_obj_pud_print_slots(فراغ *pgd_خونه_or_vspace);

ساكن فراغ arm64_cap_pt_print_slots(pte_t *pd_خونه, vptr_t vptr);
ساكن فراغ arm64_cap_pd_print_slots(pte_t *pud_خونه, vptr_t vptr);
ساكن فراغ arm64_cap_pud_print_slots(فراغ *pgd_خونه_or_vspace, vptr_t vptr);

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
ساكن فراغ cap_چوکاټ_print_attrs_pt(pte_t *pt_خونه)
{
    cap_چوکاټ_print_attrs_impl(pte_pte_پاڼه_ptr_get_SH(pt_خونه),
                               pte_pte_پاڼه_ptr_get_AP(pt_خونه),
                               pte_pte_پاڼه_ptr_get_UXN(pt_خونه));
}

ساكن فراغ cap_چوکاټ_print_attrs_impl(کلمه_t SH, کلمه_t AP, کلمه_t NXN)
{
    چاپ_کړه("(");

    /* rights */
    بدل (AP) {
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة 0b00:
        اكسر;
    حالة 0b01:
        چاپ_کړه("R");
        اكسر;
    حالة 0b10:
        چاپ_کړه("W");
        اكسر;
    حالة 0b11:
        چاپ_کړه("RW");
        اكسر;
#وإلا
    حالة 0b00:
        اكسر;
    حالة 0b01:
        چاپ_کړه("RW");
    حالة 0b10:
        اكسر;
    حالة 0b11:
        چاپ_کړه("R");
#د_که_پای /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    افتراضي:
        اكسر;
    }

    إذا (!NXN) {
        چاپ_کړه("X");
    }

    /* Only has effect if SMP enabled */
    إذا (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        چاپ_کړه(", uncached");
    }

    چاپ_کړه(")\n");
}

/* use when only have access to vptr of frames */
ساكن فراغ cap_چوکاټ_print_attrs_vptr_2(کلمه_t vptr, vspace_ریښه_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    إذا (!pte_ptr_get_valid(ret.pt_خونه) ||
        (pte_pte_جدول_ptr_get_present(ret.pt_خونه) && ret.pt_بټونه_کیڼ > پاڼه_بټونه)) {
        أكد(0);
    }

    کلمه_t جدول_شاخص;
    بدل (ret.pt_بټونه_کیڼ) {

    حالة ARMHuge_پاڼه:
        جدول_شاخص = GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(1));
        اكسر;
    حالة ARMLarge_پاڼه:
        جدول_شاخص = GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(2));
        اكسر;
    حالة ARMSmall_پاڼه:
        جدول_شاخص = GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(3));
        اكسر;
    افتراضي:
        أكد(0);

    }
    چاپ_کړه("frame_%p_%04lu ", ret.pt_خونه, جدول_شاخص);
    cap_چوکاټ_print_attrs_pt(ret.pt_خونه);
}

فراغ cap_چوکاټ_print_attrs_vptr(کلمه_t vptr, cap_t vspace)
{
    cap_چوکاټ_print_attrs_vptr_2(vptr, VSPACE_PTR(pptr_د_cap(vspace)));
}

/*
 * print object slots
 */
ساكن فراغ arm64_cap_pt_print_slots(pte_t *pd_خونه, vptr_t vptr)
{
    pte_t *pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنسټ_پته(pd_خونه));
    چاپ_کړه("pt_%p_%04lu {\n", pd_خونه, GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    لكل (کلمه_t i = 0; i < بټ(PT_شاخص_بټونه); i ++) {
        pte_t *pt_خونه = pt + i;

        إذا (pte_4k_پاڼه_ptr_get_present(pt_خونه)) {
            // print pte entries
            چاپ_کړه("0x%lx: frame_%p_%04lu", i, pt_خونه, i);
            cap_چوکاټ_print_attrs_pt(pt_خونه);
        }
    }
    چاپ_کړه("}\n"); /* pt */
}

ساكن فراغ arm64_cap_pd_print_slots(pte_t *pud_خونه, vptr_t vptr)
{
    چاپ_کړه("pd_%p_%04lu {\n", pud_خونه, GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنسټ_پته(pud_خونه));

    لكل (کلمه_t i = 0; i < بټ(PT_شاخص_بټونه); i++) {
        pte_t *pd_خونه = pd + i;

        بدل (pte_ptr_get_pte_type(pd_خونه)) {

        حالة pte_pte_پاڼه:
            چاپ_کړه("0x%lx: frame_%p_%04lu", i, pd_خونه, i);
            cap_چوکاټ_print_attrs_pt(pd_خونه);
            اكسر;

        حالة pte_pte_جدول:
            چاپ_کړه("0x%lx: pt_%p_%04lu\n", i, pd_خونه, i);
            اكسر;
        }
    }

    چاپ_کړه("}\n"); /* pd */

    لكل (کلمه_t i = 0; i < بټ(PT_شاخص_بټونه); i++) {
        pte_t *pd_خونه = pd + i;
        إذا (pte_ptr_get_pte_type(pd_خونه) == pte_pte_جدول) {
            arm64_cap_pt_print_slots(pd_خونه, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

ساكن فراغ arm64_cap_pud_print_slots(فراغ *pgd_خونه_or_vspace, vptr_t vptr)
{
#که_تعریف_شوی AARCH64_VSPACE_S2_پیل_L1
    pte_t *pud = pgd_خونه_or_vspace;
    کلمه_t شاخص_بټونه = se_L4_VSpace_شاخص_بټونه;
    چاپ_کړه("%p_pd {\n", pgd_خونه_or_vspace);
#وإلا
    pte_t *pud = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنسټ_پته(pgd_خونه_or_vspace));
    کلمه_t شاخص_بټونه = se_L4_پاڼه_جدول_شاخص_بټونه;
    چاپ_کړه("pud_%p_%04lu {\n", pgd_خونه_or_vspace, GET_UPT_شاخص(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#د_که_پای

    لكل (کلمه_t i = 0; i < بټ(شاخص_بټونه); i++) {
        pte_t *pud_خونه = pud + i;
        إذا (pte_ptr_get_pte_type(pud_خونه) == pte_pte_جدول) {
            چاپ_کړه("0x%lx: pd_%p_%04lu\n", i, pud_خونه, i);
        }
    }

    چاپ_کړه("}\n"); /* pgd/pud */

    لكل (کلمه_t i = 0; i < بټ(شاخص_بټونه); i++) {
        pte_t *pud_خونه = pud + GET_UPT_شاخص(i, ULVL_FRM_ARM_PT_LVL(1));
        إذا (pte_ptr_get_pte_type(pud_خونه) == pte_pte_جدول) {
            arm64_cap_pd_print_slots(pud_خونه, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    إذا (is_VTable_ریښه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !کتل_شوی(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جمع_کړه_منزل_کتل_شوی(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_ریښه_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#که_تعریف_شوی AARCH64_VSPACE_S2_پیل_L1
        arm64_cap_pud_print_slots(vspace, 0);
#وإلا
        چاپ_کړه("%p_pd {\n", vspace);
        لكل (کلمه_t i = 0; i < PT_شاخص_بټونه; i++) {
            pte_t *pt_خونه = vspace + i;
            إذا (pte_pte_جدول_ptr_get_present(pt_خونه)) {
                چاپ_کړه("0x%lx: pud_%p_%04lu\n", i, pt_خونه, i);
            }
        }
        چاپ_کړه("}\n"); /* pd */

        لكل (کلمه_t i = 0; i < PT_شاخص_بټونه; i++) {
            pte_t *pt_خونه = vspace + i;
            إذا (pte_pte_جدول_ptr_get_present(pt_خونه)) {
                arm64_cap_pud_print_slots(pt_خونه, i * GET_ULVL_PGSIZE(0));
            }
        }
#د_که_پای
    }
}

فراغ print_ipc_بفر_خونه(tcb_t *tcb)
{
    کلمه_t vptr = tcb->tcbIPCBuffer;
    چاپ_کړه("ipc_buffer_slot: ");
    cap_چوکاټ_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

فراغ print_cap_arch(cap_t cap)
{

    بدل (cap_get_capType(cap)) {
    حالة cap_پاڼه_جدول_cap: {
        asid_t asid = cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(cap);
        vptr_t vptr = cap_پاڼه_جدول_cap_get_cap_PTMapped_پته(cap);
        pte_t *موخه_pt = PT_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(cap));

        پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
        pte_t *pt_خونه = عدم;
        pte_t *pt = (pte_t *)پیدا_کړه_ret.vspace_ریښه;
        کلمه_t level;
        لكل (level = 0; level < UPT_پوړونه - 1 && pt != موخه_pt; level++) {
            pt_خونه = pt + GET_UPT_شاخص(vptr, level);
            إذا (unlikely(!pte_pte_جدول_ptr_get_present(pt_خونه))) {
                /* couldn't find it */
                اكسر;
            }
            pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنسټ_پته(pt_خونه));
        }
        إذا (pt != موخه_pt) {
            /* didn't find it */
            اكسر;
        }


        إذا (asid) {
            چاپ_کړه("pt_%p_%04lu (asid: %lu)\n",
                   موخه_pt, GET_UPT_شاخص(vptr, level), (اوږد بینښه صحيح)asid);
        } وإلا {
            چاپ_کړه("pt_%p_%04lu\n", موخه_pt, GET_UPT_شاخص(vptr, level));
        }
        اكسر;
    }
    حالة cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
        إذا (asid) {
            چاپ_کړه("%p_pd (asid: %lu)\n",
                   پیدا_کړه_ret.vspace_ریښه, (اوږد بینښه صحيح)asid);
        } وإلا {
            چاپ_کړه("%p_pd\n", پیدا_کړه_ret.vspace_ریښه);
        }
        اكسر;
    }
    حالة cap_asid_control_cap: {
        /* only one in the system */
        چاپ_کړه("asid_control\n");
        اكسر;
    }
    حالة cap_چوکاټ_cap: {
        vptr_t vptr = cap_چوکاټ_cap_get_cap_FMapped_پته(cap);
        پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(cap_چوکاټ_cap_get_cap_FMapped_ASID(cap));
        أكد(پیدا_کړه_ret.حالت == EXCEPTION_NONE);
        cap_چوکاټ_print_attrs_vptr_2(vptr, پیدا_کړه_ret.vspace_ریښه);
        اكسر;
    }
    حالة cap_asid_pool_cap: {
        چاپ_کړه("%p_asid_pool\n", (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        اكسر;
    }
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        چاپ_کړه("%p_vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#د_که_پای

        /* ARM specific caps */
#که_تعریف_شوی CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        چاپ_کړه("%p_io_space\n", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        اكسر;
    }
#د_که_پای
    افتراضي: {
        چاپ_کړه("[unknown cap %lu]\n", (اوږد بینښه صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ print_object_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_چوکاټ_cap:
    حالة cap_پاڼه_جدول_cap:
    حالة cap_vspace_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        چاپ_کړه("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        اكسر;
    }
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        چاپ_کړه("%p_vcpu = vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#د_که_پای
        /* ARM specific objects */
#که_تعریف_شوی CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        چاپ_کړه("%p_io_space = io_space ", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        اكسر;
    }
#د_که_پای
    افتراضي: {
        چاپ_کړه("[unknown object %lu]\n", (اوږد بینښه صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ obj_چوکاټ_print_attrs(vm_پاڼه_اندازه_t چوکاټ_اندازه, paddr_t چوکاټ_بنسټ)
{
    چاپ_کړه("(");

    /* VM size */
    بدل (چوکاټ_اندازه) {
    حالة ARMHuge_پاڼه:
        چاپ_کړه("1G");
        اكسر;
    حالة ARMLarge_پاڼه:
        چاپ_کړه("2M");
        اكسر;
    حالة ARMSmall_پاڼه:
        چاپ_کړه("4k");
        اكسر;
    }

    چاپ_کړه(", paddr: 0x%p)\n", (فراغ *)چوکاټ_بنسټ);
}

فراغ arm64_obj_pt_print_slots(pte_t *pd_خونه)
{
    pte_t *pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنسټ_پته(pd_خونه));

    لكل (کلمه_t i = 0; i < بټ(PT_شاخص_بټونه); i++) {
        pte_t *pt_خونه = pt + i;

        إذا (pte_4k_پاڼه_ptr_get_present(pt_خونه)) {
            چاپ_کړه("frame_%p_%04lu = frame ", pt_خونه, i);
            obj_چوکاټ_print_attrs(ARMSmall_پاڼه, pte_پاڼه_ptr_get_پاڼه_بنسټ_پته(pt_خونه));
        }
    }
}

فراغ arm64_obj_pd_print_slots(pte_t *pud_خونه)
{
    pte_t *pd = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنسټ_پته(pud_خونه));

    لكل (کلمه_t i = 0; i < بټ(PT_شاخص_بټونه); i++) {
        pte_t *pd_خونه = pd + i;

        إذا (pte_ptr_get_pte_type(pd_خونه) == pte_pte_پاڼه) {
            چاپ_کړه("frame_%p_%04lu = frame ", pd_خونه, i);
            obj_چوکاټ_print_attrs(ARMLarge_پاڼه, pte_پاڼه_ptr_get_پاڼه_بنسټ_پته(pd_خونه));
        }

        إذا (pte_ptr_get_pte_type(pd_خونه) == pte_pte_جدول) {
            چاپ_کړه("pt_%p_%04lu = pt\n", pd_خونه, i);
            arm64_obj_pt_print_slots(pd_خونه);
        }
    }
}

فراغ arm64_obj_pud_print_slots(فراغ *pgd_خونه_or_vspace)
{
    pte_t *pud = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنسټ_پته(pgd_خونه_or_vspace));
#که_تعریف_شوی AARCH64_VSPACE_S2_پیل_L1
    کلمه_t شاخص_بټونه = se_L4_VSpace_شاخص_بټونه;
#وإلا
    کلمه_t شاخص_بټونه = se_L4_پاڼه_جدول_شاخص_بټونه;
#د_که_پای
    لكل (کلمه_t i = 0; i < بټ(شاخص_بټونه); i++) {
        pte_t *pud_خونه = pud + i;

        بدل (pte_ptr_get_pte_type(pud_خونه)) {
        حالة pte_pte_پاڼه:
            چاپ_کړه("frame_%p_%04lu = frame ", pud_خونه, i);
            obj_چوکاټ_print_attrs(ARMHuge_پاڼه, pte_پاڼه_ptr_get_پاڼه_بنسټ_پته(pud_خونه));
            اكسر;

        حالة pte_pte_جدول: {
            چاپ_کړه("pd_%p_%04lu = pd\n", pud_خونه, i);
            arm64_obj_pd_print_slots(pud_خونه);

        }
        }
    }
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    إذا (is_VTable_ریښه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !کتل_شوی(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جمع_کړه_منزل_کتل_شوی(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_ریښه_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#که_تعریف_شوی AARCH64_VSPACE_S2_پیل_L1
        چاپ_کړه("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#وإلا
        چاپ_کړه("%p_pd = pgd\n", vspace);
        لكل (کلمه_t i = 0; i < PT_شاخص_بټونه; i++) {
            pte_t *pt_خونه = vspace + i;
            إذا (pte_pte_جدول_ptr_get_present(pt_خونه)) {
                چاپ_کړه("pud_%p_%04lu = pud\n", pt_خونه, i);
                arm64_obj_pud_print_slots(pt_خونه);
            }
        }
#د_که_پای
    }
}

#د_که_پای /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
{
    چاپ_کړه("arch aarch64\n");
    چاپ_کړه("objects {\n");
#که_تعریف_شوی CONFIG_PRINTING
    print_objects();
#د_که_پای
    چاپ_کړه("}\n");

    چاپ_کړه("caps {\n");

    /* reset the seen list */
    reset_کتل_شوی_list();

#که_تعریف_شوی CONFIG_PRINTING
    print_caps();
    چاپ_کړه("}\n");

    obj_irq_print_maps();
#د_که_پای /* CONFIG_PRINTING */
}

#د_که_پای /* CONFIG_DEBUG_BUILD */
