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
لفظ_t ks_عدد_CPUs;

/* Pointer to the head of the scheduler queue for each priority */
اتے_STATE_DEFINE(tcb_قطار_t, ksReadyQueues[عدد_READY_QUEUES]);
اتے_STATE_DEFINE(لفظ_t, ksReadyQueuesL1Bitmap[CONFIG_عدد_DOMAINS]);
اتے_STATE_DEFINE(لفظ_t, ksReadyQueuesL2Bitmap[CONFIG_عدد_DOMAINS][L2_BITMAP_ناپ]);
compile_assert(ksReadyQueuesL1BitmapBigEnough, (L2_BITMAP_ناپ - 1) <= لفظ_بٹ)
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
/* Head of the queue of threads waiting for their budget to be replenished */
اتے_STATE_DEFINE(tcb_قطار_t, ks_Release_قطار);
#جے_انت

/* Current thread TCB pointer */
اتے_STATE_DEFINE(tcb_t *, ksCurThread);

/* Idle thread TCB pointer */
اتے_STATE_DEFINE(tcb_t *, ksIdleThread);

/* Values of 0 and ~0 encode ResumeCurrentThread and ChooseNewThread
 * respectively; other values encode SwitchToThread and must be valid
 * tcb pointers */
اتے_STATE_DEFINE(tcb_t *, ksSchedulerAction);

#جے_تعریف_شدہ CONFIG_HAVE_FPU
/* The thread using the FPU, or NULL if FPU state is invalid */
اتے_STATE_DEFINE(tcb_t *, ksCurFPUOwner);
#جے_انت /* CONFIG_HAVE_FPU */

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
/* the amount of time passed since the kernel time was last updated */
اتے_STATE_DEFINE(ticks_t, ksConsumed);
/* whether we need to reprogram the timer before exiting the kernel */
اتے_STATE_DEFINE(bool_t, ksReprogram);
/* the current kernel time (recorded on kernel entry) */
اتے_STATE_DEFINE(ticks_t, ks_Cur_ویلا);
/* current scheduling context pointer */
اتے_STATE_DEFINE(sched_context_t *, ksCurSC);
اتے_STATE_DEFINE(sched_context_t *, ksIdleSC);
#جے_انت

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
اتے_STATE_DEFINE(tcb_t *, ksDebugTCBs);
#جے_انت /* CONFIG_DEBUG_BUILD */
#جے_تعریف_شدہ CONFIG_BENCHMARK_TRACK_UTILISATION
اتے_STATE_DEFINE(bool_t, benchmark_log_utilisation_enabled);
اتے_STATE_DEFINE(timestamp_t, benchmark_شروع_ویلا);
اتے_STATE_DEFINE(timestamp_t, benchmark_سرا_ویلا);
اتے_STATE_DEFINE(timestamp_t, benchmark_kernel_ویلا);
اتے_STATE_DEFINE(timestamp_t, benchmark_kernel_نمبر_entries);
اتے_STATE_DEFINE(timestamp_t, benchmark_kernel_نمبر_schedules);
#جے_انت /* CONFIG_BENCHMARK_TRACK_UTILISATION */

/* Units of work we have completed since the last time we checked for
 * pending interrupts */
لفظ_t ksWorkUnitsCompleted;

irq_state_t intStateIRQTable[INT_STATE_ARRAY_ناپ];
/* CNode containing interrupt handler endpoints - like all seL4 objects, this CNode needs to be
 * of a size that is a power of 2 and aligned to its size. */
cte_t intStateIRQNode[ثنائی_ہندسہ(IRQ_CNODE_تھاں_بٹ)] ALIGN(ثنائی_ہندسہ(IRQ_CNODE_تھاں_بٹ + se_L4_تھاں_بٹ));
compile_assert(irq_CNode_ناپ, حجم(intStateIRQNode) >= ((INT_STATE_ARRAY_ناپ) *حجم(cte_t)));

/* Currently active domain */
dom_t ksCurDomain;

/* Domain timeslice remaining */
ticks_t ks_Domain_ویلا;

/* An index into ksDomSchedule for active domain and duration. */
لفظ_t ks_Dom_ویلا_سارنی_Idx;

/* The value ksDomScheduleIdx will be set to when reaching either the end of
 * ksDomSchedule, or an end marker (entry with zero domain and duration). */
لفظ_t ks_Dom_ویلا_سارنی_شروع;

/* Idle thread. */
SECTION("._idle_thread") حرف ksIdleThreadTCB[CONFIG_MAX_عدد_NODES][ثنائی_ہندسہ(seL4_TCBBits)] ALIGN(ثنائی_ہندسہ(seL4_TCBBits));

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
/* Idle thread Schedcontexts */
حرف ksIdleThreadSC[CONFIG_MAX_عدد_NODES][ثنائی_ہندسہ(se_L4_Min_Sched_Context_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_Min_Sched_Context_بٹ));
#جے_انت

#إذا (defined CONFIG_DEBUG_BUILD || defined CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES)
kernel_اندراج_t ks_Kernel_اندراج;
#جے_انت /* DEBUG */

#جے_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
paddr_t ks_User_Log_بفر;
#جے_انت /* CONFIG_KERNEL_LOG_BUFFER */
