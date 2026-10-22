/*
 * Copyright 2020, DornerWorks
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <types.h>
#అన్తర్భావ <benchmark/benchmark.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <kernel/boot.h>
#అన్తర్భావ <kernel/cspace.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <object/tcb.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <model/preemption.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <object/cnode.h>
#అన్తర్భావ <object/untyped.h>
#అన్తర్భావ <arch/api/invocation.h>
#అన్తర్భావ <arch/kernel/vspace.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <plat/machine/hardware.h>
#అన్తర్భావ <kernel/stack.h>
#అన్తర్భావ <util.h>

నిర్మాణం resolve_ret {
    paddr_t చట్రం_పునాది;
    vm_పుట_కొలత_t చట్రం_కొలత;
    bool_t valid;
};
రకనిర్వచనం నిర్మాణం resolve_ret resolve_ret_t;

స్థిర exception_t perform_పుట_Get_చిరునామా(శూన్య *vbase_ptr, bool_t call);

స్థిర మాట_t CONST RISCVGetWriteFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported. */
    తిరిగి vm_rights == VMReadWrite;
}

స్థిర అంతఃవరుస మాట_t CONST RISCVGetReadFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported.
     * Kernel-only conveys no user rights. */
    తిరిగి vm_rights != VMKernelOnly;
}

స్థిర అంతఃవరుస bool_t is_PTEPage_పట్టిక(pte_t *pte)
{
    తిరిగి pte_ptr_get_valid(pte) &&
           !(pte_ptr_get_read(pte) || pte_ptr_get_write(pte) || pte_ptr_get_execute(pte));
}

/** Helper function meant only to be used for mapping the kernel
 * window.
 *
 * Maps all pages with full RWX and supervisor perms by default.
 */
స్థిర pte_t pte_తదుపరి(మాట_t phys_addr, bool_t is_ఆకు)
{
    మాట_t ppn = (మాట_t)(phys_addr >> 12);

    uint8_t పఠ = is_ఆకు ? 1 : 0;
    uint8_t లిఖ = పఠ;
    uint8_t exec = పఠ;

    తిరిగి pte_new(ppn,
                   0,     /* sw */
                   is_ఆకు ? 1 : 0,     /* dirty (leaf)/reserved (non-leaf) */
                   is_ఆకు ? 1 : 0,     /* accessed (leaf)/reserved (non-leaf) */
                   1,     /* global */
                   is_ఆకు ? 0 : 0,     /* user (leaf)/reserved (non-leaf) */
                   exec,  /* execute */
                   లిఖ, /* write */
                   పఠ,  /* read */
                   1      /* valid */
                  );
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_సంకేతం శూన్య map_kernel_చట్రం(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights)
{
#ఒకవేళ __riscv_xlen == 32
    paddr = విడత_కిందికి(paddr, RISCV_GET_LVL_PGSIZE_బిట్లు(0));
    ప్రతిజ్ఞా((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
    kernel_మూలం_పుట_పట్టిక[RISCV_GET_PT_సూచీ(vaddr, 0)] = pte_తదుపరి(paddr, true);
#లేకపోతే
    ఒకవేళ (vaddr >= KDEV_పునాది) {
        /* Map devices in level 1 page table */
        paddr = విడత_కిందికి(paddr, RISCV_GET_LVL_PGSIZE_బిట్లు(1));
        ప్రతిజ్ఞా((paddr % RISCV_GET_LVL_PGSIZE(1)) == 0);
        kernel_image_level1_dev_pt[RISCV_GET_PT_సూచీ(vaddr, 1)] = pte_తదుపరి(paddr, true);
    } లేకపోతే {
        paddr = విడత_కిందికి(paddr, RISCV_GET_LVL_PGSIZE_బిట్లు(0));
        ప్రతిజ్ఞా((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
        kernel_మూలం_పుట_పట్టిక[RISCV_GET_PT_సూచీ(vaddr, 0)] = pte_తదుపరి(paddr, true);
    }
#అయితే_ముగింపు
}

BOOT_సంకేతం VISIBLE శూన్య map_kernel_కిటికీ(శూన్య)
{
    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    ప్రతిజ్ఞా(CONFIG_PT_స్థాయిలు > 1 && CONFIG_PT_స్థాయిలు <= 4);
    /* Kernel image finishes before KDEV_BASE */
    ప్రతిజ్ఞా(KERNEL_ELF_శిఖరం <= KDEV_పునాది);

    /* kernel window starts at PPTR_BASE */
    మాట_t pptr = PPTR_పునాది;

    /* first we map in memory from PADDR_BASE */
    మాట_t paddr = PADDR_పునాది;
    అయితే (pptr < PPTR_శిఖరం) {
        ప్రతిజ్ఞా(IS_ALIGNED(pptr, RISCV_GET_LVL_PGSIZE_బిట్లు(0)));
        ప్రతిజ్ఞా(IS_ALIGNED(paddr, RISCV_GET_LVL_PGSIZE_బిట్లు(0)));

        kernel_మూలం_పుట_పట్టిక[RISCV_GET_PT_సూచీ(pptr, 0)] = pte_తదుపరి(paddr, true);

        pptr += RISCV_GET_LVL_PGSIZE(0);
        paddr += RISCV_GET_LVL_PGSIZE(0);
    }
    /* now map KERNEL_ELF_BASE to KERNEL_ELF_PADDR_BASE */
    ప్రతిజ్ఞా(pptr == PPTR_శిఖరం);
    pptr = విడత_కిందికి(KERNEL_ELF_పునాది, RISCV_GET_LVL_PGSIZE_బిట్లు(0));
    paddr = విడత_కిందికి(KERNEL_ELF_PADDR_పునాది, RISCV_GET_LVL_PGSIZE_బిట్లు(0));

#ఒకవేళ __riscv_xlen == 32
    kernel_మూలం_పుట_పట్టిక[RISCV_GET_PT_సూచీ(pptr, 0)] = pte_తదుపరి(paddr, true);
    pptr += RISCV_GET_LVL_PGSIZE(0);
    paddr += RISCV_GET_LVL_PGSIZE(0);
#నిర్వచితమైతే CONFIG_KERNEL_LOG_బఫరు
    kernel_మూలం_పుట_పట్టిక[RISCV_GET_PT_సూచీ(KS_LOG_PPTR, 0)] =
        pte_తదుపరి(kpptr_గమ్యం_paddr(kernel_image_level1_log_బఫరు_pt), false);
#అయితే_ముగింపు
#లేకపోతే
    మాట_t సూచీ = 0;
    /* The kernel image is mapped twice, locating the two indexes in the
     * root page table, pointing them to the same level 1 page table.
     */
    kernel_మూలం_పుట_పట్టిక[RISCV_GET_PT_సూచీ(KERNEL_ELF_PADDR_పునాది + PPTR_పునాది_స్థానభ్రంశం, 0)] =
        pte_తదుపరి(kpptr_గమ్యం_paddr(kernel_image_level1_pt), false);
    kernel_మూలం_పుట_పట్టిక[RISCV_GET_PT_సూచీ(pptr, 0)] =
        pte_తదుపరి(kpptr_గమ్యం_paddr(kernel_image_level1_pt), false);
    అయితే (pptr < PPTR_శిఖరం + RISCV_GET_LVL_PGSIZE(0)) {
        kernel_image_level1_pt[సూచీ] = pte_తదుపరి(paddr, true);
        సూచీ++;
        pptr += RISCV_GET_LVL_PGSIZE(1);
        paddr += RISCV_GET_LVL_PGSIZE(1);
    }

    /* Map kernel device page table */
    kernel_మూలం_పుట_పట్టిక[RISCV_GET_PT_సూచీ(KDEV_పునాది, 0)] =
        pte_తదుపరి(kpptr_గమ్యం_paddr(kernel_image_level1_dev_pt), false);
#అయితే_ముగింపు

    /* There should be free space where we put device mapping */
    ప్రతిజ్ఞా(pptr == UINTPTR_MAX - RISCV_GET_LVL_PGSIZE(0) + 1);
    map_kernel_devices();
}

BOOT_సంకేతం శూన్య map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    lookupPTSlot_ret_t pt_ret;
    pte_t *లక్ష్యం_స్థానం;
    vptr_t vptr = cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(pt_cap);
    pte_t *lvl1pt = PTE_PTR(pptr_యొక్క_cap(vspace_cap));

    /* pt to be mapped */
    pte_t *pt   = PTE_PTR(pptr_యొక్క_cap(pt_cap));

    /* Get PT slot to install the address in */
    pt_ret = lookupPTSlot(lvl1pt, vptr);

    లక్ష్యం_స్థానం = pt_ret.pt_స్థానం;

    *లక్ష్యం_స్థానం = pte_new(
                      (addrFromPPtr(pt) >> se_L4_పుట_బిట్లు),
                      0, /* sw */
                      0, /* dirty (reserved non-leaf) */
                      0, /* accessed (reserved non-leaf) */
                      0,  /* global */
                      0,  /* user (reserved non-leaf) */
                      0,  /* execute */
                      0,  /* write */
                      0,  /* read */
                      1 /* valid */
                  );
    sfence();
}

BOOT_సంకేతం శూన్య map_it_చట్రం_cap(cap_t vspace_cap, cap_t చట్రం_cap)
{
    pte_t *lvl1pt   = PTE_PTR(pptr_యొక్క_cap(vspace_cap));
    pte_t *చట్రం_pptr   = PTE_PTR(pptr_యొక్క_cap(చట్రం_cap));
    vptr_t చట్రం_vptr = cap_చట్రం_cap_get_cap_FMapped_చిరునామా(చట్రం_cap);

    /* We deal with a frame as 4KiB */
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, చట్రం_vptr);
    ప్రతిజ్ఞా(lu_ret.pt_బిట్లు_ఎడమ == se_L4_పుట_బిట్లు);

    pte_t *లక్ష్యం_స్థానం = lu_ret.pt_స్థానం;

    *లక్ష్యం_స్థానం = pte_new(
                      (pptr_గమ్యం_paddr(చట్రం_pptr) >> se_L4_పుట_బిట్లు),
                      0, /* sw */
                      1, /* dirty (leaf) */
                      1, /* accessed (leaf) */
                      0,  /* global */
                      1,  /* user (leaf) */
                      1,  /* execute */
                      1,  /* write */
                      1,  /* read */
                      1   /* valid */
                  );
    sfence();
}

BOOT_సంకేతం cap_t create_unmapped_it_చట్రం_cap(pptr_t pptr, bool_t use_large)
{
    cap_t cap = cap_చట్రం_cap_new(
                    asidInvalid,                     /* capFMappedASID       */
                    pptr,                            /* capFBasePtr          */
                    0,                               /* capFSize             */
                    0,                               /* capFVMRights         */
                    0,
                    0                                /* capFMappedAddress    */
                );

    తిరిగి cap;
}

/* Create a page table for the initial thread */
స్థిర BOOT_సంకేతం cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_పుట_పట్టిక_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );

    map_it_pt_cap(vspace_cap, cap);
    తిరిగి cap;
}

BOOT_సంకేతం మాట_t arch_get_n_paging(v_region_t it_v_reg)
{
    మాట_t n = 0;
    కొరకు (పూర్ణాంకం i = 0; i < CONFIG_PT_స్థాయిలు - 1; i++) {
        n += get_n_paging(it_v_reg, RISCV_GET_LVL_PGSIZE_బిట్లు(i));
    }
    తిరిగి n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_సంకేతం cap_t create_it_చిరునామా_space(cap_t మూలం_cnode_cap, v_region_t it_v_reg)
{
    cap_t      lvl1pt_cap;
    vptr_t     pt_vptr;

    copyGlobalMappings(PTE_PTR(rootserver.vspace));

    lvl1pt_cap =
        cap_పుట_పట్టిక_cap_new(
            IT_ASID,               /* capPTMappedASID    */
            (మాట_t) rootserver.vspace,  /* capPTBasePtr       */
            1,                     /* capPTIsMapped      */
            (మాట_t) rootserver.vspace   /* capPTMappedAddress */
        );

    se_L4_స్థానం_Pos స్థానం_pos_before = ndks_boot.స్థానం_pos_cur;
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapInitThreadVSpace), lvl1pt_cap);

    /* create all n level PT caps necessary to cover userland image in 4KiB pages */
    కొరకు (పూర్ణాంకం i = 0; i < CONFIG_PT_స్థాయిలు - 1; i++) {

        కొరకు (pt_vptr = విడత_కిందికి(it_v_reg.ప్రారంభం, RISCV_GET_LVL_PGSIZE_బిట్లు(i));
             pt_vptr < it_v_reg.చివర;
             pt_vptr += RISCV_GET_LVL_PGSIZE(i)) {
            ఒకవేళ (!provide_cap(మూలం_cnode_cap,
                             create_it_pt_cap(lvl1pt_cap, it_alloc_paging(), pt_vptr, IT_ASID))
               ) {
                తిరిగి cap_null_cap_new();
            }
        }

    }

    se_L4_స్థానం_Pos స్థానం_pos_after = ndks_boot.స్థానం_pos_cur;
    ndks_boot.bi_చట్రం->userImagePaging = (se_L4_స్థానం_Region) {
        స్థానం_pos_before, స్థానం_pos_after
    };

    తిరిగి lvl1pt_cap;
}

BOOT_సంకేతం శూన్య activate_kernel_vspace(శూన్య)
{
    సమితి_VSpace_మూలం(kpptr_గమ్యం_paddr(&kernel_మూలం_పుట_పట్టిక), 0);
}

BOOT_సంకేతం శూన్య write_it_asid_pool(cap_t it_ap_cap, cap_t it_lvl1pt_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_యొక్క_cap(it_ap_cap));
    ap->array[ASID_దిగువ(IT_ASID)] = PTE_PTR(pptr_యొక్క_cap(it_lvl1pt_cap));
    riscvKSASIDTable[ASID_ఎగువ(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

స్థిర కనుగొను_VSpace_For_ASID_ret_t కనుగొను_VSpace_For_ASID(asid_t asid)
{
    కనుగొను_VSpace_For_ASID_ret_t ret;
    asid_pool_t        *poolPtr;
    pte_t     *vspace_మూలం;

    poolPtr = riscvKSASIDTable[ASID_ఎగువ(asid)];
    ఒకవేళ (!poolPtr) {
        current_lookup_fault = lookup_fault_invalid_మూలం_new();

        ret.vspace_మూలం = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }

    vspace_మూలం = poolPtr->array[ASID_దిగువ(asid)];
    ఒకవేళ (!vspace_మూలం) {
        current_lookup_fault = lookup_fault_invalid_మూలం_new();

        ret.vspace_మూలం = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }

    ret.vspace_మూలం = vspace_మూలం;
    ret.స్థితి_2 = EXCEPTION_NONE;
    తిరిగి ret;
}

శూన్య copyGlobalMappings(pte_t *newLvl1pt)
{
    గుర్తులేని దీర్ఘ i;
    pte_t *global_kernel_vspace = kernel_మూలం_పుట_పట్టిక;

    కొరకు (i = RISCV_GET_PT_సూచీ(PPTR_పునాది, 0); i < బిట్టు(PT_సూచీ_బిట్లు); i++) {
        newLvl1pt[i] = global_kernel_vspace[i];
    }
}

మాట_t *శుద్ధ lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    మాట_t w_బఫరు_Ptr;
    cap_t బఫరు_Cap;
    vm_rights_t vm_rights;

    w_బఫరు_Ptr = thread->tcbIPCBuffer;
    బఫరు_Cap = TCB_PTR_CTE_PTR(thread, tcb_బఫరు)->cap;

    ఒకవేళ (unlikely(cap_get_capType(బఫరు_Cap) != cap_చట్రం_cap)) {
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (unlikely(cap_చట్రం_cap_get_cap_FIs_Device(బఫరు_Cap))) {
        తిరిగి ఖాళీ;
    }

    vm_rights = cap_చట్రం_cap_get_cap_FVMRights(బఫరు_Cap);
    ఒకవేళ (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        మాట_t పునాది_Ptr, పుట_బిట్లు_2;

        పునాది_Ptr = cap_చట్రం_cap_get_cap_FBase_Ptr(బఫరు_Cap);
        పుట_బిట్లు_2 = పుట_బిట్లు_For_కొలత(cap_చట్రం_cap_get_cap_FSize(బఫరు_Cap));
        తిరిగి (మాట_t *)(పునాది_Ptr + (w_బఫరు_Ptr & ముసుగు(పుట_బిట్లు_2)));
    } లేకపోతే {
        తిరిగి ఖాళీ;
    }
}

స్థిర అంతఃవరుస pte_t *getPPtrFromHWPTE(pte_t *pte)
{
    తిరిగి PTE_PTR(ptrFromPAddr(pte_ptr_get_ppn(pte) << se_L4_పుట_పట్టిక_బిట్లు));
}

lookupPTSlot_ret_t lookupPTSlot(pte_t *lvl1pt, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    మాట_t level = CONFIG_PT_స్థాయిలు - 1;
    pte_t *pt = lvl1pt;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_బిట్లు_ఎడమ = PT_సూచీ_బిట్లు * level + se_L4_పుట_బిట్లు;
    ret.pt_స్థానం = pt + ((vptr >> ret.pt_బిట్లు_ఎడమ) & ముసుగు(PT_సూచీ_బిట్లు));

    అయితే (is_PTEPage_పట్టిక(ret.pt_స్థానం) && likely(0 < level)) {
        level--;
        ret.pt_బిట్లు_ఎడమ -= PT_సూచీ_బిట్లు;
        pt = getPPtrFromHWPTE(ret.pt_స్థానం);
        ret.pt_స్థానం = pt + ((vptr >> ret.pt_బిట్లు_ఎడమ) & ముసుగు(PT_సూచీ_బిట్లు));
    }

    తిరిగి ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    uint64_t addr;

    addr = read_stval();

    ఎంపిక (vm_faultType) {
    స్థితి RISCVLoad_పుట_Fault:
    స్థితి RISCVLoadAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVLoadAccessFault, false);
        తిరిగి EXCEPTION_FAULT;
    స్థితి RISCVStore_పుట_Fault:
    స్థితి RISCVStoreAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVStoreAccessFault, false);
        తిరిగి EXCEPTION_FAULT;
    స్థితి RISCVInstruction_పుట_Fault:
    స్థితి RISCVInstructionAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVInstructionAccessFault, true);
        తిరిగి EXCEPTION_FAULT;

    అప్రమేయ:
        fail("Invalid VM fault type");
    }
}

శూన్య deleteASIDPool(asid_t asid_పునాది, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    ప్రతిజ్ఞా(IS_ALIGNED(asid_పునాది, asid_దిగువ_బిట్లు));

    ఒకవేళ (riscvKSASIDTable[ASID_ఎగువ(asid_పునాది)] == pool) {
        riscvKSASIDTable[ASID_ఎగువ(asid_పునాది)] = ఖాళీ;
        సమితి_VMRoot(కణుపు_STATE(ksCurThread));
    }
}

స్థిర exception_t performASIDControlInvocation(శూన్య *చట్రం, cte_t *స్థానం, cte_t *మాతృ, asid_t asid_పునాది)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_సమితి_cap_Free_సూచీ(&(మాతృ->cap),
                                         MAX_FREE_సూచీ(cap_untyped_cap_get_cap_ఖండం_కొలత(మాతృ->cap)));

    memzero(చట్రం, బిట్టు(పుట_బిట్లు_For_కొలత(RISCV_4_K_పుట)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_చొప్పించు(
        cap_asid_pool_cap_new(
            asid_పునాది,          /* capASIDBase  */
            మాట_REF(చట్రం)     /* capASIDPool  */
        ),
        మాతృ,
        స్థానం
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    ప్రతిజ్ఞా((asid_పునాది & ముసుగు(asid_దిగువ_బిట్లు)) == 0);
    riscvKSASIDTable[ASID_ఎగువ(asid_పునాది)] = (asid_pool_t *)చట్రం;

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspace_Cap_స్థానం)
{
    cap_t cap = vspace_Cap_స్థానం->cap;
    pte_t *region_పునాది = PTE_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap));
    cap = cap_పుట_పట్టిక_cap_సమితి_cap_PTMapped_ASID(cap, asid);
    cap = cap_పుట_పట్టిక_cap_సమితి_cap_PTMapped_చిరునామా(cap, 0);
    cap = cap_పుట_పట్టిక_cap_సమితి_cap_PTIs_Mapped(cap, 1);
    vspace_Cap_స్థానం->cap = cap;

    copyGlobalMappings(region_పునాది);

    poolPtr->array[ASID_దిగువ(asid)] = region_పునాది;

    తిరిగి EXCEPTION_NONE;
}

శూన్య deleteASID(asid_t asid, pte_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = riscvKSASIDTable[ASID_ఎగువ(asid)];
    ఒకవేళ (poolPtr != ఖాళీ && poolPtr->array[ASID_దిగువ(asid)] == vspace) {
        hwASIDFlush(asid);
        poolPtr->array[ASID_దిగువ(asid)] = ఖాళీ;
        సమితి_VMRoot(కణుపు_STATE(ksCurThread));
    }
}

శూన్య unmap_పుట_పట్టిక(asid_t asid, vptr_t vptr, pte_t *లక్ష్యం_pt)
{
    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
    ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE)) {
        /* nothing to do */
        తిరిగి;
    }
    /* We won't ever unmap a top level page table */
    ప్రతిజ్ఞా(కనుగొను_ret.vspace_మూలం != లక్ష్యం_pt);
    pte_t *pt_స్థానం = ఖాళీ;
    pte_t *pt = కనుగొను_ret.vspace_మూలం;

    కొరకు (మాట_t i = 0; i < CONFIG_PT_స్థాయిలు - 1 && pt != లక్ష్యం_pt; i++) {
        pt_స్థానం = pt + RISCV_GET_PT_సూచీ(vptr, i);
        ఒకవేళ (unlikely(!is_PTEPage_పట్టిక(pt_స్థానం))) {
            /* couldn't find it */
            తిరిగి;
        }
        pt = getPPtrFromHWPTE(pt_స్థానం);
    }

    ఒకవేళ (pt != లక్ష్యం_pt) {
        /* didn't find it */
        తిరిగి;
    }
    /* If we found a pt then ptSlot won't be null */
    ప్రతిజ్ఞా(pt_స్థానం != ఖాళీ);
    *pt_స్థానం = pte_new(
                  0,  /* phy_address */
                  0,  /* sw */
                  0,  /* dirty (reserved non-leaf) */
                  0,  /* accessed (reserved non-leaf) */
                  0,  /* global */
                  0,  /* user (reserved non-leaf) */
                  0,  /* execute */
                  0,  /* write */
                  0,  /* read */
                  0  /* valid */
              );
    sfence();
}

స్థిర pte_t pte_pte_invalid_new(శూన్య)
{
    తిరిగి (pte_t) {
        0
    };
}

శూన్య unmap_పుట(vm_పుట_కొలత_t పుట_కొలత, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret;
    lookupPTSlot_ret_t  lu_ret;

    కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
    ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి;
    }

    lu_ret = lookupPTSlot(కనుగొను_ret.vspace_మూలం, vptr);
    ఒకవేళ (unlikely(lu_ret.pt_బిట్లు_ఎడమ != పుట_బిట్లు_For_కొలత(పుట_కొలత))) {
        తిరిగి;
    }
    ఒకవేళ (!pte_ptr_get_valid(lu_ret.pt_స్థానం) || is_PTEPage_పట్టిక(lu_ret.pt_స్థానం)
        || (pte_ptr_get_ppn(lu_ret.pt_స్థానం) << se_L4_పుట_బిట్లు) != pptr_గమ్యం_paddr((శూన్య *)pptr)) {
        తిరిగి;
    }

    lu_ret.pt_స్థానం[0] = pte_pte_invalid_new();
    sfence();
}

శూన్య సమితి_VMRoot(tcb_t *tcb)
{
    cap_t thread_మూలం;
    asid_t asid;
    pte_t *lvl1pt;
    కనుగొను_VSpace_For_ASID_ret_t  కనుగొను_ret;

    thread_మూలం = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    ఒకవేళ (cap_get_capType(thread_మూలం) != cap_పుట_పట్టిక_cap) {
        సమితి_VSpace_మూలం(kpptr_గమ్యం_paddr(&kernel_మూలం_పుట_పట్టిక), 0);
        తిరిగి;
    }

    lvl1pt = PTE_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(thread_మూలం));

    asid = cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(thread_మూలం);
    కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
    ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE || కనుగొను_ret.vspace_మూలం != lvl1pt)) {
        సమితి_VSpace_మూలం(kpptr_గమ్యం_paddr(&kernel_మూలం_పుట_పట్టిక), 0);
        తిరిగి;
    }

    సమితి_VSpace_మూలం(addrFromPPtr(lvl1pt), asid);
}

bool_t CONST is_Valid_VTable_మూలం(cap_t cap)
{
    తిరిగి (cap_get_capType(cap) == cap_పుట_పట్టిక_cap &&
            cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(cap));
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    ఒకవేళ (unlikely(cap_get_capType(cap) != cap_చట్రం_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (unlikely(cap_చట్రం_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (unlikely(!IS_ALIGNED(vptr, se_L4_IPCBuffer_కొలత_బిట్లు))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (పూర్ణాంకం)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    తిరిగి EXCEPTION_NONE;
}

vm_rights_t CONST ముసుగు_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_ముసుగు)
{
    ఒకవేళ (vm_rights == VMReadOnly && seL4_CapRights_get_capAllowRead(cap_rights_ముసుగు)) {
        తిరిగి VMReadOnly;
    }
    ఒకవేళ (vm_rights == VMReadWrite && seL4_CapRights_get_capAllowRead(cap_rights_ముసుగు)) {
        ఒకవేళ (!seL4_CapRights_get_capAllowWrite(cap_rights_ముసుగు)) {
            తిరిగి VMReadOnly;
        } లేకపోతే {
            తిరిగి VMReadWrite;
        }
    }
    ఒకవేళ (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_ముసుగు) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_ముసుగు)) {
        userError("Attempted to make unsupported write only mapping");
    }
    తిరిగి VMKernelOnly;
}

/* The rest of the file implements the RISCV object invocations */

స్థిర pte_t CONST makeUserPTE(paddr_t paddr, bool_t executable, vm_rights_t vm_rights)
{
    మాట_t లిఖ = RISCVGetWriteFromVMRights(vm_rights);
    మాట_t పఠ = RISCVGetReadFromVMRights(vm_rights);
    ఒకవేళ (unlikely(!పఠ && !లిఖ && !executable)) {
        తిరిగి pte_pte_invalid_new();
    } లేకపోతే {
        తిరిగి pte_new(
                   paddr >> se_L4_పుట_బిట్లు,
                   0, /* sw */
                   1, /* dirty (leaf) */
                   1, /* accessed (leaf) */
                   0, /* global */
                   1, /* user (leaf) */
                   executable, /* execute */
                   RISCVGetWriteFromVMRights(vm_rights), /* write */
                   RISCVGetReadFromVMRights(vm_rights), /* read */
                   1 /* valid */
               );
    }
}

స్థిర అంతఃవరుస bool_t CONST checkVPAlignment(vm_పుట_కొలత_t sz, మాట_t w)
{
    తిరిగి (w & ముసుగు(పుట_బిట్లు_For_కొలత(sz))) == 0;
}

స్థిర exception_t decode_RISCVPage_పట్టిక_Invocation(మాట_t label, మాట_t నిడివి,
                                                  cte_t *cte, cap_t cap, మాట_t *బఫరు)
{
    ఒకవేళ (label == RISCVPage_పట్టిక_Unmap) {
        ఒకవేళ (unlikely(!isFinalCapability(cte))) {
            userError("RISCVPageTableUnmap: cannot unmap if more than once cap exists");
            current_syscall_error.type = se_L4_Revoke_మొదటి;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        /* Ensure that if the page table is mapped, it is not a top level table */
        ఒకవేళ (likely(cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(cap))) {
            asid_t asid = cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(cap);
            కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
            pte_t *pte = PTE_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap));
            ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 == EXCEPTION_NONE &&
                         కనుగొను_ret.vspace_మూలం == pte)) {
                userError("RISCVPageTableUnmap: cannot call unmap on top level PageTable");
                current_syscall_error.type = se_L4_Revoke_మొదటి;
                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_పుట_పట్టిక_Invocation_Unmap(cap, cte);
    }

    ఒకవేళ (unlikely((label != RISCVPage_పట్టిక_Map))) {
        userError("RISCVPageTable: Illegal Operation");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (unlikely(నిడివి < 2 || current_extra_caps.excaprefs[0] == ఖాళీ)) {
        userError("RISCVPageTable: truncated message");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (unlikely(cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(cap))) {
        userError("RISCVPageTable: PageTable is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    మాట_t vaddr = getSyscallArg(0, బఫరు);
    cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (unlikely(cap_get_capType(lvl1ptCap) != cap_పుట_పట్టిక_cap ||
                 cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(lvl1ptCap) == asidInvalid)) {
        userError("RISCVPageTableMap: Invalid top-level PageTable.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    pte_t *lvl1pt = PTE_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(lvl1ptCap));
    asid_t asid = cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(lvl1ptCap);

    ఒకవేళ (unlikely(vaddr >= USER_శిఖరం)) {
        userError("RISCVPageTableMap: Virtual address cannot be in kernel window.");
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 0;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
    ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (unlikely(కనుగొను_ret.vspace_మూలం != lvl1pt)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);

    /* if there is already something mapped (valid is set) or we have traversed far enough
     * that a page table is not valid to map then tell the user that they have to delete
     * something before they can put a PT here */
    ఒకవేళ (lu_ret.pt_బిట్లు_ఎడమ == se_L4_పుట_బిట్లు || pte_ptr_get_valid(lu_ret.pt_స్థానం)) {
        userError("RISCVPageTableMap: All objects mapped at this address");
        current_syscall_error.type = se_L4_Delete_మొదటి;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    /* Get the slot to install the PT in */
    pte_t *pt_స్థానం = lu_ret.pt_స్థానం;

    paddr_t paddr = addrFromPPtr(
                        PTE_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap)));
    pte_t pte = pte_new((paddr >> se_L4_పుట_బిట్లు),
                        0, /* sw */
                        0, /* dirty (reserved non-leaf) */
                        0, /* accessed (reserved non-leaf) */
                        0,  /* global */
                        0,  /* user (reserved non-leaf) */
                        0,  /* execute */
                        0,  /* write */
                        0,  /* read */
                        1 /* valid */
                       );

    cap = cap_పుట_పట్టిక_cap_సమితి_cap_PTIs_Mapped(cap, 1);
    cap = cap_పుట_పట్టిక_cap_సమితి_cap_PTMapped_ASID(cap, asid);
    cap = cap_పుట_పట్టిక_cap_సమితి_cap_PTMapped_చిరునామా(cap, (vaddr & ~ముసుగు(lu_ret.pt_బిట్లు_ఎడమ)));

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి perform_పుట_పట్టిక_Invocation_Map(cap, cte, pte, pt_స్థానం);
}

స్థిర exception_t decodeRISCVFrameInvocation(మాట_t label, మాట_t నిడివి,
                                              cte_t *cte, cap_t cap, bool_t call, మాట_t *బఫరు)
{
    ఎంపిక (label) {
    స్థితి RISCVPageMap: {
        ఒకవేళ (unlikely(నిడివి < 3 || current_extra_caps.excaprefs[0] == ఖాళీ)) {
            userError("RISCVPageMap: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        మాట_t vaddr = getSyscallArg(0, బఫరు);
        మాట_t w_rights_ముసుగు = getSyscallArg(1, బఫరు);
        vm_లక్షణాలు_t attr = vm_లక్షణాలు_From_మాట(getSyscallArg(2, బఫరు));
        cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

        vm_పుట_కొలత_t చట్రం_కొలత = cap_చట్రం_cap_get_cap_FSize(cap);
        vm_rights_t capVMRights = cap_చట్రం_cap_get_cap_FVMRights(cap);

        ఒకవేళ (unlikely(cap_get_capType(lvl1ptCap) != cap_పుట_పట్టిక_cap ||
                     !cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(lvl1ptCap))) {
            userError("RISCVPageMap: Bad PageTable cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        pte_t *lvl1pt = PTE_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(lvl1ptCap));
        asid_t asid = cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(lvl1ptCap);

        కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
        ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE)) {
            userError("RISCVPageMap: No PageTable for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = false;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (unlikely(కనుగొను_ret.vspace_మూలం != lvl1pt)) {
            userError("RISCVPageMap: ASID lookup failed");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        /* check the vaddr is valid */
        మాట_t vtop = vaddr + బిట్టు(పుట_బిట్లు_For_కొలత(చట్రం_కొలత)) - 1;
        ఒకవేళ (unlikely(vtop >= USER_శిఖరం)) {
            current_syscall_error.type = se_L4_Invalid_వాదం;
            current_syscall_error.invalid_వాదం_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        ఒకవేళ (unlikely(!checkVPAlignment(చట్రం_కొలత, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        /* Check if this page is already mapped */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);
        ఒకవేళ (unlikely(lu_ret.pt_బిట్లు_ఎడమ != పుట_బిట్లు_For_కొలత(చట్రం_కొలత))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_బిట్లు_ఎడమ);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = false;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        asid_t చట్రం_asid = cap_చట్రం_cap_get_cap_FMapped_ASID(cap);
        ఒకవేళ (unlikely(చట్రం_asid != asidInvalid)) {
            /* this frame is already mapped */
            ఒకవేళ (చట్రం_asid != asid) {
                userError("RISCVPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_సంఖ్య = 1;
                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }
            మాట_t mapped_vaddr = cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap);
            ఒకవేళ (unlikely(mapped_vaddr != vaddr)) {
                userError("RISCVPageMap: attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_వాదం;
                current_syscall_error.invalid_వాదం_సంఖ్య = 0;
                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }
            /* this check is redundant, as lookupPTSlot does not stop on a page
             * table PTE */
            ఒకవేళ (unlikely(is_PTEPage_పట్టిక(lu_ret.pt_స్థానం))) {
                userError("RISCVPageMap: no mapping to remap.");
                current_syscall_error.type = se_L4_Delete_మొదటి;
                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }
        } లేకపోతే {
            /* check this vaddr isn't already mapped */
            ఒకవేళ (unlikely(pte_ptr_get_valid(lu_ret.pt_స్థానం))) {
                userError("Virtual address (0x%"SEL4_PRIx_మాట") already mapped", vaddr);
                current_syscall_error.type = se_L4_Delete_మొదటి;
                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }
        }

        vm_rights_t vmRights = ముసుగు_VMRights(capVMRights, rights_From_మాట(w_rights_ముసుగు));
        paddr_t చట్రం_paddr = addrFromPPtr((శూన్య *) cap_చట్రం_cap_get_cap_FBase_Ptr(cap));
        cap = cap_చట్రం_cap_సమితి_cap_FMapped_ASID(cap, asid);
        cap = cap_చట్రం_cap_సమితి_cap_FMapped_చిరునామా(cap,  vaddr);

        bool_t executable = !vm_లక్షణాలు_get_riscv_Execute_Never(attr);
        pte_t pte = makeUserPTE(చట్రం_paddr, executable, vmRights);
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_పుట_Invocation_Map_PTE(cap, cte, pte, lu_ret.pt_స్థానం);
    }

    స్థితి RISCVPageUnmap: {
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_పుట_Invocation_Unmap(cap, cte);
    }

    స్థితి RISCVPage_Get_చిరునామా: {

        /* Check that there are enough message registers */
        ప్రతిజ్ఞా(n_సందేశం_Registers >= 1);

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_పుట_Get_చిరునామా((శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(cap), call);
    }

    అప్రమేయ:
        userError("RISCVPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

}

exception_t decodeRISCVMMUInvocation(మాట_t label, మాట_t నిడివి, cptr_t cptr,
                                     cte_t *cte, cap_t cap, bool_t call, మాట_t *బఫరు)
{
    ఎంపిక (cap_get_capType(cap)) {

    స్థితి cap_పుట_పట్టిక_cap:
        తిరిగి decode_RISCVPage_పట్టిక_Invocation(label, నిడివి, cte, cap, బఫరు);

    స్థితి cap_చట్రం_cap:
        తిరిగి decodeRISCVFrameInvocation(label, నిడివి, cte, cap, call, బఫరు);

    స్థితి cap_asid_control_cap: {
        మాట_t     i;
        asid_t           asid_పునాది;
        మాట_t           సూచీ;
        మాట_t           లోతు;
        cap_t            untyped;
        cap_t            మూలం;
        cte_t           *మాతృ_స్థానం;
        cte_t           *dest_స్థానం;
        lookup_స్థానం_ret_t lu_ret;
        శూన్య            *చట్రం;
        exception_t      స్థితి_2;

        ఒకవేళ (label != RISCVASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (నిడివి < 2 || current_extra_caps.excaprefs[0] == ఖాళీ
            || current_extra_caps.excaprefs[1] == ఖాళీ) {
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        సూచీ = getSyscallArg(0, బఫరు);
        లోతు = getSyscallArg(1, బఫరు);
        మాతృ_స్థానం = current_extra_caps.excaprefs[0];
        untyped = మాతృ_స్థానం->cap;
        మూలం = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        కొరకు (i = 0; i < nASIDPools && riscvKSASIDTable[i]; i++);

        ఒకవేళ (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_మొదటి;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        asid_పునాది = i << asid_దిగువ_బిట్లు;

        ఒకవేళ (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_cap_ఖండం_కొలత(untyped) != se_L4_ASIDPool_బిట్లు ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        స్థితి_2 = ensureNoChildren(మాతృ_స్థానం);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి స్థితి_2;
        }

        చట్రం = మాట_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_లక్ష్యం_స్థానం(మూలం, సూచీ, లోతు);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి lu_ret.స్థితి_2;
        }
        dest_స్థానం = lu_ret.స్థానం;

        స్థితి_2 = ensure_ఖాళీలు_స్థానం(dest_స్థానం);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి స్థితి_2;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performASIDControlInvocation(చట్రం, dest_స్థానం, మాతృ_స్థానం, asid_పునాది);
    }

    స్థితి cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspace_Cap_స్థానం;
        asid_pool_t *pool;
        మాట_t i;
        asid_t       asid;

        ఒకవేళ (label != RISCVASIDPool_కేటాయించు) {
            current_syscall_error.type = seL4_IllegalOperation;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        ఒకవేళ (current_extra_caps.excaprefs[0] == ఖాళీ) {
            current_syscall_error.type = se_L4_Truncated_సందేశం;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_స్థానం = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_స్థానం->cap;

        ఒకవేళ (unlikely(
                cap_get_capType(vspaceCap) != cap_పుట_పట్టిక_cap ||
                cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(vspaceCap))) {
            userError("RISCVASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        pool = riscvKSASIDTable[ASID_ఎగువ(cap_asid_pool_cap_get_capASIDBase(cap))];
        ఒకవేళ (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = false;
            current_lookup_fault = lookup_fault_invalid_మూలం_new();
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        కొరకు (i = 0; i < బిట్టు(asid_దిగువ_బిట్లు) && (asid + i == 0 || pool->array[i]); i++);

        ఒకవేళ (i == బిట్టు(asid_దిగువ_బిట్లు)) {
            current_syscall_error.type = se_L4_Delete_మొదటి;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performASIDPoolInvocation(asid, pool, vspace_Cap_స్థానం);
    }
    అప్రమేయ:
        fail("Invalid arch cap type");
    }
}

exception_t perform_పుట_పట్టిక_Invocation_Map(cap_t cap, cte_t *ct_స్థానం,
                                          pte_t pte, pte_t *pt_స్థానం)
{
    ct_స్థానం->cap = cap;
    *pt_స్థానం = pte;
    sfence();

    తిరిగి EXCEPTION_NONE;
}

exception_t perform_పుట_పట్టిక_Invocation_Unmap(cap_t cap, cte_t *ct_స్థానం)
{
    ఒకవేళ (cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap));
        unmap_పుట_పట్టిక(
            cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(cap),
            cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(cap),
            pt
        );
        clear_జ్ఞాపకం((శూన్య *)pt, se_L4_పుట_పట్టిక_బిట్లు);
    }
    cap_పుట_పట్టిక_cap_ptr_సమితి_cap_PTIs_Mapped(&(ct_స్థానం->cap), 0);

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_పుట_Get_చిరునామా(శూన్య *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = కణుపు_STATE(ksCurThread);
    ఒకవేళ (call) {
        మాట_t *ipc_బఫరు = lookupIPCBuffer(true, thread);
        సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);
        గుర్తులేని పూర్ణాంకం నిడివి = సమితి_MR(thread, ipc_బఫరు, 0, capFBasePtr);
        సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                        se_L4_సందేశం_Info_new(0, 0, 0, నిడివి)));
    }
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Running);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t updatePTE(pte_t pte, pte_t *పునాది)
{
    *పునాది = pte;
    sfence();
    తిరిగి EXCEPTION_NONE;
}

exception_t perform_పుట_Invocation_Map_PTE(cap_t cap, cte_t *ct_స్థానం,
                                        pte_t pte, pte_t *పునాది)
{
    ct_స్థానం->cap = cap;
    తిరిగి updatePTE(pte, పునాది);
}

exception_t perform_పుట_Invocation_Unmap(cap_t cap, cte_t *ct_స్థానం)
{
    ఒకవేళ (cap_చట్రం_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        unmap_పుట(cap_చట్రం_cap_get_cap_FSize(cap),
                  cap_చట్రం_cap_get_cap_FMapped_ASID(cap),
                  cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap),
                  cap_చట్రం_cap_get_cap_FBase_Ptr(cap)
                 );
    }

    cap_t స్థానం_Cap = ct_స్థానం->cap;
    స్థానం_Cap = cap_చట్రం_cap_సమితి_cap_FMapped_చిరునామా(స్థానం_Cap, 0);
    స్థానం_Cap = cap_చట్రం_cap_సమితి_cap_FMapped_ASID(స్థానం_Cap, asidInvalid);
    ct_స్థానం->cap = స్థానం_Cap;

    తిరిగి EXCEPTION_NONE;
}

#నిర్వచితమైతే CONFIG_PRINTING
శూన్య Arch_user_దొంతర_Trace(tcb_t *tptr)
{
    cap_t thread_మూలం = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;
    ఒకవేళ (!is_Valid_VTable_మూలం(thread_మూలం)) {
        రాయి("Invalid vspace\n");
        తిరిగి;
    }

    మాట_t sp = get_రిజిస్టరు(tptr, SP);
    ఒకవేళ (!IS_ALIGNED(sp, se_L4_మాట_కొలత_బిట్లు)) {
        రాయి("SP %p not aligned", (శూన్య *) sp);
        తిరిగి;
    }

    pte_t *vspace_మూలం = PTE_PTR(pptr_యొక్క_cap(thread_మూలం));
    కొరకు (పూర్ణాంకం i = 0; i < CONFIG_USER_దొంతర_TRACE_నిడివి; i++) {
        మాట_t చిరునామా = sp + (i * పరిమాణం(మాట_t));
        lookupPTSlot_ret_t ret = lookupPTSlot(vspace_మూలం, చిరునామా);
        ఒకవేళ (చిరునామా < USER_శిఖరం && pte_ptr_get_valid(ret.pt_స్థానం) && !is_PTEPage_పట్టిక(ret.pt_స్థానం)) {
            pptr_t pptr = (pptr_t)(getPPtrFromHWPTE(ret.pt_స్థానం));
            మాట_t *విలువ = (మాట_t *)((మాట_t)pptr + (చిరునామా & ముసుగు(ret.pt_బిట్లు_ఎడమ)));
            రాయి("0x%lx: 0x%lx\n", (దీర్ఘ) చిరునామా, (దీర్ఘ) *విలువ);
        } లేకపోతే {
            రాయి("0x%lx: INVALID\n", (దీర్ఘ) చిరునామా);
        }
    }
}
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_KERNEL_LOG_బఫరు
exception_t benchmark_arch_map_log_బఫరు(మాట_t చట్రం_cptr)
{
    lookup_Cap_And_స్థానం_ret_t lu_ret;
    vm_పుట_కొలత_t చట్రం_కొలత;
    pptr_t  చట్రం_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_స్థానం(కణుపు_STATE(ksCurThread), చట్రం_cptr);

    ఒకవేళ (unlikely(lu_ret.స్థితి_2 != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", చట్రం_cptr);
        current_fault = seL4_Fault_CapFault_new(చట్రం_cptr, false);

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (cap_get_capType(lu_ret.cap) != cap_చట్రం_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(చట్రం_cptr, false);

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    చట్రం_కొలత = cap_చట్రం_cap_get_cap_FSize(lu_ret.cap);

    ఒకవేళ (చట్రం_కొలత != RISCV_Mega_పుట) {
        userError("Invalid frame size. The kernel expects large page log buffer");
        current_fault = seL4_Fault_CapFault_new(చట్రం_cptr, false);

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    చట్రం_pptr = cap_చట్రం_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_బఫరు = pptr_గమ్యం_paddr((శూన్య *) చట్రం_pptr);

#ఒకవేళ __riscv_xlen == 32
    paddr_t physical_చిరునామా = ks_User_Log_బఫరు;
    కొరకు (మాట_t i = 0; i < బిట్టు(PT_సూచీ_బిట్లు); i += 1) {
        kernel_image_level1_log_బఫరు_pt[i] = pte_తదుపరి(physical_చిరునామా, true);
        physical_చిరునామా += బిట్టు(పుట_బిట్లు);
    }
    ప్రతిజ్ఞా(physical_చిరునామా - ks_User_Log_బఫరు == బిట్టు(se_L4_Large_పుట_బిట్లు));
#లేకపోతే
    kernel_image_level1_dev_pt[RISCV_GET_PT_సూచీ(KS_LOG_PPTR, 1)] = pte_తదుపరి(ks_User_Log_బఫరు, true);
#అయితే_ముగింపు

    sfence();

    తిరిగి EXCEPTION_NONE;
}
#అయితే_ముగింపు /* CONFIG_KERNEL_LOG_BUFFER */
