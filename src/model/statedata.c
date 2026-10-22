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
SMP_STATE_DEFINE(smpStatedata_t, ksSMP[CONFIG_MAX_رقم_NODES]);

/* Global count of how many cpus there are */
کلمه_t ks_رقم_CPUs;

/* Pointer to the head of the scheduler queue for each priority */
به_بالا_STATE_DEFINE(tcb_صف_t, ksReadyQueues[رقم_READY_QUEUES]);
به_بالا_STATE_DEFINE(کلمه_t, ksReadyQueuesL1Bitmap[CONFIG_رقم_DOMAINS]);
به_بالا_STATE_DEFINE(کلمه_t, ksReadyQueuesL2Bitmap[CONFIG_رقم_DOMAINS][L2_BITMAP_اندازه]);
compile_assert(ksReadyQueuesL1BitmapBigEnough, (L2_BITMAP_اندازه - 1) <= کلمه_بیتها)
#اگر_تعریف_شده CONFIG_KERNEL_MCS
/* Head of the queue of threads waiting for their budget to be replenished */
به_بالا_STATE_DEFINE(tcb_صف_t, ks_Release_صف);
#پایان_اگر

/* Current thread TCB pointer */
به_بالا_STATE_DEFINE(tcb_t *, ksCurThread);

/* Idle thread TCB pointer */
به_بالا_STATE_DEFINE(tcb_t *, ksIdleThread);

/* Values of 0 and ~0 encode ResumeCurrentThread and ChooseNewThread
 * respectively; other values encode SwitchToThread and must be valid
 * tcb pointers */
به_بالا_STATE_DEFINE(tcb_t *, ksSchedulerAction);

#اگر_تعریف_شده CONFIG_HAVE_FPU
/* The thread using the FPU, or NULL if FPU state is invalid */
به_بالا_STATE_DEFINE(tcb_t *, ksCurFPUOwner);
#پایان_اگر /* CONFIG_HAVE_FPU */

#اگر_تعریف_شده CONFIG_KERNEL_MCS
/* the amount of time passed since the kernel time was last updated */
به_بالا_STATE_DEFINE(ticks_t, ksConsumed);
/* whether we need to reprogram the timer before exiting the kernel */
به_بالا_STATE_DEFINE(bool_t, ksReprogram);
/* the current kernel time (recorded on kernel entry) */
به_بالا_STATE_DEFINE(ticks_t, ks_Cur_زمان);
/* current scheduling context pointer */
به_بالا_STATE_DEFINE(sched_context_t *, ksCurSC);
به_بالا_STATE_DEFINE(sched_context_t *, ksIdleSC);
#پایان_اگر

#اگر_تعریف_شده CONFIG_DEBUG_BUILD
به_بالا_STATE_DEFINE(tcb_t *, ksDebugTCBs);
#پایان_اگر /* CONFIG_DEBUG_BUILD */
#اگر_تعریف_شده CONFIG_BENCHMARK_TRACK_UTILISATION
به_بالا_STATE_DEFINE(bool_t, benchmark_log_utilisation_enabled);
به_بالا_STATE_DEFINE(timestamp_t, benchmark_شروع_زمان);
به_بالا_STATE_DEFINE(timestamp_t, benchmark_انتها_زمان);
به_بالا_STATE_DEFINE(timestamp_t, benchmark_kernel_زمان);
به_بالا_STATE_DEFINE(timestamp_t, benchmark_kernel_عدد_entries);
به_بالا_STATE_DEFINE(timestamp_t, benchmark_kernel_عدد_schedules);
#پایان_اگر /* CONFIG_BENCHMARK_TRACK_UTILISATION */

/* Units of work we have completed since the last time we checked for
 * pending interrupts */
کلمه_t ksWorkUnitsCompleted;

irq_state_t intStateIRQTable[INT_STATE_ARRAY_اندازه];
/* CNode containing interrupt handler endpoints - like all seL4 objects, this CNode needs to be
 * of a size that is a power of 2 and aligned to its size. */
cte_t intStateIRQNode[بیت(IRQ_CNODE_جایگاه_بیتها)] ALIGN(بیت(IRQ_CNODE_جایگاه_بیتها + se_L4_جایگاه_بیتها));
compile_assert(irq_CNode_اندازه, حجم(intStateIRQNode) >= ((INT_STATE_ARRAY_اندازه) *حجم(cte_t)));

/* Currently active domain */
dom_t ksCurDomain;

/* Domain timeslice remaining */
ticks_t ks_Domain_زمان;

/* An index into ksDomSchedule for active domain and duration. */
کلمه_t ks_Dom_زمانبندی_Idx;

/* The value ksDomScheduleIdx will be set to when reaching either the end of
 * ksDomSchedule, or an end marker (entry with zero domain and duration). */
کلمه_t ks_Dom_زمانبندی_شروع;

/* Idle thread. */
SECTION("._idle_thread") حرف ksIdleThreadTCB[CONFIG_MAX_رقم_NODES][بیت(seL4_TCBBits)] ALIGN(بیت(seL4_TCBBits));

#اگر_تعریف_شده CONFIG_KERNEL_MCS
/* Idle thread Schedcontexts */
حرف ksIdleThreadSC[CONFIG_MAX_رقم_NODES][بیت(se_L4_Min_Sched_Context_بیتها)] ALIGN(بیت(se_L4_Min_Sched_Context_بیتها));
#پایان_اگر

#إذا (defined CONFIG_DEBUG_BUILD || defined CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES)
kernel_مدخل_t ks_Kernel_مدخل;
#پایان_اگر /* DEBUG */

#اگر_تعریف_شده CONFIG_KERNEL_LOG_میانگیر
paddr_t ks_User_Log_میانگیر;
#پایان_اگر /* CONFIG_KERNEL_LOG_BUFFER */
