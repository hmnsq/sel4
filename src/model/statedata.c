/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <api/debug.h>
#அந்தர்பாவ <types.h>
#அந்தர்பாவ <plat/machine.h>
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <model/smp.h>
#அந்தர்பாவ <object/structures.h>
#அந்தர்பாவ <object/tcb.h>
#அந்தர்பாவ <benchmark/benchmark_track.h>

/* Collective cpu states, including both pre core architecture dependant and independent data */
SMP_STATE_DEFINE(smpStatedata_t, ksSMP[CONFIG_MAX_இலக்கம்_NODES]);

/* Global count of how many cpus there are */
word_t ks_இலக்கம்_CPUs;

/* Pointer to the head of the scheduler queue for each priority */
UP_STATE_DEFINE(tcb_queue_t, ksReadyQueues[இலக்கம்_READY_QUEUES]);
UP_STATE_DEFINE(word_t, ksReadyQueuesL1Bitmap[CONFIG_இலக்கம்_DOMAINS]);
UP_STATE_DEFINE(word_t, ksReadyQueuesL2Bitmap[CONFIG_இலக்கம்_DOMAINS][L2_BITMAP_SIZE]);
compile_assert(ksReadyQueuesL1BitmapBigEnough, (L2_BITMAP_SIZE - 1) <= wordBits)
#ifdef CONFIG_KERNEL_MCS
/* Head of the queue of threads waiting for their budget to be replenished */
UP_STATE_DEFINE(tcb_queue_t, ksReleaseQueue);
#endif

/* Current thread TCB pointer */
UP_STATE_DEFINE(tcb_t *, ksCurThread);

/* Idle thread TCB pointer */
UP_STATE_DEFINE(tcb_t *, ksIdleThread);

/* Values of 0 and ~0 encode ResumeCurrentThread and ChooseNewThread
 * respectively; other values encode SwitchToThread and must be valid
 * tcb pointers */
UP_STATE_DEFINE(tcb_t *, ksSchedulerAction);

#ifdef CONFIG_HAVE_FPU
/* The thread using the FPU, or NULL if FPU state is invalid */
UP_STATE_DEFINE(tcb_t *, ksCurFPUOwner);
#endif /* CONFIG_HAVE_FPU */

#ifdef CONFIG_KERNEL_MCS
/* the amount of time passed since the kernel time was last updated */
UP_STATE_DEFINE(ticks_t, ksConsumed);
/* whether we need to reprogram the timer before exiting the kernel */
UP_STATE_DEFINE(bool_t, ksReprogram);
/* the current kernel time (recorded on kernel entry) */
UP_STATE_DEFINE(ticks_t, ksCurTime);
/* current scheduling context pointer */
UP_STATE_DEFINE(sched_context_t *, ksCurSC);
UP_STATE_DEFINE(sched_context_t *, ksIdleSC);
#endif

#ifdef CONFIG_DEBUG_BUILD
UP_STATE_DEFINE(tcb_t *, ksDebugTCBs);
#endif /* CONFIG_DEBUG_BUILD */
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
UP_STATE_DEFINE(bool_t, benchmark_log_utilisation_enabled);
UP_STATE_DEFINE(timestamp_t, benchmark_start_time);
UP_STATE_DEFINE(timestamp_t, benchmark_end_time);
UP_STATE_DEFINE(timestamp_t, benchmark_kernel_time);
UP_STATE_DEFINE(timestamp_t, benchmark_kernel_எண்_entries);
UP_STATE_DEFINE(timestamp_t, benchmark_kernel_எண்_schedules);
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

/* Units of work we have completed since the last time we checked for
 * pending interrupts */
word_t ksWorkUnitsCompleted;

irq_state_t intStateIRQTable[INT_STATE_ARRAY_SIZE];
/* CNode containing interrupt handler endpoints - like all seL4 objects, this CNode needs to be
 * of a size that is a power of 2 and aligned to its size. */
cte_t intStateIRQNode[BIT(IRQ_CNODE_SLOT_BITS)] ALIGN(BIT(IRQ_CNODE_SLOT_BITS + seL4_SlotBits));
compile_assert(irqCNodeSize, அளவு(intStateIRQNode) >= ((INT_STATE_ARRAY_SIZE) *அளவு(cte_t)));

/* Currently active domain */
dom_t ksCurDomain;

/* Domain timeslice remaining */
ticks_t ksDomainTime;

/* An index into ksDomSchedule for active domain and duration. */
word_t ksDomScheduleIdx;

/* The value ksDomScheduleIdx will be set to when reaching either the end of
 * ksDomSchedule, or an end marker (entry with zero domain and duration). */
word_t ksDomScheduleStart;

/* Idle thread. */
SECTION("._idle_thread") எழுத்து ksIdleThreadTCB[CONFIG_MAX_இலக்கம்_NODES][BIT(seL4_TCBBits)] ALIGN(BIT(seL4_TCBBits));

#ifdef CONFIG_KERNEL_MCS
/* Idle thread Schedcontexts */
எழுத்து ksIdleThreadSC[CONFIG_MAX_இலக்கம்_NODES][BIT(seL4_MinSchedContextBits)] ALIGN(BIT(seL4_MinSchedContextBits));
#endif

#எனில் (defined CONFIG_DEBUG_BUILD || defined CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES)
kernel_entry_t ksKernelEntry;
#endif /* DEBUG */

#ifdef CONFIG_KERNEL_LOG_BUFFER
paddr_t ksUserLogBuffer;
#endif /* CONFIG_KERNEL_LOG_BUFFER */
