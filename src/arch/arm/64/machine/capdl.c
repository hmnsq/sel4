/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD

#شامل <arch/machine/capdl.h>
#شامل <string.h>
#شامل <kernel/cspace.h>

لفظ_t get_tcb_sp(tcb_t *tcb)
{
    واپس tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#اگر_تعریف_شدہ CONFIG_PRINTING

ساکن خالی obj_چوکھٹا_print_attrs(vm_صفحہ_سائز_t چوکھٹا_سائز, paddr_t چوکھٹا_بنیاد);
ساکن خالی cap_چوکھٹا_print_attrs_pt(pte_t *pt_خانہ);
ساکن خالی cap_چوکھٹا_print_attrs_impl(لفظ_t SH, لفظ_t AP, لفظ_t NXN);
ساکن خالی cap_چوکھٹا_print_attrs_vptr(لفظ_t vptr, cap_t vspace);

ساکن خالی cap_چوکھٹا_print_attrs_vptr_2(لفظ_t vptr, vspace_جڑ_t *vspace_جڑ_2);

ساکن خالی arm64_obj_pt_print_slots(pte_t *pd_خانہ);
ساکن خالی arm64_obj_pd_print_slots(pte_t *pud_خانہ);
ساکن خالی arm64_obj_pud_print_slots(خالی *pgd_خانہ_or_vspace);

ساکن خالی arm64_cap_pt_print_slots(pte_t *pd_خانہ, vptr_t vptr);
ساکن خالی arm64_cap_pd_print_slots(pte_t *pud_خانہ, vptr_t vptr);
ساکن خالی arm64_cap_pud_print_slots(خالی *pgd_خانہ_or_vspace, vptr_t vptr);

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
ساکن خالی cap_چوکھٹا_print_attrs_pt(pte_t *pt_خانہ)
{
    cap_چوکھٹا_print_attrs_impl(pte_pte_صفحہ_ptr_get_SH(pt_خانہ),
                               pte_pte_صفحہ_ptr_get_AP(pt_خانہ),
                               pte_pte_صفحہ_ptr_get_UXN(pt_خانہ));
}

ساکن خالی cap_چوکھٹا_print_attrs_impl(لفظ_t SH, لفظ_t AP, لفظ_t NXN)
{
    لکھو("(");

    /* rights */
    انتخاب (AP) {
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت 0b00:
        توڑو;
    حالت 0b01:
        لکھو("R");
        توڑو;
    حالت 0b10:
        لکھو("W");
        توڑو;
    حالت 0b11:
        لکھو("RW");
        توڑو;
#ورنہ
    حالت 0b00:
        توڑو;
    حالت 0b01:
        لکھو("RW");
    حالت 0b10:
        توڑو;
    حالت 0b11:
        لکھو("R");
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    طےشدہ:
        توڑو;
    }

    اگر (!NXN) {
        لکھو("X");
    }

    /* Only has effect if SMP enabled */
    اگر (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        لکھو(", uncached");
    }

    لکھو(")\n");
}

/* use when only have access to vptr of frames */
ساکن خالی cap_چوکھٹا_print_attrs_vptr_2(لفظ_t vptr, vspace_جڑ_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    اگر (!pte_ptr_get_valid(ret.pt_خانہ) ||
        (pte_pte_جدول_ptr_get_present(ret.pt_خانہ) && ret.pt_بٹ_بایاں > صفحہ_بٹ)) {
        أكد(0);
    }

    لفظ_t جدول_اشاریہ;
    انتخاب (ret.pt_بٹ_بایاں) {

    حالت ARMHuge_صفحہ:
        جدول_اشاریہ = GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(1));
        توڑو;
    حالت ARMLarge_صفحہ:
        جدول_اشاریہ = GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(2));
        توڑو;
    حالت ARMSmall_صفحہ:
        جدول_اشاریہ = GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(3));
        توڑو;
    طےشدہ:
        أكد(0);

    }
    لکھو("frame_%p_%04lu ", ret.pt_خانہ, جدول_اشاریہ);
    cap_چوکھٹا_print_attrs_pt(ret.pt_خانہ);
}

خالی cap_چوکھٹا_print_attrs_vptr(لفظ_t vptr, cap_t vspace)
{
    cap_چوکھٹا_print_attrs_vptr_2(vptr, VSPACE_PTR(pptr_کا_cap(vspace)));
}

/*
 * print object slots
 */
ساکن خالی arm64_cap_pt_print_slots(pte_t *pd_خانہ, vptr_t vptr)
{
    pte_t *pt = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_بنیاد_پتہ(pd_خانہ));
    لکھو("pt_%p_%04lu {\n", pd_خانہ, GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(PT_اشاریہ_بٹ); i ++) {
        pte_t *pt_خانہ = pt + i;

        اگر (pte_4k_صفحہ_ptr_get_present(pt_خانہ)) {
            // print pte entries
            لکھو("0x%lx: frame_%p_%04lu", i, pt_خانہ, i);
            cap_چوکھٹا_print_attrs_pt(pt_خانہ);
        }
    }
    لکھو("}\n"); /* pt */
}

ساکن خالی arm64_cap_pd_print_slots(pte_t *pud_خانہ, vptr_t vptr)
{
    لکھو("pd_%p_%04lu {\n", pud_خانہ, GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_بنیاد_پتہ(pud_خانہ));

    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(PT_اشاریہ_بٹ); i++) {
        pte_t *pd_خانہ = pd + i;

        انتخاب (pte_ptr_get_pte_type(pd_خانہ)) {

        حالت pte_pte_صفحہ:
            لکھو("0x%lx: frame_%p_%04lu", i, pd_خانہ, i);
            cap_چوکھٹا_print_attrs_pt(pd_خانہ);
            توڑو;

        حالت pte_pte_جدول:
            لکھو("0x%lx: pt_%p_%04lu\n", i, pd_خانہ, i);
            توڑو;
        }
    }

    لکھو("}\n"); /* pd */

    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(PT_اشاریہ_بٹ); i++) {
        pte_t *pd_خانہ = pd + i;
        اگر (pte_ptr_get_pte_type(pd_خانہ) == pte_pte_جدول) {
            arm64_cap_pt_print_slots(pd_خانہ, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

ساکن خالی arm64_cap_pud_print_slots(خالی *pgd_خانہ_or_vspace, vptr_t vptr)
{
#اگر_تعریف_شدہ AARCH64_VSPACE_S2_آغاز_L1
    pte_t *pud = pgd_خانہ_or_vspace;
    لفظ_t اشاریہ_بٹ = se_L4_VSpace_اشاریہ_بٹ;
    لکھو("%p_pd {\n", pgd_خانہ_or_vspace);
#ورنہ
    pte_t *pud = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_بنیاد_پتہ(pgd_خانہ_or_vspace));
    لفظ_t اشاریہ_بٹ = se_L4_صفحہ_جدول_اشاریہ_بٹ;
    لکھو("pud_%p_%04lu {\n", pgd_خانہ_or_vspace, GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#اگر_ختم

    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(اشاریہ_بٹ); i++) {
        pte_t *pud_خانہ = pud + i;
        اگر (pte_ptr_get_pte_type(pud_خانہ) == pte_pte_جدول) {
            لکھو("0x%lx: pd_%p_%04lu\n", i, pud_خانہ, i);
        }
    }

    لکھو("}\n"); /* pgd/pud */

    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(اشاریہ_بٹ); i++) {
        pte_t *pud_خانہ = pud + GET_UPT_اشاریہ(i, ULVL_FRM_ARM_PT_LVL(1));
        اگر (pte_ptr_get_pte_type(pud_خانہ) == pte_pte_جدول) {
            arm64_cap_pd_print_slots(pud_خانہ, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

خالی obj_vtable_print_slots(tcb_t *tcb)
{
    اگر (is_VTable_جڑ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !نظر_آیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جوڑو_مقصد_نظر_آیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_جڑ_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#اگر_تعریف_شدہ AARCH64_VSPACE_S2_آغاز_L1
        arm64_cap_pud_print_slots(vspace, 0);
#ورنہ
        لکھو("%p_pd {\n", vspace);
        برائے (لفظ_t i = 0; i < PT_اشاریہ_بٹ; i++) {
            pte_t *pt_خانہ = vspace + i;
            اگر (pte_pte_جدول_ptr_get_present(pt_خانہ)) {
                لکھو("0x%lx: pud_%p_%04lu\n", i, pt_خانہ, i);
            }
        }
        لکھو("}\n"); /* pd */

        برائے (لفظ_t i = 0; i < PT_اشاریہ_بٹ; i++) {
            pte_t *pt_خانہ = vspace + i;
            اگر (pte_pte_جدول_ptr_get_present(pt_خانہ)) {
                arm64_cap_pud_print_slots(pt_خانہ, i * GET_ULVL_PGSIZE(0));
            }
        }
#اگر_ختم
    }
}

خالی print_ipc_بفر_خانہ(tcb_t *tcb)
{
    لفظ_t vptr = tcb->tcbIPCBuffer;
    لکھو("ipc_buffer_slot: ");
    cap_چوکھٹا_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

خالی print_cap_arch(cap_t cap)
{

    انتخاب (cap_get_capType(cap)) {
    حالت cap_صفحہ_جدول_cap: {
        asid_t asid = cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(cap);
        vptr_t vptr = cap_صفحہ_جدول_cap_get_cap_PTMapped_پتہ(cap);
        pte_t *ہدف_pt = PT_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap));

        ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
        pte_t *pt_خانہ = تہی;
        pte_t *pt = (pte_t *)ڈھونڈو_ret.vspace_جڑ;
        لفظ_t level;
        برائے (level = 0; level < UPT_سطحیں - 1 && pt != ہدف_pt; level++) {
            pt_خانہ = pt + GET_UPT_اشاریہ(vptr, level);
            اگر (unlikely(!pte_pte_جدول_ptr_get_present(pt_خانہ))) {
                /* couldn't find it */
                توڑو;
            }
            pt = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_بنیاد_پتہ(pt_خانہ));
        }
        اگر (pt != ہدف_pt) {
            /* didn't find it */
            توڑو;
        }


        اگر (asid) {
            لکھو("pt_%p_%04lu (asid: %lu)\n",
                   ہدف_pt, GET_UPT_اشاریہ(vptr, level), (طویل غیرعلامتی صحیح)asid);
        } ورنہ {
            لکھو("pt_%p_%04lu\n", ہدف_pt, GET_UPT_اشاریہ(vptr, level));
        }
        توڑو;
    }
    حالت cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
        اگر (asid) {
            لکھو("%p_pd (asid: %lu)\n",
                   ڈھونڈو_ret.vspace_جڑ, (طویل غیرعلامتی صحیح)asid);
        } ورنہ {
            لکھو("%p_pd\n", ڈھونڈو_ret.vspace_جڑ);
        }
        توڑو;
    }
    حالت cap_asid_control_cap: {
        /* only one in the system */
        لکھو("asid_control\n");
        توڑو;
    }
    حالت cap_چوکھٹا_cap: {
        vptr_t vptr = cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap);
        ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap));
        أكد(ڈھونڈو_ret.حالت_2 == EXCEPTION_NONE);
        cap_چوکھٹا_print_attrs_vptr_2(vptr, ڈھونڈو_ret.vspace_جڑ);
        توڑو;
    }
    حالت cap_asid_pool_cap: {
        لکھو("%p_asid_pool\n", (خالی *)cap_asid_pool_cap_get_capASIDPool(cap));
        توڑو;
    }
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap: {
        لکھو("%p_vcpu\n", (خالی *)cap_vcpu_cap_get_capVCPUPtr(cap));
        توڑو;
    }
#اگر_ختم

        /* ARM specific caps */
#اگر_تعریف_شدہ CONFIG_TK1_SMMU
    حالت cap_io_space_cap: {
        لکھو("%p_io_space\n", (خالی *)cap_io_space_cap_get_capModuleID(cap));
        توڑو;
    }
#اگر_ختم
    طےشدہ: {
        لکھو("[unknown cap %lu]\n", (طویل غیرعلامتی صحیح)cap_get_capType(cap));
        توڑو;
    }
    }
}

خالی print_object_arch(cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_چوکھٹا_cap:
    حالت cap_صفحہ_جدول_cap:
    حالت cap_vspace_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        توڑو;

    حالت cap_asid_pool_cap: {
        لکھو("%p_asid_pool = asid_pool ",
               (خالی *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        توڑو;
    }
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap: {
        لکھو("%p_vcpu = vcpu\n", (خالی *)cap_vcpu_cap_get_capVCPUPtr(cap));
        توڑو;
    }
#اگر_ختم
        /* ARM specific objects */
#اگر_تعریف_شدہ CONFIG_TK1_SMMU
    حالت cap_io_space_cap: {
        لکھو("%p_io_space = io_space ", (خالی *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        توڑو;
    }
#اگر_ختم
    طےشدہ: {
        لکھو("[unknown object %lu]\n", (طویل غیرعلامتی صحیح)cap_get_capType(cap));
        توڑو;
    }
    }
}

خالی obj_چوکھٹا_print_attrs(vm_صفحہ_سائز_t چوکھٹا_سائز, paddr_t چوکھٹا_بنیاد)
{
    لکھو("(");

    /* VM size */
    انتخاب (چوکھٹا_سائز) {
    حالت ARMHuge_صفحہ:
        لکھو("1G");
        توڑو;
    حالت ARMLarge_صفحہ:
        لکھو("2M");
        توڑو;
    حالت ARMSmall_صفحہ:
        لکھو("4k");
        توڑو;
    }

    لکھو(", paddr: 0x%p)\n", (خالی *)چوکھٹا_بنیاد);
}

خالی arm64_obj_pt_print_slots(pte_t *pd_خانہ)
{
    pte_t *pt = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_بنیاد_پتہ(pd_خانہ));

    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(PT_اشاریہ_بٹ); i++) {
        pte_t *pt_خانہ = pt + i;

        اگر (pte_4k_صفحہ_ptr_get_present(pt_خانہ)) {
            لکھو("frame_%p_%04lu = frame ", pt_خانہ, i);
            obj_چوکھٹا_print_attrs(ARMSmall_صفحہ, pte_صفحہ_ptr_get_صفحہ_بنیاد_پتہ(pt_خانہ));
        }
    }
}

خالی arm64_obj_pd_print_slots(pte_t *pud_خانہ)
{
    pte_t *pd = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_بنیاد_پتہ(pud_خانہ));

    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(PT_اشاریہ_بٹ); i++) {
        pte_t *pd_خانہ = pd + i;

        اگر (pte_ptr_get_pte_type(pd_خانہ) == pte_pte_صفحہ) {
            لکھو("frame_%p_%04lu = frame ", pd_خانہ, i);
            obj_چوکھٹا_print_attrs(ARMLarge_صفحہ, pte_صفحہ_ptr_get_صفحہ_بنیاد_پتہ(pd_خانہ));
        }

        اگر (pte_ptr_get_pte_type(pd_خانہ) == pte_pte_جدول) {
            لکھو("pt_%p_%04lu = pt\n", pd_خانہ, i);
            arm64_obj_pt_print_slots(pd_خانہ);
        }
    }
}

خالی arm64_obj_pud_print_slots(خالی *pgd_خانہ_or_vspace)
{
    pte_t *pud = paddr_مقصد_pptr(pte_pte_جدول_ptr_get_pt_بنیاد_پتہ(pgd_خانہ_or_vspace));
#اگر_تعریف_شدہ AARCH64_VSPACE_S2_آغاز_L1
    لفظ_t اشاریہ_بٹ = se_L4_VSpace_اشاریہ_بٹ;
#ورنہ
    لفظ_t اشاریہ_بٹ = se_L4_صفحہ_جدول_اشاریہ_بٹ;
#اگر_ختم
    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(اشاریہ_بٹ); i++) {
        pte_t *pud_خانہ = pud + i;

        انتخاب (pte_ptr_get_pte_type(pud_خانہ)) {
        حالت pte_pte_صفحہ:
            لکھو("frame_%p_%04lu = frame ", pud_خانہ, i);
            obj_چوکھٹا_print_attrs(ARMHuge_صفحہ, pte_صفحہ_ptr_get_صفحہ_بنیاد_پتہ(pud_خانہ));
            توڑو;

        حالت pte_pte_جدول: {
            لکھو("pd_%p_%04lu = pd\n", pud_خانہ, i);
            arm64_obj_pd_print_slots(pud_خانہ);

        }
        }
    }
}

خالی obj_tcb_print_vtable(tcb_t *tcb)
{
    اگر (is_VTable_جڑ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !نظر_آیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جوڑو_مقصد_نظر_آیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_جڑ_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#اگر_تعریف_شدہ AARCH64_VSPACE_S2_آغاز_L1
        لکھو("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#ورنہ
        لکھو("%p_pd = pgd\n", vspace);
        برائے (لفظ_t i = 0; i < PT_اشاریہ_بٹ; i++) {
            pte_t *pt_خانہ = vspace + i;
            اگر (pte_pte_جدول_ptr_get_present(pt_خانہ)) {
                لکھو("pud_%p_%04lu = pud\n", pt_خانہ, i);
                arm64_obj_pud_print_slots(pt_خانہ);
            }
        }
#اگر_ختم
    }
}

#اگر_ختم /* CONFIG_PRINTING */

خالی debug_capDL(خالی)
{
    لکھو("arch aarch64\n");
    لکھو("objects {\n");
#اگر_تعریف_شدہ CONFIG_PRINTING
    print_objects();
#اگر_ختم
    لکھو("}\n");

    لکھو("caps {\n");

    /* reset the seen list */
    reset_نظر_آیا_list();

#اگر_تعریف_شدہ CONFIG_PRINTING
    print_caps();
    لکھو("}\n");

    obj_irq_print_maps();
#اگر_ختم /* CONFIG_PRINTING */
}

#اگر_ختم /* CONFIG_DEBUG_BUILD */
