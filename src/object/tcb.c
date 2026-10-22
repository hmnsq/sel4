/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <types.h>
#include <api/failures.h>
#include <api/invocation.h>
#include <api/syscall.h>
#include <sel4/shared_types.h>
#include <machine/io.h>
#include <object/structures.h>
#include <object/objecttype.h>
#include <object/cnode.h>
#ifdef CONFIG_KERNEL_MCS
#include <object/schedcontext.h>
#endif
#include <object/tcb.h>
#include <kernel/cspace.h>
#include <kernel/thread.h>
#include <kernel/vspace.h>
#include <model/statedata.h>
#include <util.h>
#include <string.h>
#include <stdint.h>
#include <arch/smp/ipi_inline.h>

#define NULL_PRIO 0

static exception_t checkPrio(prio_t prio, tcb_t *auth)
{
    prio_t mcp;

    mcp = auth->tcbMCP;

    /* system invariant: existing MCPs are bounded */
    assert(mcp <= seL4_MaxPrio);

    /* can't assign a priority greater than our own mcp */
    ถ้า (prio > mcp) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = seL4_MinPrio;
        current_syscall_error.rangeErrorMax = mcp;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    คืน EXCEPTION_NONE;
}

static anta_hstha void addToBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    NODE_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= BIT(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    NODE_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= BIT(prio & MASK(wordRadix));
}

static anta_hstha void removeFromBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    NODE_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~BIT(prio & MASK(wordRadix));
    ถ้า (unlikely(!NODE_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        NODE_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~BIT(l1index);
    }
}

tcb_queue_t tcb_queue_remove(tcb_queue_t queue, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcbSchedPrev;
    after = tcb->tcbSchedNext;

    ถ้า (queue.head == tcb && queue.end == tcb) {
        queue.head = NULL;
        queue.end = NULL;
    } มิฉะนั้น {
        ถ้า (queue.head == tcb) {
            after->tcbSchedPrev = NULL;
            tcb->tcbSchedNext = NULL;
            queue.head = after;
        } มิฉะนั้น {
            ถ้า (queue.end == tcb) {
                before->tcbSchedNext = NULL;
                tcb->tcbSchedPrev = NULL;
                queue.end = before;
            } มิฉะนั้น {
                before->tcbSchedNext = after;
                after->tcbSchedPrev = before;
                tcb->tcbSchedPrev = NULL;
                tcb->tcbSchedNext = NULL;
            }
        }
    }

    คืน queue;
}

/* Add TCB to the head of a scheduler queue */
void tcbSchedEnqueue(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    assert(isSchedulable(tcb));
    assert(refill_sufficient(tcb->tcbSchedContext, 0));
#endif

    ถ้า (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = NODE_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        ถ้า (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        NODE_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
void tcbSchedAppend(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    assert(isSchedulable(tcb));
    assert(refill_sufficient(tcb->tcbSchedContext, 0));
    assert(refill_ready(tcb->tcbSchedContext));
#endif
    ถ้า (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = NODE_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        ถ้า (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        NODE_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_append(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
void tcbSchedDequeue(tcb_t *tcb)
{
    ถ้า (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        tcb_queue_t new_queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = NODE_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_queue = tcb_queue_remove(queue, tcb);

        NODE_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_queue;

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, false);

        ถ้า (likely(tcb_queue_empty(new_queue))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#ifdef CONFIG_DEBUG_BUILD
void tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcbDebugPrev = NULL;

    debug_tcb->tcbDebugNext = NODE_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    ถ้า (NODE_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(NODE_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcbDebugPrev = tcb;
    }

    NODE_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

void tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    assert(NODE_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != NULL);
    ถ้า (tcb == NODE_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        NODE_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(NODE_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcbDebugNext;
    } มิฉะนั้น {
        assert(TCB_PTR_DEBUG_PTR(tcb)->tcbDebugPrev);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugPrev)->tcbDebugNext = debug_tcb->tcbDebugNext;
    }

    ถ้า (debug_tcb->tcbDebugNext) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugNext)->tcbDebugPrev = debug_tcb->tcbDebugPrev;
    }

    debug_tcb->tcbDebugPrev = NULL;
    debug_tcb->tcbDebugNext = NULL;
}
#endif /* CONFIG_DEBUG_BUILD */

#ifndef CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_queue_t tcbEPAppend(tcb_t *tcb, tcb_queue_t queue)
{
    ถ้า (!queue.head) { /* Empty list */
        queue.head = tcb;
    } มิฉะนั้น {
        queue.end->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = queue.end;
    tcb->tcbEPNext = NULL;
    queue.end = tcb;

    คืน queue;
}

/* Remove TCB from an endpoint queue */
tcb_queue_t tcbEPDequeue(tcb_t *tcb, tcb_queue_t queue)
{
    ถ้า (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } มิฉะนั้น {
        queue.head = tcb->tcbEPNext;
    }

    ถ้า (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } มิฉะนั้น {
        queue.end = tcb->tcbEPPrev;
    }

    คืน queue;
}
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS

void tcbReleaseRemove(tcb_t *tcb)
{
    ถ้า (likely(thread_state_get_tcbInReleaseQueue(tcb->tcbState))) {
        tcb_queue_t queue = NODE_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);

        ถ้า (queue.head == tcb) {
            NODE_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        NODE_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_remove(queue, tcb);

        thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, false);
    }
}

static anta_hstha ticks_t PURE tcbReadyTime(tcb_t *tcb)
{
    คืน refill_head(tcb->tcbSchedContext)->rTime;
}

static anta_hstha bool_t PURE time_after(tcb_t *tcb, ticks_t new_time)
{
    คืน tcb != NULL && new_time >= tcbReadyTime(tcb);
}

static tcb_t *find_time_after(tcb_t *tcb, ticks_t new_time)
{
    tcb_t *after = tcb;

    ขณะ (time_after(after, new_time)) {
        after = after->tcbSchedNext;
    }

    คืน after;
}

void tcbReleaseEnqueue(tcb_t *tcb)
{
    assert(thread_state_get_tcbInReleaseQueue(tcb->tcbState) == false);
    assert(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_time;
    tcb_queue_t queue;

    queue = NODE_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);
    new_time = tcbReadyTime(tcb);

    ถ้า (tcb_queue_empty(queue) || new_time < tcbReadyTime(queue.head)) {
        NODE_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);
    } มิฉะนั้น {
        ถ้า (tcbReadyTime(queue.end) <= new_time) {
            NODE_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_append(queue, tcb);
        } มิฉะนั้น {
            tcb_t *after;
            after = find_time_after(queue.head, new_time);
            tcb_queue_insert(tcb, after);
        }
    }

    thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, true);

    ถ้า (queue.head != NODE_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity).head) {
        NODE_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#endif

cptr_t PURE getExtraCPtr(word_t *bufferPtr, word_t i)
{
    คืน (cptr_t)bufferPtr[seL4_MsgMaxLength + 2 + i];
}

void setExtraBadge(word_t *bufferPtr, word_t badge,
                   word_t i)
{
    bufferPtr[seL4_MsgMaxLength + 2 + i] = badge;
}

#ifndef CONFIG_KERNEL_MCS
void setupCallerCap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *replySlot, *callerSlot;
    cap_t masterCap UNUSED, callerCap UNUSED;

    setThreadState(sender, ThreadState_BlockedOnReply);
    replySlot = TCB_PTR_CTE_PTR(sender, tcbReply);
    masterCap = replySlot->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    assert(cap_get_capType(masterCap) == cap_reply_cap);
    assert(cap_reply_cap_get_capReplyMaster(masterCap));
    assert(cap_reply_cap_get_capReplyCanGrant(masterCap));
    assert(TCB_PTR(cap_reply_cap_get_capTCBPtr(masterCap)) == sender);
    callerSlot = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = callerSlot->cap;
    /* Haskell error: "Caller cap must not already exist" */
    assert(cap_get_capType(callerCap) == cap_null_cap);
    cteInsert(cap_reply_cap_new(canGrant, false, TCB_REF(sender)),
              replySlot, callerSlot);
}

void deleteCallerCap(tcb_t *receiver)
{
    cte_t *callerSlot;

    callerSlot = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
    cteDeleteOne(callerSlot);
}
#endif

extra_caps_t current_extra_caps;

exception_t lookupExtraCaps(tcb_t *thread, word_t *bufferPtr, seL4_MessageInfo_t info)
{
    lookupSlot_raw_ret_t lu_ret;
    cptr_t cptr;
    word_t i, length;

    ถ้า (!bufferPtr) {
        current_extra_caps.excaprefs[0] = NULL;
        คืน EXCEPTION_NONE;
    }

    length = seL4_MessageInfo_get_extraCaps(info);

    สำหรับ (i = 0; i < length; i++) {
        cptr = getExtraCPtr(bufferPtr, i);

        lu_ret = lookupSlot(thread, cptr);
        ถ้า (lu_ret.status != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            คืน lu_ret.status;
        }

        current_extra_caps.excaprefs[i] = lu_ret.slot;
    }
    ถ้า (i < seL4_MsgMaxExtraCaps) {
        current_extra_caps.excaprefs[i] = NULL;
    }

    คืน EXCEPTION_NONE;
}

/* Copy IPC MRs from one thread to another */
word_t copyMRs(tcb_t *sender, word_t *sendBuf, tcb_t *receiver,
               word_t *recvBuf, word_t n)
{
    word_t i;

    /* Copy inline words */
    สำหรับ (i = 0; i < n && i < n_msgRegisters; i++) {
        setRegister(receiver, msgRegisters[i],
                    getRegister(sender, msgRegisters[i]));
    }

    ถ้า (!recvBuf || !sendBuf) {
        คืน i;
    }

    /* Copy out-of-line words */
    สำหรับ (; i < n; i++) {
        recvBuf[i + 1] = sendBuf[i + 1];
    }

    คืน i;
}

#ifdef ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
void remoteQueueUpdate(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    ถ้า (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *targetCurThread = NODE_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        ถ้า (targetCurThread == NODE_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcbPriority > targetCurThread->tcbPriority
#ifdef CONFIG_KERNEL_MCS
            || NODE_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#endif
           ) {
            ARCH_NODE_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
void remoteTCBStall(tcb_t *tcb)
{

    ถ้า (
#ifdef CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#endif
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        NODE_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_NODE_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
    }
}

#ifndef CONFIG_KERNEL_MCS
static exception_t invokeTCB_SetAffinity(tcb_t *thread, word_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcbSchedDequeue(thread);
    migrateTCB(thread, affinity);
    ถ้า (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    ถ้า (thread == NODE_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    คืน EXCEPTION_NONE;
}

static exception_t decodeSetAffinity(cap_t cap, word_t length, word_t *buffer)
{
    tcb_t *tcb;
    word_t affinity;

    ถ้า (length < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, buffer);
    ถ้า (affinity >= ksNumCPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    คืน invokeTCB_SetAffinity(tcb, affinity);
}
#endif
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_HARDWARE_DEBUG_API
static exception_t invokeConfigureSingleStepping(bool_t call, word_t *buffer, tcb_t *t,
                                                 uint16_t bp_num, word_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = NODE_STATE(ksCurThread);
    word_t value;

    bp_was_consumed = configureSingleStepping(t, bp_num, n_instrs, false);
    ถ้า (n_instrs == 0) {
#ifndef CONFIG_ARCH_AARCH64
        unsetBreakpointUsedFlag(t, bp_num);
#endif /* CONFIG_ARCH_AARCH64 */
        value = false;
    } มิฉะนั้น {
#ifndef CONFIG_ARCH_AARCH64
        setBreakpointUsedFlag(t, bp_num);
#endif /* CONFIG_ARCH_AARCH64 */
        value = bp_was_consumed;
    }

    ถ้า (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned จำนวนเต็ม length = setMR(thread, buffer, 0, value);
        setRegister(thread, msgInfoRegister, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Running);
    คืน EXCEPTION_NONE;
}

static exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, word_t *buffer)
{
    uint16_t bp_num;
    word_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_num = getSyscallArg(0, buffer);
    n_instrs = getSyscallArg(1, buffer);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_num, n_instrs, false);
    ถ้า (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    คืน invokeConfigureSingleStepping(call, buffer, tcb, bp_num, n_instrs);
}

static exception_t invokeSetBreakpoint(tcb_t *tcb, uint16_t bp_num,
                                       word_t vaddr, word_t type, word_t size, word_t rw)
{
    setBreakpoint(tcb, bp_num, vaddr, type, size, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    setBreakpointUsedFlag(tcb, bp_num);
    คืน EXCEPTION_NONE;
}

static exception_t decodeSetBreakpoint(cap_t cap, word_t *buffer)
{
    uint16_t bp_num;
    word_t vaddr, type, size, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_num = getSyscallArg(0, buffer);
    vaddr = getSyscallArg(1, buffer);
    type = getSyscallArg(2, buffer);
    size = getSyscallArg(3, buffer);
    rw = getSyscallArg(4, buffer);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    ถ้า (vaddr >= (word_t)USER_TOP) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalidArgumentNumber = 1;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    ถ้า (type != seL4_InstructionBreakpoint && type != seL4_DataBreakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalidArgumentNumber = 2;
        คืน EXCEPTION_SYSCALL_ERROR;
    } มิฉะนั้น ถ้า (type == seL4_InstructionBreakpoint) {
        ถ้า (size != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalidArgumentNumber = 3;
            คืน EXCEPTION_SYSCALL_ERROR;
        }
        ถ้า (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalidArgumentNumber = 4;
            คืน EXCEPTION_SYSCALL_ERROR;
        }
        ถ้า ((seL4_FirstWatchpoint == -1 || bp_num >= seL4_FirstWatchpoint)
            && seL4_FirstBreakpoint != seL4_FirstWatchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalidArgumentNumber = 2;
            คืน EXCEPTION_SYSCALL_ERROR;
        }
    } มิฉะนั้น ถ้า (type == seL4_DataBreakpoint) {
        ถ้า (size == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalidArgumentNumber = 3;
            คืน EXCEPTION_SYSCALL_ERROR;
        }
        ถ้า (seL4_FirstWatchpoint != -1 && bp_num < seL4_FirstWatchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalidArgumentNumber = 2;
            คืน EXCEPTION_SYSCALL_ERROR;
        }
    } มิฉะนั้น ถ้า (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalidArgumentNumber = 2;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    ถ้า (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalidArgumentNumber = 3;
        คืน EXCEPTION_SYSCALL_ERROR;
    }
    ถ้า (size != 0 && size != 1 && size != 2 && size != 4 && size != 8) {
        userError("Debug: Invalid size %lu.", size);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalidArgumentNumber = 3;
        คืน EXCEPTION_SYSCALL_ERROR;
    }
    ถ้า (size > 0 && vaddr & (size - 1)) {
        /* Just Don't allow unaligned watchpoints. They are undefined
         * both ARM and x86.
         *
         * X86: Intel manuals, vol3, 17.2.5:
         *  "Two-byte ranges must be aligned on word boundaries; 4-byte
         *   ranges must be aligned on doubleword boundaries"
         *  "Unaligned data or I/O breakpoint addresses do not yield valid
         *   results"
         *
         * ARM: ARMv7 manual, C11.11.44:
         *  "A DBGWVR is programmed with a word-aligned address."
         */
        userError("Debug: Unaligned data watchpoint address %lx (size %lx) "
                  "rejected.\n",
                  vaddr, size);

        current_syscall_error.type = seL4_AlignmentError;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decodeSetBreakpoint(tcb, bp_num, vaddr, type, size, rw);
    ถ้า (error.type != seL4_NoError) {
        current_syscall_error = error;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    คืน invokeSetBreakpoint(tcb, bp_num,
                               vaddr, type, size, rw);
}

static exception_t invokeGetBreakpoint(bool_t call, word_t *buffer, tcb_t *tcb, uint16_t bp_num)
{
    tcb_t *thread;
    thread = NODE_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_num);
    ถ้า (call) {
        setRegister(thread, badgeRegister, 0);
        setMR(NODE_STATE(ksCurThread), buffer, 0, res.vaddr);
        setMR(NODE_STATE(ksCurThread), buffer, 1, res.type);
        setMR(NODE_STATE(ksCurThread), buffer, 2, res.size);
        setMR(NODE_STATE(ksCurThread), buffer, 3, res.rw);
        setMR(NODE_STATE(ksCurThread), buffer, 4, res.is_enabled);
        setRegister(thread, msgInfoRegister, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, 5)));
    }
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Running);
    คืน EXCEPTION_NONE;
}

static exception_t decodeGetBreakpoint(cap_t cap, bool_t call, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_num;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_num = getSyscallArg(0, buffer);

    error = Arch_decodeGetBreakpoint(tcb, bp_num);
    ถ้า (error.type != seL4_NoError) {
        current_syscall_error = error;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    คืน invokeGetBreakpoint(call, buffer, tcb, bp_num);
}

static exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_num)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_num);
    unsetBreakpointUsedFlag(tcb, bp_num);
    คืน EXCEPTION_NONE;
}

static exception_t decodeUnsetBreakpoint(cap_t cap, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_num;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_num = getSyscallArg(0, buffer);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_num);
    ถ้า (error.type != seL4_NoError) {
        current_syscall_error = error;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    คืน invokeUnsetBreakpoint(tcb, bp_num);
}
#endif /* CONFIG_HARDWARE_DEBUG_API */

static exception_t invokeSetTLSBase(tcb_t *thread, word_t tls_base)
{
    setRegister(thread, TLS_BASE, tls_base);
    ถ้า (thread == NODE_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    คืน EXCEPTION_NONE;
}

static exception_t decodeSetTLSBase(cap_t cap, word_t length, word_t *buffer)
{
    word_t tls_base;

    ถ้า (length < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    tls_base = getSyscallArg(0, buffer);

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    คืน invokeSetTLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_base);
}

static void invokeSetFlags(tcb_t *thread, word_t clear, word_t set, bool_t call)
{
    tcb_t *cur_thread = NODE_STATE(ksCurThread);
    word_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= set & seL4_TCBFlag_MASK;
    thread->tcbFlags = flags;

#ifdef CONFIG_HAVE_FPU
    ถ้า (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } มิฉะนั้น ถ้า (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#endif
    ถ้า (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, cur_thread);
        setRegister(cur_thread, badgeRegister, 0);
        unsigned จำนวนเต็ม length = setMR(cur_thread, ipcBuffer, 0, flags);
        setRegister(cur_thread, msgInfoRegister, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(cur_thread, ThreadState_Running);
}

static exception_t decodeSetFlags(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ถ้า (length < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    word_t clear = getSyscallArg(0, buffer);
    word_t set   = getSyscallArg(1, buffer);

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    invokeSetFlags(thread, clear, set, call);
    คืน EXCEPTION_NONE;
}

/* The following functions sit in the syscall error monad, but include the
 * exception cases for the preemptible bottom end, as they call the invoke
 * functions directly.  This is a significant deviation from the Haskell
 * spec. */
exception_t decodeTCBInvocation(word_t invLabel, word_t length, cap_t cap,
                                cte_t *slot, bool_t call, word_t *buffer)
{
    /* Stall the core if we are operating on a remote TCB that is currently running */
    SMP_COND_STATEMENT(remoteTCBStall(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));)

    switch (invLabel) {
    pak_sa TCBReadRegisters:
        /* Second level of decoding */
        คืน decodeReadRegisters(cap, length, call, buffer);

    pak_sa TCBWriteRegisters:
        คืน decodeWriteRegisters(cap, length, buffer);

    pak_sa TCBCopyRegisters:
        คืน decodeCopyRegisters(cap, length, buffer);

    pak_sa TCBSuspend:
        /* Jump straight to the invoke */
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        คืน invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    pak_sa TCBResume:
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        คืน invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    pak_sa TCBConfigure:
        คืน decodeTCBConfigure(cap, length, slot, buffer);

    pak_sa TCBSetPriority:
        คืน decodeSetPriority(cap, length, buffer);

    pak_sa TCBSetMCPriority:
        คืน decodeSetMCPriority(cap, length, buffer);

    pak_sa TCBSetSchedParams:
#ifdef CONFIG_KERNEL_MCS
        คืน decodeSetSchedParams(cap, length, slot, buffer);
#มิฉะนั้น
        คืน decodeSetSchedParams(cap, length, buffer);
#endif

    pak_sa TCBSetIPCBuffer:
        คืน decodeSetIPCBuffer(cap, length, slot, buffer);

    pak_sa TCBSetSpace:
        คืน decodeSetSpace(cap, length, slot, buffer);

    pak_sa TCBBindNotification:
        คืน decodeBindNotification(cap);

    pak_sa TCBUnbindNotification:
        คืน decodeUnbindNotification(cap);

#ifdef CONFIG_KERNEL_MCS
    pak_sa TCBSetTimeoutEndpoint:
        คืน decodeSetTimeoutEndpoint(cap, slot);
#มิฉะนั้น
#ifdef ENABLE_SMP_SUPPORT
    pak_sa TCBSetAffinity:
        คืน decodeSetAffinity(cap, length, buffer);
#endif /* ENABLE_SMP_SUPPORT */
#endif

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#ifdef CONFIG_VTX
    pak_sa TCBSetEPTRoot:
        คืน decodeSetEPTRoot(cap);
#endif

#ifdef CONFIG_HARDWARE_DEBUG_API
    pak_sa TCBConfigureSingleStepping:
        คืน decodeConfigureSingleStepping(cap, call, buffer);

    pak_sa TCBSetBreakpoint:
        คืน decodeSetBreakpoint(cap, buffer);

    pak_sa TCBGetBreakpoint:
        คืน decodeGetBreakpoint(cap, call, buffer);

    pak_sa TCBUnsetBreakpoint:
        คืน decodeUnsetBreakpoint(cap, buffer);
#endif

    pak_sa TCBSetTLSBase:
        คืน decodeSetTLSBase(cap, length, buffer);

    pak_sa TCBSetFlags:
        คืน decodeSetFlags(cap, length, call, buffer);

    default:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        คืน EXCEPTION_SYSCALL_ERROR;
    }
}

ga_nanaa CopyRegistersFlags {
    CopyRegisters_suspendSource = 0,
    CopyRegisters_resumeTarget = 1,
    CopyRegisters_transferFrame = 2,
    CopyRegisters_transferInteger = 3
};

exception_t decodeCopyRegisters(cap_t cap, word_t length, word_t *buffer)
{
    word_t transferArch;
    tcb_t *srcTCB;
    cap_t source_cap;
    word_t flags;

    ถ้า (length < 1 || current_extra_caps.excaprefs[0] == NULL) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);

    transferArch = Arch_decodeTransfer(flags >> 8);

    source_cap = current_extra_caps.excaprefs[0]->cap;

    ถ้า (cap_get_capType(source_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(source_cap));
    } มิฉะนั้น {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    คืน invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & BIT(CopyRegisters_suspendSource),
               flags & BIT(CopyRegisters_resumeTarget),
               flags & BIT(CopyRegisters_transferFrame),
               flags & BIT(CopyRegisters_transferInteger),
               transferArch);

}

ga_nanaa ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, word_t length, bool_t call,
                                word_t *buffer)
{
    word_t transferArch, flags, n;
    tcb_t *thread;

    ถ้า (length < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    n     = getSyscallArg(1, buffer);

    ถ้า (n < 1 || n > n_frameRegisters + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (จำนวนเต็ม)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_frameRegisters +
                                              n_gpRegisters;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    ถ้า (thread == NODE_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    คืน invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & BIT(ReadRegisters_suspend),
               n, transferArch, call);
}

ga_nanaa WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
    word_t flags, w;
    word_t transferArch;
    tcb_t *thread;

    ถ้า (length < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    w     = getSyscallArg(1, buffer);

    ถ้า (length - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (จำนวนเต็ม)(length - 2), (จำนวนเต็ม)w);
        current_syscall_error.type = seL4_TruncatedMessage;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    ถ้า (thread == NODE_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    คืน invokeTCB_WriteRegisters(thread,
                                    flags & BIT(WriteRegisters_resume),
                                    w, transferArch, buffer);
}

#ifdef CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    switch (cap_get_capType(cap)) {
    pak_sa cap_endpoint_cap:
        คืน (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_capCanGrantReply(cap)));
    pak_sa cap_null_cap:
        /* just has no fault endpoint */
        คืน true;
    default:
        คืน false;
    }
}
#endif

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cte_t *bufferSlot, *cRootSlot, *vRootSlot;
    cap_t bufferCap, cRootCap, vRootCap;
    deriveCap_ret_t dc_ret;
    word_t cRootData, vRootData, bufferAddr;
#ifdef CONFIG_KERNEL_MCS
#define TCBCONFIGURE_ARGS 3
#มิฉะนั้น
#define TCBCONFIGURE_ARGS 4
#endif
    ถ้า (length < TCBCONFIGURE_ARGS || current_extra_caps.excaprefs[0] == NULL
        || current_extra_caps.excaprefs[1] == NULL
        || current_extra_caps.excaprefs[2] == NULL) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    cRootData     = getSyscallArg(0, buffer);
    vRootData     = getSyscallArg(1, buffer);
    bufferAddr    = getSyscallArg(2, buffer);
#มิฉะนั้น
    cptr_t faultEP       = getSyscallArg(0, buffer);
    cRootData     = getSyscallArg(1, buffer);
    vRootData     = getSyscallArg(2, buffer);
    bufferAddr    = getSyscallArg(3, buffer);
#endif

    cRootSlot  = current_extra_caps.excaprefs[0];
    cRootCap   = current_extra_caps.excaprefs[0]->cap;
    vRootSlot  = current_extra_caps.excaprefs[1];
    vRootCap   = current_extra_caps.excaprefs[1]->cap;
    bufferSlot = current_extra_caps.excaprefs[2];
    bufferCap  = current_extra_caps.excaprefs[2]->cap;

    ถ้า (bufferAddr == 0) {
        bufferSlot = NULL;
    } มิฉะนั้น {
        dc_ret = deriveCap(bufferSlot, bufferCap);
        ถ้า (dc_ret.status != EXCEPTION_NONE) {
            คืน dc_ret.status;
        }
        bufferCap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(bufferAddr, bufferCap);
        ถ้า (e != EXCEPTION_NONE) {
            คืน e;
        }
    }

    ถ้า (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    ถ้า (cRootData != 0) {
        cRootCap = updateCapData(false, cRootData, cRootCap);
    }

    dc_ret = deriveCap(cRootSlot, cRootCap);
    ถ้า (dc_ret.status != EXCEPTION_NONE) {
        คืน dc_ret.status;
    }
    cRootCap = dc_ret.cap;

    ถ้า (cap_get_capType(cRootCap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    ถ้า (vRootData != 0) {
        vRootCap = updateCapData(false, vRootData, vRootCap);
    }

    dc_ret = deriveCap(vRootSlot, vRootCap);
    ถ้า (dc_ret.status != EXCEPTION_NONE) {
        คืน dc_ret.status;
    }
    vRootCap = dc_ret.cap;

    ถ้า (!isValidVTableRoot(vRootCap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    คืน invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), NULL,
               cap_null_cap_new(), NULL,
               cRootCap, cRootSlot,
               vRootCap, vRootSlot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_caps_update_space |
               thread_control_caps_update_ipc_buffer);
#มิฉะนั้น
    คืน invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP, NULL_PRIO, NULL_PRIO,
               cRootCap, cRootSlot,
               vRootCap, vRootSlot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_update_space |
               thread_control_update_ipc_buffer);
#endif
}

exception_t decodeSetPriority(cap_t cap, word_t length, word_t *buffer)
{
    ถ้า (length < 1 || current_extra_caps.excaprefs[0] == NULL) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    ถ้า (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newPrio, authTCB);
    ถ้า (status != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        คืน status;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    คืน invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), NULL,
               cap_null_cap_new(), NULL,
               NULL_PRIO, newPrio,
               NULL, thread_control_sched_update_priority);
#มิฉะนั้น
    คืน invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), NULL,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), NULL,
               cap_null_cap_new(), NULL,
               0, cap_null_cap_new(),
               NULL, thread_control_update_priority);
#endif
}

exception_t decodeSetMCPriority(cap_t cap, word_t length, word_t *buffer)
{
    ถ้า (length < 1 || current_extra_caps.excaprefs[0] == NULL) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    ถ้า (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    ถ้า (status != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        คืน status;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    คืน invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), NULL,
               cap_null_cap_new(), NULL,
               newMcp, NULL_PRIO,
               NULL, thread_control_sched_update_mcp);
#มิฉะนั้น
    คืน invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), NULL,
               0, newMcp, NULL_PRIO,
               cap_null_cap_new(), NULL,
               cap_null_cap_new(), NULL,
               0, cap_null_cap_new(),
               NULL, thread_control_update_mcp);
#endif
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetTimeoutEndpoint(cap_t cap, cte_t *slot)
{
    ถ้า (current_extra_caps.excaprefs[0] == NULL) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *thSlot = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    ถ้า (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    คืน invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), NULL,
               thCap, thSlot,
               cap_null_cap_new(), NULL,
               cap_null_cap_new(), NULL,
               0, cap_null_cap_new(), NULL,
               thread_control_caps_update_timeout);
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetSchedParams(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
#มิฉะนั้น
exception_t decodeSetSchedParams(cap_t cap, word_t length, word_t *buffer)
#endif
{
    ถ้า (length < 2 || current_extra_caps.excaprefs[0] == NULL
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == NULL || current_extra_caps.excaprefs[2] == NULL
#endif
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    prio_t newPrio = getSyscallArg(1, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#ifdef CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fhSlot = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#endif

    ถ้า (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    ถ้า (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        คืน status;
    }

    status = checkPrio(newPrio, authTCB);
    ถ้า (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        คืน status;
    }

#ifdef CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = NULL;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_priority |
                                         thread_control_sched_update_fault;
    switch (cap_get_capType(scCap)) {
    pak_sa cap_sched_context_cap:
        sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(scCap));
        ถ้า (tcb->tcbSchedContext != sc) {
            ถ้า (tcb->tcbSchedContext) {
                userError("TCB Configure: tcb already has a scheduling context.");
                current_syscall_error.type = seL4_IllegalOperation;
                คืน EXCEPTION_SYSCALL_ERROR;
            }
            ถ้า (sc->scTcb) {
                userError("TCB Configure: sched context already bound.");
                current_syscall_error.type = seL4_IllegalOperation;
                คืน EXCEPTION_SYSCALL_ERROR;
            }
        }
        ถ้า (isBlocked(tcb) && !sc_released(sc)) {
            userError("TCB Configure: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            คืน EXCEPTION_SYSCALL_ERROR;
        }
        bha_nga;
    pak_sa cap_null_cap:
        ถ้า (tcb == NODE_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            คืน EXCEPTION_SYSCALL_ERROR;
        }
        bha_nga;
    default:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 2;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    ถ้า (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    ถ้า (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 3;
        คืน EXCEPTION_SYSCALL_ERROR;
    }
#endif
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    คืน invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               newMcp, newPrio,
               sc, update_flags);
#มิฉะนั้น
    คืน invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), NULL,
               0, newMcp, newPrio,
               cap_null_cap_new(), NULL,
               cap_null_cap_new(), NULL,
               0, cap_null_cap_new(),
               NULL, thread_control_update_mcp |
               thread_control_update_priority);
#endif
}


exception_t decodeSetIPCBuffer(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cptr_t cptr_bufferPtr;
    cap_t bufferCap;
    cte_t *bufferSlot;

    ถ้า (length < 1 || current_extra_caps.excaprefs[0] == NULL) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    cptr_bufferPtr  = getSyscallArg(0, buffer);
    bufferSlot = current_extra_caps.excaprefs[0];
    bufferCap  = current_extra_caps.excaprefs[0]->cap;

    ถ้า (cptr_bufferPtr == 0) {
        bufferSlot = NULL;
    } มิฉะนั้น {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(bufferSlot, bufferCap);
        ถ้า (dc_ret.status != EXCEPTION_NONE) {
            คืน dc_ret.status;
        }
        bufferCap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_bufferPtr, bufferCap);
        ถ้า (e != EXCEPTION_NONE) {
            คืน e;
        }
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    คืน invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), NULL,
               cap_null_cap_new(), NULL,
               cap_null_cap_new(), NULL,
               cap_null_cap_new(), NULL,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_caps_update_ipc_buffer);
#มิฉะนั้น
    คืน invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), NULL,
               cap_null_cap_new(), NULL,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_update_ipc_buffer);

#endif
}

#ifdef CONFIG_KERNEL_MCS
#define DECODE_SET_SPACE_PARAMS 2
#มิฉะนั้น
#define DECODE_SET_SPACE_PARAMS 3
#endif
exception_t decodeSetSpace(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    word_t cRootData, vRootData;
    cte_t *cRootSlot, *vRootSlot;
    cap_t cRootCap, vRootCap;
    deriveCap_ret_t dc_ret;

    ถ้า (length < DECODE_SET_SPACE_PARAMS || current_extra_caps.excaprefs[0] == NULL
        || current_extra_caps.excaprefs[1] == NULL
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == NULL
#endif
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    cRootData = getSyscallArg(0, buffer);
    vRootData = getSyscallArg(1, buffer);

    cte_t *fhSlot     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    cRootSlot  = current_extra_caps.excaprefs[1];
    cRootCap   = current_extra_caps.excaprefs[1]->cap;
    vRootSlot  = current_extra_caps.excaprefs[2];
    vRootCap   = current_extra_caps.excaprefs[2]->cap;
#มิฉะนั้น
    cptr_t faultEP   = getSyscallArg(0, buffer);
    cRootData = getSyscallArg(1, buffer);
    vRootData = getSyscallArg(2, buffer);

    cRootSlot  = current_extra_caps.excaprefs[0];
    cRootCap   = current_extra_caps.excaprefs[0]->cap;
    vRootSlot  = current_extra_caps.excaprefs[1];
    vRootCap   = current_extra_caps.excaprefs[1]->cap;
#endif

    ถ้า (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    ถ้า (cRootData != 0) {
        cRootCap = updateCapData(false, cRootData, cRootCap);
    }

    dc_ret = deriveCap(cRootSlot, cRootCap);
    ถ้า (dc_ret.status != EXCEPTION_NONE) {
        คืน dc_ret.status;
    }
    cRootCap = dc_ret.cap;

    ถ้า (cap_get_capType(cRootCap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    ถ้า (vRootData != 0) {
        vRootCap = updateCapData(false, vRootData, vRootCap);
    }

    dc_ret = deriveCap(vRootSlot, vRootCap);
    ถ้า (dc_ret.status != EXCEPTION_NONE) {
        คืน dc_ret.status;
    }
    vRootCap = dc_ret.cap;

    ถ้า (!isValidVTableRoot(vRootCap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    /* fault handler */
    ถ้า (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;
        คืน EXCEPTION_SYSCALL_ERROR;
    }
#endif

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    คืน invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               cap_null_cap_new(), NULL,
               cRootCap, cRootSlot,
               vRootCap, vRootSlot,
               0, cap_null_cap_new(), NULL, thread_control_caps_update_space | thread_control_caps_update_fault);
#มิฉะนั้น
    คืน invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               cRootCap, cRootSlot,
               vRootCap, vRootSlot,
               0, cap_null_cap_new(), NULL, thread_control_update_space);
#endif
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    ถ้า (current_extra_caps.excaprefs[0] == NULL) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ถ้า (tcb->tcbBoundNotification) {
        userError("TCB BindNotification: TCB already has a bound notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    ntfn_cap = current_extra_caps.excaprefs[0]->cap;

    ถ้า (cap_get_capType(ntfn_cap) == cap_notification_cap) {
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(ntfn_cap));
    } มิฉะนั้น {
        userError("TCB BindNotification: Notification is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    ถ้า (!cap_notification_cap_get_capNtfnCanReceive(ntfn_cap)) {
        userError("TCB BindNotification: Insufficient access rights");
        current_syscall_error.type = seL4_IllegalOperation;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    ถ้า ((tcb_t *)notification_ptr_get_ntfnQueue_head(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        คืน EXCEPTION_SYSCALL_ERROR;
    }


    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    คืน invokeTCB_NotificationControl(tcb, ntfnPtr);
}

exception_t decodeUnbindNotification(cap_t cap)
{
    tcb_t *tcb;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ถ้า (!tcb->tcbBoundNotification) {
        userError("TCB UnbindNotification: TCB already has no bound Notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        คืน EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    คืน invokeTCB_NotificationControl(tcb, NULL);
}

/* The following functions sit in the preemption monad and implement the
 * preemptible, non-faulting bottom end of a TCB invocation. */
exception_t invokeTCB_Suspend(tcb_t *thread)
{
    suspend(thread);
    คืน EXCEPTION_NONE;
}

exception_t invokeTCB_Resume(tcb_t *thread)
{
    restart(thread);
    คืน EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
static anta_hstha exception_t installTCBCap(tcb_t *target, cap_t tCap, cte_t *slot,
                                        tcb_cnode_index_t index, cap_t newCap, cte_t *srcSlot)
{
    cte_t *rootSlot = TCB_PTR_CTE_PTR(target, index);
    UNUSED exception_t e = cteDelete(rootSlot, true);
    ถ้า (e != EXCEPTION_NONE) {
        คืน e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    ถ้า (cap_get_capType(newCap) != cap_null_cap) {
        ถ้า (sameObjectAs(newCap, srcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cteInsert(newCap, srcSlot, rootSlot);
        }
    }
    คืน e;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *target, cte_t *slot,
                                        cap_t fh_newCap, cte_t *fh_srcSlot,
                                        cap_t th_newCap, cte_t *th_srcSlot,
                                        cap_t cRoot_newCap, cte_t *cRoot_srcSlot,
                                        cap_t vRoot_newCap, cte_t *vRoot_srcSlot,
                                        word_t bufferAddr, cap_t bufferCap,
                                        cte_t *bufferSrcSlot,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    ถ้า (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        ถ้า (e != EXCEPTION_NONE) {
            คืน e;
        }

    }

    ถ้า (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(target, tCap, slot, tcbTimeoutHandler, th_newCap, th_srcSlot);
        ถ้า (e != EXCEPTION_NONE) {
            คืน e;
        }
    }

    ถ้า (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(target, tCap, slot, tcbCTable, cRoot_newCap, cRoot_srcSlot);
        ถ้า (e != EXCEPTION_NONE) {
            คืน e;
        }

        e = installTCBCap(target, tCap, slot, tcbVTable, vRoot_newCap, vRoot_srcSlot);
        ถ้า (e != EXCEPTION_NONE) {
            คืน e;
        }
    }

    ถ้า (updateFlags & thread_control_caps_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        ถ้า (e != EXCEPTION_NONE) {
            คืน e;
        }
        target->tcbIPCBuffer = bufferAddr;

        ถ้า (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cteInsert(bufferCap, bufferSrcSlot, bufferSlot);
        }

        ถ้า (target == NODE_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    คืน EXCEPTION_NONE;
}
#มิฉะนั้น
exception_t invokeTCB_ThreadControl(tcb_t *target, cte_t *slot,
                                    cptr_t faultep, prio_t mcp, prio_t priority,
                                    cap_t cRoot_newCap, cte_t *cRoot_srcSlot,
                                    cap_t vRoot_newCap, cte_t *vRoot_srcSlot,
                                    word_t bufferAddr, cap_t bufferCap,
                                    cte_t *bufferSrcSlot,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    ถ้า (updateFlags & thread_control_update_space) {
        target->tcbFaultHandler = faultep;
    }

    ถ้า (updateFlags & thread_control_update_mcp) {
        setMCPriority(target, mcp);
    }

    ถ้า (updateFlags & thread_control_update_space) {
        cte_t *rootSlot;

        rootSlot = TCB_PTR_CTE_PTR(target, tcbCTable);
        e = cteDelete(rootSlot, true);
        ถ้า (e != EXCEPTION_NONE) {
            คืน e;
        }
        ถ้า (sameObjectAs(cRoot_newCap, cRoot_srcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cteInsert(cRoot_newCap, cRoot_srcSlot, rootSlot);
        }
    }

    ถ้า (updateFlags & thread_control_update_space) {
        cte_t *rootSlot;

        rootSlot = TCB_PTR_CTE_PTR(target, tcbVTable);
        e = cteDelete(rootSlot, true);
        ถ้า (e != EXCEPTION_NONE) {
            คืน e;
        }
        ถ้า (sameObjectAs(vRoot_newCap, vRoot_srcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cteInsert(vRoot_newCap, vRoot_srcSlot, rootSlot);
        }
    }

    ถ้า (updateFlags & thread_control_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        ถ้า (e != EXCEPTION_NONE) {
            คืน e;
        }
        target->tcbIPCBuffer = bufferAddr;

        ถ้า (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cteInsert(bufferCap, bufferSrcSlot, bufferSlot);
        }

        ถ้า (target == NODE_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    ถ้า (updateFlags & thread_control_update_priority) {
        setPriority(target, priority);
    }

    คืน EXCEPTION_NONE;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *target, cte_t *slot,
                                         cap_t fh_newCap, cte_t *fh_srcSlot,
                                         prio_t mcp, prio_t priority,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    ถ้า (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((word_t)target);
        exception_t e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        ถ้า (e != EXCEPTION_NONE) {
            คืน e;
        }
    }

    ถ้า (updateFlags & thread_control_sched_update_mcp) {
        setMCPriority(target, mcp);
    }

    ถ้า (updateFlags & thread_control_sched_update_priority) {
        setPriority(target, priority);
    }

    ถ้า (updateFlags & thread_control_sched_update_sc) {
        ถ้า (sc != NULL) {
            schedContext_bindTCB(sc, target);
        } มิฉะนั้น ถ้า (sc == NULL) {
            schedContext_unbindTCB(target->tcbSchedContext);
        }
    }

    คืน EXCEPTION_NONE;
}
#endif

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspendSource, bool_t resumeTarget,
                                    bool_t transferFrame, bool_t transferInteger,
                                    word_t transferArch)
{
    ถ้า (suspendSource) {
        suspend(tcb_src);
    }

    ถ้า (resumeTarget) {
        restart(dest);
    }

    ถ้า (transferFrame) {
        word_t i;
        word_t v;
        word_t pc;

        สำหรับ (i = 0; i < n_frameRegisters; i++) {
            v = getRegister(tcb_src, frameRegisters[i]);
            setRegister(dest, frameRegisters[i], v);
        }

        pc = getRestartPC(dest);
        setNextPC(dest, pc);
    }

    ถ้า (transferInteger) {
        word_t i;
        word_t v;

        สำหรับ (i = 0; i < n_gpRegisters; i++) {
            v = getRegister(tcb_src, gpRegisters[i]);
            setRegister(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    ถ้า (dest == NODE_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    คืน Arch_performTransfer(transferArch, tcb_src, dest);
}

/* ReadRegisters is a special case: replyFromKernel & setMRs are
 * unfolded here, in order to avoid passing the large reply message up
 * to the top level in a global (and double-copying). We prevent the
 * top-level replyFromKernel_success_empty() from running by setting the
 * thread state. Retype does this too.
 */
exception_t invokeTCB_ReadRegisters(tcb_t *tcb_src, bool_t suspendSource,
                                    word_t n, word_t arch, bool_t call)
{
    word_t i, j;
    exception_t e;
    tcb_t *thread;

    thread = NODE_STATE(ksCurThread);

    ถ้า (suspendSource) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, NODE_STATE(ksCurThread));
    ถ้า (e != EXCEPTION_NONE) {
        คืน e;
    }

    ถ้า (call) {
        word_t *ipcBuffer;

        ipcBuffer = lookupIPCBuffer(true, thread);

        setRegister(thread, badgeRegister, 0);

        สำหรับ (i = 0; i < n && i < n_frameRegisters && i < n_msgRegisters; i++) {
            setRegister(thread, msgRegisters[i],
                        getRegister(tcb_src, frameRegisters[i]));
        }

        ถ้า (ipcBuffer != NULL && i < n && i < n_frameRegisters) {
            สำหรับ (; i < n && i < n_frameRegisters; i++) {
                ipcBuffer[i + 1] = getRegister(tcb_src, frameRegisters[i]);
            }
        }

        j = i;

        สำหรับ (i = 0; i < n_gpRegisters && i + n_frameRegisters < n
             && i + n_frameRegisters < n_msgRegisters; i++) {
            setRegister(thread, msgRegisters[i + n_frameRegisters],
                        getRegister(tcb_src, gpRegisters[i]));
        }

        ถ้า (ipcBuffer != NULL && i < n_gpRegisters
            && i + n_frameRegisters < n) {
            สำหรับ (; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
                ipcBuffer[i + n_frameRegisters + 1] =
                    getRegister(tcb_src, gpRegisters[i]);
            }
        }

        setRegister(thread, msgInfoRegister, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, i + j)));
    }
    setThreadState(thread, ThreadState_Running);

    คืน EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resumeTarget,
                                     word_t n, word_t arch, word_t *buffer)
{
    word_t i;
    word_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, NODE_STATE(ksCurThread), dest);
    ถ้า (e != EXCEPTION_NONE) {
        คืน e;
    }

    ถ้า (n > n_frameRegisters + n_gpRegisters) {
        n = n_frameRegisters + n_gpRegisters;
    }

    archInfo = Arch_getSanitiseRegisterInfo(dest);

    สำหรับ (i = 0; i < n_frameRegisters && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        setRegister(dest, frameRegisters[i],
                    sanitiseRegister(frameRegisters[i],
                                     getSyscallArg(i + 2, buffer), archInfo));
    }

    สำหรับ (i = 0; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
        setRegister(dest, gpRegisters[i],
                    sanitiseRegister(gpRegisters[i],
                                     getSyscallArg(i + n_frameRegisters + 2,
                                                   buffer), archInfo));
    }

    pc = getRestartPC(dest);
    setNextPC(dest, pc);

    Arch_postModifyRegisters(dest);

    ถ้า (resumeTarget) {
        restart(dest);
    }

    ถ้า (dest == NODE_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    คืน EXCEPTION_NONE;
}

exception_t invokeTCB_NotificationControl(tcb_t *tcb, notification_t *ntfnPtr)
{
    ถ้า (ntfnPtr) {
        bindNotification(tcb, ntfnPtr);
    } มิฉะนั้น {
        unbindNotification(tcb);
    }

    คืน EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD
void setThreadName(tcb_t *tcb, const var_na *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_LENGTH);
}
#endif

word_t setMRs_syscall_error(tcb_t *thread, word_t *receiveIPCBuffer)
{
    switch (current_syscall_error.type) {
    pak_sa seL4_InvalidArgument:
        คืน setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalidArgumentNumber);

    pak_sa seL4_InvalidCapability:
        คืน setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalidCapNumber);

    pak_sa seL4_IllegalOperation:
        คืน 0;

    pak_sa seL4_RangeError:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        คืน setMR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    pak_sa seL4_AlignmentError:
        คืน 0;

    pak_sa seL4_FailedLookup:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failedLookupWasSource ? 1 : 0);
        คืน setMRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    pak_sa seL4_TruncatedMessage:
    pak_sa seL4_DeleteFirst:
    pak_sa seL4_RevokeFirst:
        คืน 0;
    pak_sa seL4_NotEnoughMemory:
        คืน setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.memoryLeft);
    default:
        fail("Invalid syscall error");
    }
}
