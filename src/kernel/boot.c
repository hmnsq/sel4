/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <kernel/boot.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <machine/registerset.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <object/domain.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/kernel/boot.h>
#अन्तर्भाव <arch/kernel/vspace.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <hardware.h>
#अन्तर्भाव <util.h>

/* (node-local) state accessed only during bootstrapping */
BOOT_BSS ndks_boot_t ndks_boot;

BOOT_BSS rootserver_mem_t rootserver;
BOOT_BSS स्थिर region_t rootserver_mem;

/* Returns the physical region of the kernel image boot part, which is the part
 * that is no longer needed once booting is finished. */
बाह्य वर्ण ki_boot_अन्तः[1];
BOOT_संहिता p_region_t get_p_reg_kernel_img_boot(शून्य)
{
    प्रत्यागम (p_region_t) {
        .start = kpptr_गन्तव्यम्_paddr((ध्रुव शून्य *)KERNEL_ELF_आधारः),
        .अन्तः_2   = kpptr_गन्तव्यम्_paddr(ki_boot_अन्तः)
    };
}

/* Returns the physical region of the kernel image. */
BOOT_संहिता p_region_t get_p_reg_kernel_img(शून्य)
{
    प्रत्यागम (p_region_t) {
        .start = kpptr_गन्तव्यम्_paddr((ध्रुव शून्य *)KERNEL_ELF_आधारः),
        .अन्तः_2   = kpptr_गन्तव्यम्_paddr((ध्रुव शून्य *)KERNEL_ELF_शिखरम्)
    };
}

BOOT_संहिता स्थिर शून्य merge_regions(शून्य)
{
    /* Walk through reserved regions and see if any can be merged */
    word_t शिखरम् = 0;
    प्रति (word_t i = 1; i < ndks_boot.resv_गणनम्; i++) {
        यदि (ndks_boot.reserved[शिखरम्].अन्तः_2 == ndks_boot.reserved[i].start) {
            /* extend earlier region */
            ndks_boot.reserved[शिखरम्].अन्तः_2 = ndks_boot.reserved[i].अन्तः_2;
        } अन्यथा {
            /* add region */
            शिखरम्++;
            ndks_boot.reserved[शिखरम्] = ndks_boot.reserved[i];
        }
    }
    ndks_boot.resv_गणनम् = ndks_boot.resv_गणनम् > 0 ? शिखरम् + 1 : 0;
}

BOOT_संहिता bool_t reserve_region(p_region_t reg)
{
    word_t i;
    प्रतिज्ञा(reg.start <= reg.अन्तः_2);
    यदि (reg.start == reg.अन्तः_2) {
        प्रत्यागम true;
    }

    /* keep the regions in order */
    प्रति (i = 0; i < ndks_boot.resv_गणनम्; i++) {
        /* Try and merge the region to an existing one, if possible */
        यदि (ndks_boot.reserved[i].start == reg.अन्तः_2) {
            ndks_boot.reserved[i].start = reg.start;
            merge_regions();
            प्रत्यागम true;
        }
        यदि (ndks_boot.reserved[i].अन्तः_2 == reg.start) {
            ndks_boot.reserved[i].अन्तः_2 = reg.अन्तः_2;
            merge_regions();
            प्रत्यागम true;
        }
        /* Otherwise figure out where it should go. */
        यदि (ndks_boot.reserved[i].start > reg.अन्तः_2) {
            /* move regions down, making sure there's enough room */
            यदि (ndks_boot.resv_गणनम् + 1 >= MAX_अङ्कः_RESV_REG) {
                प्रारूप_लिख("Can't mark region 0x%"SEL4_PRIx_word"-0x%"SEL4_PRIx_word
                       " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
                       reg.start, reg.अन्तः_2, (पूर्णाङ्क)MAX_अङ्कः_RESV_REG);
                प्रत्यागम false;
            }
            प्रति (word_t j = ndks_boot.resv_गणनम्; j > i; j--) {
                ndks_boot.reserved[j] = ndks_boot.reserved[j - 1];
            }
            /* insert the new region */
            ndks_boot.reserved[i] = reg;
            ndks_boot.resv_गणनम्++;
            प्रत्यागम true;
        }
    }

    यदि (i + 1 == MAX_अङ्कः_RESV_REG) {
        प्रारूप_लिख("Can't mark region 0x%"SEL4_PRIx_word"-0x%"SEL4_PRIx_word
               " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
               reg.start, reg.अन्तः_2, (पूर्णाङ्क)MAX_अङ्कः_RESV_REG);
        प्रत्यागम false;
    }

    ndks_boot.reserved[i] = reg;
    ndks_boot.resv_गणनम्++;

    प्रत्यागम true;
}

BOOT_संहिता स्थिर bool_t निवेशय_region(region_t reg)
{
    प्रतिज्ञा(reg.start <= reg.अन्तः_2);
    यदि (is_reg_रिक्तानि(reg)) {
        प्रत्यागम true;
    }

    प्रति (word_t i = 0; i < ARRAY_आकार(ndks_boot.freemem); i++) {
        यदि (is_reg_रिक्तानि(ndks_boot.freemem[i])) {
            reserve_region(pptr_गन्तव्यम्_paddr_reg(reg));
            ndks_boot.freemem[i] = reg;
            प्रत्यागम true;
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
    प्रारूप_लिख("no free memory slot left for [%"SEL4_PRIx_word"..%"SEL4_PRIx_word"),"
           " consider increasing MAX_NUM_FREEMEM_REG (%u)\n",
           reg.start, reg.अन्तः_2, (अचिह्नित पूर्णाङ्क)MAX_अङ्कः_FREEMEM_REG);

    /* For debug builds we consider this a fatal error. Rationale is, that the
     * caller does not check the error code at the moment, but just ignores any
     * failures silently. */
    प्रतिज्ञा(0);

    प्रत्यागम false;
}

BOOT_संहिता स्थिर pptr_t alloc_rootserver_obj(word_t आकार_द्वयङ्काः, word_t n)
{
    pptr_t allocated = rootserver_mem.start;
    /* allocated memory must be aligned */
    प्रतिज्ञा(allocated % BIT(आकार_द्वयङ्काः) == 0);
    rootserver_mem.start += (n * BIT(आकार_द्वयङ्काः));
    /* we must not have run out of memory */
    प्रतिज्ञा(rootserver_mem.start <= rootserver_mem.अन्तः_2);
    memzero((शून्य *) allocated, n * BIT(आकार_द्वयङ्काः));
    प्रत्यागम allocated;
}

BOOT_संहिता स्थिर word_t rootserver_max_आकार_द्वयङ्काः(word_t extra_bi_आकार_द्वयङ्काः)
{
    word_t cnode_आकार_द्वयङ्काः = CONFIG_मूलम्_CNODE_आकार_द्वयङ्काः + se_L4_Slot_द्वयङ्काः;
    word_t max = MAX(cnode_आकार_द्वयङ्काः, se_L4_VSpace_द्वयङ्काः);
    प्रत्यागम MAX(max, extra_bi_आकार_द्वयङ्काः);
}

BOOT_संहिता स्थिर word_t calculate_rootserver_आकार(v_region_t it_v_reg, word_t extra_bi_आकार_द्वयङ्काः)
{
    /* work out how much memory we need for root server objects */
    word_t आकार = BIT(CONFIG_मूलम्_CNODE_आकार_द्वयङ्काः + se_L4_Slot_द्वयङ्काः);
    आकार += BIT(seL4_TCBBits); // root thread tcb
    आकार += BIT(se_L4_Page_द्वयङ्काः); // ipc buf
    आकार += BIT(se_L4_Boot_Info_Frame_द्वयङ्काः); // boot info
    आकार += BIT(se_L4_ASIDPool_द्वयङ्काः);
    आकार += extra_bi_आकार_द्वयङ्काः > 0 ? BIT(extra_bi_आकार_द्वयङ्काः) : 0;
    आकार += BIT(se_L4_VSpace_द्वयङ्काः); // root vspace
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    आकार += BIT(se_L4_Min_Sched_Context_द्वयङ्काः); // root sched context
#यदि_अन्तः
    /* for all archs, seL4_PageTable Bits is the size of all non top-level paging structures */
    प्रत्यागम आकार + arch_get_n_paging(it_v_reg) * BIT(se_L4_Page_सारणी_द्वयङ्काः);
}

BOOT_संहिता स्थिर शून्य maybe_alloc_extra_bi(word_t cmp_आकार_द्वयङ्काः, word_t extra_bi_आकार_द्वयङ्काः)
{
    यदि (extra_bi_आकार_द्वयङ्काः >= cmp_आकार_द्वयङ्काः && rootserver.extra_bi == 0) {
        rootserver.extra_bi = alloc_rootserver_obj(extra_bi_आकार_द्वयङ्काः, 1);
    }
}

/* Create pptrs for all root server objects, starting at a give start address,
 * to cover the virtual memory region v_reg, and any extra boot info.
 */
BOOT_संहिता स्थिर शून्य create_rootserver_objects(pptr_t start, v_region_t it_v_reg,
                                                word_t extra_bi_आकार_द्वयङ्काः)
{
    /* the largest object the PD, the root cnode, or the extra boot info */
    word_t cnode_आकार_द्वयङ्काः = CONFIG_मूलम्_CNODE_आकार_द्वयङ्काः + se_L4_Slot_द्वयङ्काः;
    word_t max = rootserver_max_आकार_द्वयङ्काः(extra_bi_आकार_द्वयङ्काः);

    word_t आकार = calculate_rootserver_आकार(it_v_reg, extra_bi_आकार_द्वयङ्काः);
    rootserver_mem.start = start;
    rootserver_mem.अन्तः_2 = start + आकार;

    maybe_alloc_extra_bi(max, extra_bi_आकार_द्वयङ्काः);

    /* the root cnode is at least 4k, so it could be larger or smaller than a pd. */
#यदि (CONFIG_मूलम्_CNODE_आकार_द्वयङ्काः + se_L4_Slot_द्वयङ्काः) > se_L4_VSpace_द्वयङ्काः
    rootserver.cnode = alloc_rootserver_obj(cnode_आकार_द्वयङ्काः, 1);
    maybe_alloc_extra_bi(se_L4_VSpace_द्वयङ्काः, extra_bi_आकार_द्वयङ्काः);
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_द्वयङ्काः, 1);
#अन्यथा
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_द्वयङ्काः, 1);
    maybe_alloc_extra_bi(cnode_आकार_द्वयङ्काः, extra_bi_आकार_द्वयङ्काः);
    rootserver.cnode = alloc_rootserver_obj(cnode_आकार_द्वयङ्काः, 1);
#यदि_अन्तः

    /* at this point we are up to creating 4k objects - which is the min size of
     * extra_bi so this is the last chance to allocate it */
    maybe_alloc_extra_bi(se_L4_Page_द्वयङ्काः, extra_bi_आकार_द्वयङ्काः);
    compile_assert(invalid_se_L4_ASIDPool_द्वयङ्काः, se_L4_ASIDPool_द्वयङ्काः == se_L4_Page_द्वयङ्काः);
    rootserver.asid_pool = alloc_rootserver_obj(se_L4_ASIDPool_द्वयङ्काः, 1);
    rootserver.ipc_buf = alloc_rootserver_obj(se_L4_Page_द्वयङ्काः, 1);
    /* The boot info size must be at least one page. Due to the hard-coded order
     * of allocations used in the current implementation here, it can't be any
     * bigger.
     */
    compile_assert(invalid_se_L4_Boot_Info_Frame_द्वयङ्काः, se_L4_Boot_Info_Frame_द्वयङ्काः == se_L4_Page_द्वयङ्काः);
    rootserver.boot_info = alloc_rootserver_obj(se_L4_Boot_Info_Frame_द्वयङ्काः, 1);

    /* TCBs on aarch32 can be larger than page tables in certain configs */
#यदि seL4_TCBBits >= se_L4_Page_सारणी_द्वयङ्काः
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#यदि_अन्तः

    /* paging structures are 4k on every arch except aarch32 (1k) */
    word_t n = arch_get_n_paging(it_v_reg);
    rootserver.paging.start = alloc_rootserver_obj(se_L4_Page_सारणी_द्वयङ्काः, n);
    rootserver.paging.अन्तः_2 = rootserver.paging.start + n * BIT(se_L4_Page_सारणी_द्वयङ्काः);

    /* for most archs, TCBs are smaller than page tables */
#यदि seL4_TCBBits < se_L4_Page_सारणी_द्वयङ्काः
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    rootserver.sc = alloc_rootserver_obj(se_L4_Min_Sched_Context_द्वयङ्काः, 1);
#यदि_अन्तः
    /* we should have allocated all our memory */
    प्रतिज्ञा(rootserver_mem.start == rootserver_mem.अन्तः_2);
}

BOOT_संहिता शून्य write_slot(slot_ptr_t slot_ptr, cap_t cap)
{
    slot_ptr->cap = cap;

    slot_ptr->cteMDBNode = nullMDBNode;
    mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_Revocable(&slot_ptr->cteMDBNode, true);
    mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_First_Badged(&slot_ptr->cteMDBNode, true);
}

/* Our root CNode needs to be able to fit all the initial caps and not
 * cover all of memory.
 */
compile_assert(मूलम्_cnode_आकार_valid,
               CONFIG_मूलम्_CNODE_आकार_द्वयङ्काः < 32 - se_L4_Slot_द्वयङ्काः &&
               BIT(CONFIG_मूलम्_CNODE_आकार_द्वयङ्काः) >= se_L4_अङ्कः_Initial_Caps &&
               CONFIG_मूलम्_CNODE_आकार_द्वयङ्काः >= (se_L4_Page_द्वयङ्काः - se_L4_Slot_द्वयङ्काः))

BOOT_संहिता cap_t
create_मूलम्_cnode(शून्य)
{
    cap_t cap = cap_cnode_cap_new(
                    CONFIG_मूलम्_CNODE_आकार_द्वयङ्काः, /* radix */
                    word_द्वयङ्काः - CONFIG_मूलम्_CNODE_आकार_द्वयङ्काः, /* guard size */
                    0, /* guard */
                    rootserver.cnode); /* pptr */

    /* write the root CNode cap into the root CNode */
    write_slot(SLOT_PTR(rootserver.cnode, seL4_CapInitThreadCNode), cap);

    प्रत्यागम cap;
}

/* Check domain scheduler assumptions. */
compile_assert(अङ्कः_domains_valid,
               CONFIG_अङ्कः_DOMAINS >= 1 && CONFIG_अङ्कः_DOMAINS <= 256)
compile_assert(अङ्कः_priorities_valid,
               CONFIG_अङ्कः_PRIORITIES >= 1 && CONFIG_अङ्कः_PRIORITIES <= 256)

BOOT_संहिता शून्य
create_domain_cap(cap_t मूलम्_cnode_cap)
{
    cap_t cap = cap_domain_cap_new();
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapDomain), cap);
}

BOOT_संहिता cap_t create_ipcbuf_frame_cap(cap_t मूलम्_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    clear_स्मृतिः((शून्य *)rootserver.ipc_buf, PAGE_द्वयङ्काः);

    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_frame_cap(pd_cap, rootserver.ipc_buf, vptr, IT_ASID, false, false);
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapInitThreadIPCBuffer), cap);

    प्रत्यागम cap;
}

BOOT_संहिता शून्य create_bi_frame_cap(cap_t मूलम्_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_frame_cap(pd_cap, rootserver.boot_info, vptr, IT_ASID, false, false);
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapBootInfoFrame), cap);
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
BOOT_संहिता word_t calculate_extra_bi_आकार_द्वयङ्काः(word_t extra_आकार)
{
    यदि (extra_आकार == 0) {
        प्रत्यागम 0;
    }

    word_t clzl_ret = clzl(आवर्तनम्_ऊर्ध्वम्(extra_आकार, se_L4_Page_द्वयङ्काः));
    word_t msb = se_L4_Word_द्वयङ्काः - 1 - clzl_ret;
    /* If region is bigger than a page, make sure we overallocate rather than
     * underallocate
     */
    यदि (extra_आकार > BIT(msb)) {
        msb++;
    }
    प्रत्यागम msb;
}

BOOT_संहिता शून्य populate_bi_frame(ग्रन्थिः_id_t ग्रन्थिः_id, word_t अङ्कः_nodes,
                                 vptr_t ipcbuf_vptr, word_t extra_bi_आकार)
{
    /* clear boot info memory */
    clear_स्मृतिः((शून्य *)rootserver.boot_info, se_L4_Boot_Info_Frame_द्वयङ्काः);
    यदि (extra_bi_आकार) {
        clear_स्मृतिः((शून्य *)rootserver.extra_bi,
                    calculate_extra_bi_आकार_द्वयङ्काः(extra_bi_आकार));
    }

    /* initialise bootinfo-related global state */
    seL4_BootInfo *bi = BI_PTR(rootserver.boot_info);
    bi->ग्रन्थिः_ID = ग्रन्थिः_id;
    bi->अङ्कः_Nodes = अङ्कः_nodes;
    bi->अङ्कः_IOPTLevels = 0;
    bi->ipc_बफर = (seL4_IPCBuffer *)ipcbuf_vptr;
    bi->init_Thread_CNode_आकार_द्वयङ्काः = CONFIG_मूलम्_CNODE_आकार_द्वयङ्काः;
    bi->initThreadDomain = 0;
    bi->extraLen = extra_bi_आकार;

    ndks_boot.bi_frame = bi;
    ndks_boot.slot_pos_cur = se_L4_अङ्कः_Initial_Caps;
}

BOOT_संहिता bool_t provide_cap(cap_t मूलम्_cnode_cap, cap_t cap)
{
    यदि (ndks_boot.slot_pos_cur >= BIT(CONFIG_मूलम्_CNODE_आकार_द्वयङ्काः)) {
        प्रारूप_लिख("ERROR: can't add another cap, all %"SEL4_PRIu_word
               " (=2^CONFIG_ROOT_CNODE_SIZE_BITS) slots used\n",
               BIT(CONFIG_मूलम्_CNODE_आकार_द्वयङ्काः));
        प्रत्यागम false;
    }
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), ndks_boot.slot_pos_cur), cap);
    ndks_boot.slot_pos_cur++;
    प्रत्यागम true;
}

BOOT_संहिता create_चौकटाः_सम्बन्धि_region_ret_t create_चौकटाः_सम्बन्धि_region(
    cap_t    मूलम्_cnode_cap,
    cap_t    pd_cap,
    region_t reg,
    bool_t   do_map,
    sword_t  pv_विस्थापनम्
)
{
    pptr_t     f;
    cap_t      frame_cap;
    seL4_SlotPos slot_pos_before;
    seL4_SlotPos slot_pos_after;

    slot_pos_before = ndks_boot.slot_pos_cur;

    प्रति (f = reg.start; f < reg.अन्तः_2; f += BIT(PAGE_द्वयङ्काः)) {
        यदि (do_map) {
            frame_cap = create_mapped_it_frame_cap(pd_cap, f, pptr_गन्तव्यम्_paddr((शून्य *)(f - pv_विस्थापनम्)), IT_ASID, false, true);
        } अन्यथा {
            frame_cap = create_unmapped_it_frame_cap(f, false);
        }
        यदि (!provide_cap(मूलम्_cnode_cap, frame_cap)) {
            प्रत्यागम (create_चौकटाः_सम्बन्धि_region_ret_t) {
                .region  = S_REG_रिक्तानि,
                .success = false
            };
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;

    प्रत्यागम (create_चौकटाः_सम्बन्धि_region_ret_t) {
        .region = (seL4_SlotRegion) {
            .start = slot_pos_before,
            .अन्तः_2   = slot_pos_after
        },
        .success = true
    };
}

BOOT_संहिता cap_t create_it_asid_pool(cap_t मूलम्_cnode_cap)
{
    cap_t ap_cap = cap_asid_pool_cap_new(ASID_उच्चः(IT_ASID) << asid_नीचः_द्वयङ्काः, rootserver.asid_pool);
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapInitThreadASIDPool), ap_cap);

    /* create ASID control cap */
    write_slot(
        SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapASIDControl),
        cap_asid_control_cap_new()
    );

    प्रत्यागम ap_cap;
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
BOOT_संहिता स्थिर शून्य configure_sched_context(tcb_t *tcb, sched_context_t *sc_pptr, ticks_t timeslice)
{
    tcb->tcbSchedContext = sc_pptr;
    refill_new(tcb->tcbSchedContext, MIN_REFILLS, timeslice, 0);
    tcb->tcbSchedContext->scTcb = tcb;
}

BOOT_संहिता bool_t init_sched_control(cap_t मूलम्_cnode_cap, word_t अङ्कः_nodes)
{
    seL4_SlotPos slot_pos_before = ndks_boot.slot_pos_cur;

    /* create a sched control cap for each core */
    प्रति (अचिह्नित पूर्णाङ्क i = 0; i < अङ्कः_nodes; i++) {
        यदि (!provide_cap(मूलम्_cnode_cap, cap_sched_control_cap_new(i))) {
            प्रारूप_लिख("can't init sched_control for node %u, provide_cap() failed\n", i);
            प्रत्यागम false;
        }
    }

    /* update boot info with slot region for sched control caps */
    ndks_boot.bi_frame->schedcontrol = (seL4_SlotRegion) {
        .start = slot_pos_before,
        .अन्तः_2 = ndks_boot.slot_pos_cur
    };

    प्रत्यागम true;
}
#यदि_अन्तः

BOOT_संहिता शून्य create_idle_thread(शून्य)
{
    pptr_t pptr;

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    प्रति (अचिह्नित पूर्णाङ्क i = 0; i < CONFIG_MAX_अङ्कः_NODES; i++) {
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */
        pptr = (pptr_t) &ksIdleThreadTCB[SMP_TERNARY(i, 0)];
        ग्रन्थिः_STATE_ON_CORE(ksIdleThread, i) = TCB_PTR(pptr + TCB_विस्थापनम्);
        configureIdleThread(ग्रन्थिः_STATE_ON_CORE(ksIdleThread, i));
#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
        समुच्चयः_Thread_Name(ग्रन्थिः_STATE_ON_CORE(ksIdleThread, i), "idle_thread");
#यदि_अन्तः
        SMP_COND_STATEMENT(ग्रन्थिः_STATE_ON_CORE(ksIdleThread, i)->tcbAffinity = i);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        configure_sched_context(ग्रन्थिः_STATE_ON_CORE(ksIdleThread, i), SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]),
                                MAX_PERIOD_TICKS);
        SMP_COND_STATEMENT(ग्रन्थिः_STATE_ON_CORE(ksIdleThread, i)->tcbSchedContext->scCore = i;)
        ग्रन्थिः_STATE_ON_CORE(ksIdleSC, i) = SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]);
#यदि_अन्तः
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    }
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */
}

BOOT_संहिता tcb_t *create_initial_thread(cap_t मूलम्_cnode_cap, cap_t it_pd_cap, vptr_t ui_v_प्रविष्टिः, vptr_t bi_frame_vptr,
                                       vptr_t ipcbuf_vptr, cap_t ipcbuf_cap)
{
    tcb_t *tcb = TCB_PTR(rootserver.tcb + TCB_विस्थापनम्);
#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
    tcb->tcb_Time_खण्डिका = CONFIG_TIME_खण्डिका;
#यदि_अन्तः

    Arch_initContext(&tcb->tcbArch.tcbContext);

    /* derive a copy of the IPC buffer cap for inserting */
    deriveCap_ret_t dc_ret = deriveCap(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapInitThreadIPCBuffer), ipcbuf_cap);
    यदि (dc_ret.स्थितिः != EXCEPTION_NONE) {
        प्रारूप_लिख("Failed to derive copy of IPC Buffer\n");
        प्रत्यागम रिक्त;
    }

    /* initialise TCB (corresponds directly to abstract specification) */
    cte_निवेशय(
        मूलम्_cnode_cap,
        SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapInitThreadCNode),
        SLOT_PTR(rootserver.tcb, tcbCTable)
    );
    cte_निवेशय(
        it_pd_cap,
        SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapInitThreadVSpace),
        SLOT_PTR(rootserver.tcb, tcbVTable)
    );
    cte_निवेशय(
        dc_ret.cap,
        SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapInitThreadIPCBuffer),
        SLOT_PTR(rootserver.tcb, tcb_बफर)
    );
    tcb->tcbIPCBuffer = ipcbuf_vptr;

    समुच्चयः_पञ्जिकाकोशः(tcb, cap_पञ्जिकाकोशः, bi_frame_vptr);
    समुच्चयः_अग्र_PC(tcb, ui_v_प्रविष्टिः);

    /* initialise TCB */
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    configure_sched_context(tcb, SC_PTR(rootserver.sc), us_गन्तव्यम्_Ticks(CONFIG_BOOT_THREAD_TIME_खण्डिका * US_अन्तर्_MS));
#यदि_अन्तः

    tcb->tcb_प्राथमिकता = seL4_MaxPrio;
    tcb->tcbMCP = seL4_MaxPrio;
    tcb->tcbDomain = 0;
#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
    सज्जता_प्रत्युत्तरम्_Master(tcb);
#यदि_अन्तः
    समुच्चयः_Thread_State(tcb, ThreadState_Running);

#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
    SMP_COND_STATEMENT(tcb->tcbAffinity = 0);
#यदि_अन्तः

    /* create initial thread's TCB cap */
    cap_t cap = cap_thread_cap_new(TCB_REF(tcb));
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapInitThreadTCB), cap);

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    cap = cap_sched_context_cap_new(SC_REF(tcb->tcbSchedContext), se_L4_Min_Sched_Context_द्वयङ्काः);
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapInitThreadSC), cap);
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
    समुच्चयः_Thread_Name(tcb, "rootserver");
#यदि_अन्तः

    प्रत्यागम tcb;
}

#यदि_परिभाषितम् ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
BOOT_संहिता स्थिर bool_t hypervisor_present(शून्य)
{
#यदि_परिभाषितम् CONFIG_ARCH_X86
    uint32_t ebx = x86_cpuid_ebx(KVM_CPUID_SIGNATURE, 0);
    uint32_t ecx = x86_cpuid_ecx(KVM_CPUID_SIGNATURE, 0);
    uint32_t edx = x86_cpuid_edx(KVM_CPUID_SIGNATURE, 0);

    यदि ((ebx == CPUID_KVM_EBX && ecx == CPUID_KVM_ECX && edx == CPUID_KVM_EDX)
        || (ebx == CPUID_TCG_EBX && ecx == CPUID_TCG_ECX && edx == CPUID_TCG_EDX)) {
        प्रत्यागम true;
    }
#यदि_अन्तः
    प्रत्यागम false;
}

BOOT_संहिता शून्य clock_sync_test(शून्य)
{
    ticks_t t, t0;
    ticks_t margin = us_गन्तव्यम्_Ticks(CLOCK_SYNC_DELTA) + getTimerPrecision();

    प्रतिज्ञा(getCurrentCPUIndex() != 0);
    t = ग्रन्थिः_STATE_ON_CORE(ksCurTime, 0);
    कुरु {
        /* perform a memory acquire to get new values of ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        t0 = ग्रन्थिः_STATE_ON_CORE(ksCurTime, 0);
    } यावत् (t0 == t);
    t = getCurrentTime();
    प्रारूप_लिख("clock_sync_test[%d]: t0 = %"PRIu64", t = %"PRIu64", td = %"PRIi64"\n",
           (पूर्णाङ्क)getCurrentCPUIndex(), t0, t, t - t0);
    /*
     * The test does not consistently work if we are in a virtual machine (e.g
     * within QEMU) because the measurement cannot distinguish between
     * interrupted clock reads and out-of-sync clocks.
     */
    यदि (hypervisor_present()) {
        प्रारूप_लिख("clock_sync_test[%d]: disabled, detected running as VM\n", (पूर्णाङ्क)getCurrentCPUIndex());
    } अन्यथा {
        प्रतिज्ञा(t0 <= margin + t && t <= t0 + margin);
    }
}
#यदि_अन्तः

BOOT_संहिता शून्य init_core_state(tcb_t *scheduler_action)
{
#यदि_परिभाषितम् CONFIG_HAVE_FPU
    ग्रन्थिः_STATE(ksCurFPUOwner) = रिक्त;
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
    /* add initial threads to the debug queue */
    ग्रन्थिः_STATE(ksDebugTCBs) = रिक्त;
    यदि (scheduler_action != SchedulerAction_ResumeCurrentThread &&
        scheduler_action != Scheduler_Action_वृणु_New_Thread) {
        tcbDebugAppend(scheduler_action);
    }
    tcbDebugAppend(ग्रन्थिः_STATE(ksIdleThread));
#यदि_अन्तः
    ग्रन्थिः_STATE(ksSchedulerAction) = scheduler_action;
    ग्रन्थिः_STATE(ksCurThread) = ग्रन्थिः_STATE(ksIdleThread);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    ग्रन्थिः_STATE(ksCurSC) = ग्रन्थिः_STATE(ksCurThread->tcbSchedContext);
    ग्रन्थिः_STATE(ksConsumed) = 0;
    ग्रन्थिः_STATE(ksReprogram) = true;
    ग्रन्थिः_STATE(ksReleaseQueue.शीर्ष) = रिक्त;
    ग्रन्थिः_STATE(ksReleaseQueue.अन्तः_2) = रिक्त;
    ग्रन्थिः_STATE(ksCurTime) = getCurrentTime();
#यदि_अन्तः
    /* No need for NODE_STATE() as there is no SMP support for domains */
    ksCurDomain = 0;
    ksDomainTime = DSCHED_MAX_DURATION;
    ks_Dom_कालसारणी[0] = dschedule_make(0, DSCHED_MAX_DURATION);
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
BOOT_संहिता स्थिर bool_t pptr_अन्तर्_kernel_गवाक्षः(pptr_t pptr)
{
    प्रत्यागम pptr >= PPTR_आधारः && pptr < PPTR_शिखरम्;
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
BOOT_संहिता स्थिर bool_t provide_untyped_cap(
    cap_t      मूलम्_cnode_cap,
    bool_t     device_स्मृतिः_2,
    pptr_t     pptr,
    word_t     आकार_द्वयङ्काः,
    seL4_SlotPos first_untyped_slot
)
{
    bool_t ret;
    cap_t ut_cap;

    /* Since we are in boot code, we can do extensive error checking and
       return failure if anything unexpected happens. */

    /* Bounds check for size parameter */
    यदि (आकार_द्वयङ्काः > se_L4_Max_Untyped_द्वयङ्काः || आकार_द्वयङ्काः < se_L4_Min_Untyped_द्वयङ्काः) {
        प्रारूप_लिख("Kernel init: Invalid untyped size %"SEL4_PRIu_word"\n", आकार_द्वयङ्काः);
        प्रत्यागम false;
    }

    /* All cap ptrs must be aligned to object size */
    यदि (!IS_ALIGNED(pptr, आकार_द्वयङ्काः)) {
        प्रारूप_लिख("Kernel init: Unaligned untyped pptr %p (alignment %"SEL4_PRIu_word")\n", (शून्य *)pptr, आकार_द्वयङ्काः);
        प्रत्यागम false;
    }

    /* All cap ptrs apart from device untypeds must be in the kernel window. */
    यदि (!device_स्मृतिः_2 && !pptr_अन्तर्_kernel_गवाक्षः(pptr)) {
        प्रारूप_लिख("Kernel init: Non-device untyped pptr %p outside kernel window\n",
               (शून्य *)pptr);
        प्रत्यागम false;
    }

    /* Check that the end of the region is also in the kernel window, so we don't
       need to assume that the kernel window is aligned up to potentially
       seL4_MaxUntypedBits. */
    यदि (!device_स्मृतिः_2 && !pptr_अन्तर्_kernel_गवाक्षः(pptr + आवरणम्(आकार_द्वयङ्काः))) {
        प्रारूप_लिख("Kernel init: End of non-device untyped at %p outside kernel window (size %"SEL4_PRIu_word")\n",
               (शून्य *)pptr, आकार_द्वयङ्काः);
        प्रत्यागम false;
    }

    word_t i = ndks_boot.slot_pos_cur - first_untyped_slot;
    यदि (i < CONFIG_MAX_अङ्कः_BOOTINFO_UNTYPED_CAPS) {
        ndks_boot.bi_frame->untypedList[i] = (seL4_UntypedDesc) {
            .paddr    = pptr_गन्तव्यम्_paddr((शून्य *)pptr),
            .आकार_द्वयङ्काः_2 = आकार_द्वयङ्काः,
            .isDevice = device_स्मृतिः_2,
            .padding  = {0}
        };
        ut_cap = cap_untyped_cap_new(MAX_FREE_सूचकांक(आकार_द्वयङ्काः),
                                     device_स्मृतिः_2, आकार_द्वयङ्काः, pptr);
        ret = provide_cap(मूलम्_cnode_cap, ut_cap);
    } अन्यथा {
        प्रारूप_लिख("Kernel init: Too many untyped regions for boot info\n");
        ret = true;
    }
    प्रत्यागम ret;
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
BOOT_संहिता स्थिर bool_t create_untypeds_for_region(
    cap_t      मूलम्_cnode_cap,
    bool_t     device_स्मृतिः_2,
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
    यावत् (!is_reg_रिक्तानि(reg)) {

        /* Calculate the bit size of the region. This is also correct for end < start: it will
           return the correct size of the set [start..-1] union [0..end). This will then be too
           large for alignment, so the code further down will reduce the size. */
        अचिह्नित पूर्णाङ्क आकार_द्वयङ्काः = se_L4_Word_द्वयङ्काः - 1 - clzl(reg.अन्तः_2 - reg.start);
        /* The size can't exceed the largest possible untyped size. */
        यदि (आकार_द्वयङ्काः > se_L4_Max_Untyped_द्वयङ्काः) {
            आकार_द्वयङ्काः = se_L4_Max_Untyped_द्वयङ्काः;
        }
        /* The start address 0 satisfies any alignment needs, otherwise ensure
         * the region's bit size does not exceed the alignment of the region.
         */
        यदि (0 != reg.start) {
            अचिह्नित पूर्णाङ्क align_द्वयङ्काः = ctzl(reg.start);
            यदि (आकार_द्वयङ्काः > align_द्वयङ्काः) {
                आकार_द्वयङ्काः = align_द्वयङ्काः;
            }
        }
        /* Provide an untyped capability for the region only if it is large
         * enough to be retyped into objects later. Otherwise the region can't
         * be used anyway.
         */
        यदि (आकार_द्वयङ्काः >= se_L4_Min_Untyped_द्वयङ्काः) {
            यदि (!provide_untyped_cap(मूलम्_cnode_cap, device_स्मृतिः_2, reg.start, आकार_द्वयङ्काः, first_untyped_slot)) {
                प्रत्यागम false;
            }
        }
        reg.start += BIT(आकार_द्वयङ्काः);
    }
    प्रत्यागम true;
}

BOOT_संहिता bool_t create_untypeds(cap_t मूलम्_cnode_cap)
{
    seL4_SlotPos first_untyped_slot = ndks_boot.slot_pos_cur;

    paddr_t start = 0;
    प्रति (word_t i = 0; i < ndks_boot.resv_गणनम्; i++) {
        यदि (start < ndks_boot.reserved[i].start) {
            region_t reg = paddr_गन्तव्यम्_pptr_reg((p_region_t) {
                start, ndks_boot.reserved[i].start
            });
            यदि (!create_untypeds_for_region(मूलम्_cnode_cap, true, reg, first_untyped_slot)) {
                प्रारूप_लिख("ERROR: creation of untypeds for device region #%u at"
                       " [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") failed\n",
                       (अचिह्नित पूर्णाङ्क)i, reg.start, reg.अन्तः_2);
                प्रत्यागम false;
            }
        }

        start = ndks_boot.reserved[i].अन्तः_2;
    }

    यदि (start <= CONFIG_PADDR_USER_DEVICE_शिखरम् - 1) {
        region_t reg = paddr_गन्तव्यम्_pptr_reg((p_region_t) {
            /* CONFIG_PADDR_USER_DEVICE_TOP cast to paddr_t can be 0.
             * create_untypeds_for_region() can deal with that correctly. */
            start, (paddr_t) CONFIG_PADDR_USER_DEVICE_शिखरम्
        });

        यदि (!create_untypeds_for_region(मूलम्_cnode_cap, true, reg, first_untyped_slot)) {
            प्रारूप_लिख("ERROR: creation of untypeds for top device region"
                   " [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") failed\n",
                   reg.start, reg.अन्तः_2);
            प्रत्यागम false;
        }
    }

    /* There is a part of the kernel (code/data) that is only needed for the
     * boot process. We can create UT objects for these frames, so the memory
     * can be reused.
     */
    region_t boot_mem_reuse_reg = paddr_गन्तव्यम्_pptr_reg(get_p_reg_kernel_img_boot());
    यदि (!create_untypeds_for_region(मूलम्_cnode_cap, false, boot_mem_reuse_reg, first_untyped_slot)) {
        प्रारूप_लिख("ERROR: creation of untypeds for recycled boot memory"
               " [%"SEL4_PRIx_word"..%"SEL4_PRIx_word"] failed\n",
               boot_mem_reuse_reg.start, boot_mem_reuse_reg.अन्तः_2);
        प्रत्यागम false;
    }

    /* convert remaining freemem into UT objects and provide the caps */
    प्रति (word_t i = 0; i < ARRAY_आकार(ndks_boot.freemem); i++) {
        region_t reg = ndks_boot.freemem[i];
        ndks_boot.freemem[i] = REG_रिक्तानि;
        यदि (!create_untypeds_for_region(मूलम्_cnode_cap, false, reg, first_untyped_slot)) {
            प्रारूप_लिख("ERROR: creation of untypeds for free memory region #%u at"
                   " [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") failed\n",
                   (अचिह्नित पूर्णाङ्क)i, reg.start, reg.अन्तः_2);
            प्रत्यागम false;
        }
    }

    ndks_boot.bi_frame->untyped = (seL4_SlotRegion) {
        .start = first_untyped_slot,
        .अन्तः_2   = ndks_boot.slot_pos_cur
    };

    प्रत्यागम true;
}

BOOT_संहिता शून्य bi_finalise(शून्य)
{

    यदि (rootserver.paging.start != rootserver.paging.अन्तः_2) {
        प्रारूप_लिख("WARNING: internal book keeping error. Less pagetables allocated than predicted: "
               "%ld page tables allocated but not used.\n", (rootserver.paging.अन्तः_2 - rootserver.paging.start) >> se_L4_Page_सारणी_द्वयङ्काः);
    }

    ndks_boot.bi_frame->रिक्तानि = (seL4_SlotRegion) {
        .start = ndks_boot.slot_pos_cur,
        .अन्तः_2   = BIT(CONFIG_मूलम्_CNODE_आकार_द्वयङ्काः)
    };
}

BOOT_संहिता स्थिर bool_t check_उपलब्धम्_स्मृतिः(word_t n_उपलब्धम्,
                                               ध्रुव p_region_t *उपलब्धम्)
{
    /* The system configuration is broken if no region is available. */
    यदि (0 == n_उपलब्धम्) {
        प्रारूप_लिख("ERROR: no memory regions available\n");
        प्रत्यागम false;
    }

    प्रारूप_लिख("Available phys memory regions: %"SEL4_PRIu_word"\n", n_उपलब्धम्);
    /* Force ordering and exclusivity of available regions. */
    प्रति (word_t i = 0; i < n_उपलब्धम्; i++) {
        ध्रुव p_region_t *r = &उपलब्धम्[i];
        प्रारूप_लिख("  [%"SEL4_PRIx_word"..%"SEL4_PRIx_word")\n", r->start, r->अन्तः_2);

        /* Available regions must not wrap */
        यदि (r->start > r->अन्तः_2) {
            प्रारूप_लिख("ERROR: memory region %"SEL4_PRIu_word" has start > end\n", i);
            प्रत्यागम false;
        }

        /* Available regions must be non-empty. */
        यदि (r->start == r->अन्तः_2) {
            प्रारूप_लिख("ERROR: memory region %"SEL4_PRIu_word" empty\n", i);
            प्रत्यागम false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
         * so the '==' case is fine. Directly adjacent regions are allowed.
         */
        यदि ((i > 0) && (r->start < उपलब्धम्[i - 1].अन्तः_2)) {
            प्रारूप_लिख("ERROR: memory region %"SEL4_PRIu_word" in wrong order\n", i);
            प्रत्यागम false;
        }
    }

    प्रत्यागम true;
}

/* We can't declare arrays on the stack, so this is space for init_avail_reg()
 * below to populate and init_freemem() to consume. */
BOOT_BSS स्थिर region_t avail_reg[MAX_अङ्कः_FREEMEM_REG];

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
BOOT_संहिता स्थिर word_t init_avail_reg(word_t n_उपलब्धम्,
                                       ध्रुव p_region_t *उपलब्धम्)
{
    word_t cnt = 0;
    प्रति (word_t i = 0; i < n_उपलब्धम्; i++) {
        ध्रुव p_region_t *r = &उपलब्धम्[i];

        यदि ((r->start >= PADDR_शिखरम्) || (r->अन्तः_2 <= PADDR_आधारः)) {
            प्रारूप_लिख("  region [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") is outside the kernel"
                   " window, only potentially available as device untypeds\n", r->start, r->अन्तः_2);
        } अन्यथा {
            p_region_t usable_reg = {
                .start = MAX(r->start, PADDR_आधारः),
                .अन्तः_2   = MIN(r->अन्तः_2, PADDR_शिखरम्)
            };

            यदि (cnt >= ARRAY_आकार(avail_reg)) {
                /* No space left in the array, seems MAX_NUM_FREEMEM_REG should
                 * be increased. Debug builds raise an assert here because this
                 * is likely a porting issue that should be looked into. Release
                 * builds will continue booting, but can't use this memory
                 * region. This might still be sufficient to run.
                 */
                प्रारूप_लिख("  WARNING: cannot use region [%"SEL4_PRIx_word"..%"SEL4_PRIx_word"),"
                       " avail_reg[] is full\n", r->start, r->अन्तः_2);
                प्रतिज्ञा(0);
            } अन्यथा {
                avail_reg[cnt] = paddr_गन्तव्यम्_pptr_reg(usable_reg);
                cnt++;
            }
        }
    }
    प्रत्यागम cnt;
}


BOOT_संहिता स्थिर bool_t check_reserved_स्मृतिः(word_t n_reserved,
                                              ध्रुव region_t *reserved)
{
    प्रारूप_लिख("Reserved virt address space regions: %"SEL4_PRIu_word"\n",
           n_reserved);
    /* Force ordering and exclusivity of reserved regions. */
    प्रति (word_t i = 0; i < n_reserved; i++) {
        ध्रुव region_t *r = &reserved[i];
        प्रारूप_लिख("  [%"SEL4_PRIx_word"..%"SEL4_PRIx_word")\n", r->start, r->अन्तः_2);

        /* Reserved regions must be sane, the size is allowed to be zero. */
        यदि (r->start > r->अन्तः_2) {
            प्रारूप_लिख("ERROR: reserved region %"SEL4_PRIu_word" has start > end\n", i);
            प्रत्यागम false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
           so the == case is fine. Directly adjacent regions are allowed. */
        यदि ((i > 0) && (r->start < reserved[i - 1].अन्तः_2)) {
            प्रारूप_लिख("ERROR: reserved region %"SEL4_PRIu_word" in wrong order\n", i);
            प्रत्यागम false;
        }
    }

    प्रत्यागम true;
}

/**
 * Dynamically initialise the available memory on the platform.
 * A region represents an area of memory.
 */
BOOT_संहिता bool_t init_freemem(word_t n_उपलब्धम्, ध्रुव p_region_t *उपलब्धम्,
                              word_t n_reserved, ध्रुव region_t *reserved,
                              v_region_t it_v_reg, word_t extra_bi_आकार_द्वयङ्काः)
{
    यदि (!check_उपलब्धम्_स्मृतिः(n_उपलब्धम्, उपलब्धम्)) {
        प्रत्यागम false;
    }

    /* After here, avail_reg[0..n_available) are all regions within the kernel window. */
    n_उपलब्धम् = init_avail_reg(n_उपलब्धम्, उपलब्धम्);
    यदि (0 == n_उपलब्धम्) {
        प्रारूप_लिख("ERROR: no available memory within the kernel window\n");
        प्रत्यागम false;
    }

    यदि (!check_reserved_स्मृतिः(n_reserved, reserved)) {
        प्रत्यागम false;
    }

    प्रति (word_t i = 0; i < ARRAY_आकार(ndks_boot.freemem); i++) {
        ndks_boot.freemem[i] = REG_रिक्तानि;
    }

    word_t a = 0;
    word_t r = 0;
    /* Now iterate through the available regions, removing any reserved regions. */
    यावत् (a < n_उपलब्धम् && r < n_reserved) {
        यदि (reserved[r].start == reserved[r].अन्तः_2) {
            /* reserved region is empty - skip it */
            r++;
        } अन्यथा यदि (avail_reg[a].start >= avail_reg[a].अन्तः_2) {
            /* skip the entire region - it's empty now after trimming */
            a++;
        } अन्यथा यदि (reserved[r].अन्तः_2 <= avail_reg[a].start) {
            /* the reserved region is below the available region - skip it */
            reserve_region(pptr_गन्तव्यम्_paddr_reg(reserved[r]));
            r++;
        } अन्यथा यदि (reserved[r].start >= avail_reg[a].अन्तः_2) {
            /* the reserved region is above the available region - take the whole thing */
            निवेशय_region(avail_reg[a]);
            a++;
        } अन्यथा {
            /* the reserved region overlaps with the available region */
            यदि (reserved[r].start <= avail_reg[a].start) {
                /* the region overlaps with the start of the available region.
                 * trim start of the available region */
                avail_reg[a].start = MIN(avail_reg[a].अन्तः_2, reserved[r].अन्तः_2);
                /* do not increment reserved index here - there could be more overlapping regions */
            } अन्यथा {
                प्रतिज्ञा(reserved[r].start < avail_reg[a].अन्तः_2);
                /* take the first chunk of the available region and move
                 * the start to the end of the reserved region */
                region_t m = avail_reg[a];
                m.अन्तः_2 = reserved[r].start;
                निवेशय_region(m);
                यदि (avail_reg[a].अन्तः_2 > reserved[r].अन्तः_2) {
                    avail_reg[a].start = reserved[r].अन्तः_2;
                    /* we could increment reserved index here, but it's more consistent with the
                     * other overlapping case if we don't */
                } अन्यथा {
                    a++;
                }
            }
        }
    }

    प्रति (; r < n_reserved; r++) {
        यदि (reserved[r].start < reserved[r].अन्तः_2) {
            reserve_region(pptr_गन्तव्यम्_paddr_reg(reserved[r]));
        }
    }

    /* no more reserved regions - add the rest */
    प्रति (; a < n_उपलब्धम्; a++) {
        यदि (avail_reg[a].start < avail_reg[a].अन्तः_2) {
            निवेशय_region(avail_reg[a]);
        }
    }

    /* now try to fit the root server objects into a region */
    पूर्णाङ्क i = ARRAY_आकार(ndks_boot.freemem) - 1;
    यदि (!is_reg_रिक्तानि(ndks_boot.freemem[i])) {
        प्रारूप_लिख("ERROR: insufficient MAX_NUM_FREEMEM_REG (%u)\n",
               (अचिह्नित पूर्णाङ्क)MAX_अङ्कः_FREEMEM_REG);
        प्रत्यागम false;
    }
    /* skip any empty regions */
    प्रति (; i >= 0 && is_reg_रिक्तानि(ndks_boot.freemem[i]); i--);

    /* try to grab the last available p region to create the root server objects
     * from. If possible, retain any left over memory as an extra p region */
    word_t आकार = calculate_rootserver_आकार(it_v_reg, extra_bi_आकार_द्वयङ्काः);
    word_t max = rootserver_max_आकार_द्वयङ्काः(extra_bi_आकार_द्वयङ्काः);
    प्रति (; i >= 0; i--) {
        /* Invariant: both i and (i + 1) are valid indices in ndks_boot.freemem. */
        प्रतिज्ञा(i < ARRAY_आकार(ndks_boot.freemem) - 1);
        /* Invariant; the region at index i is the current candidate.
         * Invariant: regions 0 up to (i - 1), if any, are additional candidates.
         * Invariant: region (i + 1) is empty. */
        प्रतिज्ञा(is_reg_रिक्तानि(ndks_boot.freemem[i + 1]));
        /* Invariant: regions above (i + 1), if any, are empty or too small to use.
         * Invariant: all non-empty regions are ordered, disjoint and unallocated. */

        /* We make a fresh variable to index the known-empty region, because the
         * SimplExportAndRefine verification test has poor support for array
         * indices that are sums of variables and small constants. */
        पूर्णाङ्क रिक्तानि_सूचकांक = i + 1;

        /* Try to take the top-most suitably sized and aligned chunk. */
        pptr_t unaligned_start = ndks_boot.freemem[i].अन्तः_2 - आकार;
        pptr_t start = आवर्तनम्_अधस्तात्(unaligned_start, max);
        /* if unaligned_start didn't underflow, and start fits in the region,
         * then we've found a region that fits the root server objects. */
        यदि (unaligned_start <= ndks_boot.freemem[i].अन्तः_2
            && start >= ndks_boot.freemem[i].start) {
            create_rootserver_objects(start, it_v_reg, extra_bi_आकार_द्वयङ्काः);
            /* There may be leftovers before and after the memory we used. */
            /* Shuffle the after leftover up to the empty slot (i + 1). */
            ndks_boot.freemem[रिक्तानि_सूचकांक] = (region_t) {
                .start = start + आकार,
                .अन्तः_2 = ndks_boot.freemem[i].अन्तः_2
            };
            /* Leave the before leftover in current slot i. */
            ndks_boot.freemem[i].अन्तः_2 = start;
            /* Regions i and (i + 1) are now well defined, ordered, disjoint,
             * and unallocated, so we can return successfully. */
            प्रत्यागम true;
        }
        /* Region i isn't big enough, so shuffle it up to slot (i + 1),
         * which we know is unused. */
        ndks_boot.freemem[रिक्तानि_सूचकांक] = ndks_boot.freemem[i];
        /* Now region i is unused, so make it empty to reestablish the invariant
         * for the next iteration (when it will be slot i + 1). */
        ndks_boot.freemem[i] = REG_रिक्तानि;
    }

    /* We didn't find a big enough region. */
    प्रारूप_लिख("ERROR: no free memory region is big enough for root server "
           "objects, need size/alignment of 2^%"SEL4_PRIu_word"\n", max);
    प्रत्यागम false;
}
