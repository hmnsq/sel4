/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/smp.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/tcb.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_track.h>

/* Collective cpu states, including both pre core architecture dependant and independent data */
SMP_STATE_DEFINE(smpStatedata_t, ksSMP[CONFIG_MAX_ਅੰਕ_NODES]);

/* Global count of how many cpus there are */
ਸ਼ਬਦ_t ks_ਅੰਕ_CPUs;

/* Pointer to the head of the scheduler queue for each priority */
ਉੱਪਰ_STATE_DEFINE(tcb_ਕਤਾਰ_t, ksReadyQueues[ਅੰਕ_READY_QUEUES]);
ਉੱਪਰ_STATE_DEFINE(ਸ਼ਬਦ_t, ksReadyQueuesL1Bitmap[CONFIG_ਅੰਕ_DOMAINS]);
ਉੱਪਰ_STATE_DEFINE(ਸ਼ਬਦ_t, ksReadyQueuesL2Bitmap[CONFIG_ਅੰਕ_DOMAINS][L2_BITMAP_ਮਾਪ]);
compile_assert(ksReadyQueuesL1BitmapBigEnough, (L2_BITMAP_ਮਾਪ - 1) <= ਸ਼ਬਦ_ਬਿੱਟ)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
/* Head of the queue of threads waiting for their budget to be replenished */
ਉੱਪਰ_STATE_DEFINE(tcb_ਕਤਾਰ_t, ks_Release_ਕਤਾਰ);
#ਜੇ_ਅੰਤ

/* Current thread TCB pointer */
ਉੱਪਰ_STATE_DEFINE(tcb_t *, ksCurThread);

/* Idle thread TCB pointer */
ਉੱਪਰ_STATE_DEFINE(tcb_t *, ksIdleThread);

/* Values of 0 and ~0 encode ResumeCurrentThread and ChooseNewThread
 * respectively; other values encode SwitchToThread and must be valid
 * tcb pointers */
ਉੱਪਰ_STATE_DEFINE(tcb_t *, ksSchedulerAction);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
/* The thread using the FPU, or NULL if FPU state is invalid */
ਉੱਪਰ_STATE_DEFINE(tcb_t *, ksCurFPUOwner);
#ਜੇ_ਅੰਤ /* CONFIG_HAVE_FPU */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
/* the amount of time passed since the kernel time was last updated */
ਉੱਪਰ_STATE_DEFINE(ticks_t, ksConsumed);
/* whether we need to reprogram the timer before exiting the kernel */
ਉੱਪਰ_STATE_DEFINE(bool_t, ksReprogram);
/* the current kernel time (recorded on kernel entry) */
ਉੱਪਰ_STATE_DEFINE(ticks_t, ks_Cur_ਸਮਾਂ);
/* current scheduling context pointer */
ਉੱਪਰ_STATE_DEFINE(sched_context_t *, ksCurSC);
ਉੱਪਰ_STATE_DEFINE(sched_context_t *, ksIdleSC);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
ਉੱਪਰ_STATE_DEFINE(tcb_t *, ksDebugTCBs);
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_BENCHMARK_TRACK_UTILISATION
ਉੱਪਰ_STATE_DEFINE(bool_t, benchmark_log_utilisation_enabled);
ਉੱਪਰ_STATE_DEFINE(timestamp_t, benchmark_ਸ਼ੁਰੂ_ਸਮਾਂ);
ਉੱਪਰ_STATE_DEFINE(timestamp_t, benchmark_ਸਿਰਾ_ਸਮਾਂ);
ਉੱਪਰ_STATE_DEFINE(timestamp_t, benchmark_kernel_ਸਮਾਂ);
ਉੱਪਰ_STATE_DEFINE(timestamp_t, benchmark_kernel_ਸੰਖਿਆ_entries);
ਉੱਪਰ_STATE_DEFINE(timestamp_t, benchmark_kernel_ਸੰਖਿਆ_schedules);
#ਜੇ_ਅੰਤ /* CONFIG_BENCHMARK_TRACK_UTILISATION */

/* Units of work we have completed since the last time we checked for
 * pending interrupts */
ਸ਼ਬਦ_t ksWorkUnitsCompleted;

irq_state_t intStateIRQTable[INT_STATE_ARRAY_ਮਾਪ];
/* CNode containing interrupt handler endpoints - like all seL4 objects, this CNode needs to be
 * of a size that is a power of 2 and aligned to its size. */
cte_t intStateIRQNode[ਦੋਆਧਾਰੀ_ਅੰਕ(IRQ_CNODE_ਥਾਂ_ਬਿੱਟ)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(IRQ_CNODE_ਥਾਂ_ਬਿੱਟ + se_L4_ਥਾਂ_ਬਿੱਟ));
compile_assert(irq_CNode_ਮਾਪ, ਆਕਾਰ(intStateIRQNode) >= ((INT_STATE_ARRAY_ਮਾਪ) *ਆਕਾਰ(cte_t)));

/* Currently active domain */
dom_t ksCurDomain;

/* Domain timeslice remaining */
ticks_t ks_Domain_ਸਮਾਂ;

/* An index into ksDomSchedule for active domain and duration. */
ਸ਼ਬਦ_t ks_Dom_ਸਮਾਂ_ਸਾਰਣੀ_Idx;

/* The value ksDomScheduleIdx will be set to when reaching either the end of
 * ksDomSchedule, or an end marker (entry with zero domain and duration). */
ਸ਼ਬਦ_t ks_Dom_ਸਮਾਂ_ਸਾਰਣੀ_ਸ਼ੁਰੂ;

/* Idle thread. */
SECTION("._idle_thread") ਅੱਖਰ ksIdleThreadTCB[CONFIG_MAX_ਅੰਕ_NODES][ਦੋਆਧਾਰੀ_ਅੰਕ(seL4_TCBBits)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(seL4_TCBBits));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
/* Idle thread Schedcontexts */
ਅੱਖਰ ksIdleThreadSC[CONFIG_MAX_ਅੰਕ_NODES][ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Min_Sched_Context_ਬਿੱਟ)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Min_Sched_Context_ਬਿੱਟ));
#ਜੇ_ਅੰਤ

#ਜੇ (defined CONFIG_DEBUG_BUILD || defined CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES)
kernel_ਇੰਦਰਾਜ_t ks_Kernel_ਇੰਦਰਾਜ;
#ਜੇ_ਅੰਤ /* DEBUG */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_LOG_ਬਫਰ
paddr_t ks_User_Log_ਬਫਰ;
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_LOG_BUFFER */
