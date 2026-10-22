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
    واپس tcb->tcbArch.tcbContext.registers[RSP];
}

#اگر_تعریف_شدہ CONFIG_PRINTING

ساکن خالی obj_چوکھٹا_print_attrs(paddr_t paddr, لفظ_t صفحہ_سائز);
ساکن خالی cap_چوکھٹا_print_attrs_vptr_2(لفظ_t vptr, vspace_جڑ_t *vspace);
ساکن خالی cap_چوکھٹا_print_attrs_vptr(لفظ_t vptr, cap_t vspace);
ساکن خالی obj_asidpool_print_attrs(cap_t asid_cap);
ساکن خالی cap_چوکھٹا_print_attrs_pdpt(pdpte_t *pdpt_خانہ);
ساکن خالی cap_چوکھٹا_print_attrs_pd(pde_t *pd_خانہ);
ساکن خالی cap_چوکھٹا_print_attrs_pt(pte_t *pt_خانہ);
ساکن خالی cap_چوکھٹا_print_attrs_impl(لفظ_t super_user, لفظ_t read_write, لفظ_t cache_disabled, لفظ_t xd);

/* use when only have access to pte of frames */
ساکن خالی cap_چوکھٹا_print_attrs_pdpt(pdpte_t *pdpt_خانہ)
{
    cap_چوکھٹا_print_attrs_impl(pdpte_pdpte_1g_ptr_get_super_user(pdpt_خانہ),
                               pdpte_pdpte_1g_ptr_get_read_write(pdpt_خانہ),
                               pdpte_pdpte_1g_ptr_get_cache_disabled(pdpt_خانہ),
                               pdpte_pdpte_1g_ptr_get_xd(pdpt_خانہ));
}
ساکن خالی cap_چوکھٹا_print_attrs_pd(pde_t *pd_خانہ)
{
    cap_چوکھٹا_print_attrs_impl(pde_pde_large_ptr_get_super_user(pd_خانہ),
                               pde_pde_large_ptr_get_read_write(pd_خانہ),
                               pde_pde_large_ptr_get_cache_disabled(pd_خانہ),
                               pde_pde_large_ptr_get_xd(pd_خانہ));
}

ساکن خالی cap_چوکھٹا_print_attrs_pt(pte_t *pt_خانہ)
{
    cap_چوکھٹا_print_attrs_impl(pte_ptr_get_super_user(pt_خانہ),
                               pte_ptr_get_read_write(pt_خانہ),
                               pte_ptr_get_cache_disabled(pt_خانہ),
                               pte_ptr_get_xd(pt_خانہ));
}

ساکن خالی cap_چوکھٹا_print_attrs_impl(لفظ_t super_user, لفظ_t read_write, لفظ_t cache_disabled, لفظ_t xd)
{
    لکھو("(");

    /* rights */
    اگر (read_write) {
        لکھو("RW");
    } ورنہ اگر (super_user) {
        لکھو("R");
    }

    اگر (!xd) {
        لکھو("X");
    }

    /* asid, mapping */

    اگر (cache_disabled) {
        لکھو(", uncached");
    }

    لکھو(")\n");
}

ساکن خالی obj_چوکھٹا_print_attrs(paddr_t paddr, لفظ_t صفحہ_سائز)
{
    لکھو("(");

    /* VM size */
    انتخاب (صفحہ_سائز) {
    حالت se_L4_Huge_صفحہ_بٹ:
        لکھو("1G");
        توڑو;
    حالت se_L4_Large_صفحہ_بٹ:
        لکھو("2M");
        توڑو;
    حالت se_L4_صفحہ_بٹ:
        لکھو("4k");
        توڑو;
    }

    لکھو(", paddr: 0x%p)\n", (خالی *)paddr);
}

ساکن خالی x86_64_obj_pt_print_slots(pde_t *pd_خانہ)
{
    paddr_t paddr;
    لفظ_t صفحہ_سائز;
    pte_t *pt = paddr_مقصد_pptr(pde_pde_pt_ptr_get_pt_بنیاد_پتہ(pd_خانہ));

    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(PT_اشاریہ_انحراف + PT_اشاریہ_بٹ); i += (1UL << PT_اشاریہ_انحراف)) {
        pte_t *pt_خانہ = pt + GET_PT_اشاریہ(i);

        اگر (pte_ptr_get_present(pt_خانہ)) {
            paddr = pte_ptr_get_صفحہ_بنیاد_پتہ(pt_خانہ);
            صفحہ_سائز = se_L4_صفحہ_بٹ;
            لکھو("frame_%p_%04lu = frame ", pt_خانہ, GET_PT_اشاریہ(i));
            obj_چوکھٹا_print_attrs(paddr, صفحہ_سائز);
        }
    }
}

ساکن خالی x86_64_obj_pd_print_slots(pdpte_t *pdpt_خانہ)
{
    paddr_t paddr;
    لفظ_t صفحہ_سائز;
    pde_t *pd = paddr_مقصد_pptr(pdpte_pdpte_pd_ptr_get_pd_بنیاد_پتہ(pdpt_خانہ));

    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(PD_اشاریہ_انحراف + PD_اشاریہ_بٹ); i += (1UL << PD_اشاریہ_انحراف)) {
        pde_t *pd_خانہ = pd + GET_PD_اشاریہ(i);

        اگر ((pde_ptr_get_صفحہ_سائز(pd_خانہ) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_خانہ)) {
            paddr = pde_pde_large_ptr_get_صفحہ_بنیاد_پتہ(pd_خانہ);
            صفحہ_سائز = se_L4_Large_صفحہ_بٹ;

            لکھو("frame_%p_%04lu = frame ", pd_خانہ, GET_PD_اشاریہ(i));
            obj_چوکھٹا_print_attrs(paddr, صفحہ_سائز);

        } ورنہ اگر (pde_pde_pt_ptr_get_present(pd_خانہ)) {
            لکھو("pt_%p_%04lu = pt\n", pd_خانہ, GET_PD_اشاریہ(i));
            x86_64_obj_pt_print_slots(pd_خانہ);
        }
    }
}

ساکن خالی x86_64_obj_pdpt_print_slots(pml4e_t *pml4_خانہ)
{
    paddr_t paddr;
    لفظ_t صفحہ_سائز;
    pdpte_t *pdpt = paddr_مقصد_pptr(pml4e_ptr_get_pdpt_بنیاد_پتہ(pml4_خانہ));

    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(PDPT_اشاریہ_انحراف + PDPT_اشاریہ_بٹ); i += (1UL << PDPT_اشاریہ_انحراف)) {
        pdpte_t *pdpt_خانہ = pdpt + GET_PDPT_اشاریہ(i);

        اگر (pdpte_ptr_get_صفحہ_سائز(pdpt_خانہ) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdpt_خانہ)) {
            paddr = pdpte_pdpte_1g_ptr_get_صفحہ_بنیاد_پتہ(pdpt_خانہ);
            صفحہ_سائز = se_L4_Huge_صفحہ_بٹ;

            لکھو("frame_%p_%04lu = frame ", pdpt_خانہ, GET_PDPT_اشاریہ(i));
            obj_چوکھٹا_print_attrs(paddr, صفحہ_سائز);

        } ورنہ اگر (pdpte_pdpte_pd_ptr_get_present(pdpt_خانہ)) {
            لکھو("pd_%p_%04lu = pd\n", pdpt_خانہ, GET_PDPT_اشاریہ(i));
            x86_64_obj_pd_print_slots(pdpt_خانہ);
        }
    }
}

ساکن خالی x86_64_obj_pml4_print_slots(pml4e_t *pml4)
{
    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(PML4_اشاریہ_انحراف + PML4_اشاریہ_بٹ); i += (1UL << PML4_اشاریہ_انحراف)) {
        pml4e_t *pml4_خانہ = lookup_PML4_خانہ(pml4, i);
        اگر (pml4e_ptr_get_present(pml4_خانہ)) {
            لکھو("pdpt_%p_%04lu = pdpt\n", pml4_خانہ, GET_PML4_اشاریہ(i));
            x86_64_obj_pdpt_print_slots(pml4_خانہ);
        }
    }
}

خالی obj_tcb_print_vtable(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    اگر (is_Valid_VTable_جڑ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !نظر_آیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_بنیاد_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        جوڑو_مقصد_نظر_آیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        لکھو("%p_pd = pml4\n", pml4);
        x86_64_obj_pml4_print_slots(pml4);
    }
}

/* use when only have access to vptr of frames */
ساکن خالی cap_چوکھٹا_print_attrs_vptr_2(لفظ_t vptr, vspace_جڑ_t *vspace)
{
    lookupPDPTSlot_ret_t pdpt_خانہ = lookupPDPTSlot(vspace, vptr);
    lookupPTSlot_ret_t pt_خانہ;
    lookupPDSlot_ret_t pd_خانہ;

    اگر (pdpt_خانہ.حالت_2 == EXCEPTION_NONE &&
        pdpte_ptr_get_صفحہ_سائز(pdpt_خانہ.pdpt_خانہ) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdpt_خانہ.pdpt_خانہ)) {
        لکھو("frame_%p_%04lu ", pdpt_خانہ.pdpt_خانہ, GET_PDPT_اشاریہ(vptr));
        cap_چوکھٹا_print_attrs_pdpt(pdpt_خانہ.pdpt_خانہ);
    } ورنہ {
        pd_خانہ = lookupPDSlot(vspace, vptr);
        اگر (pd_خانہ.حالت_2 == EXCEPTION_NONE &&
            ((pde_ptr_get_صفحہ_سائز(pd_خانہ.pd_خانہ) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pd_خانہ.pd_خانہ))) {
            لکھو("frame_%p_%04lu ", pd_خانہ.pd_خانہ, GET_PD_اشاریہ(vptr));
            cap_چوکھٹا_print_attrs_pd(pd_خانہ.pd_خانہ);
        } ورنہ {
            pt_خانہ = lookupPTSlot(vspace, vptr);
            أكد(pt_خانہ.حالت_2 == EXCEPTION_NONE && pte_ptr_get_present(pt_خانہ.pt_خانہ));
            لکھو("frame_%p_%04lu ", pt_خانہ.pt_خانہ, GET_PT_اشاریہ(vptr));
            cap_چوکھٹا_print_attrs_pt(pt_خانہ.pt_خانہ);
        }
    }
}

ساکن خالی cap_چوکھٹا_print_attrs_vptr(لفظ_t vptr, cap_t vspace)
{
    asid_t asid = cap_pml4_cap_get_capPML4MappedASID(vspace);
    ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
    cap_چوکھٹا_print_attrs_vptr_2(vptr, ڈھونڈو_ret.vspace_جڑ);
}

خالی print_cap_arch(cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    /* arch specific caps */
    حالت cap_صفحہ_جدول_cap: {
        asid_t asid = cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(cap);
        ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
        vptr_t vptr = cap_صفحہ_جدول_cap_get_cap_PTMapped_پتہ(cap);
        اگر (asid) {
            لکھو("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(ڈھونڈو_ret.vspace_جڑ, vptr).pd_خانہ, GET_PD_اشاریہ(vptr), (طویل غیرعلامتی صحیح)asid);
        } ورنہ {
            لکھو("pt_%p_%04lu\n", lookupPDSlot(ڈھونڈو_ret.vspace_جڑ, vptr).pd_خانہ, GET_PD_اشاریہ(vptr));
        }
        توڑو;
    }
    حالت cap_صفحہ_directory_cap: {
        asid_t asid = cap_صفحہ_directory_cap_get_cap_PDMapped_ASID(cap);
        ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
        vptr_t vptr = cap_صفحہ_directory_cap_get_cap_PDMapped_پتہ(cap);
        اگر (asid) {
            لکھو("pd_%p_%04lu (asid: %lu)\n",
                   lookupPDPTSlot(ڈھونڈو_ret.vspace_جڑ, vptr).pdpt_خانہ, GET_PDPT_اشاریہ(vptr), (طویل غیرعلامتی صحیح)asid);
        } ورنہ {
            لکھو("pd_%p_%04lu\n", lookupPDPTSlot(ڈھونڈو_ret.vspace_جڑ, vptr).pdpt_خانہ, GET_PDPT_اشاریہ(vptr));
        }
        توڑو;
    }
    حالت cap_pdpt_cap: {
        asid_t asid = cap_pdpt_cap_get_capPDPTMappedASID(cap);
        ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
        vptr_t vptr = cap_pdpt_cap_get_cap_PDPTMapped_پتہ(cap);
        اگر (asid) {
            لکھو("pdpt_%p_%04lu (asid: %lu)\n",
                   lookup_PML4_خانہ(ڈھونڈو_ret.vspace_جڑ, vptr), GET_PML4_اشاریہ(vptr), (طویل غیرعلامتی صحیح)asid);
        } ورنہ {
            لکھو("pdpt_%p_%04lu\n", lookup_PML4_خانہ(ڈھونڈو_ret.vspace_جڑ, vptr), GET_PML4_اشاریہ(vptr));
        }
        توڑو;
    }
    حالت cap_pml4_cap: {
        asid_t asid = cap_pml4_cap_get_capPML4MappedASID(cap);
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
#اگر_تعریف_شدہ CONFIG_VTX
    حالت cap_vcpu_cap: {
        لکھو("%p_vcpu\n", (خالی *)cap_vcpu_cap_get_capVCPUPtr(cap));
        توڑو;
    }
#اگر_ختم
    /* X86 specific caps */
    حالت cap_io_پورٹ_cap: {
        لکھو("%p%p_io_port\n", (خالی *)cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(cap),
               (خالی *)cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(cap));
        توڑو;
    }
#اگر_تعریف_شدہ CONFIG_IOMMU
    حالت cap_io_space_cap: {
        لکھو("%p_io_space\n", (خالی *)cap_io_space_cap_get_capPCIDevice(cap));
        توڑو;
    }
    حالت cap_io_صفحہ_جدول_cap: {
        لکھو("%p_iopt\n", (خالی *)cap_io_صفحہ_جدول_cap_get_cap_IOPTBase_Ptr(cap));
        توڑو;
    }
#اگر_ختم
    طےشدہ: {
        لکھو("[unknown cap %lu]\n", (طویل غیرعلامتی صحیح)cap_get_capType(cap));
        توڑو;
    }
    }
}

ساکن خالی obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    لکھو("(asid_high: 0x%lx)\n", ASID_بلند(asid));
}

خالی print_object_arch(cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    /* arch specific objects */
    حالت cap_چوکھٹا_cap:
    حالت cap_صفحہ_جدول_cap:
    حالت cap_صفحہ_directory_cap:
    حالت cap_pdpt_cap:
    حالت cap_pml4_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        توڑو;

    حالت cap_asid_pool_cap: {
        لکھو("%p_asid_pool = asid_pool ",
               (خالی *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        توڑو;
    }
#اگر_تعریف_شدہ CONFIG_VTX
    حالت cap_vcpu_cap: {
        لکھو("%p_vcpu = vcpu\n", (خالی *)cap_vcpu_cap_get_capVCPUPtr(cap));
        توڑو;
    }
#اگر_ختم
    /* X86 specific caps */
    حالت cap_io_پورٹ_cap: {
        لکھو("%p%p_io_port = io_port ",
               (خالی *)cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(cap),
               (خالی *)cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(cap));
        x86_obj_ioports_print_attrs(cap);
        توڑو;
    }
#اگر_تعریف_شدہ CONFIG_IOMMU
    حالت cap_io_space_cap: {
        لکھو("%p_io_space = io_space ", (خالی *)cap_io_space_cap_get_capPCIDevice(cap));
        x86_obj_iospace_print_attrs(cap);
        توڑو;
    }
    حالت cap_io_صفحہ_جدول_cap: {
        لکھو("%p_iopt = iopt ", (خالی *)cap_io_صفحہ_جدول_cap_get_cap_IOPTBase_Ptr(cap));
        x86_obj_iopt_print_attrs(cap);
        توڑو;
    }
#اگر_ختم
    طےشدہ: {
        لکھو("[unknown object %lu]\n", (طویل غیرعلامتی صحیح)cap_get_capType(cap));
        توڑو;
    }
    }
}

خالی print_ipc_بفر_خانہ(tcb_t *tcb)
{
    لفظ_t vptr = tcb->tcbIPCBuffer;
    لکھو("ipc_buffer_slot: ");
    cap_چوکھٹا_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

ساکن خالی x86_64_cap_pt_print_slots(pde_t *pd_خانہ, vptr_t vptr)
{
    pte_t *pt = paddr_مقصد_pptr(pde_pde_pt_ptr_get_pt_بنیاد_پتہ(pd_خانہ));
    لکھو("pt_%p_%04lu {\n", pd_خانہ, GET_PD_اشاریہ(vptr));

    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(PT_اشاریہ_انحراف + PT_اشاریہ_بٹ); i += (1UL << PT_اشاریہ_انحراف)) {
        pte_t *pt_خانہ = pt + GET_PT_اشاریہ(i);

        اگر (pte_ptr_get_present(pt_خانہ)) {
            لکھو("0x%lx: frame_%p_%04lu ", GET_PT_اشاریہ(i), pt_خانہ, GET_PT_اشاریہ(i));
            cap_چوکھٹا_print_attrs_pt(pt_خانہ);
        }
    }
    لکھو("}\n"); /* pt */
}

ساکن خالی x86_64_cap_pd_print_slots(pdpte_t *pdpt_خانہ, vptr_t vptr)
{
    pde_t *pd = paddr_مقصد_pptr(pdpte_pdpte_pd_ptr_get_pd_بنیاد_پتہ(pdpt_خانہ));
    لکھو("pd_%p_%04lu {\n", pdpt_خانہ, GET_PDPT_اشاریہ(vptr));

    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(PD_اشاریہ_انحراف + PD_اشاریہ_بٹ); i += (1UL << PD_اشاریہ_انحراف)) {
        pde_t *pd_خانہ = pd + GET_PD_اشاریہ(i);

        اگر ((pde_ptr_get_صفحہ_سائز(pd_خانہ) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_خانہ)) {
            لکھو("0x%lx: frame_%p_%04lu ", GET_PD_اشاریہ(i), pd_خانہ, GET_PD_اشاریہ(i));
            cap_چوکھٹا_print_attrs_pd(pd_خانہ);

        } ورنہ اگر (pde_pde_pt_ptr_get_present(pd_خانہ)) {
            لکھو("0x%lx: pt_%p_%04lu\n", GET_PD_اشاریہ(i), pd_خانہ, GET_PD_اشاریہ(i));
        }
    }
    لکھو("}\n"); /* pd */

    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(PD_اشاریہ_انحراف + PD_اشاریہ_بٹ); i += (1UL << PD_اشاریہ_انحراف)) {
        pde_t *pd_خانہ = pd + GET_PD_اشاریہ(i);
        اگر ((pde_ptr_get_صفحہ_سائز(pd_خانہ) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pd_خانہ)) {
            x86_64_cap_pt_print_slots(pd_خانہ, i);
        }
    }
}

ساکن خالی x86_64_cap_pdpt_print_slots(pml4e_t *pml4_خانہ, vptr_t vptr)
{
    pdpte_t *pdpt = paddr_مقصد_pptr(pml4e_ptr_get_pdpt_بنیاد_پتہ(pml4_خانہ));

    لکھو("pdpt_%p_%04lu {\n", pml4_خانہ, GET_PML4_اشاریہ(vptr));
    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(PDPT_اشاریہ_انحراف + PDPT_اشاریہ_بٹ); i += (1UL << PDPT_اشاریہ_انحراف)) {
        pdpte_t *pdpt_خانہ = pdpt + GET_PDPT_اشاریہ(i);

        اگر (pdpte_ptr_get_صفحہ_سائز(pdpt_خانہ) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdpt_خانہ)) {
            لکھو("0x%lx: frame_%p_%04lu ", GET_PDPT_اشاریہ(i), pdpt_خانہ, GET_PDPT_اشاریہ(i));
            cap_چوکھٹا_print_attrs_pdpt(pdpt_خانہ);

        } ورنہ اگر (pdpte_pdpte_pd_ptr_get_present(pdpt_خانہ)) {
            لکھو("0x%lx: pd_%p_%04lu\n", GET_PDPT_اشاریہ(i), pdpt_خانہ, GET_PDPT_اشاریہ(i));
        }
    }
    لکھو("}\n"); /* pdpt */

    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(PDPT_اشاریہ_انحراف + PDPT_اشاریہ_بٹ); i += (1UL << PDPT_اشاریہ_انحراف)) {
        pdpte_t *pdpt_خانہ = pdpt + GET_PDPT_اشاریہ(i);

        اگر (pdpte_ptr_get_صفحہ_سائز(pdpt_خانہ) == pdpte_pdpte_pd && pdpte_pdpte_pd_ptr_get_present(pdpt_خانہ)) {
            x86_64_cap_pd_print_slots(pdpt_خانہ, i);
        }
    }
}

ساکن خالی x86_64_cap_pml4_print_slots(pml4e_t *pml4)
{
    لکھو("%p_pd {\n", pml4);
    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(PML4_اشاریہ_انحراف + PML4_اشاریہ_بٹ); i += (1UL << PML4_اشاریہ_انحراف)) {
        pml4e_t *pml4_خانہ = lookup_PML4_خانہ(pml4, i);
        اگر (pml4e_ptr_get_present(pml4_خانہ)) {
            لکھو("0x%lx: pdpt_%p_%04lu\n", GET_PML4_اشاریہ(i), pml4_خانہ, GET_PML4_اشاریہ(i));
        }
    }
    لکھو("}\n"); /* pml4 */

    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(PML4_اشاریہ_انحراف + PML4_اشاریہ_بٹ); i += (1UL << PML4_اشاریہ_انحراف)) {
        pml4e_t *pml4_خانہ = lookup_PML4_خانہ(pml4, i);
        اگر (pml4e_ptr_get_present(pml4_خانہ)) {
            x86_64_cap_pdpt_print_slots(pml4_خانہ, i);
        }
    }
}

خالی obj_vtable_print_slots(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    اگر (is_Valid_VTable_جڑ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !نظر_آیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_بنیاد_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        جوڑو_مقصد_نظر_آیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        x86_64_cap_pml4_print_slots(pml4);
    }
}

#اگر_ختم /* CONFIG_PRINTING */

خالی debug_capDL(خالی)
{
    لکھو("arch x86_64\n");
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
#اگر_ختم
}

#اگر_ختم /* CONFIG_DEBUG_BUILD */
