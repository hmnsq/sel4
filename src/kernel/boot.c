/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <kernel/boot.h>
#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <machine/registerset.h>
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <object/domain.h>
#அந்தர்பாவ <arch/machine.h>
#அந்தர்பாவ <arch/kernel/boot.h>
#அந்தர்பாவ <arch/kernel/vspace.h>
#அந்தர்பாவ <linker.h>
#அந்தர்பாவ <hardware.h>
#அந்தர்பாவ <util.h>

/* (node-local) state accessed only during bootstrapping */
BOOT_BSS ndks_boot_t ndks_boot;

BOOT_BSS rootserver_mem_t rootserver;
BOOT_BSS நிலையான region_t rootserver_mem;

/* Returns the physical region of the kernel image boot part, which is the part
 * that is no longer needed once booting is finished. */
வெளி எழுத்து ki_boot_end[1];
BOOT_CODE p_region_t get_p_reg_kernel_img_boot(வெறுமை)
{
    திரும்பு (p_region_t) {
        .start = kpptr_to_paddr((மாறிலி வெறுமை *)KERNEL_ELF_BASE),
        .end   = kpptr_to_paddr(ki_boot_end)
    };
}

/* Returns the physical region of the kernel image. */
BOOT_CODE p_region_t get_p_reg_kernel_img(வெறுமை)
{
    திரும்பு (p_region_t) {
        .start = kpptr_to_paddr((மாறிலி வெறுமை *)KERNEL_ELF_BASE),
        .end   = kpptr_to_paddr((மாறிலி வெறுமை *)KERNEL_ELF_உச்சி)
    };
}

BOOT_CODE நிலையான வெறுமை merge_regions(வெறுமை)
{
    /* Walk through reserved regions and see if any can be merged */
    word_t உச்சி = 0;
    க்கு (word_t i = 1; i < ndks_boot.resv_மொத்தம்; i++) {
        எனில் (ndks_boot.reserved[உச்சி].end == ndks_boot.reserved[i].start) {
            /* extend earlier region */
            ndks_boot.reserved[உச்சி].end = ndks_boot.reserved[i].end;
        } இல்லையெனில் {
            /* add region */
            உச்சி++;
            ndks_boot.reserved[உச்சி] = ndks_boot.reserved[i];
        }
    }
    ndks_boot.resv_மொத்தம் = ndks_boot.resv_மொத்தம் > 0 ? உச்சி + 1 : 0;
}

BOOT_CODE bool_t reserve_region(p_region_t reg)
{
    word_t i;
    ப்ரதிஜ்ஞா(reg.start <= reg.end);
    எனில் (reg.start == reg.end) {
        திரும்பு true;
    }

    /* keep the regions in order */
    க்கு (i = 0; i < ndks_boot.resv_மொத்தம்; i++) {
        /* Try and merge the region to an existing one, if possible */
        எனில் (ndks_boot.reserved[i].start == reg.end) {
            ndks_boot.reserved[i].start = reg.start;
            merge_regions();
            திரும்பு true;
        }
        எனில் (ndks_boot.reserved[i].end == reg.start) {
            ndks_boot.reserved[i].end = reg.end;
            merge_regions();
            திரும்பு true;
        }
        /* Otherwise figure out where it should go. */
        எனில் (ndks_boot.reserved[i].start > reg.end) {
            /* move regions down, making sure there's enough room */
            எனில் (ndks_boot.resv_மொத்தம் + 1 >= MAX_இலக்கம்_RESV_REG) {
                printf("Can't mark region 0x%"SEL4_PRIx_word"-0x%"SEL4_PRIx_word
                       " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
                       reg.start, reg.end, (முழு)MAX_இலக்கம்_RESV_REG);
                திரும்பு false;
            }
            க்கு (word_t j = ndks_boot.resv_மொத்தம்; j > i; j--) {
                ndks_boot.reserved[j] = ndks_boot.reserved[j - 1];
            }
            /* insert the new region */
            ndks_boot.reserved[i] = reg;
            ndks_boot.resv_மொத்தம்++;
            திரும்பு true;
        }
    }

    எனில் (i + 1 == MAX_இலக்கம்_RESV_REG) {
        printf("Can't mark region 0x%"SEL4_PRIx_word"-0x%"SEL4_PRIx_word
               " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
               reg.start, reg.end, (முழு)MAX_இலக்கம்_RESV_REG);
        திரும்பு false;
    }

    ndks_boot.reserved[i] = reg;
    ndks_boot.resv_மொத்தம்++;

    திரும்பு true;
}

BOOT_CODE நிலையான bool_t செருகு_region(region_t reg)
{
    ப்ரதிஜ்ஞா(reg.start <= reg.end);
    எனில் (is_reg_empty(reg)) {
        திரும்பு true;
    }

    க்கு (word_t i = 0; i < ARRAY_SIZE(ndks_boot.freemem); i++) {
        எனில் (is_reg_empty(ndks_boot.freemem[i])) {
            reserve_region(pptr_to_paddr_reg(reg));
            ndks_boot.freemem[i] = reg;
            திரும்பு true;
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
    printf("no free memory slot left for [%"SEL4_PRIx_word"..%"SEL4_PRIx_word"),"
           " consider increasing MAX_NUM_FREEMEM_REG (%u)\n",
           reg.start, reg.end, (unsigned முழு)MAX_இலக்கம்_FREEMEM_REG);

    /* For debug builds we consider this a fatal error. Rationale is, that the
     * caller does not check the error code at the moment, but just ignores any
     * failures silently. */
    ப்ரதிஜ்ஞா(0);

    திரும்பு false;
}

BOOT_CODE நிலையான pptr_t alloc_rootserver_obj(word_t size_bits, word_t n)
{
    pptr_t allocated = rootserver_mem.start;
    /* allocated memory must be aligned */
    ப்ரதிஜ்ஞா(allocated % BIT(size_bits) == 0);
    rootserver_mem.start += (n * BIT(size_bits));
    /* we must not have run out of memory */
    ப்ரதிஜ்ஞா(rootserver_mem.start <= rootserver_mem.end);
    memzero((வெறுமை *) allocated, n * BIT(size_bits));
    திரும்பு allocated;
}

BOOT_CODE நிலையான word_t rootserver_max_size_bits(word_t extra_bi_size_bits)
{
    word_t cnode_size_bits = CONFIG_வேர்_CNODE_SIZE_BITS + seL4_SlotBits;
    word_t max = MAX(cnode_size_bits, seL4_VSpaceBits);
    திரும்பு MAX(max, extra_bi_size_bits);
}

BOOT_CODE நிலையான word_t calculate_rootserver_size(v_region_t it_v_reg, word_t extra_bi_size_bits)
{
    /* work out how much memory we need for root server objects */
    word_t size = BIT(CONFIG_வேர்_CNODE_SIZE_BITS + seL4_SlotBits);
    size += BIT(seL4_TCBBits); // root thread tcb
    size += BIT(seL4_PageBits); // ipc buf
    size += BIT(seL4_BootInfoFrameBits); // boot info
    size += BIT(seL4_ASIDPoolBits);
    size += extra_bi_size_bits > 0 ? BIT(extra_bi_size_bits) : 0;
    size += BIT(seL4_VSpaceBits); // root vspace
#ifdef CONFIG_KERNEL_MCS
    size += BIT(seL4_MinSchedContextBits); // root sched context
#endif
    /* for all archs, seL4_PageTable Bits is the size of all non top-level paging structures */
    திரும்பு size + arch_get_n_paging(it_v_reg) * BIT(seL4_PageTableBits);
}

BOOT_CODE நிலையான வெறுமை maybe_alloc_extra_bi(word_t cmp_size_bits, word_t extra_bi_size_bits)
{
    எனில் (extra_bi_size_bits >= cmp_size_bits && rootserver.extra_bi == 0) {
        rootserver.extra_bi = alloc_rootserver_obj(extra_bi_size_bits, 1);
    }
}

/* Create pptrs for all root server objects, starting at a give start address,
 * to cover the virtual memory region v_reg, and any extra boot info.
 */
BOOT_CODE நிலையான வெறுமை create_rootserver_objects(pptr_t start, v_region_t it_v_reg,
                                                word_t extra_bi_size_bits)
{
    /* the largest object the PD, the root cnode, or the extra boot info */
    word_t cnode_size_bits = CONFIG_வேர்_CNODE_SIZE_BITS + seL4_SlotBits;
    word_t max = rootserver_max_size_bits(extra_bi_size_bits);

    word_t size = calculate_rootserver_size(it_v_reg, extra_bi_size_bits);
    rootserver_mem.start = start;
    rootserver_mem.end = start + size;

    maybe_alloc_extra_bi(max, extra_bi_size_bits);

    /* the root cnode is at least 4k, so it could be larger or smaller than a pd. */
#எனில் (CONFIG_வேர்_CNODE_SIZE_BITS + seL4_SlotBits) > seL4_VSpaceBits
    rootserver.cnode = alloc_rootserver_obj(cnode_size_bits, 1);
    maybe_alloc_extra_bi(seL4_VSpaceBits, extra_bi_size_bits);
    rootserver.vspace = alloc_rootserver_obj(seL4_VSpaceBits, 1);
#இல்லையெனில்
    rootserver.vspace = alloc_rootserver_obj(seL4_VSpaceBits, 1);
    maybe_alloc_extra_bi(cnode_size_bits, extra_bi_size_bits);
    rootserver.cnode = alloc_rootserver_obj(cnode_size_bits, 1);
#endif

    /* at this point we are up to creating 4k objects - which is the min size of
     * extra_bi so this is the last chance to allocate it */
    maybe_alloc_extra_bi(seL4_PageBits, extra_bi_size_bits);
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
#எனில் seL4_TCBBits >= seL4_PageTableBits
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#endif

    /* paging structures are 4k on every arch except aarch32 (1k) */
    word_t n = arch_get_n_paging(it_v_reg);
    rootserver.paging.start = alloc_rootserver_obj(seL4_PageTableBits, n);
    rootserver.paging.end = rootserver.paging.start + n * BIT(seL4_PageTableBits);

    /* for most archs, TCBs are smaller than page tables */
#எனில் seL4_TCBBits < seL4_PageTableBits
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#endif

#ifdef CONFIG_KERNEL_MCS
    rootserver.sc = alloc_rootserver_obj(seL4_MinSchedContextBits, 1);
#endif
    /* we should have allocated all our memory */
    ப்ரதிஜ்ஞா(rootserver_mem.start == rootserver_mem.end);
}

BOOT_CODE வெறுமை write_slot(slot_ptr_t slot_ptr, cap_t cap)
{
    slot_ptr->cap = cap;

    slot_ptr->cteMDBNode = nullMDBNode;
    mdb_முனை_ptr_set_mdb_Revocable(&slot_ptr->cteMDBNode, true);
    mdb_முனை_ptr_set_mdb_First_Badged(&slot_ptr->cteMDBNode, true);
}

/* Our root CNode needs to be able to fit all the initial caps and not
 * cover all of memory.
 */
compile_assert(வேர்_cnode_size_valid,
               CONFIG_வேர்_CNODE_SIZE_BITS < 32 - seL4_SlotBits &&
               BIT(CONFIG_வேர்_CNODE_SIZE_BITS) >= se_L4_இலக்கம்_Initial_Caps &&
               CONFIG_வேர்_CNODE_SIZE_BITS >= (seL4_PageBits - seL4_SlotBits))

BOOT_CODE cap_t
create_வேர்_cnode(வெறுமை)
{
    cap_t cap = cap_cnode_cap_new(
                    CONFIG_வேர்_CNODE_SIZE_BITS, /* radix */
                    wordBits - CONFIG_வேர்_CNODE_SIZE_BITS, /* guard size */
                    0, /* guard */
                    rootserver.cnode); /* pptr */

    /* write the root CNode cap into the root CNode */
    write_slot(SLOT_PTR(rootserver.cnode, seL4_CapInitThreadCNode), cap);

    திரும்பு cap;
}

/* Check domain scheduler assumptions. */
compile_assert(இலக்கம்_domains_valid,
               CONFIG_இலக்கம்_DOMAINS >= 1 && CONFIG_இலக்கம்_DOMAINS <= 256)
compile_assert(இலக்கம்_priorities_valid,
               CONFIG_இலக்கம்_PRIORITIES >= 1 && CONFIG_இலக்கம்_PRIORITIES <= 256)

BOOT_CODE வெறுமை
create_domain_cap(cap_t வேர்_cnode_cap)
{
    cap_t cap = cap_domain_cap_new();
    write_slot(SLOT_PTR(pptr_of_cap(வேர்_cnode_cap), seL4_CapDomain), cap);
}

BOOT_CODE cap_t create_ipcbuf_frame_cap(cap_t வேர்_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    clearMemory((வெறுமை *)rootserver.ipc_buf, PAGE_BITS);

    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_frame_cap(pd_cap, rootserver.ipc_buf, vptr, IT_ASID, false, false);
    write_slot(SLOT_PTR(pptr_of_cap(வேர்_cnode_cap), seL4_CapInitThreadIPCBuffer), cap);

    திரும்பு cap;
}

BOOT_CODE வெறுமை create_bi_frame_cap(cap_t வேர்_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_frame_cap(pd_cap, rootserver.boot_info, vptr, IT_ASID, false, false);
    write_slot(SLOT_PTR(pptr_of_cap(வேர்_cnode_cap), seL4_CapBootInfoFrame), cap);
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
BOOT_CODE word_t calculate_extra_bi_size_bits(word_t extra_size)
{
    எனில் (extra_size == 0) {
        திரும்பு 0;
    }

    word_t clzl_ret = clzl(ROUND_UP(extra_size, seL4_PageBits));
    word_t msb = seL4_WordBits - 1 - clzl_ret;
    /* If region is bigger than a page, make sure we overallocate rather than
     * underallocate
     */
    எனில் (extra_size > BIT(msb)) {
        msb++;
    }
    திரும்பு msb;
}

BOOT_CODE வெறுமை populate_bi_frame(முனை_id_t முனை_id, word_t இலக்கம்_nodes,
                                 vptr_t ipcbuf_vptr, word_t extra_bi_size)
{
    /* clear boot info memory */
    clearMemory((வெறுமை *)rootserver.boot_info, seL4_BootInfoFrameBits);
    எனில் (extra_bi_size) {
        clearMemory((வெறுமை *)rootserver.extra_bi,
                    calculate_extra_bi_size_bits(extra_bi_size));
    }

    /* initialise bootinfo-related global state */
    seL4_BootInfo *bi = BI_PTR(rootserver.boot_info);
    bi->முனை_ID = முனை_id;
    bi->இலக்கம்_Nodes = இலக்கம்_nodes;
    bi->இலக்கம்_IOPTLevels = 0;
    bi->ipcBuffer = (seL4_IPCBuffer *)ipcbuf_vptr;
    bi->initThreadCNodeSizeBits = CONFIG_வேர்_CNODE_SIZE_BITS;
    bi->initThreadDomain = 0;
    bi->extraLen = extra_bi_size;

    ndks_boot.bi_frame = bi;
    ndks_boot.slot_pos_cur = se_L4_இலக்கம்_Initial_Caps;
}

BOOT_CODE bool_t provide_cap(cap_t வேர்_cnode_cap, cap_t cap)
{
    எனில் (ndks_boot.slot_pos_cur >= BIT(CONFIG_வேர்_CNODE_SIZE_BITS)) {
        printf("ERROR: can't add another cap, all %"SEL4_PRIu_word
               " (=2^CONFIG_ROOT_CNODE_SIZE_BITS) slots used\n",
               BIT(CONFIG_வேர்_CNODE_SIZE_BITS));
        திரும்பு false;
    }
    write_slot(SLOT_PTR(pptr_of_cap(வேர்_cnode_cap), ndks_boot.slot_pos_cur), cap);
    ndks_boot.slot_pos_cur++;
    திரும்பு true;
}

BOOT_CODE create_frames_of_region_ret_t create_frames_of_region(
    cap_t    வேர்_cnode_cap,
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

    க்கு (f = reg.start; f < reg.end; f += BIT(PAGE_BITS)) {
        எனில் (do_map) {
            frame_cap = create_mapped_it_frame_cap(pd_cap, f, pptr_to_paddr((வெறுமை *)(f - pv_offset)), IT_ASID, false, true);
        } இல்லையெனில் {
            frame_cap = create_unmapped_it_frame_cap(f, false);
        }
        எனில் (!provide_cap(வேர்_cnode_cap, frame_cap)) {
            திரும்பு (create_frames_of_region_ret_t) {
                .region  = S_REG_EMPTY,
                .success = false
            };
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;

    திரும்பு (create_frames_of_region_ret_t) {
        .region = (seL4_SlotRegion) {
            .start = slot_pos_before,
            .end   = slot_pos_after
        },
        .success = true
    };
}

BOOT_CODE cap_t create_it_asid_pool(cap_t வேர்_cnode_cap)
{
    cap_t ap_cap = cap_asid_pool_cap_new(ASID_உயர்வு(IT_ASID) << asid_கீழ்_Bits, rootserver.asid_pool);
    write_slot(SLOT_PTR(pptr_of_cap(வேர்_cnode_cap), seL4_CapInitThreadASIDPool), ap_cap);

    /* create ASID control cap */
    write_slot(
        SLOT_PTR(pptr_of_cap(வேர்_cnode_cap), seL4_CapASIDControl),
        cap_asid_control_cap_new()
    );

    திரும்பு ap_cap;
}

#ifdef CONFIG_KERNEL_MCS
BOOT_CODE நிலையான வெறுமை configure_sched_context(tcb_t *tcb, sched_context_t *sc_pptr, ticks_t timeslice)
{
    tcb->tcbSchedContext = sc_pptr;
    refill_new(tcb->tcbSchedContext, MIN_REFILLS, timeslice, 0);
    tcb->tcbSchedContext->scTcb = tcb;
}

BOOT_CODE bool_t init_sched_control(cap_t வேர்_cnode_cap, word_t இலக்கம்_nodes)
{
    seL4_SlotPos slot_pos_before = ndks_boot.slot_pos_cur;

    /* create a sched control cap for each core */
    க்கு (unsigned முழு i = 0; i < இலக்கம்_nodes; i++) {
        எனில் (!provide_cap(வேர்_cnode_cap, cap_sched_control_cap_new(i))) {
            printf("can't init sched_control for node %u, provide_cap() failed\n", i);
            திரும்பு false;
        }
    }

    /* update boot info with slot region for sched control caps */
    ndks_boot.bi_frame->schedcontrol = (seL4_SlotRegion) {
        .start = slot_pos_before,
        .end = ndks_boot.slot_pos_cur
    };

    திரும்பு true;
}
#endif

BOOT_CODE வெறுமை create_idle_thread(வெறுமை)
{
    pptr_t pptr;

#ifdef ENABLE_SMP_SUPPORT
    க்கு (unsigned முழு i = 0; i < CONFIG_MAX_இலக்கம்_NODES; i++) {
#endif /* ENABLE_SMP_SUPPORT */
        pptr = (pptr_t) &ksIdleThreadTCB[SMP_TERNARY(i, 0)];
        முனை_STATE_ON_CORE(ksIdleThread, i) = TCB_PTR(pptr + TCB_OFFSET);
        configureIdleThread(முனை_STATE_ON_CORE(ksIdleThread, i));
#ifdef CONFIG_DEBUG_BUILD
        setThreadName(முனை_STATE_ON_CORE(ksIdleThread, i), "idle_thread");
#endif
        SMP_COND_STATEMENT(முனை_STATE_ON_CORE(ksIdleThread, i)->tcbAffinity = i);
#ifdef CONFIG_KERNEL_MCS
        configure_sched_context(முனை_STATE_ON_CORE(ksIdleThread, i), SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]),
                                MAX_PERIOD_TICKS);
        SMP_COND_STATEMENT(முனை_STATE_ON_CORE(ksIdleThread, i)->tcbSchedContext->scCore = i;)
        முனை_STATE_ON_CORE(ksIdleSC, i) = SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]);
#endif
#ifdef ENABLE_SMP_SUPPORT
    }
#endif /* ENABLE_SMP_SUPPORT */
}

BOOT_CODE tcb_t *create_initial_thread(cap_t வேர்_cnode_cap, cap_t it_pd_cap, vptr_t ui_v_entry, vptr_t bi_frame_vptr,
                                       vptr_t ipcbuf_vptr, cap_t ipcbuf_cap)
{
    tcb_t *tcb = TCB_PTR(rootserver.tcb + TCB_OFFSET);
#ifndef CONFIG_KERNEL_MCS
    tcb->tcbTimeSlice = CONFIG_TIME_SLICE;
#endif

    Arch_initContext(&tcb->tcbArch.tcbContext);

    /* derive a copy of the IPC buffer cap for inserting */
    deriveCap_ret_t dc_ret = deriveCap(SLOT_PTR(pptr_of_cap(வேர்_cnode_cap), seL4_CapInitThreadIPCBuffer), ipcbuf_cap);
    எனில் (dc_ret.status != EXCEPTION_NONE) {
        printf("Failed to derive copy of IPC Buffer\n");
        திரும்பு வெற்று;
    }

    /* initialise TCB (corresponds directly to abstract specification) */
    cte_செருகு(
        வேர்_cnode_cap,
        SLOT_PTR(pptr_of_cap(வேர்_cnode_cap), seL4_CapInitThreadCNode),
        SLOT_PTR(rootserver.tcb, tcbCTable)
    );
    cte_செருகு(
        it_pd_cap,
        SLOT_PTR(pptr_of_cap(வேர்_cnode_cap), seL4_CapInitThreadVSpace),
        SLOT_PTR(rootserver.tcb, tcbVTable)
    );
    cte_செருகு(
        dc_ret.cap,
        SLOT_PTR(pptr_of_cap(வேர்_cnode_cap), seL4_CapInitThreadIPCBuffer),
        SLOT_PTR(rootserver.tcb, tcbBuffer)
    );
    tcb->tcbIPCBuffer = ipcbuf_vptr;

    setRegister(tcb, capRegister, bi_frame_vptr);
    setNextPC(tcb, ui_v_entry);

    /* initialise TCB */
#ifdef CONFIG_KERNEL_MCS
    configure_sched_context(tcb, SC_PTR(rootserver.sc), usToTicks(CONFIG_BOOT_THREAD_TIME_SLICE * US_IN_MS));
#endif

    tcb->tcbPriority = seL4_MaxPrio;
    tcb->tcbMCP = seL4_MaxPrio;
    tcb->tcbDomain = 0;
#ifndef CONFIG_KERNEL_MCS
    setupReplyMaster(tcb);
#endif
    setThreadState(tcb, ThreadState_Running);

#ifndef CONFIG_KERNEL_MCS
    SMP_COND_STATEMENT(tcb->tcbAffinity = 0);
#endif

    /* create initial thread's TCB cap */
    cap_t cap = cap_thread_cap_new(TCB_REF(tcb));
    write_slot(SLOT_PTR(pptr_of_cap(வேர்_cnode_cap), seL4_CapInitThreadTCB), cap);

#ifdef CONFIG_KERNEL_MCS
    cap = cap_sched_context_cap_new(SC_REF(tcb->tcbSchedContext), seL4_MinSchedContextBits);
    write_slot(SLOT_PTR(pptr_of_cap(வேர்_cnode_cap), seL4_CapInitThreadSC), cap);
#endif
#ifdef CONFIG_DEBUG_BUILD
    setThreadName(tcb, "rootserver");
#endif

    திரும்பு tcb;
}

#ifdef ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
BOOT_CODE நிலையான bool_t hypervisor_present(வெறுமை)
{
#ifdef CONFIG_ARCH_X86
    uint32_t ebx = x86_cpuid_ebx(KVM_CPUID_SIGNATURE, 0);
    uint32_t ecx = x86_cpuid_ecx(KVM_CPUID_SIGNATURE, 0);
    uint32_t edx = x86_cpuid_edx(KVM_CPUID_SIGNATURE, 0);

    எனில் ((ebx == CPUID_KVM_EBX && ecx == CPUID_KVM_ECX && edx == CPUID_KVM_EDX)
        || (ebx == CPUID_TCG_EBX && ecx == CPUID_TCG_ECX && edx == CPUID_TCG_EDX)) {
        திரும்பு true;
    }
#endif
    திரும்பு false;
}

BOOT_CODE வெறுமை clock_sync_test(வெறுமை)
{
    ticks_t t, t0;
    ticks_t margin = usToTicks(CLOCK_SYNC_DELTA) + getTimerPrecision();

    ப்ரதிஜ்ஞா(getCurrentCPUIndex() != 0);
    t = முனை_STATE_ON_CORE(ksCurTime, 0);
    செய் {
        /* perform a memory acquire to get new values of ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        t0 = முனை_STATE_ON_CORE(ksCurTime, 0);
    } போது (t0 == t);
    t = getCurrentTime();
    printf("clock_sync_test[%d]: t0 = %"PRIu64", t = %"PRIu64", td = %"PRIi64"\n",
           (முழு)getCurrentCPUIndex(), t0, t, t - t0);
    /*
     * The test does not consistently work if we are in a virtual machine (e.g
     * within QEMU) because the measurement cannot distinguish between
     * interrupted clock reads and out-of-sync clocks.
     */
    எனில் (hypervisor_present()) {
        printf("clock_sync_test[%d]: disabled, detected running as VM\n", (முழு)getCurrentCPUIndex());
    } இல்லையெனில் {
        ப்ரதிஜ்ஞா(t0 <= margin + t && t <= t0 + margin);
    }
}
#endif

BOOT_CODE வெறுமை init_core_state(tcb_t *scheduler_action)
{
#ifdef CONFIG_HAVE_FPU
    முனை_STATE(ksCurFPUOwner) = வெற்று;
#endif
#ifdef CONFIG_DEBUG_BUILD
    /* add initial threads to the debug queue */
    முனை_STATE(ksDebugTCBs) = வெற்று;
    எனில் (scheduler_action != SchedulerAction_ResumeCurrentThread &&
        scheduler_action != SchedulerAction_ChooseNewThread) {
        tcbDebugAppend(scheduler_action);
    }
    tcbDebugAppend(முனை_STATE(ksIdleThread));
#endif
    முனை_STATE(ksSchedulerAction) = scheduler_action;
    முனை_STATE(ksCurThread) = முனை_STATE(ksIdleThread);
#ifdef CONFIG_KERNEL_MCS
    முனை_STATE(ksCurSC) = முனை_STATE(ksCurThread->tcbSchedContext);
    முனை_STATE(ksConsumed) = 0;
    முனை_STATE(ksReprogram) = true;
    முனை_STATE(ksReleaseQueue.head) = வெற்று;
    முனை_STATE(ksReleaseQueue.end) = வெற்று;
    முனை_STATE(ksCurTime) = getCurrentTime();
#endif
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
BOOT_CODE நிலையான bool_t pptr_in_kernel_window(pptr_t pptr)
{
    திரும்பு pptr >= PPTR_BASE && pptr < PPTR_உச்சி;
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
BOOT_CODE நிலையான bool_t provide_untyped_cap(
    cap_t      வேர்_cnode_cap,
    bool_t     device_memory,
    pptr_t     pptr,
    word_t     size_bits,
    seL4_SlotPos first_untyped_slot
)
{
    bool_t ret;
    cap_t ut_cap;

    /* Since we are in boot code, we can do extensive error checking and
       return failure if anything unexpected happens. */

    /* Bounds check for size parameter */
    எனில் (size_bits > seL4_MaxUntypedBits || size_bits < seL4_MinUntypedBits) {
        printf("Kernel init: Invalid untyped size %"SEL4_PRIu_word"\n", size_bits);
        திரும்பு false;
    }

    /* All cap ptrs must be aligned to object size */
    எனில் (!IS_ALIGNED(pptr, size_bits)) {
        printf("Kernel init: Unaligned untyped pptr %p (alignment %"SEL4_PRIu_word")\n", (வெறுமை *)pptr, size_bits);
        திரும்பு false;
    }

    /* All cap ptrs apart from device untypeds must be in the kernel window. */
    எனில் (!device_memory && !pptr_in_kernel_window(pptr)) {
        printf("Kernel init: Non-device untyped pptr %p outside kernel window\n",
               (வெறுமை *)pptr);
        திரும்பு false;
    }

    /* Check that the end of the region is also in the kernel window, so we don't
       need to assume that the kernel window is aligned up to potentially
       seL4_MaxUntypedBits. */
    எனில் (!device_memory && !pptr_in_kernel_window(pptr + MASK(size_bits))) {
        printf("Kernel init: End of non-device untyped at %p outside kernel window (size %"SEL4_PRIu_word")\n",
               (வெறுமை *)pptr, size_bits);
        திரும்பு false;
    }

    word_t i = ndks_boot.slot_pos_cur - first_untyped_slot;
    எனில் (i < CONFIG_MAX_இலக்கம்_BOOTINFO_UNTYPED_CAPS) {
        ndks_boot.bi_frame->untypedList[i] = (seL4_UntypedDesc) {
            .paddr    = pptr_to_paddr((வெறுமை *)pptr),
            .sizeBits = size_bits,
            .isDevice = device_memory,
            .padding  = {0}
        };
        ut_cap = cap_untyped_cap_new(MAX_FREE_INDEX(size_bits),
                                     device_memory, size_bits, pptr);
        ret = provide_cap(வேர்_cnode_cap, ut_cap);
    } இல்லையெனில் {
        printf("Kernel init: Too many untyped regions for boot info\n");
        ret = true;
    }
    திரும்பு ret;
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
BOOT_CODE நிலையான bool_t create_untypeds_for_region(
    cap_t      வேர்_cnode_cap,
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
    போது (!is_reg_empty(reg)) {

        /* Calculate the bit size of the region. This is also correct for end < start: it will
           return the correct size of the set [start..-1] union [0..end). This will then be too
           large for alignment, so the code further down will reduce the size. */
        unsigned முழு size_bits = seL4_WordBits - 1 - clzl(reg.end - reg.start);
        /* The size can't exceed the largest possible untyped size. */
        எனில் (size_bits > seL4_MaxUntypedBits) {
            size_bits = seL4_MaxUntypedBits;
        }
        /* The start address 0 satisfies any alignment needs, otherwise ensure
         * the region's bit size does not exceed the alignment of the region.
         */
        எனில் (0 != reg.start) {
            unsigned முழு align_bits = ctzl(reg.start);
            எனில் (size_bits > align_bits) {
                size_bits = align_bits;
            }
        }
        /* Provide an untyped capability for the region only if it is large
         * enough to be retyped into objects later. Otherwise the region can't
         * be used anyway.
         */
        எனில் (size_bits >= seL4_MinUntypedBits) {
            எனில் (!provide_untyped_cap(வேர்_cnode_cap, device_memory, reg.start, size_bits, first_untyped_slot)) {
                திரும்பு false;
            }
        }
        reg.start += BIT(size_bits);
    }
    திரும்பு true;
}

BOOT_CODE bool_t create_untypeds(cap_t வேர்_cnode_cap)
{
    seL4_SlotPos first_untyped_slot = ndks_boot.slot_pos_cur;

    paddr_t start = 0;
    க்கு (word_t i = 0; i < ndks_boot.resv_மொத்தம்; i++) {
        எனில் (start < ndks_boot.reserved[i].start) {
            region_t reg = paddr_to_pptr_reg((p_region_t) {
                start, ndks_boot.reserved[i].start
            });
            எனில் (!create_untypeds_for_region(வேர்_cnode_cap, true, reg, first_untyped_slot)) {
                printf("ERROR: creation of untypeds for device region #%u at"
                       " [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") failed\n",
                       (unsigned முழு)i, reg.start, reg.end);
                திரும்பு false;
            }
        }

        start = ndks_boot.reserved[i].end;
    }

    எனில் (start <= CONFIG_PADDR_USER_DEVICE_உச்சி - 1) {
        region_t reg = paddr_to_pptr_reg((p_region_t) {
            /* CONFIG_PADDR_USER_DEVICE_TOP cast to paddr_t can be 0.
             * create_untypeds_for_region() can deal with that correctly. */
            start, (paddr_t) CONFIG_PADDR_USER_DEVICE_உச்சி
        });

        எனில் (!create_untypeds_for_region(வேர்_cnode_cap, true, reg, first_untyped_slot)) {
            printf("ERROR: creation of untypeds for top device region"
                   " [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") failed\n",
                   reg.start, reg.end);
            திரும்பு false;
        }
    }

    /* There is a part of the kernel (code/data) that is only needed for the
     * boot process. We can create UT objects for these frames, so the memory
     * can be reused.
     */
    region_t boot_mem_reuse_reg = paddr_to_pptr_reg(get_p_reg_kernel_img_boot());
    எனில் (!create_untypeds_for_region(வேர்_cnode_cap, false, boot_mem_reuse_reg, first_untyped_slot)) {
        printf("ERROR: creation of untypeds for recycled boot memory"
               " [%"SEL4_PRIx_word"..%"SEL4_PRIx_word"] failed\n",
               boot_mem_reuse_reg.start, boot_mem_reuse_reg.end);
        திரும்பு false;
    }

    /* convert remaining freemem into UT objects and provide the caps */
    க்கு (word_t i = 0; i < ARRAY_SIZE(ndks_boot.freemem); i++) {
        region_t reg = ndks_boot.freemem[i];
        ndks_boot.freemem[i] = REG_EMPTY;
        எனில் (!create_untypeds_for_region(வேர்_cnode_cap, false, reg, first_untyped_slot)) {
            printf("ERROR: creation of untypeds for free memory region #%u at"
                   " [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") failed\n",
                   (unsigned முழு)i, reg.start, reg.end);
            திரும்பு false;
        }
    }

    ndks_boot.bi_frame->untyped = (seL4_SlotRegion) {
        .start = first_untyped_slot,
        .end   = ndks_boot.slot_pos_cur
    };

    திரும்பு true;
}

BOOT_CODE வெறுமை bi_finalise(வெறுமை)
{

    எனில் (rootserver.paging.start != rootserver.paging.end) {
        printf("WARNING: internal book keeping error. Less pagetables allocated than predicted: "
               "%ld page tables allocated but not used.\n", (rootserver.paging.end - rootserver.paging.start) >> seL4_PageTableBits);
    }

    ndks_boot.bi_frame->empty = (seL4_SlotRegion) {
        .start = ndks_boot.slot_pos_cur,
        .end   = BIT(CONFIG_வேர்_CNODE_SIZE_BITS)
    };
}

BOOT_CODE நிலையான bool_t check_available_memory(word_t n_available,
                                               மாறிலி p_region_t *available)
{
    /* The system configuration is broken if no region is available. */
    எனில் (0 == n_available) {
        printf("ERROR: no memory regions available\n");
        திரும்பு false;
    }

    printf("Available phys memory regions: %"SEL4_PRIu_word"\n", n_available);
    /* Force ordering and exclusivity of available regions. */
    க்கு (word_t i = 0; i < n_available; i++) {
        மாறிலி p_region_t *r = &available[i];
        printf("  [%"SEL4_PRIx_word"..%"SEL4_PRIx_word")\n", r->start, r->end);

        /* Available regions must not wrap */
        எனில் (r->start > r->end) {
            printf("ERROR: memory region %"SEL4_PRIu_word" has start > end\n", i);
            திரும்பு false;
        }

        /* Available regions must be non-empty. */
        எனில் (r->start == r->end) {
            printf("ERROR: memory region %"SEL4_PRIu_word" empty\n", i);
            திரும்பு false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
         * so the '==' case is fine. Directly adjacent regions are allowed.
         */
        எனில் ((i > 0) && (r->start < available[i - 1].end)) {
            printf("ERROR: memory region %"SEL4_PRIu_word" in wrong order\n", i);
            திரும்பு false;
        }
    }

    திரும்பு true;
}

/* We can't declare arrays on the stack, so this is space for init_avail_reg()
 * below to populate and init_freemem() to consume. */
BOOT_BSS நிலையான region_t avail_reg[MAX_இலக்கம்_FREEMEM_REG];

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
BOOT_CODE நிலையான word_t init_avail_reg(word_t n_available,
                                       மாறிலி p_region_t *available)
{
    word_t cnt = 0;
    க்கு (word_t i = 0; i < n_available; i++) {
        மாறிலி p_region_t *r = &available[i];

        எனில் ((r->start >= PADDR_உச்சி) || (r->end <= PADDR_BASE)) {
            printf("  region [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") is outside the kernel"
                   " window, only potentially available as device untypeds\n", r->start, r->end);
        } இல்லையெனில் {
            p_region_t usable_reg = {
                .start = MAX(r->start, PADDR_BASE),
                .end   = MIN(r->end, PADDR_உச்சி)
            };

            எனில் (cnt >= ARRAY_SIZE(avail_reg)) {
                /* No space left in the array, seems MAX_NUM_FREEMEM_REG should
                 * be increased. Debug builds raise an assert here because this
                 * is likely a porting issue that should be looked into. Release
                 * builds will continue booting, but can't use this memory
                 * region. This might still be sufficient to run.
                 */
                printf("  WARNING: cannot use region [%"SEL4_PRIx_word"..%"SEL4_PRIx_word"),"
                       " avail_reg[] is full\n", r->start, r->end);
                ப்ரதிஜ்ஞா(0);
            } இல்லையெனில் {
                avail_reg[cnt] = paddr_to_pptr_reg(usable_reg);
                cnt++;
            }
        }
    }
    திரும்பு cnt;
}


BOOT_CODE நிலையான bool_t check_reserved_memory(word_t n_reserved,
                                              மாறிலி region_t *reserved)
{
    printf("Reserved virt address space regions: %"SEL4_PRIu_word"\n",
           n_reserved);
    /* Force ordering and exclusivity of reserved regions. */
    க்கு (word_t i = 0; i < n_reserved; i++) {
        மாறிலி region_t *r = &reserved[i];
        printf("  [%"SEL4_PRIx_word"..%"SEL4_PRIx_word")\n", r->start, r->end);

        /* Reserved regions must be sane, the size is allowed to be zero. */
        எனில் (r->start > r->end) {
            printf("ERROR: reserved region %"SEL4_PRIu_word" has start > end\n", i);
            திரும்பு false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
           so the == case is fine. Directly adjacent regions are allowed. */
        எனில் ((i > 0) && (r->start < reserved[i - 1].end)) {
            printf("ERROR: reserved region %"SEL4_PRIu_word" in wrong order\n", i);
            திரும்பு false;
        }
    }

    திரும்பு true;
}

/**
 * Dynamically initialise the available memory on the platform.
 * A region represents an area of memory.
 */
BOOT_CODE bool_t init_freemem(word_t n_available, மாறிலி p_region_t *available,
                              word_t n_reserved, மாறிலி region_t *reserved,
                              v_region_t it_v_reg, word_t extra_bi_size_bits)
{
    எனில் (!check_available_memory(n_available, available)) {
        திரும்பு false;
    }

    /* After here, avail_reg[0..n_available) are all regions within the kernel window. */
    n_available = init_avail_reg(n_available, available);
    எனில் (0 == n_available) {
        printf("ERROR: no available memory within the kernel window\n");
        திரும்பு false;
    }

    எனில் (!check_reserved_memory(n_reserved, reserved)) {
        திரும்பு false;
    }

    க்கு (word_t i = 0; i < ARRAY_SIZE(ndks_boot.freemem); i++) {
        ndks_boot.freemem[i] = REG_EMPTY;
    }

    word_t a = 0;
    word_t r = 0;
    /* Now iterate through the available regions, removing any reserved regions. */
    போது (a < n_available && r < n_reserved) {
        எனில் (reserved[r].start == reserved[r].end) {
            /* reserved region is empty - skip it */
            r++;
        } இல்லையெனில் எனில் (avail_reg[a].start >= avail_reg[a].end) {
            /* skip the entire region - it's empty now after trimming */
            a++;
        } இல்லையெனில் எனில் (reserved[r].end <= avail_reg[a].start) {
            /* the reserved region is below the available region - skip it */
            reserve_region(pptr_to_paddr_reg(reserved[r]));
            r++;
        } இல்லையெனில் எனில் (reserved[r].start >= avail_reg[a].end) {
            /* the reserved region is above the available region - take the whole thing */
            செருகு_region(avail_reg[a]);
            a++;
        } இல்லையெனில் {
            /* the reserved region overlaps with the available region */
            எனில் (reserved[r].start <= avail_reg[a].start) {
                /* the region overlaps with the start of the available region.
                 * trim start of the available region */
                avail_reg[a].start = MIN(avail_reg[a].end, reserved[r].end);
                /* do not increment reserved index here - there could be more overlapping regions */
            } இல்லையெனில் {
                ப்ரதிஜ்ஞா(reserved[r].start < avail_reg[a].end);
                /* take the first chunk of the available region and move
                 * the start to the end of the reserved region */
                region_t m = avail_reg[a];
                m.end = reserved[r].start;
                செருகு_region(m);
                எனில் (avail_reg[a].end > reserved[r].end) {
                    avail_reg[a].start = reserved[r].end;
                    /* we could increment reserved index here, but it's more consistent with the
                     * other overlapping case if we don't */
                } இல்லையெனில் {
                    a++;
                }
            }
        }
    }

    க்கு (; r < n_reserved; r++) {
        எனில் (reserved[r].start < reserved[r].end) {
            reserve_region(pptr_to_paddr_reg(reserved[r]));
        }
    }

    /* no more reserved regions - add the rest */
    க்கு (; a < n_available; a++) {
        எனில் (avail_reg[a].start < avail_reg[a].end) {
            செருகு_region(avail_reg[a]);
        }
    }

    /* now try to fit the root server objects into a region */
    முழு i = ARRAY_SIZE(ndks_boot.freemem) - 1;
    எனில் (!is_reg_empty(ndks_boot.freemem[i])) {
        printf("ERROR: insufficient MAX_NUM_FREEMEM_REG (%u)\n",
               (unsigned முழு)MAX_இலக்கம்_FREEMEM_REG);
        திரும்பு false;
    }
    /* skip any empty regions */
    க்கு (; i >= 0 && is_reg_empty(ndks_boot.freemem[i]); i--);

    /* try to grab the last available p region to create the root server objects
     * from. If possible, retain any left over memory as an extra p region */
    word_t size = calculate_rootserver_size(it_v_reg, extra_bi_size_bits);
    word_t max = rootserver_max_size_bits(extra_bi_size_bits);
    க்கு (; i >= 0; i--) {
        /* Invariant: both i and (i + 1) are valid indices in ndks_boot.freemem. */
        ப்ரதிஜ்ஞா(i < ARRAY_SIZE(ndks_boot.freemem) - 1);
        /* Invariant; the region at index i is the current candidate.
         * Invariant: regions 0 up to (i - 1), if any, are additional candidates.
         * Invariant: region (i + 1) is empty. */
        ப்ரதிஜ்ஞா(is_reg_empty(ndks_boot.freemem[i + 1]));
        /* Invariant: regions above (i + 1), if any, are empty or too small to use.
         * Invariant: all non-empty regions are ordered, disjoint and unallocated. */

        /* We make a fresh variable to index the known-empty region, because the
         * SimplExportAndRefine verification test has poor support for array
         * indices that are sums of variables and small constants. */
        முழு empty_index = i + 1;

        /* Try to take the top-most suitably sized and aligned chunk. */
        pptr_t unaligned_start = ndks_boot.freemem[i].end - size;
        pptr_t start = ROUND_DOWN(unaligned_start, max);
        /* if unaligned_start didn't underflow, and start fits in the region,
         * then we've found a region that fits the root server objects. */
        எனில் (unaligned_start <= ndks_boot.freemem[i].end
            && start >= ndks_boot.freemem[i].start) {
            create_rootserver_objects(start, it_v_reg, extra_bi_size_bits);
            /* There may be leftovers before and after the memory we used. */
            /* Shuffle the after leftover up to the empty slot (i + 1). */
            ndks_boot.freemem[empty_index] = (region_t) {
                .start = start + size,
                .end = ndks_boot.freemem[i].end
            };
            /* Leave the before leftover in current slot i. */
            ndks_boot.freemem[i].end = start;
            /* Regions i and (i + 1) are now well defined, ordered, disjoint,
             * and unallocated, so we can return successfully. */
            திரும்பு true;
        }
        /* Region i isn't big enough, so shuffle it up to slot (i + 1),
         * which we know is unused. */
        ndks_boot.freemem[empty_index] = ndks_boot.freemem[i];
        /* Now region i is unused, so make it empty to reestablish the invariant
         * for the next iteration (when it will be slot i + 1). */
        ndks_boot.freemem[i] = REG_EMPTY;
    }

    /* We didn't find a big enough region. */
    printf("ERROR: no free memory region is big enough for root server "
           "objects, need size/alignment of 2^%"SEL4_PRIu_word"\n", max);
    திரும்பு false;
}
