/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <assert.h>
#شامل <kernel/boot.h>
#شامل <kernel/thread.h>
#شامل <machine/io.h>
#شامل <machine/registerset.h>
#شامل <model/statedata.h>
#شامل <object/domain.h>
#شامل <arch/machine.h>
#شامل <arch/kernel/boot.h>
#شامل <arch/kernel/vspace.h>
#شامل <linker.h>
#شامل <hardware.h>
#شامل <util.h>

/* (node-local) state accessed only during bootstrapping */
BOOT_BSS ndks_boot_t ndks_boot;

BOOT_BSS rootserver_mem_t rootserver;
BOOT_BSS ساکن region_t rootserver_mem;

/* Returns the physical region of the kernel image boot part, which is the part
 * that is no longer needed once booting is finished. */
خارجی حرف ki_boot_انتہا[1];
BOOT_رمز p_region_t get_p_reg_kernel_img_boot(خالی)
{
    واپس (p_region_t) {
        .آغاز = kpptr_مقصد_paddr((ثابت خالی *)KERNEL_ELF_بنیاد),
        .انتہا   = kpptr_مقصد_paddr(ki_boot_انتہا)
    };
}

/* Returns the physical region of the kernel image. */
BOOT_رمز p_region_t get_p_reg_kernel_img(خالی)
{
    واپس (p_region_t) {
        .آغاز = kpptr_مقصد_paddr((ثابت خالی *)KERNEL_ELF_بنیاد),
        .انتہا   = kpptr_مقصد_paddr((ثابت خالی *)KERNEL_ELF_چوٹی)
    };
}

BOOT_رمز ساکن خالی ملاؤ_regions(خالی)
{
    /* Walk through reserved regions and see if any can be merged */
    لفظ_t چوٹی = 0;
    برائے (لفظ_t i = 1; i < ndks_boot.resv_گنتی; i++) {
        اگر (ndks_boot.reserved[چوٹی].انتہا == ndks_boot.reserved[i].آغاز) {
            /* extend earlier region */
            ndks_boot.reserved[چوٹی].انتہا = ndks_boot.reserved[i].انتہا;
        } ورنہ {
            /* add region */
            چوٹی++;
            ndks_boot.reserved[چوٹی] = ndks_boot.reserved[i];
        }
    }
    ndks_boot.resv_گنتی = ndks_boot.resv_گنتی > 0 ? چوٹی + 1 : 0;
}

BOOT_رمز bool_t reserve_region(p_region_t reg)
{
    لفظ_t i;
    أكد(reg.آغاز <= reg.انتہا);
    اگر (reg.آغاز == reg.انتہا) {
        واپس true;
    }

    /* keep the regions in order */
    برائے (i = 0; i < ndks_boot.resv_گنتی; i++) {
        /* Try and merge the region to an existing one, if possible */
        اگر (ndks_boot.reserved[i].آغاز == reg.انتہا) {
            ndks_boot.reserved[i].آغاز = reg.آغاز;
            ملاؤ_regions();
            واپس true;
        }
        اگر (ndks_boot.reserved[i].انتہا == reg.آغاز) {
            ndks_boot.reserved[i].انتہا = reg.انتہا;
            ملاؤ_regions();
            واپس true;
        }
        /* Otherwise figure out where it should go. */
        اگر (ndks_boot.reserved[i].آغاز > reg.انتہا) {
            /* move regions down, making sure there's enough room */
            اگر (ndks_boot.resv_گنتی + 1 >= MAX_عدد_RESV_REG) {
                لکھو("Can't mark region 0x%"SEL4_PRIx_لفظ"-0x%"SEL4_PRIx_لفظ
                       " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
                       reg.آغاز, reg.انتہا, (صحیح)MAX_عدد_RESV_REG);
                واپس false;
            }
            برائے (لفظ_t j = ndks_boot.resv_گنتی; j > i; j--) {
                ndks_boot.reserved[j] = ndks_boot.reserved[j - 1];
            }
            /* insert the new region */
            ndks_boot.reserved[i] = reg;
            ndks_boot.resv_گنتی++;
            واپس true;
        }
    }

    اگر (i + 1 == MAX_عدد_RESV_REG) {
        لکھو("Can't mark region 0x%"SEL4_PRIx_لفظ"-0x%"SEL4_PRIx_لفظ
               " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
               reg.آغاز, reg.انتہا, (صحیح)MAX_عدد_RESV_REG);
        واپس false;
    }

    ndks_boot.reserved[i] = reg;
    ndks_boot.resv_گنتی++;

    واپس true;
}

BOOT_رمز ساکن bool_t داخل_کرو_region(region_t reg)
{
    أكد(reg.آغاز <= reg.انتہا);
    اگر (is_reg_خالی_خانے(reg)) {
        واپس true;
    }

    برائے (لفظ_t i = 0; i < ARRAY_سائز(ndks_boot.freemem); i++) {
        اگر (is_reg_خالی_خانے(ndks_boot.freemem[i])) {
            reserve_region(pptr_مقصد_paddr_reg(reg));
            ndks_boot.freemem[i] = reg;
            واپس true;
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
    لکھو("no free memory slot left for [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ"),"
           " consider increasing MAX_NUM_FREEMEM_REG (%u)\n",
           reg.آغاز, reg.انتہا, (غیرعلامتی صحیح)MAX_عدد_FREEMEM_REG);

    /* For debug builds we consider this a fatal error. Rationale is, that the
     * caller does not check the error code at the moment, but just ignores any
     * failures silently. */
    أكد(0);

    واپس false;
}

BOOT_رمز ساکن pptr_t alloc_rootserver_obj(لفظ_t سائز_بٹ, لفظ_t n)
{
    pptr_t allocated = rootserver_mem.آغاز;
    /* allocated memory must be aligned */
    أكد(allocated % ثنائی_ہندسہ(سائز_بٹ) == 0);
    rootserver_mem.آغاز += (n * ثنائی_ہندسہ(سائز_بٹ));
    /* we must not have run out of memory */
    أكد(rootserver_mem.آغاز <= rootserver_mem.انتہا);
    memzero((خالی *) allocated, n * ثنائی_ہندسہ(سائز_بٹ));
    واپس allocated;
}

BOOT_رمز ساکن لفظ_t rootserver_max_سائز_بٹ(لفظ_t extra_bi_سائز_بٹ)
{
    لفظ_t cnode_سائز_بٹ = CONFIG_جڑ_CNODE_سائز_بٹ + se_L4_خانہ_بٹ;
    لفظ_t max = MAX(cnode_سائز_بٹ, se_L4_VSpace_بٹ);
    واپس MAX(max, extra_bi_سائز_بٹ);
}

BOOT_رمز ساکن لفظ_t calculate_rootserver_سائز(v_region_t it_v_reg, لفظ_t extra_bi_سائز_بٹ)
{
    /* work out how much memory we need for root server objects */
    لفظ_t سائز = ثنائی_ہندسہ(CONFIG_جڑ_CNODE_سائز_بٹ + se_L4_خانہ_بٹ);
    سائز += ثنائی_ہندسہ(seL4_TCBBits); // root thread tcb
    سائز += ثنائی_ہندسہ(se_L4_صفحہ_بٹ); // ipc buf
    سائز += ثنائی_ہندسہ(se_L4_Boot_Info_چوکھٹا_بٹ); // boot info
    سائز += ثنائی_ہندسہ(se_L4_ASIDPool_بٹ);
    سائز += extra_bi_سائز_بٹ > 0 ? ثنائی_ہندسہ(extra_bi_سائز_بٹ) : 0;
    سائز += ثنائی_ہندسہ(se_L4_VSpace_بٹ); // root vspace
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    سائز += ثنائی_ہندسہ(se_L4_Min_Sched_Context_بٹ); // root sched context
#اگر_ختم
    /* for all archs, seL4_PageTable Bits is the size of all non top-level paging structures */
    واپس سائز + arch_get_n_paging(it_v_reg) * ثنائی_ہندسہ(se_L4_صفحہ_جدول_بٹ);
}

BOOT_رمز ساکن خالی maybe_alloc_extra_bi(لفظ_t cmp_سائز_بٹ, لفظ_t extra_bi_سائز_بٹ)
{
    اگر (extra_bi_سائز_بٹ >= cmp_سائز_بٹ && rootserver.extra_bi == 0) {
        rootserver.extra_bi = alloc_rootserver_obj(extra_bi_سائز_بٹ, 1);
    }
}

/* Create pptrs for all root server objects, starting at a give start address,
 * to cover the virtual memory region v_reg, and any extra boot info.
 */
BOOT_رمز ساکن خالی create_rootserver_objects(pptr_t آغاز, v_region_t it_v_reg,
                                                لفظ_t extra_bi_سائز_بٹ)
{
    /* the largest object the PD, the root cnode, or the extra boot info */
    لفظ_t cnode_سائز_بٹ = CONFIG_جڑ_CNODE_سائز_بٹ + se_L4_خانہ_بٹ;
    لفظ_t max = rootserver_max_سائز_بٹ(extra_bi_سائز_بٹ);

    لفظ_t سائز = calculate_rootserver_سائز(it_v_reg, extra_bi_سائز_بٹ);
    rootserver_mem.آغاز = آغاز;
    rootserver_mem.انتہا = آغاز + سائز;

    maybe_alloc_extra_bi(max, extra_bi_سائز_بٹ);

    /* the root cnode is at least 4k, so it could be larger or smaller than a pd. */
#اگر (CONFIG_جڑ_CNODE_سائز_بٹ + se_L4_خانہ_بٹ) > se_L4_VSpace_بٹ
    rootserver.cnode = alloc_rootserver_obj(cnode_سائز_بٹ, 1);
    maybe_alloc_extra_bi(se_L4_VSpace_بٹ, extra_bi_سائز_بٹ);
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_بٹ, 1);
#ورنہ
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_بٹ, 1);
    maybe_alloc_extra_bi(cnode_سائز_بٹ, extra_bi_سائز_بٹ);
    rootserver.cnode = alloc_rootserver_obj(cnode_سائز_بٹ, 1);
#اگر_ختم

    /* at this point we are up to creating 4k objects - which is the min size of
     * extra_bi so this is the last chance to allocate it */
    maybe_alloc_extra_bi(se_L4_صفحہ_بٹ, extra_bi_سائز_بٹ);
    compile_assert(invalid_se_L4_ASIDPool_بٹ, se_L4_ASIDPool_بٹ == se_L4_صفحہ_بٹ);
    rootserver.asid_pool = alloc_rootserver_obj(se_L4_ASIDPool_بٹ, 1);
    rootserver.ipc_buf = alloc_rootserver_obj(se_L4_صفحہ_بٹ, 1);
    /* The boot info size must be at least one page. Due to the hard-coded order
     * of allocations used in the current implementation here, it can't be any
     * bigger.
     */
    compile_assert(invalid_se_L4_Boot_Info_چوکھٹا_بٹ, se_L4_Boot_Info_چوکھٹا_بٹ == se_L4_صفحہ_بٹ);
    rootserver.boot_info = alloc_rootserver_obj(se_L4_Boot_Info_چوکھٹا_بٹ, 1);

    /* TCBs on aarch32 can be larger than page tables in certain configs */
#اگر seL4_TCBBits >= se_L4_صفحہ_جدول_بٹ
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#اگر_ختم

    /* paging structures are 4k on every arch except aarch32 (1k) */
    لفظ_t n = arch_get_n_paging(it_v_reg);
    rootserver.paging.آغاز = alloc_rootserver_obj(se_L4_صفحہ_جدول_بٹ, n);
    rootserver.paging.انتہا = rootserver.paging.آغاز + n * ثنائی_ہندسہ(se_L4_صفحہ_جدول_بٹ);

    /* for most archs, TCBs are smaller than page tables */
#اگر seL4_TCBBits < se_L4_صفحہ_جدول_بٹ
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    rootserver.sc = alloc_rootserver_obj(se_L4_Min_Sched_Context_بٹ, 1);
#اگر_ختم
    /* we should have allocated all our memory */
    أكد(rootserver_mem.آغاز == rootserver_mem.انتہا);
}

BOOT_رمز خالی write_خانہ(خانہ_ptr_t خانہ_ptr, cap_t cap)
{
    خانہ_ptr->cap = cap;

    خانہ_ptr->cteMDBNode = nullMDBNode;
    mdb_گرہ_ptr_سیٹ_mdb_Revocable(&خانہ_ptr->cteMDBNode, true);
    mdb_گرہ_ptr_سیٹ_mdb_پہلا_Badged(&خانہ_ptr->cteMDBNode, true);
}

/* Our root CNode needs to be able to fit all the initial caps and not
 * cover all of memory.
 */
compile_assert(جڑ_cnode_سائز_valid,
               CONFIG_جڑ_CNODE_سائز_بٹ < 32 - se_L4_خانہ_بٹ &&
               ثنائی_ہندسہ(CONFIG_جڑ_CNODE_سائز_بٹ) >= se_L4_عدد_Initial_Caps &&
               CONFIG_جڑ_CNODE_سائز_بٹ >= (se_L4_صفحہ_بٹ - se_L4_خانہ_بٹ))

BOOT_رمز cap_t
create_جڑ_cnode(خالی)
{
    cap_t cap = cap_cnode_cap_new(
                    CONFIG_جڑ_CNODE_سائز_بٹ, /* radix */
                    لفظ_بٹ - CONFIG_جڑ_CNODE_سائز_بٹ, /* guard size */
                    0, /* guard */
                    rootserver.cnode); /* pptr */

    /* write the root CNode cap into the root CNode */
    write_خانہ(خانہ_PTR(rootserver.cnode, seL4_CapInitThreadCNode), cap);

    واپس cap;
}

/* Check domain scheduler assumptions. */
compile_assert(عدد_domains_valid,
               CONFIG_عدد_DOMAINS >= 1 && CONFIG_عدد_DOMAINS <= 256)
compile_assert(عدد_priorities_valid,
               CONFIG_عدد_PRIORITIES >= 1 && CONFIG_عدد_PRIORITIES <= 256)

BOOT_رمز خالی
create_domain_cap(cap_t جڑ_cnode_cap)
{
    cap_t cap = cap_domain_cap_new();
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapDomain), cap);
}

BOOT_رمز cap_t create_ipcbuf_چوکھٹا_cap(cap_t جڑ_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    clear_حافظہ((خالی *)rootserver.ipc_buf, صفحہ_بٹ);

    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_چوکھٹا_cap(pd_cap, rootserver.ipc_buf, vptr, IT_ASID, false, false);
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapInitThreadIPCBuffer), cap);

    واپس cap;
}

BOOT_رمز خالی create_bi_چوکھٹا_cap(cap_t جڑ_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_چوکھٹا_cap(pd_cap, rootserver.boot_info, vptr, IT_ASID, false, false);
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), se_L4_Cap_Boot_Info_چوکھٹا), cap);
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
BOOT_رمز لفظ_t calculate_extra_bi_سائز_بٹ(لفظ_t extra_سائز)
{
    اگر (extra_سائز == 0) {
        واپس 0;
    }

    لفظ_t clzl_ret = clzl(باری_اوپر(extra_سائز, se_L4_صفحہ_بٹ));
    لفظ_t msb = se_L4_لفظ_بٹ - 1 - clzl_ret;
    /* If region is bigger than a page, make sure we overallocate rather than
     * underallocate
     */
    اگر (extra_سائز > ثنائی_ہندسہ(msb)) {
        msb++;
    }
    واپس msb;
}

BOOT_رمز خالی populate_bi_چوکھٹا(گرہ_id_t گرہ_id, لفظ_t عدد_nodes,
                                 vptr_t ipcbuf_vptr, لفظ_t extra_bi_سائز)
{
    /* clear boot info memory */
    clear_حافظہ((خالی *)rootserver.boot_info, se_L4_Boot_Info_چوکھٹا_بٹ);
    اگر (extra_bi_سائز) {
        clear_حافظہ((خالی *)rootserver.extra_bi,
                    calculate_extra_bi_سائز_بٹ(extra_bi_سائز));
    }

    /* initialise bootinfo-related global state */
    seL4_BootInfo *bi = BI_PTR(rootserver.boot_info);
    bi->گرہ_ID = گرہ_id;
    bi->عدد_Nodes = عدد_nodes;
    bi->عدد_IOPTLevels = 0;
    bi->ipc_بفر = (seL4_IPCBuffer *)ipcbuf_vptr;
    bi->init_Thread_CNode_سائز_بٹ = CONFIG_جڑ_CNODE_سائز_بٹ;
    bi->initThreadDomain = 0;
    bi->extraLen = extra_bi_سائز;

    ndks_boot.bi_چوکھٹا = bi;
    ndks_boot.خانہ_pos_cur = se_L4_عدد_Initial_Caps;
}

BOOT_رمز bool_t provide_cap(cap_t جڑ_cnode_cap, cap_t cap)
{
    اگر (ndks_boot.خانہ_pos_cur >= ثنائی_ہندسہ(CONFIG_جڑ_CNODE_سائز_بٹ)) {
        لکھو("ERROR: can't add another cap, all %"SEL4_PRIu_لفظ
               " (=2^CONFIG_ROOT_CNODE_SIZE_BITS) slots used\n",
               ثنائی_ہندسہ(CONFIG_جڑ_CNODE_سائز_بٹ));
        واپس false;
    }
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), ndks_boot.خانہ_pos_cur), cap);
    ndks_boot.خانہ_pos_cur++;
    واپس true;
}

BOOT_رمز create_چوکھٹے_کا_region_ret_t create_چوکھٹے_کا_region(
    cap_t    جڑ_cnode_cap,
    cap_t    pd_cap,
    region_t reg,
    bool_t   do_map,
    sword_t  pv_انحراف
)
{
    pptr_t     f;
    cap_t      چوکھٹا_cap;
    se_L4_خانہ_Pos خانہ_pos_before;
    se_L4_خانہ_Pos خانہ_pos_after;

    خانہ_pos_before = ndks_boot.خانہ_pos_cur;

    برائے (f = reg.آغاز; f < reg.انتہا; f += ثنائی_ہندسہ(صفحہ_بٹ)) {
        اگر (do_map) {
            چوکھٹا_cap = create_mapped_it_چوکھٹا_cap(pd_cap, f, pptr_مقصد_paddr((خالی *)(f - pv_انحراف)), IT_ASID, false, true);
        } ورنہ {
            چوکھٹا_cap = create_unmapped_it_چوکھٹا_cap(f, false);
        }
        اگر (!provide_cap(جڑ_cnode_cap, چوکھٹا_cap)) {
            واپس (create_چوکھٹے_کا_region_ret_t) {
                .region  = S_REG_خالی_خانے,
                .success = false
            };
        }
    }

    خانہ_pos_after = ndks_boot.خانہ_pos_cur;

    واپس (create_چوکھٹے_کا_region_ret_t) {
        .region = (se_L4_خانہ_Region) {
            .آغاز = خانہ_pos_before,
            .انتہا   = خانہ_pos_after
        },
        .success = true
    };
}

BOOT_رمز cap_t create_it_asid_pool(cap_t جڑ_cnode_cap)
{
    cap_t ap_cap = cap_asid_pool_cap_new(ASID_بلند(IT_ASID) << asid_کم_بٹ, rootserver.asid_pool);
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapInitThreadASIDPool), ap_cap);

    /* create ASID control cap */
    write_خانہ(
        خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapASIDControl),
        cap_asid_control_cap_new()
    );

    واپس ap_cap;
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
BOOT_رمز ساکن خالی configure_sched_context(tcb_t *tcb, sched_context_t *sc_pptr, ticks_t timeslice)
{
    tcb->tcbSchedContext = sc_pptr;
    refill_new(tcb->tcbSchedContext, MIN_REFILLS, timeslice, 0);
    tcb->tcbSchedContext->scTcb = tcb;
}

BOOT_رمز bool_t init_sched_control(cap_t جڑ_cnode_cap, لفظ_t عدد_nodes)
{
    se_L4_خانہ_Pos خانہ_pos_before = ndks_boot.خانہ_pos_cur;

    /* create a sched control cap for each core */
    برائے (غیرعلامتی صحیح i = 0; i < عدد_nodes; i++) {
        اگر (!provide_cap(جڑ_cnode_cap, cap_sched_control_cap_new(i))) {
            لکھو("can't init sched_control for node %u, provide_cap() failed\n", i);
            واپس false;
        }
    }

    /* update boot info with slot region for sched control caps */
    ndks_boot.bi_چوکھٹا->schedcontrol = (se_L4_خانہ_Region) {
        .آغاز = خانہ_pos_before,
        .انتہا = ndks_boot.خانہ_pos_cur
    };

    واپس true;
}
#اگر_ختم

BOOT_رمز خالی create_idle_thread(خالی)
{
    pptr_t pptr;

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    برائے (غیرعلامتی صحیح i = 0; i < CONFIG_MAX_عدد_NODES; i++) {
#اگر_ختم /* ENABLE_SMP_SUPPORT */
        pptr = (pptr_t) &ksIdleThreadTCB[SMP_TERNARY(i, 0)];
        گرہ_STATE_ON_CORE(ksIdleThread, i) = TCB_PTR(pptr + TCB_انحراف);
        configureIdleThread(گرہ_STATE_ON_CORE(ksIdleThread, i));
#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
        سیٹ_Thread_Name(گرہ_STATE_ON_CORE(ksIdleThread, i), "idle_thread");
#اگر_ختم
        SMP_COND_STATEMENT(گرہ_STATE_ON_CORE(ksIdleThread, i)->tcbAffinity = i);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        configure_sched_context(گرہ_STATE_ON_CORE(ksIdleThread, i), SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]),
                                MAX_PERIOD_TICKS);
        SMP_COND_STATEMENT(گرہ_STATE_ON_CORE(ksIdleThread, i)->tcbSchedContext->scCore = i;)
        گرہ_STATE_ON_CORE(ksIdleSC, i) = SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]);
#اگر_ختم
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    }
#اگر_ختم /* ENABLE_SMP_SUPPORT */
}

BOOT_رمز tcb_t *create_initial_thread(cap_t جڑ_cnode_cap, cap_t it_pd_cap, vptr_t ui_v_اندراج, vptr_t bi_چوکھٹا_vptr,
                                       vptr_t ipcbuf_vptr, cap_t ipcbuf_cap)
{
    tcb_t *tcb = TCB_PTR(rootserver.tcb + TCB_انحراف);
#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
    tcb->tcb_وقت_قاش = CONFIG_وقت_قاش;
#اگر_ختم

    Arch_initContext(&tcb->tcbArch.tcbContext);

    /* derive a copy of the IPC buffer cap for inserting */
    deriveCap_ret_t dc_ret = deriveCap(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapInitThreadIPCBuffer), ipcbuf_cap);
    اگر (dc_ret.حالت_2 != EXCEPTION_NONE) {
        لکھو("Failed to derive copy of IPC Buffer\n");
        واپس تہی;
    }

    /* initialise TCB (corresponds directly to abstract specification) */
    cte_داخل_کرو(
        جڑ_cnode_cap,
        خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapInitThreadCNode),
        خانہ_PTR(rootserver.tcb, tcbCTable)
    );
    cte_داخل_کرو(
        it_pd_cap,
        خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapInitThreadVSpace),
        خانہ_PTR(rootserver.tcb, tcbVTable)
    );
    cte_داخل_کرو(
        dc_ret.cap,
        خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapInitThreadIPCBuffer),
        خانہ_PTR(rootserver.tcb, tcb_بفر)
    );
    tcb->tcbIPCBuffer = ipcbuf_vptr;

    سیٹ_رجسٹر(tcb, cap_رجسٹر, bi_چوکھٹا_vptr);
    سیٹ_اگلا_PC(tcb, ui_v_اندراج);

    /* initialise TCB */
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    configure_sched_context(tcb, SC_PTR(rootserver.sc), us_مقصد_Ticks(CONFIG_BOOT_THREAD_وقت_قاش * US_اندر_MS));
#اگر_ختم

    tcb->tcb_ترجیح = seL4_MaxPrio;
    tcb->tcbMCP = seL4_MaxPrio;
    tcb->tcbDomain = 0;
#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
    تیاری_جواب_Master(tcb);
#اگر_ختم
    سیٹ_Thread_State(tcb, ThreadState_Running);

#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
    SMP_COND_STATEMENT(tcb->tcbAffinity = 0);
#اگر_ختم

    /* create initial thread's TCB cap */
    cap_t cap = cap_thread_cap_new(TCB_REF(tcb));
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapInitThreadTCB), cap);

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    cap = cap_sched_context_cap_new(SC_REF(tcb->tcbSchedContext), se_L4_Min_Sched_Context_بٹ);
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapInitThreadSC), cap);
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
    سیٹ_Thread_Name(tcb, "rootserver");
#اگر_ختم

    واپس tcb;
}

#اگر_تعریف_شدہ ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
BOOT_رمز ساکن bool_t hypervisor_present(خالی)
{
#اگر_تعریف_شدہ CONFIG_ARCH_X86
    uint32_t ebx = x86_cpuid_ebx(KVM_CPUID_SIGNATURE, 0);
    uint32_t ecx = x86_cpuid_ecx(KVM_CPUID_SIGNATURE, 0);
    uint32_t edx = x86_cpuid_edx(KVM_CPUID_SIGNATURE, 0);

    اگر ((ebx == CPUID_KVM_EBX && ecx == CPUID_KVM_ECX && edx == CPUID_KVM_EDX)
        || (ebx == CPUID_TCG_EBX && ecx == CPUID_TCG_ECX && edx == CPUID_TCG_EDX)) {
        واپس true;
    }
#اگر_ختم
    واپس false;
}

BOOT_رمز خالی clock_sync_test(خالی)
{
    ticks_t t, t0;
    ticks_t margin = us_مقصد_Ticks(CLOCK_SYNC_DELTA) + getTimerPrecision();

    أكد(getCurrentCPUIndex() != 0);
    t = گرہ_STATE_ON_CORE(ks_Cur_وقت, 0);
    کرو {
        /* perform a memory acquire to get new values of ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        t0 = گرہ_STATE_ON_CORE(ks_Cur_وقت, 0);
    } جب (t0 == t);
    t = get_Current_وقت();
    لکھو("clock_sync_test[%d]: t0 = %"PRIu64", t = %"PRIu64", td = %"PRIi64"\n",
           (صحیح)getCurrentCPUIndex(), t0, t, t - t0);
    /*
     * The test does not consistently work if we are in a virtual machine (e.g
     * within QEMU) because the measurement cannot distinguish between
     * interrupted clock reads and out-of-sync clocks.
     */
    اگر (hypervisor_present()) {
        لکھو("clock_sync_test[%d]: disabled, detected running as VM\n", (صحیح)getCurrentCPUIndex());
    } ورنہ {
        أكد(t0 <= margin + t && t <= t0 + margin);
    }
}
#اگر_ختم

BOOT_رمز خالی init_core_state(tcb_t *scheduler_action)
{
#اگر_تعریف_شدہ CONFIG_HAVE_FPU
    گرہ_STATE(ksCurFPUOwner) = تہی;
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
    /* add initial threads to the debug queue */
    گرہ_STATE(ksDebugTCBs) = تہی;
    اگر (scheduler_action != SchedulerAction_ResumeCurrentThread &&
        scheduler_action != Scheduler_Action_انتخاب_New_Thread) {
        tcbDebugAppend(scheduler_action);
    }
    tcbDebugAppend(گرہ_STATE(ksIdleThread));
#اگر_ختم
    گرہ_STATE(ksSchedulerAction) = scheduler_action;
    گرہ_STATE(ksCurThread) = گرہ_STATE(ksIdleThread);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    گرہ_STATE(ksCurSC) = گرہ_STATE(ksCurThread->tcbSchedContext);
    گرہ_STATE(ksConsumed) = 0;
    گرہ_STATE(ksReprogram) = true;
    گرہ_STATE(ks_Release_قطار.سر) = تہی;
    گرہ_STATE(ks_Release_قطار.انتہا) = تہی;
    گرہ_STATE(ks_Cur_وقت) = get_Current_وقت();
#اگر_ختم
    /* No need for NODE_STATE() as there is no SMP support for domains */
    ksCurDomain = 0;
    ks_Domain_وقت = DSCHED_MAX_DURATION;
    ks_Dom_جدول_وقت[0] = dschedule_make(0, DSCHED_MAX_DURATION);
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
BOOT_رمز ساکن bool_t pptr_اندر_kernel_کھڑکی(pptr_t pptr)
{
    واپس pptr >= PPTR_بنیاد && pptr < PPTR_چوٹی;
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
BOOT_رمز ساکن bool_t provide_untyped_cap(
    cap_t      جڑ_cnode_cap,
    bool_t     device_حافظہ_2,
    pptr_t     pptr,
    لفظ_t     سائز_بٹ,
    se_L4_خانہ_Pos پہلا_untyped_خانہ
)
{
    bool_t ret;
    cap_t ut_cap;

    /* Since we are in boot code, we can do extensive error checking and
       return failure if anything unexpected happens. */

    /* Bounds check for size parameter */
    اگر (سائز_بٹ > se_L4_Max_Untyped_بٹ || سائز_بٹ < se_L4_Min_Untyped_بٹ) {
        لکھو("Kernel init: Invalid untyped size %"SEL4_PRIu_لفظ"\n", سائز_بٹ);
        واپس false;
    }

    /* All cap ptrs must be aligned to object size */
    اگر (!IS_ALIGNED(pptr, سائز_بٹ)) {
        لکھو("Kernel init: Unaligned untyped pptr %p (alignment %"SEL4_PRIu_لفظ")\n", (خالی *)pptr, سائز_بٹ);
        واپس false;
    }

    /* All cap ptrs apart from device untypeds must be in the kernel window. */
    اگر (!device_حافظہ_2 && !pptr_اندر_kernel_کھڑکی(pptr)) {
        لکھو("Kernel init: Non-device untyped pptr %p outside kernel window\n",
               (خالی *)pptr);
        واپس false;
    }

    /* Check that the end of the region is also in the kernel window, so we don't
       need to assume that the kernel window is aligned up to potentially
       seL4_MaxUntypedBits. */
    اگر (!device_حافظہ_2 && !pptr_اندر_kernel_کھڑکی(pptr + نقاب(سائز_بٹ))) {
        لکھو("Kernel init: End of non-device untyped at %p outside kernel window (size %"SEL4_PRIu_لفظ")\n",
               (خالی *)pptr, سائز_بٹ);
        واپس false;
    }

    لفظ_t i = ndks_boot.خانہ_pos_cur - پہلا_untyped_خانہ;
    اگر (i < CONFIG_MAX_عدد_BOOTINFO_UNTYPED_CAPS) {
        ndks_boot.bi_چوکھٹا->untypedList[i] = (seL4_UntypedDesc) {
            .paddr    = pptr_مقصد_paddr((خالی *)pptr),
            .سائز_بٹ_2 = سائز_بٹ,
            .isDevice = device_حافظہ_2,
            .padding  = {0}
        };
        ut_cap = cap_untyped_cap_new(MAX_FREE_اشاریہ(سائز_بٹ),
                                     device_حافظہ_2, سائز_بٹ, pptr);
        ret = provide_cap(جڑ_cnode_cap, ut_cap);
    } ورنہ {
        لکھو("Kernel init: Too many untyped regions for boot info\n");
        ret = true;
    }
    واپس ret;
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
BOOT_رمز ساکن bool_t create_untypeds_for_region(
    cap_t      جڑ_cnode_cap,
    bool_t     device_حافظہ_2,
    region_t   reg,
    se_L4_خانہ_Pos پہلا_untyped_خانہ
)
{
    /* This code works with regions that wrap (where end < start), because the loop cuts up the
       region into size-aligned chunks, one for each cap. Memory chunks that are size-aligned cannot
       themselves overflow, so they satisfy alignment, size, and overflow conditions. The region
       [0..end) is not necessarily part of the kernel window (depending on the value of PPTR_BASE).
       This is fine for device untypeds. For normal untypeds, the region is assumed to be fully in
       the kernel window. This is not checked here. */
    جب (!is_reg_خالی_خانے(reg)) {

        /* Calculate the bit size of the region. This is also correct for end < start: it will
           return the correct size of the set [start..-1] union [0..end). This will then be too
           large for alignment, so the code further down will reduce the size. */
        غیرعلامتی صحیح سائز_بٹ = se_L4_لفظ_بٹ - 1 - clzl(reg.انتہا - reg.آغاز);
        /* The size can't exceed the largest possible untyped size. */
        اگر (سائز_بٹ > se_L4_Max_Untyped_بٹ) {
            سائز_بٹ = se_L4_Max_Untyped_بٹ;
        }
        /* The start address 0 satisfies any alignment needs, otherwise ensure
         * the region's bit size does not exceed the alignment of the region.
         */
        اگر (0 != reg.آغاز) {
            غیرعلامتی صحیح align_بٹ = ctzl(reg.آغاز);
            اگر (سائز_بٹ > align_بٹ) {
                سائز_بٹ = align_بٹ;
            }
        }
        /* Provide an untyped capability for the region only if it is large
         * enough to be retyped into objects later. Otherwise the region can't
         * be used anyway.
         */
        اگر (سائز_بٹ >= se_L4_Min_Untyped_بٹ) {
            اگر (!provide_untyped_cap(جڑ_cnode_cap, device_حافظہ_2, reg.آغاز, سائز_بٹ, پہلا_untyped_خانہ)) {
                واپس false;
            }
        }
        reg.آغاز += ثنائی_ہندسہ(سائز_بٹ);
    }
    واپس true;
}

BOOT_رمز bool_t create_untypeds(cap_t جڑ_cnode_cap)
{
    se_L4_خانہ_Pos پہلا_untyped_خانہ = ndks_boot.خانہ_pos_cur;

    paddr_t آغاز = 0;
    برائے (لفظ_t i = 0; i < ndks_boot.resv_گنتی; i++) {
        اگر (آغاز < ndks_boot.reserved[i].آغاز) {
            region_t reg = paddr_مقصد_pptr_reg((p_region_t) {
                آغاز, ndks_boot.reserved[i].آغاز
            });
            اگر (!create_untypeds_for_region(جڑ_cnode_cap, true, reg, پہلا_untyped_خانہ)) {
                لکھو("ERROR: creation of untypeds for device region #%u at"
                       " [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") failed\n",
                       (غیرعلامتی صحیح)i, reg.آغاز, reg.انتہا);
                واپس false;
            }
        }

        آغاز = ndks_boot.reserved[i].انتہا;
    }

    اگر (آغاز <= CONFIG_PADDR_USER_DEVICE_چوٹی - 1) {
        region_t reg = paddr_مقصد_pptr_reg((p_region_t) {
            /* CONFIG_PADDR_USER_DEVICE_TOP cast to paddr_t can be 0.
             * create_untypeds_for_region() can deal with that correctly. */
            آغاز, (paddr_t) CONFIG_PADDR_USER_DEVICE_چوٹی
        });

        اگر (!create_untypeds_for_region(جڑ_cnode_cap, true, reg, پہلا_untyped_خانہ)) {
            لکھو("ERROR: creation of untypeds for top device region"
                   " [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") failed\n",
                   reg.آغاز, reg.انتہا);
            واپس false;
        }
    }

    /* There is a part of the kernel (code/data) that is only needed for the
     * boot process. We can create UT objects for these frames, so the memory
     * can be reused.
     */
    region_t boot_mem_reuse_reg = paddr_مقصد_pptr_reg(get_p_reg_kernel_img_boot());
    اگر (!create_untypeds_for_region(جڑ_cnode_cap, false, boot_mem_reuse_reg, پہلا_untyped_خانہ)) {
        لکھو("ERROR: creation of untypeds for recycled boot memory"
               " [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ"] failed\n",
               boot_mem_reuse_reg.آغاز, boot_mem_reuse_reg.انتہا);
        واپس false;
    }

    /* convert remaining freemem into UT objects and provide the caps */
    برائے (لفظ_t i = 0; i < ARRAY_سائز(ndks_boot.freemem); i++) {
        region_t reg = ndks_boot.freemem[i];
        ndks_boot.freemem[i] = REG_خالی_خانے;
        اگر (!create_untypeds_for_region(جڑ_cnode_cap, false, reg, پہلا_untyped_خانہ)) {
            لکھو("ERROR: creation of untypeds for free memory region #%u at"
                   " [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") failed\n",
                   (غیرعلامتی صحیح)i, reg.آغاز, reg.انتہا);
            واپس false;
        }
    }

    ndks_boot.bi_چوکھٹا->untyped = (se_L4_خانہ_Region) {
        .آغاز = پہلا_untyped_خانہ,
        .انتہا   = ndks_boot.خانہ_pos_cur
    };

    واپس true;
}

BOOT_رمز خالی bi_finalise(خالی)
{

    اگر (rootserver.paging.آغاز != rootserver.paging.انتہا) {
        لکھو("WARNING: internal book keeping error. Less pagetables allocated than predicted: "
               "%ld page tables allocated but not used.\n", (rootserver.paging.انتہا - rootserver.paging.آغاز) >> se_L4_صفحہ_جدول_بٹ);
    }

    ndks_boot.bi_چوکھٹا->خالی_خانے = (se_L4_خانہ_Region) {
        .آغاز = ndks_boot.خانہ_pos_cur,
        .انتہا   = ثنائی_ہندسہ(CONFIG_جڑ_CNODE_سائز_بٹ)
    };
}

BOOT_رمز ساکن bool_t check_دستیاب_حافظہ(لفظ_t n_دستیاب,
                                               ثابت p_region_t *دستیاب)
{
    /* The system configuration is broken if no region is available. */
    اگر (0 == n_دستیاب) {
        لکھو("ERROR: no memory regions available\n");
        واپس false;
    }

    لکھو("Available phys memory regions: %"SEL4_PRIu_لفظ"\n", n_دستیاب);
    /* Force ordering and exclusivity of available regions. */
    برائے (لفظ_t i = 0; i < n_دستیاب; i++) {
        ثابت p_region_t *r = &دستیاب[i];
        لکھو("  [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ")\n", r->آغاز, r->انتہا);

        /* Available regions must not wrap */
        اگر (r->آغاز > r->انتہا) {
            لکھو("ERROR: memory region %"SEL4_PRIu_لفظ" has start > end\n", i);
            واپس false;
        }

        /* Available regions must be non-empty. */
        اگر (r->آغاز == r->انتہا) {
            لکھو("ERROR: memory region %"SEL4_PRIu_لفظ" empty\n", i);
            واپس false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
         * so the '==' case is fine. Directly adjacent regions are allowed.
         */
        اگر ((i > 0) && (r->آغاز < دستیاب[i - 1].انتہا)) {
            لکھو("ERROR: memory region %"SEL4_PRIu_لفظ" in wrong order\n", i);
            واپس false;
        }
    }

    واپس true;
}

/* We can't declare arrays on the stack, so this is space for init_avail_reg()
 * below to populate and init_freemem() to consume. */
BOOT_BSS ساکن region_t avail_reg[MAX_عدد_FREEMEM_REG];

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
BOOT_رمز ساکن لفظ_t init_avail_reg(لفظ_t n_دستیاب,
                                       ثابت p_region_t *دستیاب)
{
    لفظ_t cnt = 0;
    برائے (لفظ_t i = 0; i < n_دستیاب; i++) {
        ثابت p_region_t *r = &دستیاب[i];

        اگر ((r->آغاز >= PADDR_چوٹی) || (r->انتہا <= PADDR_بنیاد)) {
            لکھو("  region [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") is outside the kernel"
                   " window, only potentially available as device untypeds\n", r->آغاز, r->انتہا);
        } ورنہ {
            p_region_t usable_reg = {
                .آغاز = MAX(r->آغاز, PADDR_بنیاد),
                .انتہا   = MIN(r->انتہا, PADDR_چوٹی)
            };

            اگر (cnt >= ARRAY_سائز(avail_reg)) {
                /* No space left in the array, seems MAX_NUM_FREEMEM_REG should
                 * be increased. Debug builds raise an assert here because this
                 * is likely a porting issue that should be looked into. Release
                 * builds will continue booting, but can't use this memory
                 * region. This might still be sufficient to run.
                 */
                لکھو("  WARNING: cannot use region [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ"),"
                       " avail_reg[] is full\n", r->آغاز, r->انتہا);
                أكد(0);
            } ورنہ {
                avail_reg[cnt] = paddr_مقصد_pptr_reg(usable_reg);
                cnt++;
            }
        }
    }
    واپس cnt;
}


BOOT_رمز ساکن bool_t check_reserved_حافظہ(لفظ_t n_reserved,
                                              ثابت region_t *reserved)
{
    لکھو("Reserved virt address space regions: %"SEL4_PRIu_لفظ"\n",
           n_reserved);
    /* Force ordering and exclusivity of reserved regions. */
    برائے (لفظ_t i = 0; i < n_reserved; i++) {
        ثابت region_t *r = &reserved[i];
        لکھو("  [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ")\n", r->آغاز, r->انتہا);

        /* Reserved regions must be sane, the size is allowed to be zero. */
        اگر (r->آغاز > r->انتہا) {
            لکھو("ERROR: reserved region %"SEL4_PRIu_لفظ" has start > end\n", i);
            واپس false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
           so the == case is fine. Directly adjacent regions are allowed. */
        اگر ((i > 0) && (r->آغاز < reserved[i - 1].انتہا)) {
            لکھو("ERROR: reserved region %"SEL4_PRIu_لفظ" in wrong order\n", i);
            واپس false;
        }
    }

    واپس true;
}

/**
 * Dynamically initialise the available memory on the platform.
 * A region represents an area of memory.
 */
BOOT_رمز bool_t init_freemem(لفظ_t n_دستیاب, ثابت p_region_t *دستیاب,
                              لفظ_t n_reserved, ثابت region_t *reserved,
                              v_region_t it_v_reg, لفظ_t extra_bi_سائز_بٹ)
{
    اگر (!check_دستیاب_حافظہ(n_دستیاب, دستیاب)) {
        واپس false;
    }

    /* After here, avail_reg[0..n_available) are all regions within the kernel window. */
    n_دستیاب = init_avail_reg(n_دستیاب, دستیاب);
    اگر (0 == n_دستیاب) {
        لکھو("ERROR: no available memory within the kernel window\n");
        واپس false;
    }

    اگر (!check_reserved_حافظہ(n_reserved, reserved)) {
        واپس false;
    }

    برائے (لفظ_t i = 0; i < ARRAY_سائز(ndks_boot.freemem); i++) {
        ndks_boot.freemem[i] = REG_خالی_خانے;
    }

    لفظ_t a = 0;
    لفظ_t r = 0;
    /* Now iterate through the available regions, removing any reserved regions. */
    جب (a < n_دستیاب && r < n_reserved) {
        اگر (reserved[r].آغاز == reserved[r].انتہا) {
            /* reserved region is empty - skip it */
            r++;
        } ورنہ اگر (avail_reg[a].آغاز >= avail_reg[a].انتہا) {
            /* skip the entire region - it's empty now after trimming */
            a++;
        } ورنہ اگر (reserved[r].انتہا <= avail_reg[a].آغاز) {
            /* the reserved region is below the available region - skip it */
            reserve_region(pptr_مقصد_paddr_reg(reserved[r]));
            r++;
        } ورنہ اگر (reserved[r].آغاز >= avail_reg[a].انتہا) {
            /* the reserved region is above the available region - take the whole thing */
            داخل_کرو_region(avail_reg[a]);
            a++;
        } ورنہ {
            /* the reserved region overlaps with the available region */
            اگر (reserved[r].آغاز <= avail_reg[a].آغاز) {
                /* the region overlaps with the start of the available region.
                 * trim start of the available region */
                avail_reg[a].آغاز = MIN(avail_reg[a].انتہا, reserved[r].انتہا);
                /* do not increment reserved index here - there could be more overlapping regions */
            } ورنہ {
                أكد(reserved[r].آغاز < avail_reg[a].انتہا);
                /* take the first chunk of the available region and move
                 * the start to the end of the reserved region */
                region_t m = avail_reg[a];
                m.انتہا = reserved[r].آغاز;
                داخل_کرو_region(m);
                اگر (avail_reg[a].انتہا > reserved[r].انتہا) {
                    avail_reg[a].آغاز = reserved[r].انتہا;
                    /* we could increment reserved index here, but it's more consistent with the
                     * other overlapping case if we don't */
                } ورنہ {
                    a++;
                }
            }
        }
    }

    برائے (; r < n_reserved; r++) {
        اگر (reserved[r].آغاز < reserved[r].انتہا) {
            reserve_region(pptr_مقصد_paddr_reg(reserved[r]));
        }
    }

    /* no more reserved regions - add the rest */
    برائے (; a < n_دستیاب; a++) {
        اگر (avail_reg[a].آغاز < avail_reg[a].انتہا) {
            داخل_کرو_region(avail_reg[a]);
        }
    }

    /* now try to fit the root server objects into a region */
    صحیح i = ARRAY_سائز(ndks_boot.freemem) - 1;
    اگر (!is_reg_خالی_خانے(ndks_boot.freemem[i])) {
        لکھو("ERROR: insufficient MAX_NUM_FREEMEM_REG (%u)\n",
               (غیرعلامتی صحیح)MAX_عدد_FREEMEM_REG);
        واپس false;
    }
    /* skip any empty regions */
    برائے (; i >= 0 && is_reg_خالی_خانے(ndks_boot.freemem[i]); i--);

    /* try to grab the last available p region to create the root server objects
     * from. If possible, retain any left over memory as an extra p region */
    لفظ_t سائز = calculate_rootserver_سائز(it_v_reg, extra_bi_سائز_بٹ);
    لفظ_t max = rootserver_max_سائز_بٹ(extra_bi_سائز_بٹ);
    برائے (; i >= 0; i--) {
        /* Invariant: both i and (i + 1) are valid indices in ndks_boot.freemem. */
        أكد(i < ARRAY_سائز(ndks_boot.freemem) - 1);
        /* Invariant; the region at index i is the current candidate.
         * Invariant: regions 0 up to (i - 1), if any, are additional candidates.
         * Invariant: region (i + 1) is empty. */
        أكد(is_reg_خالی_خانے(ndks_boot.freemem[i + 1]));
        /* Invariant: regions above (i + 1), if any, are empty or too small to use.
         * Invariant: all non-empty regions are ordered, disjoint and unallocated. */

        /* We make a fresh variable to index the known-empty region, because the
         * SimplExportAndRefine verification test has poor support for array
         * indices that are sums of variables and small constants. */
        صحیح خالی_خانے_اشاریہ = i + 1;

        /* Try to take the top-most suitably sized and aligned chunk. */
        pptr_t unaligned_آغاز = ndks_boot.freemem[i].انتہا - سائز;
        pptr_t آغاز = باری_نیچے_کو(unaligned_آغاز, max);
        /* if unaligned_start didn't underflow, and start fits in the region,
         * then we've found a region that fits the root server objects. */
        اگر (unaligned_آغاز <= ndks_boot.freemem[i].انتہا
            && آغاز >= ndks_boot.freemem[i].آغاز) {
            create_rootserver_objects(آغاز, it_v_reg, extra_bi_سائز_بٹ);
            /* There may be leftovers before and after the memory we used. */
            /* Shuffle the after leftover up to the empty slot (i + 1). */
            ndks_boot.freemem[خالی_خانے_اشاریہ] = (region_t) {
                .آغاز = آغاز + سائز,
                .انتہا = ndks_boot.freemem[i].انتہا
            };
            /* Leave the before leftover in current slot i. */
            ndks_boot.freemem[i].انتہا = آغاز;
            /* Regions i and (i + 1) are now well defined, ordered, disjoint,
             * and unallocated, so we can return successfully. */
            واپس true;
        }
        /* Region i isn't big enough, so shuffle it up to slot (i + 1),
         * which we know is unused. */
        ndks_boot.freemem[خالی_خانے_اشاریہ] = ndks_boot.freemem[i];
        /* Now region i is unused, so make it empty to reestablish the invariant
         * for the next iteration (when it will be slot i + 1). */
        ndks_boot.freemem[i] = REG_خالی_خانے;
    }

    /* We didn't find a big enough region. */
    لکھو("ERROR: no free memory region is big enough for root server "
           "objects, need size/alignment of 2^%"SEL4_PRIu_لفظ"\n", max);
    واپس false;
}
