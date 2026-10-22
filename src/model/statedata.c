/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <api/debug.h>
#ضمن <types.h>
#ضمن <plat/machine.h>
#ضمن <model/statedata.h>
#ضمن <model/smp.h>
#ضمن <object/structures.h>
#ضمن <object/tcb.h>
#ضمن <benchmark/benchmark_track.h>

/* Collective cpu states, including both pre core architecture dependant and independent data */
SMP_STATE_DEFINE(smpStatedata_t, ksSMP[CONFIG_MAX_عدد_NODES]);

/* Global count of how many cpus there are */
کلمه_t ks_عدد_CPUs;

/* Pointer to the head of the scheduler queue for each priority */
پورته_STATE_DEFINE(tcb_کتار_t, ksReadyQueues[عدد_READY_QUEUES]);
پورته_STATE_DEFINE(کلمه_t, ksReadyQueuesL1Bitmap[CONFIG_عدد_DOMAINS]);
پورته_STATE_DEFINE(کلمه_t, ksReadyQueuesL2Bitmap[CONFIG_عدد_DOMAINS][L2_BITMAP_اندازه]);
compile_assert(ksReadyQueuesL1BitmapBigEnough, (L2_BITMAP_اندازه - 1) <= کلمه_بټونه)
#که_تعریف_شوی CONFIG_KERNEL_MCS
/* Head of the queue of threads waiting for their budget to be replenished */
پورته_STATE_DEFINE(tcb_کتار_t, ks_Release_کتار);
#د_که_پای

/* Current thread TCB pointer */
پورته_STATE_DEFINE(tcb_t *, ksCurThread);

/* Idle thread TCB pointer */
پورته_STATE_DEFINE(tcb_t *, ksIdleThread);

/* Values of 0 and ~0 encode ResumeCurrentThread and ChooseNewThread
 * respectively; other values encode SwitchToThread and must be valid
 * tcb pointers */
پورته_STATE_DEFINE(tcb_t *, ksSchedulerAction);

#که_تعریف_شوی CONFIG_HAVE_FPU
/* The thread using the FPU, or NULL if FPU state is invalid */
پورته_STATE_DEFINE(tcb_t *, ksCurFPUOwner);
#د_که_پای /* CONFIG_HAVE_FPU */

#که_تعریف_شوی CONFIG_KERNEL_MCS
/* the amount of time passed since the kernel time was last updated */
پورته_STATE_DEFINE(ticks_t, ksConsumed);
/* whether we need to reprogram the timer before exiting the kernel */
پورته_STATE_DEFINE(bool_t, ksReprogram);
/* the current kernel time (recorded on kernel entry) */
پورته_STATE_DEFINE(ticks_t, ks_Cur_وخت);
/* current scheduling context pointer */
پورته_STATE_DEFINE(sched_context_t *, ksCurSC);
پورته_STATE_DEFINE(sched_context_t *, ksIdleSC);
#د_که_پای

#که_تعریف_شوی CONFIG_DEBUG_BUILD
پورته_STATE_DEFINE(tcb_t *, ksDebugTCBs);
#د_که_پای /* CONFIG_DEBUG_BUILD */
#که_تعریف_شوی CONFIG_BENCHMARK_TRACK_UTILISATION
پورته_STATE_DEFINE(bool_t, benchmark_log_utilisation_enabled);
پورته_STATE_DEFINE(timestamp_t, benchmark_پیل_وخت);
پورته_STATE_DEFINE(timestamp_t, benchmark_پای_ټکی_وخت);
پورته_STATE_DEFINE(timestamp_t, benchmark_kernel_وخت);
پورته_STATE_DEFINE(timestamp_t, benchmark_kernel_شمېره_entries);
پورته_STATE_DEFINE(timestamp_t, benchmark_kernel_شمېره_schedules);
#د_که_پای /* CONFIG_BENCHMARK_TRACK_UTILISATION */

/* Units of work we have completed since the last time we checked for
 * pending interrupts */
کلمه_t ksWorkUnitsCompleted;

irq_state_t intStateIRQTable[INT_STATE_ARRAY_اندازه];
/* CNode containing interrupt handler endpoints - like all seL4 objects, this CNode needs to be
 * of a size that is a power of 2 and aligned to its size. */
cte_t intStateIRQNode[بټ(IRQ_CNODE_خونه_بټونه)] ALIGN(بټ(IRQ_CNODE_خونه_بټونه + se_L4_خونه_بټونه));
compile_assert(irq_CNode_اندازه, حجم(intStateIRQNode) >= ((INT_STATE_ARRAY_اندازه) *حجم(cte_t)));

/* Currently active domain */
dom_t ksCurDomain;

/* Domain timeslice remaining */
ticks_t ks_Domain_وخت;

/* An index into ksDomSchedule for active domain and duration. */
کلمه_t ks_Dom_مهالوېش_Idx;

/* The value ksDomScheduleIdx will be set to when reaching either the end of
 * ksDomSchedule, or an end marker (entry with zero domain and duration). */
کلمه_t ks_Dom_مهالوېش_پیل;

/* Idle thread. */
SECTION("._idle_thread") حرف ksIdleThreadTCB[CONFIG_MAX_عدد_NODES][بټ(seL4_TCBBits)] ALIGN(بټ(seL4_TCBBits));

#که_تعریف_شوی CONFIG_KERNEL_MCS
/* Idle thread Schedcontexts */
حرف ksIdleThreadSC[CONFIG_MAX_عدد_NODES][بټ(se_L4_Min_Sched_Context_بټونه)] ALIGN(بټ(se_L4_Min_Sched_Context_بټونه));
#د_که_پای

#إذا (defined CONFIG_DEBUG_BUILD || defined CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES)
kernel_ثبت_t ks_Kernel_ثبت;
#د_که_پای /* DEBUG */

#که_تعریف_شوی CONFIG_KERNEL_LOG_بفر
paddr_t ks_User_Log_بفر;
#د_که_پای /* CONFIG_KERNEL_LOG_BUFFER */
