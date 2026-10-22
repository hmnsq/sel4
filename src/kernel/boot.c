/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <assert.h>
#समावेश <kernel/boot.h>
#समावेश <kernel/thread.h>
#समावेश <machine/io.h>
#समावेश <machine/registerset.h>
#समावेश <model/statedata.h>
#समावेश <object/domain.h>
#समावेश <arch/machine.h>
#समावेश <arch/kernel/boot.h>
#समावेश <arch/kernel/vspace.h>
#समावेश <linker.h>
#समावेश <hardware.h>
#समावेश <util.h>

/* (node-local) state accessed only during bootstrapping */
BOOT_BSS ndks_boot_t ndks_boot;

BOOT_BSS rootserver_mem_t rootserver;
BOOT_BSS अटल region_t rootserver_mem;

/* Returns the physical region of the kernel image boot part, which is the part
 * that is no longer needed once booting is finished. */
बाह्य अक्षर ki_boot_end[1];
BOOT_CODE p_region_t get_p_reg_kernel_img_boot(व्योम)
{
    वापस (p_region_t) {
        .start = kpptr_to_paddr((स्थिर व्योम *)KERNEL_ELF_BASE),
        .end   = kpptr_to_paddr(ki_boot_end)
    };
}

/* Returns the physical region of the kernel image. */
BOOT_CODE p_region_t get_p_reg_kernel_img(व्योम)
{
    वापस (p_region_t) {
        .start = kpptr_to_paddr((स्थिर व्योम *)KERNEL_ELF_BASE),
        .end   = kpptr_to_paddr((स्थिर व्योम *)KERNEL_ELF_शीर्ष)
    };
}

BOOT_CODE अटल व्योम merge_regions(व्योम)
{
    /* Walk through reserved regions and see if any can be merged */
    word_t शीर्ष_2 = 0;
    क्रम (word_t i = 1; i < ndks_boot.resv_गिनती; i++) {
        अगर (ndks_boot.reserved[शीर्ष_2].end == ndks_boot.reserved[i].start) {
            /* extend earlier region */
            ndks_boot.reserved[शीर्ष_2].end = ndks_boot.reserved[i].end;
        } अन्यथा {
            /* add region */
            शीर्ष_2++;
            ndks_boot.reserved[शीर्ष_2] = ndks_boot.reserved[i];
        }
    }
    ndks_boot.resv_गिनती = ndks_boot.resv_गिनती > 0 ? शीर्ष_2 + 1 : 0;
}

BOOT_CODE bool_t reserve_region(p_region_t reg)
{
    word_t i;
    निश्चित(reg.start <= reg.end);
    अगर (reg.start == reg.end) {
        वापस true;
    }

    /* keep the regions in order */
    क्रम (i = 0; i < ndks_boot.resv_गिनती; i++) {
        /* Try and merge the region to an existing one, if possible */
        अगर (ndks_boot.reserved[i].start == reg.end) {
            ndks_boot.reserved[i].start = reg.start;
            merge_regions();
            वापस true;
        }
        अगर (ndks_boot.reserved[i].end == reg.start) {
            ndks_boot.reserved[i].end = reg.end;
            merge_regions();
            वापस true;
        }
        /* Otherwise figure out where it should go. */
        अगर (ndks_boot.reserved[i].start > reg.end) {
            /* move regions down, making sure there's enough room */
            अगर (ndks_boot.resv_गिनती + 1 >= MAX_अंक_RESV_REG) {
                म_लिखो("Can't mark region 0x%"SEL4_PRIx_word"-0x%"SEL4_PRIx_word
                       " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
                       reg.start, reg.end, (पूर्णांक)MAX_अंक_RESV_REG);
                वापस false;
            }
            क्रम (word_t j = ndks_boot.resv_गिनती; j > i; j--) {
                ndks_boot.reserved[j] = ndks_boot.reserved[j - 1];
            }
            /* insert the new region */
            ndks_boot.reserved[i] = reg;
            ndks_boot.resv_गिनती++;
            वापस true;
        }
    }

    अगर (i + 1 == MAX_अंक_RESV_REG) {
        म_लिखो("Can't mark region 0x%"SEL4_PRIx_word"-0x%"SEL4_PRIx_word
               " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
               reg.start, reg.end, (पूर्णांक)MAX_अंक_RESV_REG);
        वापस false;
    }

    ndks_boot.reserved[i] = reg;
    ndks_boot.resv_गिनती++;

    वापस true;
}

BOOT_CODE अटल bool_t जोड़ो_region(region_t reg)
{
    निश्चित(reg.start <= reg.end);
    अगर (is_reg_empty(reg)) {
        वापस true;
    }

    क्रम (word_t i = 0; i < ARRAY_आकार(ndks_boot.freemem); i++) {
        अगर (is_reg_empty(ndks_boot.freemem[i])) {
            reserve_region(pptr_to_paddr_reg(reg));
            ndks_boot.freemem[i] = reg;
            वापस true;
        }
    }

    /* We don't know if a platform or architecture picked MAX_NUM_FREEMEM_REG
     * arbitrarily or carefully calculated it to be big enough. Running out of
     * slots here is not really fatal, eventually memory allocation may fail
     * if there is not enough free memory. However, allocations should never
     * blindly assume to work, some error handling must always be in place even
     * if the environment has been crafted carefully to support them. Thus, we
     * don't stop the boot process here, but return an error. The caller should
     * decide how bad this is.
     */
    म_लिखो("no free memory slot left for [%"SEL4_PRIx_word"..%"SEL4_PRIx_word"),"
           " consider increasing MAX_NUM_FREEMEM_REG (%u)\n",
           reg.start, reg.end, (अचिन्हित पूर्णांक)MAX_अंक_FREEMEM_REG);

    /* For debug builds we consider this a fatal error. Rationale is, that the
     * caller does not check the error code at the moment, but just ignores any
     * failures silently. */
    निश्चित(0);

    वापस false;
}

BOOT_CODE अटल pptr_t alloc_rootserver_obj(word_t आकार_bits, word_t n)
{
    pptr_t allocated = rootserver_mem.start;
    /* allocated memory must be aligned */
    निश्चित(allocated % BIT(आकार_bits) == 0);
    rootserver_mem.start += (n * BIT(आकार_bits));
    /* we must not have run out of memory */
    निश्चित(rootserver_mem.start <= rootserver_mem.end);
    memzero((व्योम *) allocated, n * BIT(आकार_bits));
    वापस allocated;
}

BOOT_CODE अटल word_t rootserver_max_आकार_bits(word_t extra_bi_आकार_bits)
{
    word_t cnode_आकार_bits = CONFIG_मूल_CNODE_आकार_BITS + seL4_SlotBits;
    word_t max = MAX(cnode_आकार_bits, seL4_VSpaceBits);
    वापस MAX(max, extra_bi_आकार_bits);
}

BOOT_CODE अटल word_t calculate_rootserver_आकार(v_region_t it_v_reg, word_t extra_bi_आकार_bits)
{
    /* work out how much memory we need for root server objects */
    word_t आकार = BIT(CONFIG_मूल_CNODE_आकार_BITS + seL4_SlotBits);
    आकार += BIT(seL4_TCBBits); // root thread tcb
    आकार += BIT(seL4_PageBits); // ipc buf
    आकार += BIT(seL4_BootInfoFrameBits); // boot info
    आकार += BIT(seL4_ASIDPoolBits);
    आकार += extra_bi_आकार_bits > 0 ? BIT(extra_bi_आकार_bits) : 0;
    आकार += BIT(seL4_VSpaceBits); // root vspace
#अगर_घोषित CONFIG_KERNEL_MCS
    आकार += BIT(seL4_MinSchedContextBits); // root sched context
#पूर्ण_अगर
    /* for all archs, seL4_PageTable Bits is the size of all non top-level paging structures */
    वापस आकार + arch_get_n_paging(it_v_reg) * BIT(seL4_PageTableBits);
}

BOOT_CODE अटल व्योम maybe_alloc_extra_bi(word_t cmp_आकार_bits, word_t extra_bi_आकार_bits)
{
    अगर (extra_bi_आकार_bits >= cmp_आकार_bits && rootserver.extra_bi == 0) {
        rootserver.extra_bi = alloc_rootserver_obj(extra_bi_आकार_bits, 1);
    }
}

/* Create pptrs for all root server objects, starting at a give start address,
 * to cover the virtual memory region v_reg, and any extra boot info.
 */
BOOT_CODE अटल व्योम create_rootserver_objects(pptr_t start, v_region_t it_v_reg,
                                                word_t extra_bi_आकार_bits)
{
    /* the largest object the PD, the root cnode, or the extra boot info */
    word_t cnode_आकार_bits = CONFIG_मूल_CNODE_आकार_BITS + seL4_SlotBits;
    word_t max = rootserver_max_आकार_bits(extra_bi_आकार_bits);

    word_t आकार = calculate_rootserver_आकार(it_v_reg, extra_bi_आकार_bits);
    rootserver_mem.start = start;
    rootserver_mem.end = start + आकार;

    maybe_alloc_extra_bi(max, extra_bi_आकार_bits);

    /* the root cnode is at least 4k, so it could be larger or smaller than a pd. */
#अगर (CONFIG_मूल_CNODE_आकार_BITS + seL4_SlotBits) > seL4_VSpaceBits
    rootserver.cnode = alloc_rootserver_obj(cnode_आकार_bits, 1);
    maybe_alloc_extra_bi(seL4_VSpaceBits, extra_bi_आकार_bits);
    rootserver.vspace = alloc_rootserver_obj(seL4_VSpaceBits, 1);
#अन्यथा
    rootserver.vspace = alloc_rootserver_obj(seL4_VSpaceBits, 1);
    maybe_alloc_extra_bi(cnode_आकार_bits, extra_bi_आकार_bits);
    rootserver.cnode = alloc_rootserver_obj(cnode_आकार_bits, 1);
#पूर्ण_अगर

    /* at this point we are up to creating 4k objects - which is the min size of
     * extra_bi so this is the last chance to allocate it */
    maybe_alloc_extra_bi(seL4_PageBits, extra_bi_आकार_bits);
    compile_assert(invalid_seL4_ASIDPoolBits, seL4_ASIDPoolBits == seL4_PageBits);
    rootserver.asid_pool = alloc_rootserver_obj(seL4_ASIDPoolBits, 1);
    rootserver.ipc_buf = alloc_rootserver_obj(seL4_PageBits, 1);
    /* The boot info size must be at least one page. Due to the hard-coded order
     * of allocations used in the current implementation here, it can't be any
     * bigger.
     */
    compile_assert(invalid_seL4_BootInfoFrameBits, seL4_BootInfoFrameBits == seL4_PageBits);
    rootserver.boot_info = alloc_rootserver_obj(seL4_BootInfoFrameBits, 1);

    /* TCBs on aarch32 can be larger than page tables in certain configs */
#अगर seL4_TCBBits >= seL4_PageTableBits
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#पूर्ण_अगर

    /* paging structures are 4k on every arch except aarch32 (1k) */
    word_t n = arch_get_n_paging(it_v_reg);
    rootserver.paging.start = alloc_rootserver_obj(seL4_PageTableBits, n);
    rootserver.paging.end = rootserver.paging.start + n * BIT(seL4_PageTableBits);

    /* for most archs, TCBs are smaller than page tables */
#अगर seL4_TCBBits < seL4_PageTableBits
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#पूर्ण_अगर

#अगर_घोषित CONFIG_KERNEL_MCS
    rootserver.sc = alloc_rootserver_obj(seL4_MinSchedContextBits, 1);
#पूर्ण_अगर
    /* we should have allocated all our memory */
    निश्चित(rootserver_mem.start == rootserver_mem.end);
}

BOOT_CODE व्योम write_slot(slot_ptr_t slot_ptr, cap_t cap)
{
    slot_ptr->cap = cap;

    slot_ptr->cteMDBNode = nullMDBNode;
    mdb_गाँठ_ptr_set_mdb_Revocable(&slot_ptr->cteMDBNode, true);
    mdb_गाँठ_ptr_set_mdb_First_Badged(&slot_ptr->cteMDBNode, true);
}

/* Our root CNode needs to be able to fit all the initial caps and not
 * cover all of memory.
 */
compile_assert(मूल_cnode_आकार_valid,
               CONFIG_मूल_CNODE_आकार_BITS < 32 - seL4_SlotBits &&
               BIT(CONFIG_मूल_CNODE_आकार_BITS) >= se_L4_अंक_Initial_Caps &&
               CONFIG_मूल_CNODE_आकार_BITS >= (seL4_PageBits - seL4_SlotBits))

BOOT_CODE cap_t
create_मूल_cnode(व्योम)
{
    cap_t cap = cap_cnode_cap_new(
                    CONFIG_मूल_CNODE_आकार_BITS, /* radix */
                    wordBits - CONFIG_मूल_CNODE_आकार_BITS, /* guard size */
                    0, /* guard */
                    rootserver.cnode); /* pptr */

    /* write the root CNode cap into the root CNode */
    write_slot(SLOT_PTR(rootserver.cnode, seL4_CapInitThreadCNode), cap);

    वापस cap;
}

/* Check domain scheduler assumptions. */
compile_assert(अंक_domains_valid,
               CONFIG_अंक_DOMAINS >= 1 && CONFIG_अंक_DOMAINS <= 256)
compile_assert(अंक_priorities_valid,
               CONFIG_अंक_PRIORITIES >= 1 && CONFIG_अंक_PRIORITIES <= 256)

BOOT_CODE व्योम
create_domain_cap(cap_t मूल_cnode_cap)
{
    cap_t cap = cap_domain_cap_new();
    write_slot(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapDomain), cap);
}

BOOT_CODE cap_t create_ipcbuf_frame_cap(cap_t मूल_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    clearMemory((व्योम *)rootserver.ipc_buf, PAGE_BITS);

    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_frame_cap(pd_cap, rootserver.ipc_buf, vptr, IT_ASID, false, false);
    write_slot(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapInitThreadIPCBuffer), cap);

    वापस cap;
}

BOOT_CODE व्योम create_bi_frame_cap(cap_t मूल_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_frame_cap(pd_cap, rootserver.boot_info, vptr, IT_ASID, false, false);
    write_slot(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapBootInfoFrame), cap);
}

/**
 * the size_bits we return is 0 for extra_size = 0
 * and if it is non-zero the bits are always >= seL4_PageBits
 * this is relied on in a few places, and gives us code of the form
 *
 *     extra_bi_size_bits > 0 ? BIT(extra_bi_size_bits) : 0
 *
 * which handles the 0-size case.
 */
BOOT_CODE word_t calculate_extra_bi_आकार_bits(word_t extra_आकार)
{
    अगर (extra_आकार == 0) {
        वापस 0;
    }

    word_t clzl_ret = clzl(ROUND_UP(extra_आकार, seL4_PageBits));
    word_t msb = seL4_WordBits - 1 - clzl_ret;
    /* If region is bigger than a page, make sure we overallocate rather than
     * underallocate
     */
    अगर (extra_आकार > BIT(msb)) {
        msb++;
    }
    वापस msb;
}

BOOT_CODE व्योम populate_bi_frame(गाँठ_id_t गाँठ_id, word_t अंक_nodes,
                                 vptr_t ipcbuf_vptr, word_t extra_bi_आकार)
{
    /* clear boot info memory */
    clearMemory((व्योम *)rootserver.boot_info, seL4_BootInfoFrameBits);
    अगर (extra_bi_आकार) {
        clearMemory((व्योम *)rootserver.extra_bi,
                    calculate_extra_bi_आकार_bits(extra_bi_आकार));
    }

    /* initialise bootinfo-related global state */
    seL4_BootInfo *bi = BI_PTR(rootserver.boot_info);
    bi->गाँठ_ID = गाँठ_id;
    bi->अंक_Nodes = अंक_nodes;
    bi->अंक_IOPTLevels = 0;
    bi->ipc_बफर = (seL4_IPCBuffer *)ipcbuf_vptr;
    bi->init_Thread_CNode_आकार_Bits = CONFIG_मूल_CNODE_आकार_BITS;
    bi->initThreadDomain = 0;
    bi->extraLen = extra_bi_आकार;

    ndks_boot.bi_frame = bi;
    ndks_boot.slot_pos_cur = se_L4_अंक_Initial_Caps;
}

BOOT_CODE bool_t provide_cap(cap_t मूल_cnode_cap, cap_t cap)
{
    अगर (ndks_boot.slot_pos_cur >= BIT(CONFIG_मूल_CNODE_आकार_BITS)) {
        म_लिखो("ERROR: can't add another cap, all %"SEL4_PRIu_word
               " (=2^CONFIG_ROOT_CNODE_SIZE_BITS) slots used\n",
               BIT(CONFIG_मूल_CNODE_आकार_BITS));
        वापस false;
    }
    write_slot(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), ndks_boot.slot_pos_cur), cap);
    ndks_boot.slot_pos_cur++;
    वापस true;
}

BOOT_CODE create_frames_of_region_ret_t create_frames_of_region(
    cap_t    मूल_cnode_cap,
    cap_t    pd_cap,
    region_t reg,
    bool_t   do_map,
    sword_t  pv_offset
)
{
    pptr_t     f;
    cap_t      frame_cap;
    seL4_SlotPos slot_pos_before;
    seL4_SlotPos slot_pos_after;

    slot_pos_before = ndks_boot.slot_pos_cur;

    क्रम (f = reg.start; f < reg.end; f += BIT(PAGE_BITS)) {
        अगर (do_map) {
            frame_cap = create_mapped_it_frame_cap(pd_cap, f, pptr_to_paddr((व्योम *)(f - pv_offset)), IT_ASID, false, true);
        } अन्यथा {
            frame_cap = create_unmapped_it_frame_cap(f, false);
        }
        अगर (!provide_cap(मूल_cnode_cap, frame_cap)) {
            वापस (create_frames_of_region_ret_t) {
                .region  = S_REG_EMPTY,
                .success = false
            };
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;

    वापस (create_frames_of_region_ret_t) {
        .region = (seL4_SlotRegion) {
            .start = slot_pos_before,
            .end   = slot_pos_after
        },
        .success = true
    };
}

BOOT_CODE cap_t create_it_asid_pool(cap_t मूल_cnode_cap)
{
    cap_t ap_cap = cap_asid_pool_cap_new(ASID_ऊपरी(IT_ASID) << asid_निचला_Bits, rootserver.asid_pool);
    write_slot(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapInitThreadASIDPool), ap_cap);

    /* create ASID control cap */
    write_slot(
        SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapASIDControl),
        cap_asid_control_cap_new()
    );

    वापस ap_cap;
}

#अगर_घोषित CONFIG_KERNEL_MCS
BOOT_CODE अटल व्योम configure_sched_context(tcb_t *tcb, sched_context_t *sc_pptr, ticks_t timeslice)
{
    tcb->tcbSchedContext = sc_pptr;
    refill_new(tcb->tcbSchedContext, MIN_REFILLS, timeslice, 0);
    tcb->tcbSchedContext->scTcb = tcb;
}

BOOT_CODE bool_t init_sched_control(cap_t मूल_cnode_cap, word_t अंक_nodes)
{
    seL4_SlotPos slot_pos_before = ndks_boot.slot_pos_cur;

    /* create a sched control cap for each core */
    क्रम (अचिन्हित पूर्णांक i = 0; i < अंक_nodes; i++) {
        अगर (!provide_cap(मूल_cnode_cap, cap_sched_control_cap_new(i))) {
            म_लिखो("can't init sched_control for node %u, provide_cap() failed\n", i);
            वापस false;
        }
    }

    /* update boot info with slot region for sched control caps */
    ndks_boot.bi_frame->schedcontrol = (seL4_SlotRegion) {
        .start = slot_pos_before,
        .end = ndks_boot.slot_pos_cur
    };

    वापस true;
}
#पूर्ण_अगर

BOOT_CODE व्योम create_idle_thread(व्योम)
{
    pptr_t pptr;

#अगर_घोषित ENABLE_SMP_SUPPORT
    क्रम (अचिन्हित पूर्णांक i = 0; i < CONFIG_MAX_अंक_NODES; i++) {
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */
        pptr = (pptr_t) &ksIdleThreadTCB[SMP_TERNARY(i, 0)];
        गाँठ_STATE_ON_CORE(ksIdleThread, i) = TCB_PTR(pptr + TCB_OFFSET);
        configureIdleThread(गाँठ_STATE_ON_CORE(ksIdleThread, i));
#अगर_घोषित CONFIG_DEBUG_BUILD
        setThreadName(गाँठ_STATE_ON_CORE(ksIdleThread, i), "idle_thread");
#पूर्ण_अगर
        SMP_COND_STATEMENT(गाँठ_STATE_ON_CORE(ksIdleThread, i)->tcbAffinity = i);
#अगर_घोषित CONFIG_KERNEL_MCS
        configure_sched_context(गाँठ_STATE_ON_CORE(ksIdleThread, i), SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]),
                                MAX_PERIOD_TICKS);
        SMP_COND_STATEMENT(गाँठ_STATE_ON_CORE(ksIdleThread, i)->tcbSchedContext->scCore = i;)
        गाँठ_STATE_ON_CORE(ksIdleSC, i) = SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]);
#पूर्ण_अगर
#अगर_घोषित ENABLE_SMP_SUPPORT
    }
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */
}

BOOT_CODE tcb_t *create_initial_thread(cap_t मूल_cnode_cap, cap_t it_pd_cap, vptr_t ui_v_entry, vptr_t bi_frame_vptr,
                                       vptr_t ipcbuf_vptr, cap_t ipcbuf_cap)
{
    tcb_t *tcb = TCB_PTR(rootserver.tcb + TCB_OFFSET);
#अगर_अघोषित CONFIG_KERNEL_MCS
    tcb->tcbTimeSlice = CONFIG_TIME_SLICE;
#पूर्ण_अगर

    Arch_initContext(&tcb->tcbArch.tcbContext);

    /* derive a copy of the IPC buffer cap for inserting */
    deriveCap_ret_t dc_ret = deriveCap(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapInitThreadIPCBuffer), ipcbuf_cap);
    अगर (dc_ret.status != EXCEPTION_NONE) {
        म_लिखो("Failed to derive copy of IPC Buffer\n");
        वापस शून्य;
    }

    /* initialise TCB (corresponds directly to abstract specification) */
    cte_जोड़ो(
        मूल_cnode_cap,
        SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapInitThreadCNode),
        SLOT_PTR(rootserver.tcb, tcbCTable)
    );
    cte_जोड़ो(
        it_pd_cap,
        SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapInitThreadVSpace),
        SLOT_PTR(rootserver.tcb, tcbVTable)
    );
    cte_जोड़ो(
        dc_ret.cap,
        SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapInitThreadIPCBuffer),
        SLOT_PTR(rootserver.tcb, tcb_बफर)
    );
    tcb->tcbIPCBuffer = ipcbuf_vptr;

    setRegister(tcb, capRegister, bi_frame_vptr);
    set_अग्र_PC(tcb, ui_v_entry);

    /* initialise TCB */
#अगर_घोषित CONFIG_KERNEL_MCS
    configure_sched_context(tcb, SC_PTR(rootserver.sc), usToTicks(CONFIG_BOOT_THREAD_TIME_SLICE * US_IN_MS));
#पूर्ण_अगर

    tcb->tcbPriority = seL4_MaxPrio;
    tcb->tcbMCP = seL4_MaxPrio;
    tcb->tcbDomain = 0;
#अगर_अघोषित CONFIG_KERNEL_MCS
    setupReplyMaster(tcb);
#पूर्ण_अगर
    setThreadState(tcb, ThreadState_Running);

#अगर_अघोषित CONFIG_KERNEL_MCS
    SMP_COND_STATEMENT(tcb->tcbAffinity = 0);
#पूर्ण_अगर

    /* create initial thread's TCB cap */
    cap_t cap = cap_thread_cap_new(TCB_REF(tcb));
    write_slot(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapInitThreadTCB), cap);

#अगर_घोषित CONFIG_KERNEL_MCS
    cap = cap_sched_context_cap_new(SC_REF(tcb->tcbSchedContext), seL4_MinSchedContextBits);
    write_slot(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapInitThreadSC), cap);
#पूर्ण_अगर
#अगर_घोषित CONFIG_DEBUG_BUILD
    setThreadName(tcb, "rootserver");
#पूर्ण_अगर

    वापस tcb;
}

#अगर_घोषित ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
BOOT_CODE अटल bool_t hypervisor_present(व्योम)
{
#अगर_घोषित CONFIG_ARCH_X86
    uint32_t ebx = x86_cpuid_ebx(KVM_CPUID_SIGNATURE, 0);
    uint32_t ecx = x86_cpuid_ecx(KVM_CPUID_SIGNATURE, 0);
    uint32_t edx = x86_cpuid_edx(KVM_CPUID_SIGNATURE, 0);

    अगर ((ebx == CPUID_KVM_EBX && ecx == CPUID_KVM_ECX && edx == CPUID_KVM_EDX)
        || (ebx == CPUID_TCG_EBX && ecx == CPUID_TCG_ECX && edx == CPUID_TCG_EDX)) {
        वापस true;
    }
#पूर्ण_अगर
    वापस false;
}

BOOT_CODE व्योम clock_sync_test(व्योम)
{
    ticks_t t, t0;
    ticks_t margin = usToTicks(CLOCK_SYNC_DELTA) + getTimerPrecision();

    निश्चित(getCurrentCPUIndex() != 0);
    t = गाँठ_STATE_ON_CORE(ksCurTime, 0);
    करो {
        /* perform a memory acquire to get new values of ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        t0 = गाँठ_STATE_ON_CORE(ksCurTime, 0);
    } जबतक (t0 == t);
    t = getCurrentTime();
    म_लिखो("clock_sync_test[%d]: t0 = %"PRIu64", t = %"PRIu64", td = %"PRIi64"\n",
           (पूर्णांक)getCurrentCPUIndex(), t0, t, t - t0);
    /*
     * The test does not consistently work if we are in a virtual machine (e.g
     * within QEMU) because the measurement cannot distinguish between
     * interrupted clock reads and out-of-sync clocks.
     */
    अगर (hypervisor_present()) {
        म_लिखो("clock_sync_test[%d]: disabled, detected running as VM\n", (पूर्णांक)getCurrentCPUIndex());
    } अन्यथा {
        निश्चित(t0 <= margin + t && t <= t0 + margin);
    }
}
#पूर्ण_अगर

BOOT_CODE व्योम init_core_state(tcb_t *scheduler_action)
{
#अगर_घोषित CONFIG_HAVE_FPU
    गाँठ_STATE(ksCurFPUOwner) = शून्य;
#पूर्ण_अगर
#अगर_घोषित CONFIG_DEBUG_BUILD
    /* add initial threads to the debug queue */
    गाँठ_STATE(ksDebugTCBs) = शून्य;
    अगर (scheduler_action != SchedulerAction_ResumeCurrentThread &&
        scheduler_action != SchedulerAction_ChooseNewThread) {
        tcbDebugAppend(scheduler_action);
    }
    tcbDebugAppend(गाँठ_STATE(ksIdleThread));
#पूर्ण_अगर
    गाँठ_STATE(ksSchedulerAction) = scheduler_action;
    गाँठ_STATE(ksCurThread) = गाँठ_STATE(ksIdleThread);
#अगर_घोषित CONFIG_KERNEL_MCS
    गाँठ_STATE(ksCurSC) = गाँठ_STATE(ksCurThread->tcbSchedContext);
    गाँठ_STATE(ksConsumed) = 0;
    गाँठ_STATE(ksReprogram) = true;
    गाँठ_STATE(ksReleaseQueue.शीर्ष) = शून्य;
    गाँठ_STATE(ksReleaseQueue.end) = शून्य;
    गाँठ_STATE(ksCurTime) = getCurrentTime();
#पूर्ण_अगर
    /* No need for NODE_STATE() as there is no SMP support for domains */
    ksCurDomain = 0;
    ksDomainTime = DSCHED_MAX_DURATION;
    ksDomSchedule[0] = dschedule_make(0, DSCHED_MAX_DURATION);
}

/**
 * Sanity check if a kernel-virtual pointer is in the kernel window that maps
 * physical memory.
 *
 * This check is necessary, but not sufficient, because it only checks for the
 * pointer interval, not for any potential holes in the memory window.
 *
 * @param pptr the pointer to check
 * @return false if the pointer is definitely not in the kernel window, true
 *         otherwise.
 */
BOOT_CODE अटल bool_t pptr_in_kernel_window(pptr_t pptr)
{
    वापस pptr >= PPTR_BASE && pptr < PPTR_शीर्ष;
}

/**
 * Create an untyped cap, store it in a cnode and mark it in boot info.
 *
 * The function can fail if basic sanity checks fail, or if there is no space in
 * boot info or cnode to store the cap.
 *
 * @param root_cnode_cap cap to the cnode to store the untyped cap in
 * @param device_memory true if the cap to create is a device untyped
 * @param pptr the kernel-virtual address of the untyped
 * @param size_bits the size of the untyped in bits
 * @param first_untyped_slot next available slot in the boot info structure
 * @return true on success, false on failure
 */
BOOT_CODE अटल bool_t provide_untyped_cap(
    cap_t      मूल_cnode_cap,
    bool_t     device_memory,
    pptr_t     pptr,
    word_t     आकार_bits,
    seL4_SlotPos first_untyped_slot
)
{
    bool_t ret;
    cap_t ut_cap;

    /* Since we are in boot code, we can do extensive error checking and
       return failure if anything unexpected happens. */

    /* Bounds check for size parameter */
    अगर (आकार_bits > seL4_MaxUntypedBits || आकार_bits < seL4_MinUntypedBits) {
        म_लिखो("Kernel init: Invalid untyped size %"SEL4_PRIu_word"\n", आकार_bits);
        वापस false;
    }

    /* All cap ptrs must be aligned to object size */
    अगर (!IS_ALIGNED(pptr, आकार_bits)) {
        म_लिखो("Kernel init: Unaligned untyped pptr %p (alignment %"SEL4_PRIu_word")\n", (व्योम *)pptr, आकार_bits);
        वापस false;
    }

    /* All cap ptrs apart from device untypeds must be in the kernel window. */
    अगर (!device_memory && !pptr_in_kernel_window(pptr)) {
        म_लिखो("Kernel init: Non-device untyped pptr %p outside kernel window\n",
               (व्योम *)pptr);
        वापस false;
    }

    /* Check that the end of the region is also in the kernel window, so we don't
       need to assume that the kernel window is aligned up to potentially
       seL4_MaxUntypedBits. */
    अगर (!device_memory && !pptr_in_kernel_window(pptr + MASK(आकार_bits))) {
        म_लिखो("Kernel init: End of non-device untyped at %p outside kernel window (size %"SEL4_PRIu_word")\n",
               (व्योम *)pptr, आकार_bits);
        वापस false;
    }

    word_t i = ndks_boot.slot_pos_cur - first_untyped_slot;
    अगर (i < CONFIG_MAX_अंक_BOOTINFO_UNTYPED_CAPS) {
        ndks_boot.bi_frame->untypedList[i] = (seL4_UntypedDesc) {
            .paddr    = pptr_to_paddr((व्योम *)pptr),
            .आकार_Bits = आकार_bits,
            .isDevice = device_memory,
            .padding  = {0}
        };
        ut_cap = cap_untyped_cap_new(MAX_FREE_सूचकांक(आकार_bits),
                                     device_memory, आकार_bits, pptr);
        ret = provide_cap(मूल_cnode_cap, ut_cap);
    } अन्यथा {
        म_लिखो("Kernel init: Too many untyped regions for boot info\n");
        ret = true;
    }
    वापस ret;
}

/**
 * Create untyped caps for a region of kernel-virtual memory.
 *
 * Takes care of alignment, size and potentially wrapping memory regions. It is fine to provide a
 * region with end < start if the memory is device memory.
 *
 * If the region start is not aligned to seL4_MinUntypedBits, the part up to the next aligned
 * address will be ignored and is lost, because it is too small to create kernel objects in.
 *
 * @param root_cnode_cap Cap to the CNode to store the untypeds in.
 * @param device_memory  Whether the region is device memory.
 * @param reg Region of kernel-virtual memory. May wrap around.
 * @param first_untyped_slot First available untyped boot info slot.
 * @return true on success, false on failure.
 */
BOOT_CODE अटल bool_t create_untypeds_for_region(
    cap_t      मूल_cnode_cap,
    bool_t     device_memory,
    region_t   reg,
    seL4_SlotPos first_untyped_slot
)
{
    /* This code works with regions that wrap (where end < start), because the loop cuts up the
       region into size-aligned chunks, one for each cap. Memory chunks that are size-aligned cannot
       themselves overflow, so they satisfy alignment, size, and overflow conditions. The region
       [0..end) is not necessarily part of the kernel window (depending on the value of PPTR_BASE).
       This is fine for device untypeds. For normal untypeds, the region is assumed to be fully in
       the kernel window. This is not checked here. */
    जबतक (!is_reg_empty(reg)) {

        /* Calculate the bit size of the region. This is also correct for end < start: it will
           return the correct size of the set [start..-1] union [0..end). This will then be too
           large for alignment, so the code further down will reduce the size. */
        अचिन्हित पूर्णांक आकार_bits = seL4_WordBits - 1 - clzl(reg.end - reg.start);
        /* The size can't exceed the largest possible untyped size. */
        अगर (आकार_bits > seL4_MaxUntypedBits) {
            आकार_bits = seL4_MaxUntypedBits;
        }
        /* The start address 0 satisfies any alignment needs, otherwise ensure
         * the region's bit size does not exceed the alignment of the region.
         */
        अगर (0 != reg.start) {
            अचिन्हित पूर्णांक align_bits = ctzl(reg.start);
            अगर (आकार_bits > align_bits) {
                आकार_bits = align_bits;
            }
        }
        /* Provide an untyped capability for the region only if it is large
         * enough to be retyped into objects later. Otherwise the region can't
         * be used anyway.
         */
        अगर (आकार_bits >= seL4_MinUntypedBits) {
            अगर (!provide_untyped_cap(मूल_cnode_cap, device_memory, reg.start, आकार_bits, first_untyped_slot)) {
                वापस false;
            }
        }
        reg.start += BIT(आकार_bits);
    }
    वापस true;
}

BOOT_CODE bool_t create_untypeds(cap_t मूल_cnode_cap)
{
    seL4_SlotPos first_untyped_slot = ndks_boot.slot_pos_cur;

    paddr_t start = 0;
    क्रम (word_t i = 0; i < ndks_boot.resv_गिनती; i++) {
        अगर (start < ndks_boot.reserved[i].start) {
            region_t reg = paddr_to_pptr_reg((p_region_t) {
                start, ndks_boot.reserved[i].start
            });
            अगर (!create_untypeds_for_region(मूल_cnode_cap, true, reg, first_untyped_slot)) {
                म_लिखो("ERROR: creation of untypeds for device region #%u at"
                       " [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") failed\n",
                       (अचिन्हित पूर्णांक)i, reg.start, reg.end);
                वापस false;
            }
        }

        start = ndks_boot.reserved[i].end;
    }

    अगर (start <= CONFIG_PADDR_USER_DEVICE_शीर्ष - 1) {
        region_t reg = paddr_to_pptr_reg((p_region_t) {
            /* CONFIG_PADDR_USER_DEVICE_TOP cast to paddr_t can be 0.
             * create_untypeds_for_region() can deal with that correctly. */
            start, (paddr_t) CONFIG_PADDR_USER_DEVICE_शीर्ष
        });

        अगर (!create_untypeds_for_region(मूल_cnode_cap, true, reg, first_untyped_slot)) {
            म_लिखो("ERROR: creation of untypeds for top device region"
                   " [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") failed\n",
                   reg.start, reg.end);
            वापस false;
        }
    }

    /* There is a part of the kernel (code/data) that is only needed for the
     * boot process. We can create UT objects for these frames, so the memory
     * can be reused.
     */
    region_t boot_mem_reuse_reg = paddr_to_pptr_reg(get_p_reg_kernel_img_boot());
    अगर (!create_untypeds_for_region(मूल_cnode_cap, false, boot_mem_reuse_reg, first_untyped_slot)) {
        म_लिखो("ERROR: creation of untypeds for recycled boot memory"
               " [%"SEL4_PRIx_word"..%"SEL4_PRIx_word"] failed\n",
               boot_mem_reuse_reg.start, boot_mem_reuse_reg.end);
        वापस false;
    }

    /* convert remaining freemem into UT objects and provide the caps */
    क्रम (word_t i = 0; i < ARRAY_आकार(ndks_boot.freemem); i++) {
        region_t reg = ndks_boot.freemem[i];
        ndks_boot.freemem[i] = REG_EMPTY;
        अगर (!create_untypeds_for_region(मूल_cnode_cap, false, reg, first_untyped_slot)) {
            म_लिखो("ERROR: creation of untypeds for free memory region #%u at"
                   " [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") failed\n",
                   (अचिन्हित पूर्णांक)i, reg.start, reg.end);
            वापस false;
        }
    }

    ndks_boot.bi_frame->untyped = (seL4_SlotRegion) {
        .start = first_untyped_slot,
        .end   = ndks_boot.slot_pos_cur
    };

    वापस true;
}

BOOT_CODE व्योम bi_finalise(व्योम)
{

    अगर (rootserver.paging.start != rootserver.paging.end) {
        म_लिखो("WARNING: internal book keeping error. Less pagetables allocated than predicted: "
               "%ld page tables allocated but not used.\n", (rootserver.paging.end - rootserver.paging.start) >> seL4_PageTableBits);
    }

    ndks_boot.bi_frame->empty = (seL4_SlotRegion) {
        .start = ndks_boot.slot_pos_cur,
        .end   = BIT(CONFIG_मूल_CNODE_आकार_BITS)
    };
}

BOOT_CODE अटल bool_t check_available_memory(word_t n_available,
                                               स्थिर p_region_t *available)
{
    /* The system configuration is broken if no region is available. */
    अगर (0 == n_available) {
        म_लिखो("ERROR: no memory regions available\n");
        वापस false;
    }

    म_लिखो("Available phys memory regions: %"SEL4_PRIu_word"\n", n_available);
    /* Force ordering and exclusivity of available regions. */
    क्रम (word_t i = 0; i < n_available; i++) {
        स्थिर p_region_t *r = &available[i];
        म_लिखो("  [%"SEL4_PRIx_word"..%"SEL4_PRIx_word")\n", r->start, r->end);

        /* Available regions must not wrap */
        अगर (r->start > r->end) {
            म_लिखो("ERROR: memory region %"SEL4_PRIu_word" has start > end\n", i);
            वापस false;
        }

        /* Available regions must be non-empty. */
        अगर (r->start == r->end) {
            म_लिखो("ERROR: memory region %"SEL4_PRIu_word" empty\n", i);
            वापस false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
         * so the '==' case is fine. Directly adjacent regions are allowed.
         */
        अगर ((i > 0) && (r->start < available[i - 1].end)) {
            म_लिखो("ERROR: memory region %"SEL4_PRIu_word" in wrong order\n", i);
            वापस false;
        }
    }

    वापस true;
}

/* We can't declare arrays on the stack, so this is space for init_avail_reg()
 * below to populate and init_freemem() to consume. */
BOOT_BSS अटल region_t avail_reg[MAX_अंक_FREEMEM_REG];

/**
 * Initialise avail_reg with the parts of the available memory regions that are
 * in the kernel window, converted to kernel pointers. Only this memory can be
 * used for kernel objects. On 32-bit architectures it is not uncommon for
 * regions to be completely or partially outside of the kernel window, because
 * the available address space is small. Memory outside of the kernel window is
 * discarded here, but it may be made available as device untyped memory instead
 * if it is not also in a reserved region. Regions must have passed
 * check_available_memory(). Returns the number of entries populated in
 * avail_reg.
 */
BOOT_CODE अटल word_t init_avail_reg(word_t n_available,
                                       स्थिर p_region_t *available)
{
    word_t cnt = 0;
    क्रम (word_t i = 0; i < n_available; i++) {
        स्थिर p_region_t *r = &available[i];

        अगर ((r->start >= PADDR_शीर्ष) || (r->end <= PADDR_BASE)) {
            म_लिखो("  region [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") is outside the kernel"
                   " window, only potentially available as device untypeds\n", r->start, r->end);
        } अन्यथा {
            p_region_t usable_reg = {
                .start = MAX(r->start, PADDR_BASE),
                .end   = MIN(r->end, PADDR_शीर्ष)
            };

            अगर (cnt >= ARRAY_आकार(avail_reg)) {
                /* No space left in the array, seems MAX_NUM_FREEMEM_REG should
                 * be increased. Debug builds raise an assert here because this
                 * is likely a porting issue that should be looked into. Release
                 * builds will continue booting, but can't use this memory
                 * region. This might still be sufficient to run.
                 */
                म_लिखो("  WARNING: cannot use region [%"SEL4_PRIx_word"..%"SEL4_PRIx_word"),"
                       " avail_reg[] is full\n", r->start, r->end);
                निश्चित(0);
            } अन्यथा {
                avail_reg[cnt] = paddr_to_pptr_reg(usable_reg);
                cnt++;
            }
        }
    }
    वापस cnt;
}


BOOT_CODE अटल bool_t check_reserved_memory(word_t n_reserved,
                                              स्थिर region_t *reserved)
{
    म_लिखो("Reserved virt address space regions: %"SEL4_PRIu_word"\n",
           n_reserved);
    /* Force ordering and exclusivity of reserved regions. */
    क्रम (word_t i = 0; i < n_reserved; i++) {
        स्थिर region_t *r = &reserved[i];
        म_लिखो("  [%"SEL4_PRIx_word"..%"SEL4_PRIx_word")\n", r->start, r->end);

        /* Reserved regions must be sane, the size is allowed to be zero. */
        अगर (r->start > r->end) {
            म_लिखो("ERROR: reserved region %"SEL4_PRIu_word" has start > end\n", i);
            वापस false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
           so the == case is fine. Directly adjacent regions are allowed. */
        अगर ((i > 0) && (r->start < reserved[i - 1].end)) {
            म_लिखो("ERROR: reserved region %"SEL4_PRIu_word" in wrong order\n", i);
            वापस false;
        }
    }

    वापस true;
}

/**
 * Dynamically initialise the available memory on the platform.
 * A region represents an area of memory.
 */
BOOT_CODE bool_t init_freemem(word_t n_available, स्थिर p_region_t *available,
                              word_t n_reserved, स्थिर region_t *reserved,
                              v_region_t it_v_reg, word_t extra_bi_आकार_bits)
{
    अगर (!check_available_memory(n_available, available)) {
        वापस false;
    }

    /* After here, avail_reg[0..n_available) are all regions within the kernel window. */
    n_available = init_avail_reg(n_available, available);
    अगर (0 == n_available) {
        म_लिखो("ERROR: no available memory within the kernel window\n");
        वापस false;
    }

    अगर (!check_reserved_memory(n_reserved, reserved)) {
        वापस false;
    }

    क्रम (word_t i = 0; i < ARRAY_आकार(ndks_boot.freemem); i++) {
        ndks_boot.freemem[i] = REG_EMPTY;
    }

    word_t a = 0;
    word_t r = 0;
    /* Now iterate through the available regions, removing any reserved regions. */
    जबतक (a < n_available && r < n_reserved) {
        अगर (reserved[r].start == reserved[r].end) {
            /* reserved region is empty - skip it */
            r++;
        } अन्यथा अगर (avail_reg[a].start >= avail_reg[a].end) {
            /* skip the entire region - it's empty now after trimming */
            a++;
        } अन्यथा अगर (reserved[r].end <= avail_reg[a].start) {
            /* the reserved region is below the available region - skip it */
            reserve_region(pptr_to_paddr_reg(reserved[r]));
            r++;
        } अन्यथा अगर (reserved[r].start >= avail_reg[a].end) {
            /* the reserved region is above the available region - take the whole thing */
            जोड़ो_region(avail_reg[a]);
            a++;
        } अन्यथा {
            /* the reserved region overlaps with the available region */
            अगर (reserved[r].start <= avail_reg[a].start) {
                /* the region overlaps with the start of the available region.
                 * trim start of the available region */
                avail_reg[a].start = MIN(avail_reg[a].end, reserved[r].end);
                /* do not increment reserved index here - there could be more overlapping regions */
            } अन्यथा {
                निश्चित(reserved[r].start < avail_reg[a].end);
                /* take the first chunk of the available region and move
                 * the start to the end of the reserved region */
                region_t m = avail_reg[a];
                m.end = reserved[r].start;
                जोड़ो_region(m);
                अगर (avail_reg[a].end > reserved[r].end) {
                    avail_reg[a].start = reserved[r].end;
                    /* we could increment reserved index here, but it's more consistent with the
                     * other overlapping case if we don't */
                } अन्यथा {
                    a++;
                }
            }
        }
    }

    क्रम (; r < n_reserved; r++) {
        अगर (reserved[r].start < reserved[r].end) {
            reserve_region(pptr_to_paddr_reg(reserved[r]));
        }
    }

    /* no more reserved regions - add the rest */
    क्रम (; a < n_available; a++) {
        अगर (avail_reg[a].start < avail_reg[a].end) {
            जोड़ो_region(avail_reg[a]);
        }
    }

    /* now try to fit the root server objects into a region */
    पूर्णांक i = ARRAY_आकार(ndks_boot.freemem) - 1;
    अगर (!is_reg_empty(ndks_boot.freemem[i])) {
        म_लिखो("ERROR: insufficient MAX_NUM_FREEMEM_REG (%u)\n",
               (अचिन्हित पूर्णांक)MAX_अंक_FREEMEM_REG);
        वापस false;
    }
    /* skip any empty regions */
    क्रम (; i >= 0 && is_reg_empty(ndks_boot.freemem[i]); i--);

    /* try to grab the last available p region to create the root server objects
     * from. If possible, retain any left over memory as an extra p region */
    word_t आकार = calculate_rootserver_आकार(it_v_reg, extra_bi_आकार_bits);
    word_t max = rootserver_max_आकार_bits(extra_bi_आकार_bits);
    क्रम (; i >= 0; i--) {
        /* Invariant: both i and (i + 1) are valid indices in ndks_boot.freemem. */
        निश्चित(i < ARRAY_आकार(ndks_boot.freemem) - 1);
        /* Invariant; the region at index i is the current candidate.
         * Invariant: regions 0 up to (i - 1), if any, are additional candidates.
         * Invariant: region (i + 1) is empty. */
        निश्चित(is_reg_empty(ndks_boot.freemem[i + 1]));
        /* Invariant: regions above (i + 1), if any, are empty or too small to use.
         * Invariant: all non-empty regions are ordered, disjoint and unallocated. */

        /* We make a fresh variable to index the known-empty region, because the
         * SimplExportAndRefine verification test has poor support for array
         * indices that are sums of variables and small constants. */
        पूर्णांक empty_सूचकांक = i + 1;

        /* Try to take the top-most suitably sized and aligned chunk. */
        pptr_t unaligned_start = ndks_boot.freemem[i].end - आकार;
        pptr_t start = ROUND_DOWN(unaligned_start, max);
        /* if unaligned_start didn't underflow, and start fits in the region,
         * then we've found a region that fits the root server objects. */
        अगर (unaligned_start <= ndks_boot.freemem[i].end
            && start >= ndks_boot.freemem[i].start) {
            create_rootserver_objects(start, it_v_reg, extra_bi_आकार_bits);
            /* There may be leftovers before and after the memory we used. */
            /* Shuffle the after leftover up to the empty slot (i + 1). */
            ndks_boot.freemem[empty_सूचकांक] = (region_t) {
                .start = start + आकार,
                .end = ndks_boot.freemem[i].end
            };
            /* Leave the before leftover in current slot i. */
            ndks_boot.freemem[i].end = start;
            /* Regions i and (i + 1) are now well defined, ordered, disjoint,
             * and unallocated, so we can return successfully. */
            वापस true;
        }
        /* Region i isn't big enough, so shuffle it up to slot (i + 1),
         * which we know is unused. */
        ndks_boot.freemem[empty_सूचकांक] = ndks_boot.freemem[i];
        /* Now region i is unused, so make it empty to reestablish the invariant
         * for the next iteration (when it will be slot i + 1). */
        ndks_boot.freemem[i] = REG_EMPTY;
    }

    /* We didn't find a big enough region. */
    म_लिखो("ERROR: no free memory region is big enough for root server "
           "objects, need size/alignment of 2^%"SEL4_PRIu_word"\n", max);
    वापस false;
}
