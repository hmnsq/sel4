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
    أرجع tcb->tcbArch.tcbContext.registers[RSP];
}

#که_تعریف_شوی CONFIG_PRINTING

ساكن فراغ obj_چوکاټ_print_attrs(paddr_t paddr, کلمه_t پاڼه_اندازه);
ساكن فراغ cap_چوکاټ_print_attrs_vptr_2(کلمه_t vptr, vspace_ریښه_t *vspace);
ساكن فراغ cap_چوکاټ_print_attrs_vptr(کلمه_t vptr, cap_t vspace);
ساكن فراغ obj_asidpool_print_attrs(cap_t asid_cap);
ساكن فراغ cap_چوکاټ_print_attrs_pdpt(pdpte_t *pdpt_خونه);
ساكن فراغ cap_چوکاټ_print_attrs_pd(pde_t *pd_خونه);
ساكن فراغ cap_چوکاټ_print_attrs_pt(pte_t *pt_خونه);
ساكن فراغ cap_چوکاټ_print_attrs_impl(کلمه_t super_user, کلمه_t read_write, کلمه_t cache_disabled, کلمه_t xd);

/* use when only have access to pte of frames */
ساكن فراغ cap_چوکاټ_print_attrs_pdpt(pdpte_t *pdpt_خونه)
{
    cap_چوکاټ_print_attrs_impl(pdpte_pdpte_1g_ptr_get_super_user(pdpt_خونه),
                               pdpte_pdpte_1g_ptr_get_read_write(pdpt_خونه),
                               pdpte_pdpte_1g_ptr_get_cache_disabled(pdpt_خونه),
                               pdpte_pdpte_1g_ptr_get_xd(pdpt_خونه));
}
ساكن فراغ cap_چوکاټ_print_attrs_pd(pde_t *pd_خونه)
{
    cap_چوکاټ_print_attrs_impl(pde_pde_large_ptr_get_super_user(pd_خونه),
                               pde_pde_large_ptr_get_read_write(pd_خونه),
                               pde_pde_large_ptr_get_cache_disabled(pd_خونه),
                               pde_pde_large_ptr_get_xd(pd_خونه));
}

ساكن فراغ cap_چوکاټ_print_attrs_pt(pte_t *pt_خونه)
{
    cap_چوکاټ_print_attrs_impl(pte_ptr_get_super_user(pt_خونه),
                               pte_ptr_get_read_write(pt_خونه),
                               pte_ptr_get_cache_disabled(pt_خونه),
                               pte_ptr_get_xd(pt_خونه));
}

ساكن فراغ cap_چوکاټ_print_attrs_impl(کلمه_t super_user, کلمه_t read_write, کلمه_t cache_disabled, کلمه_t xd)
{
    چاپ_کړه("(");

    /* rights */
    إذا (read_write) {
        چاپ_کړه("RW");
    } وإلا إذا (super_user) {
        چاپ_کړه("R");
    }

    إذا (!xd) {
        چاپ_کړه("X");
    }

    /* asid, mapping */

    إذا (cache_disabled) {
        چاپ_کړه(", uncached");
    }

    چاپ_کړه(")\n");
}

ساكن فراغ obj_چوکاټ_print_attrs(paddr_t paddr, کلمه_t پاڼه_اندازه)
{
    چاپ_کړه("(");

    /* VM size */
    بدل (پاڼه_اندازه) {
    حالة se_L4_Huge_پاڼه_بټونه:
        چاپ_کړه("1G");
        اكسر;
    حالة se_L4_Large_پاڼه_بټونه:
        چاپ_کړه("2M");
        اكسر;
    حالة se_L4_پاڼه_بټونه:
        چاپ_کړه("4k");
        اكسر;
    }

    چاپ_کړه(", paddr: 0x%p)\n", (فراغ *)paddr);
}

ساكن فراغ x86_64_obj_pt_print_slots(pde_t *pd_خونه)
{
    paddr_t paddr;
    کلمه_t پاڼه_اندازه;
    pte_t *pt = paddr_منزل_pptr(pde_pde_pt_ptr_get_pt_بنسټ_پته(pd_خونه));

    لكل (کلمه_t i = 0; i < بټ(PT_شاخص_دننه_واټن + PT_شاخص_بټونه); i += (1UL << PT_شاخص_دننه_واټن)) {
        pte_t *pt_خونه = pt + GET_PT_شاخص(i);

        إذا (pte_ptr_get_present(pt_خونه)) {
            paddr = pte_ptr_get_پاڼه_بنسټ_پته(pt_خونه);
            پاڼه_اندازه = se_L4_پاڼه_بټونه;
            چاپ_کړه("frame_%p_%04lu = frame ", pt_خونه, GET_PT_شاخص(i));
            obj_چوکاټ_print_attrs(paddr, پاڼه_اندازه);
        }
    }
}

ساكن فراغ x86_64_obj_pd_print_slots(pdpte_t *pdpt_خونه)
{
    paddr_t paddr;
    کلمه_t پاڼه_اندازه;
    pde_t *pd = paddr_منزل_pptr(pdpte_pdpte_pd_ptr_get_pd_بنسټ_پته(pdpt_خونه));

    لكل (کلمه_t i = 0; i < بټ(PD_شاخص_دننه_واټن + PD_شاخص_بټونه); i += (1UL << PD_شاخص_دننه_واټن)) {
        pde_t *pd_خونه = pd + GET_PD_شاخص(i);

        إذا ((pde_ptr_get_پاڼه_اندازه(pd_خونه) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_خونه)) {
            paddr = pde_pde_large_ptr_get_پاڼه_بنسټ_پته(pd_خونه);
            پاڼه_اندازه = se_L4_Large_پاڼه_بټونه;

            چاپ_کړه("frame_%p_%04lu = frame ", pd_خونه, GET_PD_شاخص(i));
            obj_چوکاټ_print_attrs(paddr, پاڼه_اندازه);

        } وإلا إذا (pde_pde_pt_ptr_get_present(pd_خونه)) {
            چاپ_کړه("pt_%p_%04lu = pt\n", pd_خونه, GET_PD_شاخص(i));
            x86_64_obj_pt_print_slots(pd_خونه);
        }
    }
}

ساكن فراغ x86_64_obj_pdpt_print_slots(pml4e_t *pml4_خونه)
{
    paddr_t paddr;
    کلمه_t پاڼه_اندازه;
    pdpte_t *pdpt = paddr_منزل_pptr(pml4e_ptr_get_pdpt_بنسټ_پته(pml4_خونه));

    لكل (کلمه_t i = 0; i < بټ(PDPT_شاخص_دننه_واټن + PDPT_شاخص_بټونه); i += (1UL << PDPT_شاخص_دننه_واټن)) {
        pdpte_t *pdpt_خونه = pdpt + GET_PDPT_شاخص(i);

        إذا (pdpte_ptr_get_پاڼه_اندازه(pdpt_خونه) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdpt_خونه)) {
            paddr = pdpte_pdpte_1g_ptr_get_پاڼه_بنسټ_پته(pdpt_خونه);
            پاڼه_اندازه = se_L4_Huge_پاڼه_بټونه;

            چاپ_کړه("frame_%p_%04lu = frame ", pdpt_خونه, GET_PDPT_شاخص(i));
            obj_چوکاټ_print_attrs(paddr, پاڼه_اندازه);

        } وإلا إذا (pdpte_pdpte_pd_ptr_get_present(pdpt_خونه)) {
            چاپ_کړه("pd_%p_%04lu = pd\n", pdpt_خونه, GET_PDPT_شاخص(i));
            x86_64_obj_pd_print_slots(pdpt_خونه);
        }
    }
}

ساكن فراغ x86_64_obj_pml4_print_slots(pml4e_t *pml4)
{
    لكل (کلمه_t i = 0; i < بټ(PML4_شاخص_دننه_واټن + PML4_شاخص_بټونه); i += (1UL << PML4_شاخص_دننه_واټن)) {
        pml4e_t *pml4_خونه = lookup_PML4_خونه(pml4, i);
        إذا (pml4e_ptr_get_present(pml4_خونه)) {
            چاپ_کړه("pdpt_%p_%04lu = pdpt\n", pml4_خونه, GET_PML4_شاخص(i));
            x86_64_obj_pdpt_print_slots(pml4_خونه);
        }
    }
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    إذا (is_Valid_VTable_ریښه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !کتل_شوی(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_بنسټ_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        جمع_کړه_منزل_کتل_شوی(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        چاپ_کړه("%p_pd = pml4\n", pml4);
        x86_64_obj_pml4_print_slots(pml4);
    }
}

/* use when only have access to vptr of frames */
ساكن فراغ cap_چوکاټ_print_attrs_vptr_2(کلمه_t vptr, vspace_ریښه_t *vspace)
{
    lookupPDPTSlot_ret_t pdpt_خونه = lookupPDPTSlot(vspace, vptr);
    lookupPTSlot_ret_t pt_خونه;
    lookupPDSlot_ret_t pd_خونه;

    إذا (pdpt_خونه.حالت == EXCEPTION_NONE &&
        pdpte_ptr_get_پاڼه_اندازه(pdpt_خونه.pdpt_خونه) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdpt_خونه.pdpt_خونه)) {
        چاپ_کړه("frame_%p_%04lu ", pdpt_خونه.pdpt_خونه, GET_PDPT_شاخص(vptr));
        cap_چوکاټ_print_attrs_pdpt(pdpt_خونه.pdpt_خونه);
    } وإلا {
        pd_خونه = lookupPDSlot(vspace, vptr);
        إذا (pd_خونه.حالت == EXCEPTION_NONE &&
            ((pde_ptr_get_پاڼه_اندازه(pd_خونه.pd_خونه) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pd_خونه.pd_خونه))) {
            چاپ_کړه("frame_%p_%04lu ", pd_خونه.pd_خونه, GET_PD_شاخص(vptr));
            cap_چوکاټ_print_attrs_pd(pd_خونه.pd_خونه);
        } وإلا {
            pt_خونه = lookupPTSlot(vspace, vptr);
            أكد(pt_خونه.حالت == EXCEPTION_NONE && pte_ptr_get_present(pt_خونه.pt_خونه));
            چاپ_کړه("frame_%p_%04lu ", pt_خونه.pt_خونه, GET_PT_شاخص(vptr));
            cap_چوکاټ_print_attrs_pt(pt_خونه.pt_خونه);
        }
    }
}

ساكن فراغ cap_چوکاټ_print_attrs_vptr(کلمه_t vptr, cap_t vspace)
{
    asid_t asid = cap_pml4_cap_get_capPML4MappedASID(vspace);
    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
    cap_چوکاټ_print_attrs_vptr_2(vptr, پیدا_کړه_ret.vspace_ریښه);
}

فراغ print_cap_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    /* arch specific caps */
    حالة cap_پاڼه_جدول_cap: {
        asid_t asid = cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(cap);
        پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
        vptr_t vptr = cap_پاڼه_جدول_cap_get_cap_PTMapped_پته(cap);
        إذا (asid) {
            چاپ_کړه("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(پیدا_کړه_ret.vspace_ریښه, vptr).pd_خونه, GET_PD_شاخص(vptr), (اوږد بینښه صحيح)asid);
        } وإلا {
            چاپ_کړه("pt_%p_%04lu\n", lookupPDSlot(پیدا_کړه_ret.vspace_ریښه, vptr).pd_خونه, GET_PD_شاخص(vptr));
        }
        اكسر;
    }
    حالة cap_پاڼه_directory_cap: {
        asid_t asid = cap_پاڼه_directory_cap_get_cap_PDMapped_ASID(cap);
        پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
        vptr_t vptr = cap_پاڼه_directory_cap_get_cap_PDMapped_پته(cap);
        إذا (asid) {
            چاپ_کړه("pd_%p_%04lu (asid: %lu)\n",
                   lookupPDPTSlot(پیدا_کړه_ret.vspace_ریښه, vptr).pdpt_خونه, GET_PDPT_شاخص(vptr), (اوږد بینښه صحيح)asid);
        } وإلا {
            چاپ_کړه("pd_%p_%04lu\n", lookupPDPTSlot(پیدا_کړه_ret.vspace_ریښه, vptr).pdpt_خونه, GET_PDPT_شاخص(vptr));
        }
        اكسر;
    }
    حالة cap_pdpt_cap: {
        asid_t asid = cap_pdpt_cap_get_capPDPTMappedASID(cap);
        پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
        vptr_t vptr = cap_pdpt_cap_get_cap_PDPTMapped_پته(cap);
        إذا (asid) {
            چاپ_کړه("pdpt_%p_%04lu (asid: %lu)\n",
                   lookup_PML4_خونه(پیدا_کړه_ret.vspace_ریښه, vptr), GET_PML4_شاخص(vptr), (اوږد بینښه صحيح)asid);
        } وإلا {
            چاپ_کړه("pdpt_%p_%04lu\n", lookup_PML4_خونه(پیدا_کړه_ret.vspace_ریښه, vptr), GET_PML4_شاخص(vptr));
        }
        اكسر;
    }
    حالة cap_pml4_cap: {
        asid_t asid = cap_pml4_cap_get_capPML4MappedASID(cap);
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
#که_تعریف_شوی CONFIG_VTX
    حالة cap_vcpu_cap: {
        چاپ_کړه("%p_vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#د_که_پای
    /* X86 specific caps */
    حالة cap_io_پورټ_cap: {
        چاپ_کړه("%p%p_io_port\n", (فراغ *)cap_io_پورټ_cap_get_cap_IOPort_لومړی_پورټ(cap),
               (فراغ *)cap_io_پورټ_cap_get_cap_IOPort_وروستی_پورټ(cap));
        اكسر;
    }
#که_تعریف_شوی CONFIG_IOMMU
    حالة cap_io_space_cap: {
        چاپ_کړه("%p_io_space\n", (فراغ *)cap_io_space_cap_get_capPCIDevice(cap));
        اكسر;
    }
    حالة cap_io_پاڼه_جدول_cap: {
        چاپ_کړه("%p_iopt\n", (فراغ *)cap_io_پاڼه_جدول_cap_get_cap_IOPTBase_Ptr(cap));
        اكسر;
    }
#د_که_پای
    افتراضي: {
        چاپ_کړه("[unknown cap %lu]\n", (اوږد بینښه صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

ساكن فراغ obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    چاپ_کړه("(asid_high: 0x%lx)\n", ASID_لوړ(asid));
}

فراغ print_object_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    /* arch specific objects */
    حالة cap_چوکاټ_cap:
    حالة cap_پاڼه_جدول_cap:
    حالة cap_پاڼه_directory_cap:
    حالة cap_pdpt_cap:
    حالة cap_pml4_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        چاپ_کړه("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        اكسر;
    }
#که_تعریف_شوی CONFIG_VTX
    حالة cap_vcpu_cap: {
        چاپ_کړه("%p_vcpu = vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#د_که_پای
    /* X86 specific caps */
    حالة cap_io_پورټ_cap: {
        چاپ_کړه("%p%p_io_port = io_port ",
               (فراغ *)cap_io_پورټ_cap_get_cap_IOPort_لومړی_پورټ(cap),
               (فراغ *)cap_io_پورټ_cap_get_cap_IOPort_وروستی_پورټ(cap));
        x86_obj_ioports_print_attrs(cap);
        اكسر;
    }
#که_تعریف_شوی CONFIG_IOMMU
    حالة cap_io_space_cap: {
        چاپ_کړه("%p_io_space = io_space ", (فراغ *)cap_io_space_cap_get_capPCIDevice(cap));
        x86_obj_iospace_print_attrs(cap);
        اكسر;
    }
    حالة cap_io_پاڼه_جدول_cap: {
        چاپ_کړه("%p_iopt = iopt ", (فراغ *)cap_io_پاڼه_جدول_cap_get_cap_IOPTBase_Ptr(cap));
        x86_obj_iopt_print_attrs(cap);
        اكسر;
    }
#د_که_پای
    افتراضي: {
        چاپ_کړه("[unknown object %lu]\n", (اوږد بینښه صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ print_ipc_بفر_خونه(tcb_t *tcb)
{
    کلمه_t vptr = tcb->tcbIPCBuffer;
    چاپ_کړه("ipc_buffer_slot: ");
    cap_چوکاټ_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

ساكن فراغ x86_64_cap_pt_print_slots(pde_t *pd_خونه, vptr_t vptr)
{
    pte_t *pt = paddr_منزل_pptr(pde_pde_pt_ptr_get_pt_بنسټ_پته(pd_خونه));
    چاپ_کړه("pt_%p_%04lu {\n", pd_خونه, GET_PD_شاخص(vptr));

    لكل (کلمه_t i = 0; i < بټ(PT_شاخص_دننه_واټن + PT_شاخص_بټونه); i += (1UL << PT_شاخص_دننه_واټن)) {
        pte_t *pt_خونه = pt + GET_PT_شاخص(i);

        إذا (pte_ptr_get_present(pt_خونه)) {
            چاپ_کړه("0x%lx: frame_%p_%04lu ", GET_PT_شاخص(i), pt_خونه, GET_PT_شاخص(i));
            cap_چوکاټ_print_attrs_pt(pt_خونه);
        }
    }
    چاپ_کړه("}\n"); /* pt */
}

ساكن فراغ x86_64_cap_pd_print_slots(pdpte_t *pdpt_خونه, vptr_t vptr)
{
    pde_t *pd = paddr_منزل_pptr(pdpte_pdpte_pd_ptr_get_pd_بنسټ_پته(pdpt_خونه));
    چاپ_کړه("pd_%p_%04lu {\n", pdpt_خونه, GET_PDPT_شاخص(vptr));

    لكل (کلمه_t i = 0; i < بټ(PD_شاخص_دننه_واټن + PD_شاخص_بټونه); i += (1UL << PD_شاخص_دننه_واټن)) {
        pde_t *pd_خونه = pd + GET_PD_شاخص(i);

        إذا ((pde_ptr_get_پاڼه_اندازه(pd_خونه) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_خونه)) {
            چاپ_کړه("0x%lx: frame_%p_%04lu ", GET_PD_شاخص(i), pd_خونه, GET_PD_شاخص(i));
            cap_چوکاټ_print_attrs_pd(pd_خونه);

        } وإلا إذا (pde_pde_pt_ptr_get_present(pd_خونه)) {
            چاپ_کړه("0x%lx: pt_%p_%04lu\n", GET_PD_شاخص(i), pd_خونه, GET_PD_شاخص(i));
        }
    }
    چاپ_کړه("}\n"); /* pd */

    لكل (کلمه_t i = 0; i < بټ(PD_شاخص_دننه_واټن + PD_شاخص_بټونه); i += (1UL << PD_شاخص_دننه_واټن)) {
        pde_t *pd_خونه = pd + GET_PD_شاخص(i);
        إذا ((pde_ptr_get_پاڼه_اندازه(pd_خونه) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pd_خونه)) {
            x86_64_cap_pt_print_slots(pd_خونه, i);
        }
    }
}

ساكن فراغ x86_64_cap_pdpt_print_slots(pml4e_t *pml4_خونه, vptr_t vptr)
{
    pdpte_t *pdpt = paddr_منزل_pptr(pml4e_ptr_get_pdpt_بنسټ_پته(pml4_خونه));

    چاپ_کړه("pdpt_%p_%04lu {\n", pml4_خونه, GET_PML4_شاخص(vptr));
    لكل (کلمه_t i = 0; i < بټ(PDPT_شاخص_دننه_واټن + PDPT_شاخص_بټونه); i += (1UL << PDPT_شاخص_دننه_واټن)) {
        pdpte_t *pdpt_خونه = pdpt + GET_PDPT_شاخص(i);

        إذا (pdpte_ptr_get_پاڼه_اندازه(pdpt_خونه) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdpt_خونه)) {
            چاپ_کړه("0x%lx: frame_%p_%04lu ", GET_PDPT_شاخص(i), pdpt_خونه, GET_PDPT_شاخص(i));
            cap_چوکاټ_print_attrs_pdpt(pdpt_خونه);

        } وإلا إذا (pdpte_pdpte_pd_ptr_get_present(pdpt_خونه)) {
            چاپ_کړه("0x%lx: pd_%p_%04lu\n", GET_PDPT_شاخص(i), pdpt_خونه, GET_PDPT_شاخص(i));
        }
    }
    چاپ_کړه("}\n"); /* pdpt */

    لكل (کلمه_t i = 0; i < بټ(PDPT_شاخص_دننه_واټن + PDPT_شاخص_بټونه); i += (1UL << PDPT_شاخص_دننه_واټن)) {
        pdpte_t *pdpt_خونه = pdpt + GET_PDPT_شاخص(i);

        إذا (pdpte_ptr_get_پاڼه_اندازه(pdpt_خونه) == pdpte_pdpte_pd && pdpte_pdpte_pd_ptr_get_present(pdpt_خونه)) {
            x86_64_cap_pd_print_slots(pdpt_خونه, i);
        }
    }
}

ساكن فراغ x86_64_cap_pml4_print_slots(pml4e_t *pml4)
{
    چاپ_کړه("%p_pd {\n", pml4);
    لكل (کلمه_t i = 0; i < بټ(PML4_شاخص_دننه_واټن + PML4_شاخص_بټونه); i += (1UL << PML4_شاخص_دننه_واټن)) {
        pml4e_t *pml4_خونه = lookup_PML4_خونه(pml4, i);
        إذا (pml4e_ptr_get_present(pml4_خونه)) {
            چاپ_کړه("0x%lx: pdpt_%p_%04lu\n", GET_PML4_شاخص(i), pml4_خونه, GET_PML4_شاخص(i));
        }
    }
    چاپ_کړه("}\n"); /* pml4 */

    لكل (کلمه_t i = 0; i < بټ(PML4_شاخص_دننه_واټن + PML4_شاخص_بټونه); i += (1UL << PML4_شاخص_دننه_واټن)) {
        pml4e_t *pml4_خونه = lookup_PML4_خونه(pml4, i);
        إذا (pml4e_ptr_get_present(pml4_خونه)) {
            x86_64_cap_pdpt_print_slots(pml4_خونه, i);
        }
    }
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    إذا (is_Valid_VTable_ریښه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !کتل_شوی(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_بنسټ_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        جمع_کړه_منزل_کتل_شوی(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        x86_64_cap_pml4_print_slots(pml4);
    }
}

#د_که_پای /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
{
    چاپ_کړه("arch x86_64\n");
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
#د_که_پای
}

#د_که_پای /* CONFIG_DEBUG_BUILD */
