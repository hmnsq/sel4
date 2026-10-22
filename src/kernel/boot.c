/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <kernel/boot.h>
#ضمن <kernel/thread.h>
#ضمن <machine/io.h>
#ضمن <machine/registerset.h>
#ضمن <model/statedata.h>
#ضمن <object/domain.h>
#ضمن <arch/machine.h>
#ضمن <arch/kernel/boot.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <linker.h>
#ضمن <hardware.h>
#ضمن <util.h>

/* (node-local) state accessed only during bootstrapping */
BOOT_BSS ndks_boot_t ndks_boot;

BOOT_BSS rootserver_mem_t rootserver;
BOOT_BSS ساكن region_t rootserver_mem;

/* Returns the physical region of the kernel image boot part, which is the part
 * that is no longer needed once booting is finished. */
خارجي حرف ki_boot_انتها[1];
BOOT_کد p_region_t get_p_reg_kernel_img_boot(فراغ)
{
    أرجع (p_region_t) {
        .شروع = kpptr_مقصد_paddr((ثابت فراغ *)KERNEL_ELF_پایه),
        .انتها   = kpptr_مقصد_paddr(ki_boot_انتها)
    };
}

/* Returns the physical region of the kernel image. */
BOOT_کد p_region_t get_p_reg_kernel_img(فراغ)
{
    أرجع (p_region_t) {
        .شروع = kpptr_مقصد_paddr((ثابت فراغ *)KERNEL_ELF_پایه),
        .انتها   = kpptr_مقصد_paddr((ثابت فراغ *)KERNEL_ELF_بالاترین)
    };
}

BOOT_کد ساكن فراغ ادغام_regions(فراغ)
{
    /* Walk through reserved regions and see if any can be merged */
    کلمه_t بالاترین = 0;
    لكل (کلمه_t i = 1; i < ndks_boot.resv_شمار; i++) {
        إذا (ndks_boot.reserved[بالاترین].انتها == ndks_boot.reserved[i].شروع) {
            /* extend earlier region */
            ndks_boot.reserved[بالاترین].انتها = ndks_boot.reserved[i].انتها;
        } وإلا {
            /* add region */
            بالاترین++;
            ndks_boot.reserved[بالاترین] = ndks_boot.reserved[i];
        }
    }
    ndks_boot.resv_شمار = ndks_boot.resv_شمار > 0 ? بالاترین + 1 : 0;
}

BOOT_کد bool_t reserve_region(p_region_t reg)
{
    کلمه_t i;
    أكد(reg.شروع <= reg.انتها);
    إذا (reg.شروع == reg.انتها) {
        أرجع true;
    }

    /* keep the regions in order */
    لكل (i = 0; i < ndks_boot.resv_شمار; i++) {
        /* Try and merge the region to an existing one, if possible */
        إذا (ndks_boot.reserved[i].شروع == reg.انتها) {
            ndks_boot.reserved[i].شروع = reg.شروع;
            ادغام_regions();
            أرجع true;
        }
        إذا (ndks_boot.reserved[i].انتها == reg.شروع) {
            ndks_boot.reserved[i].انتها = reg.انتها;
            ادغام_regions();
            أرجع true;
        }
        /* Otherwise figure out where it should go. */
        إذا (ndks_boot.reserved[i].شروع > reg.انتها) {
            /* move regions down, making sure there's enough room */
            إذا (ndks_boot.resv_شمار + 1 >= MAX_رقم_RESV_REG) {
                چاپ_کن("Can't mark region 0x%"SEL4_PRIx_کلمه"-0x%"SEL4_PRIx_کلمه
                       " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
                       reg.شروع, reg.انتها, (صحيح)MAX_رقم_RESV_REG);
                أرجع false;
            }
            لكل (کلمه_t j = ndks_boot.resv_شمار; j > i; j--) {
                ndks_boot.reserved[j] = ndks_boot.reserved[j - 1];
            }
            /* insert the new region */
            ndks_boot.reserved[i] = reg;
            ndks_boot.resv_شمار++;
            أرجع true;
        }
    }

    إذا (i + 1 == MAX_رقم_RESV_REG) {
        چاپ_کن("Can't mark region 0x%"SEL4_PRIx_کلمه"-0x%"SEL4_PRIx_کلمه
               " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
               reg.شروع, reg.انتها, (صحيح)MAX_رقم_RESV_REG);
        أرجع false;
    }

    ndks_boot.reserved[i] = reg;
    ndks_boot.resv_شمار++;

    أرجع true;
}

BOOT_کد ساكن bool_t درج_region(region_t reg)
{
    أكد(reg.شروع <= reg.انتها);
    إذا (is_reg_خالی(reg)) {
        أرجع true;
    }

    لكل (کلمه_t i = 0; i < ARRAY_اندازه(ndks_boot.freemem); i++) {
        إذا (is_reg_خالی(ndks_boot.freemem[i])) {
            reserve_region(pptr_مقصد_paddr_reg(reg));
            ndks_boot.freemem[i] = reg;
            أرجع true;
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
    چاپ_کن("no free memory slot left for [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه"),"
           " consider increasing MAX_NUM_FREEMEM_REG (%u)\n",
           reg.شروع, reg.انتها, (بیعلامت صحيح)MAX_رقم_FREEMEM_REG);

    /* For debug builds we consider this a fatal error. Rationale is, that the
     * caller does not check the error code at the moment, but just ignores any
     * failures silently. */
    أكد(0);

    أرجع false;
}

BOOT_کد ساكن pptr_t alloc_rootserver_obj(کلمه_t اندازه_بیتها, کلمه_t n)
{
    pptr_t allocated = rootserver_mem.شروع;
    /* allocated memory must be aligned */
    أكد(allocated % بیت(اندازه_بیتها) == 0);
    rootserver_mem.شروع += (n * بیت(اندازه_بیتها));
    /* we must not have run out of memory */
    أكد(rootserver_mem.شروع <= rootserver_mem.انتها);
    memzero((فراغ *) allocated, n * بیت(اندازه_بیتها));
    أرجع allocated;
}

BOOT_کد ساكن کلمه_t rootserver_max_اندازه_بیتها(کلمه_t extra_bi_اندازه_بیتها)
{
    کلمه_t cnode_اندازه_بیتها = CONFIG_ریشه_CNODE_اندازه_بیتها + se_L4_جایگاه_بیتها;
    کلمه_t max = MAX(cnode_اندازه_بیتها, se_L4_VSpace_بیتها);
    أرجع MAX(max, extra_bi_اندازه_بیتها);
}

BOOT_کد ساكن کلمه_t calculate_rootserver_اندازه(v_region_t it_v_reg, کلمه_t extra_bi_اندازه_بیتها)
{
    /* work out how much memory we need for root server objects */
    کلمه_t اندازه = بیت(CONFIG_ریشه_CNODE_اندازه_بیتها + se_L4_جایگاه_بیتها);
    اندازه += بیت(seL4_TCBBits); // root thread tcb
    اندازه += بیت(se_L4_صفحه_بیتها); // ipc buf
    اندازه += بیت(se_L4_Boot_Info_قاب_بیتها); // boot info
    اندازه += بیت(se_L4_ASIDPool_بیتها);
    اندازه += extra_bi_اندازه_بیتها > 0 ? بیت(extra_bi_اندازه_بیتها) : 0;
    اندازه += بیت(se_L4_VSpace_بیتها); // root vspace
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    اندازه += بیت(se_L4_Min_Sched_Context_بیتها); // root sched context
#پایان_اگر
    /* for all archs, seL4_PageTable Bits is the size of all non top-level paging structures */
    أرجع اندازه + arch_get_n_paging(it_v_reg) * بیت(se_L4_صفحه_جدول_بیتها);
}

BOOT_کد ساكن فراغ maybe_alloc_extra_bi(کلمه_t cmp_اندازه_بیتها, کلمه_t extra_bi_اندازه_بیتها)
{
    إذا (extra_bi_اندازه_بیتها >= cmp_اندازه_بیتها && rootserver.extra_bi == 0) {
        rootserver.extra_bi = alloc_rootserver_obj(extra_bi_اندازه_بیتها, 1);
    }
}

/* Create pptrs for all root server objects, starting at a give start address,
 * to cover the virtual memory region v_reg, and any extra boot info.
 */
BOOT_کد ساكن فراغ create_rootserver_objects(pptr_t شروع, v_region_t it_v_reg,
                                                کلمه_t extra_bi_اندازه_بیتها)
{
    /* the largest object the PD, the root cnode, or the extra boot info */
    کلمه_t cnode_اندازه_بیتها = CONFIG_ریشه_CNODE_اندازه_بیتها + se_L4_جایگاه_بیتها;
    کلمه_t max = rootserver_max_اندازه_بیتها(extra_bi_اندازه_بیتها);

    کلمه_t اندازه = calculate_rootserver_اندازه(it_v_reg, extra_bi_اندازه_بیتها);
    rootserver_mem.شروع = شروع;
    rootserver_mem.انتها = شروع + اندازه;

    maybe_alloc_extra_bi(max, extra_bi_اندازه_بیتها);

    /* the root cnode is at least 4k, so it could be larger or smaller than a pd. */
#إذا (CONFIG_ریشه_CNODE_اندازه_بیتها + se_L4_جایگاه_بیتها) > se_L4_VSpace_بیتها
    rootserver.cnode = alloc_rootserver_obj(cnode_اندازه_بیتها, 1);
    maybe_alloc_extra_bi(se_L4_VSpace_بیتها, extra_bi_اندازه_بیتها);
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_بیتها, 1);
#وإلا
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_بیتها, 1);
    maybe_alloc_extra_bi(cnode_اندازه_بیتها, extra_bi_اندازه_بیتها);
    rootserver.cnode = alloc_rootserver_obj(cnode_اندازه_بیتها, 1);
#پایان_اگر

    /* at this point we are up to creating 4k objects - which is the min size of
     * extra_bi so this is the last chance to allocate it */
    maybe_alloc_extra_bi(se_L4_صفحه_بیتها, extra_bi_اندازه_بیتها);
    compile_assert(invalid_se_L4_ASIDPool_بیتها, se_L4_ASIDPool_بیتها == se_L4_صفحه_بیتها);
    rootserver.asid_pool = alloc_rootserver_obj(se_L4_ASIDPool_بیتها, 1);
    rootserver.ipc_buf = alloc_rootserver_obj(se_L4_صفحه_بیتها, 1);
    /* The boot info size must be at least one page. Due to the hard-coded order
     * of allocations used in the current implementation here, it can't be any
     * bigger.
     */
    compile_assert(invalid_se_L4_Boot_Info_قاب_بیتها, se_L4_Boot_Info_قاب_بیتها == se_L4_صفحه_بیتها);
    rootserver.boot_info = alloc_rootserver_obj(se_L4_Boot_Info_قاب_بیتها, 1);

    /* TCBs on aarch32 can be larger than page tables in certain configs */
#إذا seL4_TCBBits >= se_L4_صفحه_جدول_بیتها
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#پایان_اگر

    /* paging structures are 4k on every arch except aarch32 (1k) */
    کلمه_t n = arch_get_n_paging(it_v_reg);
    rootserver.paging.شروع = alloc_rootserver_obj(se_L4_صفحه_جدول_بیتها, n);
    rootserver.paging.انتها = rootserver.paging.شروع + n * بیت(se_L4_صفحه_جدول_بیتها);

    /* for most archs, TCBs are smaller than page tables */
#إذا seL4_TCBBits < se_L4_صفحه_جدول_بیتها
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#پایان_اگر

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    rootserver.sc = alloc_rootserver_obj(se_L4_Min_Sched_Context_بیتها, 1);
#پایان_اگر
    /* we should have allocated all our memory */
    أكد(rootserver_mem.شروع == rootserver_mem.انتها);
}

BOOT_کد فراغ write_جایگاه(جایگاه_ptr_t جایگاه_ptr, cap_t cap)
{
    جایگاه_ptr->cap = cap;

    جایگاه_ptr->cteMDBNode = nullMDBNode;
    mdb_گره_ptr_مجموعه_mdb_Revocable(&جایگاه_ptr->cteMDBNode, true);
    mdb_گره_ptr_مجموعه_mdb_اولی_Badged(&جایگاه_ptr->cteMDBNode, true);
}

/* Our root CNode needs to be able to fit all the initial caps and not
 * cover all of memory.
 */
compile_assert(ریشه_cnode_اندازه_valid,
               CONFIG_ریشه_CNODE_اندازه_بیتها < 32 - se_L4_جایگاه_بیتها &&
               بیت(CONFIG_ریشه_CNODE_اندازه_بیتها) >= se_L4_رقم_Initial_Caps &&
               CONFIG_ریشه_CNODE_اندازه_بیتها >= (se_L4_صفحه_بیتها - se_L4_جایگاه_بیتها))

BOOT_کد cap_t
create_ریشه_cnode(فراغ)
{
    cap_t cap = cap_cnode_cap_new(
                    CONFIG_ریشه_CNODE_اندازه_بیتها, /* radix */
                    کلمه_بیتها - CONFIG_ریشه_CNODE_اندازه_بیتها, /* guard size */
                    0, /* guard */
                    rootserver.cnode); /* pptr */

    /* write the root CNode cap into the root CNode */
    write_جایگاه(جایگاه_PTR(rootserver.cnode, seL4_CapInitThreadCNode), cap);

    أرجع cap;
}

/* Check domain scheduler assumptions. */
compile_assert(رقم_domains_valid,
               CONFIG_رقم_DOMAINS >= 1 && CONFIG_رقم_DOMAINS <= 256)
compile_assert(رقم_priorities_valid,
               CONFIG_رقم_PRIORITIES >= 1 && CONFIG_رقم_PRIORITIES <= 256)

BOOT_کد فراغ
create_domain_cap(cap_t ریشه_cnode_cap)
{
    cap_t cap = cap_domain_cap_new();
    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapDomain), cap);
}

BOOT_کد cap_t create_ipcbuf_قاب_cap(cap_t ریشه_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    clear_حافظه((فراغ *)rootserver.ipc_buf, صفحه_بیتها);

    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_قاب_cap(pd_cap, rootserver.ipc_buf, vptr, IT_ASID, false, false);
    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapInitThreadIPCBuffer), cap);

    أرجع cap;
}

BOOT_کد فراغ create_bi_قاب_cap(cap_t ریشه_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_قاب_cap(pd_cap, rootserver.boot_info, vptr, IT_ASID, false, false);
    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), se_L4_Cap_Boot_Info_قاب), cap);
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
BOOT_کد کلمه_t calculate_extra_bi_اندازه_بیتها(کلمه_t extra_اندازه)
{
    إذا (extra_اندازه == 0) {
        أرجع 0;
    }

    کلمه_t clzl_ret = clzl(دور_به_بالا(extra_اندازه, se_L4_صفحه_بیتها));
    کلمه_t msb = se_L4_کلمه_بیتها - 1 - clzl_ret;
    /* If region is bigger than a page, make sure we overallocate rather than
     * underallocate
     */
    إذا (extra_اندازه > بیت(msb)) {
        msb++;
    }
    أرجع msb;
}

BOOT_کد فراغ populate_bi_قاب(گره_id_t گره_id, کلمه_t رقم_nodes,
                                 vptr_t ipcbuf_vptr, کلمه_t extra_bi_اندازه)
{
    /* clear boot info memory */
    clear_حافظه((فراغ *)rootserver.boot_info, se_L4_Boot_Info_قاب_بیتها);
    إذا (extra_bi_اندازه) {
        clear_حافظه((فراغ *)rootserver.extra_bi,
                    calculate_extra_bi_اندازه_بیتها(extra_bi_اندازه));
    }

    /* initialise bootinfo-related global state */
    seL4_BootInfo *bi = BI_PTR(rootserver.boot_info);
    bi->گره_ID = گره_id;
    bi->رقم_Nodes = رقم_nodes;
    bi->رقم_IOPTLevels = 0;
    bi->ipc_میانگیر = (seL4_IPCBuffer *)ipcbuf_vptr;
    bi->init_Thread_CNode_اندازه_بیتها = CONFIG_ریشه_CNODE_اندازه_بیتها;
    bi->initThreadDomain = 0;
    bi->extraLen = extra_bi_اندازه;

    ndks_boot.bi_قاب = bi;
    ndks_boot.جایگاه_pos_cur = se_L4_رقم_Initial_Caps;
}

BOOT_کد bool_t provide_cap(cap_t ریشه_cnode_cap, cap_t cap)
{
    إذا (ndks_boot.جایگاه_pos_cur >= بیت(CONFIG_ریشه_CNODE_اندازه_بیتها)) {
        چاپ_کن("ERROR: can't add another cap, all %"SEL4_PRIu_کلمه
               " (=2^CONFIG_ROOT_CNODE_SIZE_BITS) slots used\n",
               بیت(CONFIG_ریشه_CNODE_اندازه_بیتها));
        أرجع false;
    }
    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), ndks_boot.جایگاه_pos_cur), cap);
    ndks_boot.جایگاه_pos_cur++;
    أرجع true;
}

BOOT_کد create_قابها_از_region_ret_t create_قابها_از_region(
    cap_t    ریشه_cnode_cap,
    cap_t    pd_cap,
    region_t reg,
    bool_t   do_map,
    sword_t  pv_جابه_جایی
)
{
    pptr_t     f;
    cap_t      قاب_cap;
    se_L4_جایگاه_Pos جایگاه_pos_before;
    se_L4_جایگاه_Pos جایگاه_pos_after;

    جایگاه_pos_before = ndks_boot.جایگاه_pos_cur;

    لكل (f = reg.شروع; f < reg.انتها; f += بیت(صفحه_بیتها)) {
        إذا (do_map) {
            قاب_cap = create_mapped_it_قاب_cap(pd_cap, f, pptr_مقصد_paddr((فراغ *)(f - pv_جابه_جایی)), IT_ASID, false, true);
        } وإلا {
            قاب_cap = create_unmapped_it_قاب_cap(f, false);
        }
        إذا (!provide_cap(ریشه_cnode_cap, قاب_cap)) {
            أرجع (create_قابها_از_region_ret_t) {
                .region  = S_REG_خالی,
                .success = false
            };
        }
    }

    جایگاه_pos_after = ndks_boot.جایگاه_pos_cur;

    أرجع (create_قابها_از_region_ret_t) {
        .region = (se_L4_جایگاه_Region) {
            .شروع = جایگاه_pos_before,
            .انتها   = جایگاه_pos_after
        },
        .success = true
    };
}

BOOT_کد cap_t create_it_asid_pool(cap_t ریشه_cnode_cap)
{
    cap_t ap_cap = cap_asid_pool_cap_new(ASID_بالا(IT_ASID) << asid_کم_بیتها, rootserver.asid_pool);
    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapInitThreadASIDPool), ap_cap);

    /* create ASID control cap */
    write_جایگاه(
        جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapASIDControl),
        cap_asid_control_cap_new()
    );

    أرجع ap_cap;
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
BOOT_کد ساكن فراغ configure_sched_context(tcb_t *tcb, sched_context_t *sc_pptr, ticks_t timeslice)
{
    tcb->tcbSchedContext = sc_pptr;
    refill_new(tcb->tcbSchedContext, MIN_REFILLS, timeslice, 0);
    tcb->tcbSchedContext->scTcb = tcb;
}

BOOT_کد bool_t init_sched_control(cap_t ریشه_cnode_cap, کلمه_t رقم_nodes)
{
    se_L4_جایگاه_Pos جایگاه_pos_before = ndks_boot.جایگاه_pos_cur;

    /* create a sched control cap for each core */
    لكل (بیعلامت صحيح i = 0; i < رقم_nodes; i++) {
        إذا (!provide_cap(ریشه_cnode_cap, cap_sched_control_cap_new(i))) {
            چاپ_کن("can't init sched_control for node %u, provide_cap() failed\n", i);
            أرجع false;
        }
    }

    /* update boot info with slot region for sched control caps */
    ndks_boot.bi_قاب->schedcontrol = (se_L4_جایگاه_Region) {
        .شروع = جایگاه_pos_before,
        .انتها = ndks_boot.جایگاه_pos_cur
    };

    أرجع true;
}
#پایان_اگر

BOOT_کد فراغ create_idle_thread(فراغ)
{
    pptr_t pptr;

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    لكل (بیعلامت صحيح i = 0; i < CONFIG_MAX_رقم_NODES; i++) {
#پایان_اگر /* ENABLE_SMP_SUPPORT */
        pptr = (pptr_t) &ksIdleThreadTCB[SMP_TERNARY(i, 0)];
        گره_STATE_ON_CORE(ksIdleThread, i) = TCB_PTR(pptr + TCB_جابه_جایی);
        configureIdleThread(گره_STATE_ON_CORE(ksIdleThread, i));
#اگر_تعریف_شده CONFIG_DEBUG_BUILD
        مجموعه_Thread_Name(گره_STATE_ON_CORE(ksIdleThread, i), "idle_thread");
#پایان_اگر
        SMP_COND_STATEMENT(گره_STATE_ON_CORE(ksIdleThread, i)->tcbAffinity = i);
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        configure_sched_context(گره_STATE_ON_CORE(ksIdleThread, i), SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]),
                                MAX_PERIOD_TICKS);
        SMP_COND_STATEMENT(گره_STATE_ON_CORE(ksIdleThread, i)->tcbSchedContext->scCore = i;)
        گره_STATE_ON_CORE(ksIdleSC, i) = SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]);
#پایان_اگر
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    }
#پایان_اگر /* ENABLE_SMP_SUPPORT */
}

BOOT_کد tcb_t *create_initial_thread(cap_t ریشه_cnode_cap, cap_t it_pd_cap, vptr_t ui_v_مدخل, vptr_t bi_قاب_vptr,
                                       vptr_t ipcbuf_vptr, cap_t ipcbuf_cap)
{
    tcb_t *tcb = TCB_PTR(rootserver.tcb + TCB_جابه_جایی);
#اگر_تعریف_نشده CONFIG_KERNEL_MCS
    tcb->tcb_زمان_برش = CONFIG_زمان_برش;
#پایان_اگر

    Arch_initContext(&tcb->tcbArch.tcbContext);

    /* derive a copy of the IPC buffer cap for inserting */
    deriveCap_ret_t dc_ret = deriveCap(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapInitThreadIPCBuffer), ipcbuf_cap);
    إذا (dc_ret.وضعیت != EXCEPTION_NONE) {
        چاپ_کن("Failed to derive copy of IPC Buffer\n");
        أرجع عدم;
    }

    /* initialise TCB (corresponds directly to abstract specification) */
    cte_درج(
        ریشه_cnode_cap,
        جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapInitThreadCNode),
        جایگاه_PTR(rootserver.tcb, tcbCTable)
    );
    cte_درج(
        it_pd_cap,
        جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapInitThreadVSpace),
        جایگاه_PTR(rootserver.tcb, tcbVTable)
    );
    cte_درج(
        dc_ret.cap,
        جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapInitThreadIPCBuffer),
        جایگاه_PTR(rootserver.tcb, tcb_میانگیر)
    );
    tcb->tcbIPCBuffer = ipcbuf_vptr;

    مجموعه_ثبات(tcb, cap_ثبات, bi_قاب_vptr);
    مجموعه_بعدی_PC(tcb, ui_v_مدخل);

    /* initialise TCB */
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    configure_sched_context(tcb, SC_PTR(rootserver.sc), us_مقصد_Ticks(CONFIG_BOOT_THREAD_زمان_برش * US_درون_MS));
#پایان_اگر

    tcb->tcb_اولویت = seL4_MaxPrio;
    tcb->tcbMCP = seL4_MaxPrio;
    tcb->tcbDomain = 0;
#اگر_تعریف_نشده CONFIG_KERNEL_MCS
    راهاندازی_پاسخ_Master(tcb);
#پایان_اگر
    مجموعه_Thread_State(tcb, ThreadState_Running);

#اگر_تعریف_نشده CONFIG_KERNEL_MCS
    SMP_COND_STATEMENT(tcb->tcbAffinity = 0);
#پایان_اگر

    /* create initial thread's TCB cap */
    cap_t cap = cap_thread_cap_new(TCB_REF(tcb));
    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapInitThreadTCB), cap);

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    cap = cap_sched_context_cap_new(SC_REF(tcb->tcbSchedContext), se_L4_Min_Sched_Context_بیتها);
    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapInitThreadSC), cap);
#پایان_اگر
#اگر_تعریف_شده CONFIG_DEBUG_BUILD
    مجموعه_Thread_Name(tcb, "rootserver");
#پایان_اگر

    أرجع tcb;
}

#اگر_تعریف_شده ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
BOOT_کد ساكن bool_t hypervisor_present(فراغ)
{
#اگر_تعریف_شده CONFIG_ARCH_X86
    uint32_t ebx = x86_cpuid_ebx(KVM_CPUID_SIGNATURE, 0);
    uint32_t ecx = x86_cpuid_ecx(KVM_CPUID_SIGNATURE, 0);
    uint32_t edx = x86_cpuid_edx(KVM_CPUID_SIGNATURE, 0);

    إذا ((ebx == CPUID_KVM_EBX && ecx == CPUID_KVM_ECX && edx == CPUID_KVM_EDX)
        || (ebx == CPUID_TCG_EBX && ecx == CPUID_TCG_ECX && edx == CPUID_TCG_EDX)) {
        أرجع true;
    }
#پایان_اگر
    أرجع false;
}

BOOT_کد فراغ clock_sync_test(فراغ)
{
    ticks_t t, t0;
    ticks_t margin = us_مقصد_Ticks(CLOCK_SYNC_DELTA) + getTimerPrecision();

    أكد(getCurrentCPUIndex() != 0);
    t = گره_STATE_ON_CORE(ks_Cur_زمان, 0);
    افعل {
        /* perform a memory acquire to get new values of ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        t0 = گره_STATE_ON_CORE(ks_Cur_زمان, 0);
    } طالما (t0 == t);
    t = get_Current_زمان();
    چاپ_کن("clock_sync_test[%d]: t0 = %"PRIu64", t = %"PRIu64", td = %"PRIi64"\n",
           (صحيح)getCurrentCPUIndex(), t0, t, t - t0);
    /*
     * The test does not consistently work if we are in a virtual machine (e.g
     * within QEMU) because the measurement cannot distinguish between
     * interrupted clock reads and out-of-sync clocks.
     */
    إذا (hypervisor_present()) {
        چاپ_کن("clock_sync_test[%d]: disabled, detected running as VM\n", (صحيح)getCurrentCPUIndex());
    } وإلا {
        أكد(t0 <= margin + t && t <= t0 + margin);
    }
}
#پایان_اگر

BOOT_کد فراغ init_core_state(tcb_t *scheduler_action)
{
#اگر_تعریف_شده CONFIG_HAVE_FPU
    گره_STATE(ksCurFPUOwner) = عدم;
#پایان_اگر
#اگر_تعریف_شده CONFIG_DEBUG_BUILD
    /* add initial threads to the debug queue */
    گره_STATE(ksDebugTCBs) = عدم;
    إذا (scheduler_action != SchedulerAction_ResumeCurrentThread &&
        scheduler_action != Scheduler_Action_انتخاب_New_Thread) {
        tcbDebugAppend(scheduler_action);
    }
    tcbDebugAppend(گره_STATE(ksIdleThread));
#پایان_اگر
    گره_STATE(ksSchedulerAction) = scheduler_action;
    گره_STATE(ksCurThread) = گره_STATE(ksIdleThread);
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    گره_STATE(ksCurSC) = گره_STATE(ksCurThread->tcbSchedContext);
    گره_STATE(ksConsumed) = 0;
    گره_STATE(ksReprogram) = true;
    گره_STATE(ks_Release_صف.سر) = عدم;
    گره_STATE(ks_Release_صف.انتها) = عدم;
    گره_STATE(ks_Cur_زمان) = get_Current_زمان();
#پایان_اگر
    /* No need for NODE_STATE() as there is no SMP support for domains */
    ksCurDomain = 0;
    ks_Domain_زمان = DSCHED_MAX_DURATION;
    ks_Dom_زمانبندی[0] = dschedule_make(0, DSCHED_MAX_DURATION);
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
BOOT_کد ساكن bool_t pptr_درون_kernel_پنجره(pptr_t pptr)
{
    أرجع pptr >= PPTR_پایه && pptr < PPTR_بالاترین;
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
BOOT_کد ساكن bool_t provide_untyped_cap(
    cap_t      ریشه_cnode_cap,
    bool_t     device_حافظه_2,
    pptr_t     pptr,
    کلمه_t     اندازه_بیتها,
    se_L4_جایگاه_Pos اولی_untyped_جایگاه
)
{
    bool_t ret;
    cap_t ut_cap;

    /* Since we are in boot code, we can do extensive error checking and
       return failure if anything unexpected happens. */

    /* Bounds check for size parameter */
    إذا (اندازه_بیتها > se_L4_Max_Untyped_بیتها || اندازه_بیتها < se_L4_Min_Untyped_بیتها) {
        چاپ_کن("Kernel init: Invalid untyped size %"SEL4_PRIu_کلمه"\n", اندازه_بیتها);
        أرجع false;
    }

    /* All cap ptrs must be aligned to object size */
    إذا (!IS_ALIGNED(pptr, اندازه_بیتها)) {
        چاپ_کن("Kernel init: Unaligned untyped pptr %p (alignment %"SEL4_PRIu_کلمه")\n", (فراغ *)pptr, اندازه_بیتها);
        أرجع false;
    }

    /* All cap ptrs apart from device untypeds must be in the kernel window. */
    إذا (!device_حافظه_2 && !pptr_درون_kernel_پنجره(pptr)) {
        چاپ_کن("Kernel init: Non-device untyped pptr %p outside kernel window\n",
               (فراغ *)pptr);
        أرجع false;
    }

    /* Check that the end of the region is also in the kernel window, so we don't
       need to assume that the kernel window is aligned up to potentially
       seL4_MaxUntypedBits. */
    إذا (!device_حافظه_2 && !pptr_درون_kernel_پنجره(pptr + ماسک(اندازه_بیتها))) {
        چاپ_کن("Kernel init: End of non-device untyped at %p outside kernel window (size %"SEL4_PRIu_کلمه")\n",
               (فراغ *)pptr, اندازه_بیتها);
        أرجع false;
    }

    کلمه_t i = ndks_boot.جایگاه_pos_cur - اولی_untyped_جایگاه;
    إذا (i < CONFIG_MAX_رقم_BOOTINFO_UNTYPED_CAPS) {
        ndks_boot.bi_قاب->untypedList[i] = (seL4_UntypedDesc) {
            .paddr    = pptr_مقصد_paddr((فراغ *)pptr),
            .اندازه_بیتها_2 = اندازه_بیتها,
            .isDevice = device_حافظه_2,
            .padding  = {0}
        };
        ut_cap = cap_untyped_cap_new(MAX_FREE_شاخص(اندازه_بیتها),
                                     device_حافظه_2, اندازه_بیتها, pptr);
        ret = provide_cap(ریشه_cnode_cap, ut_cap);
    } وإلا {
        چاپ_کن("Kernel init: Too many untyped regions for boot info\n");
        ret = true;
    }
    أرجع ret;
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
BOOT_کد ساكن bool_t create_untypeds_for_region(
    cap_t      ریشه_cnode_cap,
    bool_t     device_حافظه_2,
    region_t   reg,
    se_L4_جایگاه_Pos اولی_untyped_جایگاه
)
{
    /* This code works with regions that wrap (where end < start), because the loop cuts up the
       region into size-aligned chunks, one for each cap. Memory chunks that are size-aligned cannot
       themselves overflow, so they satisfy alignment, size, and overflow conditions. The region
       [0..end) is not necessarily part of the kernel window (depending on the value of PPTR_BASE).
       This is fine for device untypeds. For normal untypeds, the region is assumed to be fully in
       the kernel window. This is not checked here. */
    طالما (!is_reg_خالی(reg)) {

        /* Calculate the bit size of the region. This is also correct for end < start: it will
           return the correct size of the set [start..-1] union [0..end). This will then be too
           large for alignment, so the code further down will reduce the size. */
        بیعلامت صحيح اندازه_بیتها = se_L4_کلمه_بیتها - 1 - clzl(reg.انتها - reg.شروع);
        /* The size can't exceed the largest possible untyped size. */
        إذا (اندازه_بیتها > se_L4_Max_Untyped_بیتها) {
            اندازه_بیتها = se_L4_Max_Untyped_بیتها;
        }
        /* The start address 0 satisfies any alignment needs, otherwise ensure
         * the region's bit size does not exceed the alignment of the region.
         */
        إذا (0 != reg.شروع) {
            بیعلامت صحيح align_بیتها = ctzl(reg.شروع);
            إذا (اندازه_بیتها > align_بیتها) {
                اندازه_بیتها = align_بیتها;
            }
        }
        /* Provide an untyped capability for the region only if it is large
         * enough to be retyped into objects later. Otherwise the region can't
         * be used anyway.
         */
        إذا (اندازه_بیتها >= se_L4_Min_Untyped_بیتها) {
            إذا (!provide_untyped_cap(ریشه_cnode_cap, device_حافظه_2, reg.شروع, اندازه_بیتها, اولی_untyped_جایگاه)) {
                أرجع false;
            }
        }
        reg.شروع += بیت(اندازه_بیتها);
    }
    أرجع true;
}

BOOT_کد bool_t create_untypeds(cap_t ریشه_cnode_cap)
{
    se_L4_جایگاه_Pos اولی_untyped_جایگاه = ndks_boot.جایگاه_pos_cur;

    paddr_t شروع = 0;
    لكل (کلمه_t i = 0; i < ndks_boot.resv_شمار; i++) {
        إذا (شروع < ndks_boot.reserved[i].شروع) {
            region_t reg = paddr_مقصد_pptr_reg((p_region_t) {
                شروع, ndks_boot.reserved[i].شروع
            });
            إذا (!create_untypeds_for_region(ریشه_cnode_cap, true, reg, اولی_untyped_جایگاه)) {
                چاپ_کن("ERROR: creation of untypeds for device region #%u at"
                       " [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه") failed\n",
                       (بیعلامت صحيح)i, reg.شروع, reg.انتها);
                أرجع false;
            }
        }

        شروع = ndks_boot.reserved[i].انتها;
    }

    إذا (شروع <= CONFIG_PADDR_USER_DEVICE_بالاترین - 1) {
        region_t reg = paddr_مقصد_pptr_reg((p_region_t) {
            /* CONFIG_PADDR_USER_DEVICE_TOP cast to paddr_t can be 0.
             * create_untypeds_for_region() can deal with that correctly. */
            شروع, (paddr_t) CONFIG_PADDR_USER_DEVICE_بالاترین
        });

        إذا (!create_untypeds_for_region(ریشه_cnode_cap, true, reg, اولی_untyped_جایگاه)) {
            چاپ_کن("ERROR: creation of untypeds for top device region"
                   " [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه") failed\n",
                   reg.شروع, reg.انتها);
            أرجع false;
        }
    }

    /* There is a part of the kernel (code/data) that is only needed for the
     * boot process. We can create UT objects for these frames, so the memory
     * can be reused.
     */
    region_t boot_mem_reuse_reg = paddr_مقصد_pptr_reg(get_p_reg_kernel_img_boot());
    إذا (!create_untypeds_for_region(ریشه_cnode_cap, false, boot_mem_reuse_reg, اولی_untyped_جایگاه)) {
        چاپ_کن("ERROR: creation of untypeds for recycled boot memory"
               " [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه"] failed\n",
               boot_mem_reuse_reg.شروع, boot_mem_reuse_reg.انتها);
        أرجع false;
    }

    /* convert remaining freemem into UT objects and provide the caps */
    لكل (کلمه_t i = 0; i < ARRAY_اندازه(ndks_boot.freemem); i++) {
        region_t reg = ndks_boot.freemem[i];
        ndks_boot.freemem[i] = REG_خالی;
        إذا (!create_untypeds_for_region(ریشه_cnode_cap, false, reg, اولی_untyped_جایگاه)) {
            چاپ_کن("ERROR: creation of untypeds for free memory region #%u at"
                   " [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه") failed\n",
                   (بیعلامت صحيح)i, reg.شروع, reg.انتها);
            أرجع false;
        }
    }

    ndks_boot.bi_قاب->untyped = (se_L4_جایگاه_Region) {
        .شروع = اولی_untyped_جایگاه,
        .انتها   = ndks_boot.جایگاه_pos_cur
    };

    أرجع true;
}

BOOT_کد فراغ bi_finalise(فراغ)
{

    إذا (rootserver.paging.شروع != rootserver.paging.انتها) {
        چاپ_کن("WARNING: internal book keeping error. Less pagetables allocated than predicted: "
               "%ld page tables allocated but not used.\n", (rootserver.paging.انتها - rootserver.paging.شروع) >> se_L4_صفحه_جدول_بیتها);
    }

    ndks_boot.bi_قاب->خالی = (se_L4_جایگاه_Region) {
        .شروع = ndks_boot.جایگاه_pos_cur,
        .انتها   = بیت(CONFIG_ریشه_CNODE_اندازه_بیتها)
    };
}

BOOT_کد ساكن bool_t check_موجود_حافظه(کلمه_t n_موجود,
                                               ثابت p_region_t *موجود)
{
    /* The system configuration is broken if no region is available. */
    إذا (0 == n_موجود) {
        چاپ_کن("ERROR: no memory regions available\n");
        أرجع false;
    }

    چاپ_کن("Available phys memory regions: %"SEL4_PRIu_کلمه"\n", n_موجود);
    /* Force ordering and exclusivity of available regions. */
    لكل (کلمه_t i = 0; i < n_موجود; i++) {
        ثابت p_region_t *r = &موجود[i];
        چاپ_کن("  [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه")\n", r->شروع, r->انتها);

        /* Available regions must not wrap */
        إذا (r->شروع > r->انتها) {
            چاپ_کن("ERROR: memory region %"SEL4_PRIu_کلمه" has start > end\n", i);
            أرجع false;
        }

        /* Available regions must be non-empty. */
        إذا (r->شروع == r->انتها) {
            چاپ_کن("ERROR: memory region %"SEL4_PRIu_کلمه" empty\n", i);
            أرجع false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
         * so the '==' case is fine. Directly adjacent regions are allowed.
         */
        إذا ((i > 0) && (r->شروع < موجود[i - 1].انتها)) {
            چاپ_کن("ERROR: memory region %"SEL4_PRIu_کلمه" in wrong order\n", i);
            أرجع false;
        }
    }

    أرجع true;
}

/* We can't declare arrays on the stack, so this is space for init_avail_reg()
 * below to populate and init_freemem() to consume. */
BOOT_BSS ساكن region_t avail_reg[MAX_رقم_FREEMEM_REG];

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
BOOT_کد ساكن کلمه_t init_avail_reg(کلمه_t n_موجود,
                                       ثابت p_region_t *موجود)
{
    کلمه_t cnt = 0;
    لكل (کلمه_t i = 0; i < n_موجود; i++) {
        ثابت p_region_t *r = &موجود[i];

        إذا ((r->شروع >= PADDR_بالاترین) || (r->انتها <= PADDR_پایه)) {
            چاپ_کن("  region [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه") is outside the kernel"
                   " window, only potentially available as device untypeds\n", r->شروع, r->انتها);
        } وإلا {
            p_region_t usable_reg = {
                .شروع = MAX(r->شروع, PADDR_پایه),
                .انتها   = MIN(r->انتها, PADDR_بالاترین)
            };

            إذا (cnt >= ARRAY_اندازه(avail_reg)) {
                /* No space left in the array, seems MAX_NUM_FREEMEM_REG should
                 * be increased. Debug builds raise an assert here because this
                 * is likely a porting issue that should be looked into. Release
                 * builds will continue booting, but can't use this memory
                 * region. This might still be sufficient to run.
                 */
                چاپ_کن("  WARNING: cannot use region [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه"),"
                       " avail_reg[] is full\n", r->شروع, r->انتها);
                أكد(0);
            } وإلا {
                avail_reg[cnt] = paddr_مقصد_pptr_reg(usable_reg);
                cnt++;
            }
        }
    }
    أرجع cnt;
}


BOOT_کد ساكن bool_t check_reserved_حافظه(کلمه_t n_reserved,
                                              ثابت region_t *reserved)
{
    چاپ_کن("Reserved virt address space regions: %"SEL4_PRIu_کلمه"\n",
           n_reserved);
    /* Force ordering and exclusivity of reserved regions. */
    لكل (کلمه_t i = 0; i < n_reserved; i++) {
        ثابت region_t *r = &reserved[i];
        چاپ_کن("  [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه")\n", r->شروع, r->انتها);

        /* Reserved regions must be sane, the size is allowed to be zero. */
        إذا (r->شروع > r->انتها) {
            چاپ_کن("ERROR: reserved region %"SEL4_PRIu_کلمه" has start > end\n", i);
            أرجع false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
           so the == case is fine. Directly adjacent regions are allowed. */
        إذا ((i > 0) && (r->شروع < reserved[i - 1].انتها)) {
            چاپ_کن("ERROR: reserved region %"SEL4_PRIu_کلمه" in wrong order\n", i);
            أرجع false;
        }
    }

    أرجع true;
}

/**
 * Dynamically initialise the available memory on the platform.
 * A region represents an area of memory.
 */
BOOT_کد bool_t init_freemem(کلمه_t n_موجود, ثابت p_region_t *موجود,
                              کلمه_t n_reserved, ثابت region_t *reserved,
                              v_region_t it_v_reg, کلمه_t extra_bi_اندازه_بیتها)
{
    إذا (!check_موجود_حافظه(n_موجود, موجود)) {
        أرجع false;
    }

    /* After here, avail_reg[0..n_available) are all regions within the kernel window. */
    n_موجود = init_avail_reg(n_موجود, موجود);
    إذا (0 == n_موجود) {
        چاپ_کن("ERROR: no available memory within the kernel window\n");
        أرجع false;
    }

    إذا (!check_reserved_حافظه(n_reserved, reserved)) {
        أرجع false;
    }

    لكل (کلمه_t i = 0; i < ARRAY_اندازه(ndks_boot.freemem); i++) {
        ndks_boot.freemem[i] = REG_خالی;
    }

    کلمه_t a = 0;
    کلمه_t r = 0;
    /* Now iterate through the available regions, removing any reserved regions. */
    طالما (a < n_موجود && r < n_reserved) {
        إذا (reserved[r].شروع == reserved[r].انتها) {
            /* reserved region is empty - skip it */
            r++;
        } وإلا إذا (avail_reg[a].شروع >= avail_reg[a].انتها) {
            /* skip the entire region - it's empty now after trimming */
            a++;
        } وإلا إذا (reserved[r].انتها <= avail_reg[a].شروع) {
            /* the reserved region is below the available region - skip it */
            reserve_region(pptr_مقصد_paddr_reg(reserved[r]));
            r++;
        } وإلا إذا (reserved[r].شروع >= avail_reg[a].انتها) {
            /* the reserved region is above the available region - take the whole thing */
            درج_region(avail_reg[a]);
            a++;
        } وإلا {
            /* the reserved region overlaps with the available region */
            إذا (reserved[r].شروع <= avail_reg[a].شروع) {
                /* the region overlaps with the start of the available region.
                 * trim start of the available region */
                avail_reg[a].شروع = MIN(avail_reg[a].انتها, reserved[r].انتها);
                /* do not increment reserved index here - there could be more overlapping regions */
            } وإلا {
                أكد(reserved[r].شروع < avail_reg[a].انتها);
                /* take the first chunk of the available region and move
                 * the start to the end of the reserved region */
                region_t m = avail_reg[a];
                m.انتها = reserved[r].شروع;
                درج_region(m);
                إذا (avail_reg[a].انتها > reserved[r].انتها) {
                    avail_reg[a].شروع = reserved[r].انتها;
                    /* we could increment reserved index here, but it's more consistent with the
                     * other overlapping case if we don't */
                } وإلا {
                    a++;
                }
            }
        }
    }

    لكل (; r < n_reserved; r++) {
        إذا (reserved[r].شروع < reserved[r].انتها) {
            reserve_region(pptr_مقصد_paddr_reg(reserved[r]));
        }
    }

    /* no more reserved regions - add the rest */
    لكل (; a < n_موجود; a++) {
        إذا (avail_reg[a].شروع < avail_reg[a].انتها) {
            درج_region(avail_reg[a]);
        }
    }

    /* now try to fit the root server objects into a region */
    صحيح i = ARRAY_اندازه(ndks_boot.freemem) - 1;
    إذا (!is_reg_خالی(ndks_boot.freemem[i])) {
        چاپ_کن("ERROR: insufficient MAX_NUM_FREEMEM_REG (%u)\n",
               (بیعلامت صحيح)MAX_رقم_FREEMEM_REG);
        أرجع false;
    }
    /* skip any empty regions */
    لكل (; i >= 0 && is_reg_خالی(ndks_boot.freemem[i]); i--);

    /* try to grab the last available p region to create the root server objects
     * from. If possible, retain any left over memory as an extra p region */
    کلمه_t اندازه = calculate_rootserver_اندازه(it_v_reg, extra_bi_اندازه_بیتها);
    کلمه_t max = rootserver_max_اندازه_بیتها(extra_bi_اندازه_بیتها);
    لكل (; i >= 0; i--) {
        /* Invariant: both i and (i + 1) are valid indices in ndks_boot.freemem. */
        أكد(i < ARRAY_اندازه(ndks_boot.freemem) - 1);
        /* Invariant; the region at index i is the current candidate.
         * Invariant: regions 0 up to (i - 1), if any, are additional candidates.
         * Invariant: region (i + 1) is empty. */
        أكد(is_reg_خالی(ndks_boot.freemem[i + 1]));
        /* Invariant: regions above (i + 1), if any, are empty or too small to use.
         * Invariant: all non-empty regions are ordered, disjoint and unallocated. */

        /* We make a fresh variable to index the known-empty region, because the
         * SimplExportAndRefine verification test has poor support for array
         * indices that are sums of variables and small constants. */
        صحيح خالی_شاخص = i + 1;

        /* Try to take the top-most suitably sized and aligned chunk. */
        pptr_t unaligned_شروع = ndks_boot.freemem[i].انتها - اندازه;
        pptr_t شروع = دور_پایین(unaligned_شروع, max);
        /* if unaligned_start didn't underflow, and start fits in the region,
         * then we've found a region that fits the root server objects. */
        إذا (unaligned_شروع <= ndks_boot.freemem[i].انتها
            && شروع >= ndks_boot.freemem[i].شروع) {
            create_rootserver_objects(شروع, it_v_reg, extra_bi_اندازه_بیتها);
            /* There may be leftovers before and after the memory we used. */
            /* Shuffle the after leftover up to the empty slot (i + 1). */
            ndks_boot.freemem[خالی_شاخص] = (region_t) {
                .شروع = شروع + اندازه,
                .انتها = ndks_boot.freemem[i].انتها
            };
            /* Leave the before leftover in current slot i. */
            ndks_boot.freemem[i].انتها = شروع;
            /* Regions i and (i + 1) are now well defined, ordered, disjoint,
             * and unallocated, so we can return successfully. */
            أرجع true;
        }
        /* Region i isn't big enough, so shuffle it up to slot (i + 1),
         * which we know is unused. */
        ndks_boot.freemem[خالی_شاخص] = ndks_boot.freemem[i];
        /* Now region i is unused, so make it empty to reestablish the invariant
         * for the next iteration (when it will be slot i + 1). */
        ndks_boot.freemem[i] = REG_خالی;
    }

    /* We didn't find a big enough region. */
    چاپ_کن("ERROR: no free memory region is big enough for root server "
           "objects, need size/alignment of 2^%"SEL4_PRIu_کلمه"\n", max);
    أرجع false;
}
