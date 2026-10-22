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
SMP_STATE_DEFINE(smpStatedata_t, ksSMP[CONFIG_MAX_انگ_NODES]);

/* Global count of how many cpus there are */
لفظ_t ks_انگ_CPUs;

/* Pointer to the head of the scheduler queue for each priority */
مٿي_STATE_DEFINE(tcb_قطار_t, ksReadyQueues[انگ_READY_QUEUES]);
مٿي_STATE_DEFINE(لفظ_t, ksReadyQueuesL1Bitmap[CONFIG_انگ_DOMAINS]);
مٿي_STATE_DEFINE(لفظ_t, ksReadyQueuesL2Bitmap[CONFIG_انگ_DOMAINS][L2_BITMAP_سائيز]);
compile_assert(ksReadyQueuesL1BitmapBigEnough, (L2_BITMAP_سائيز - 1) <= لفظ_بٽ)
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
/* Head of the queue of threads waiting for their budget to be replenished */
مٿي_STATE_DEFINE(tcb_قطار_t, ks_Release_قطار);
#جيڪڏهن_پورو

/* Current thread TCB pointer */
مٿي_STATE_DEFINE(tcb_t *, ksCurThread);

/* Idle thread TCB pointer */
مٿي_STATE_DEFINE(tcb_t *, ksIdleThread);

/* Values of 0 and ~0 encode ResumeCurrentThread and ChooseNewThread
 * respectively; other values encode SwitchToThread and must be valid
 * tcb pointers */
مٿي_STATE_DEFINE(tcb_t *, ksSchedulerAction);

#جيڪڏهن_وصف_ٿيل CONFIG_HAVE_FPU
/* The thread using the FPU, or NULL if FPU state is invalid */
مٿي_STATE_DEFINE(tcb_t *, ksCurFPUOwner);
#جيڪڏهن_پورو /* CONFIG_HAVE_FPU */

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
/* the amount of time passed since the kernel time was last updated */
مٿي_STATE_DEFINE(ticks_t, ksConsumed);
/* whether we need to reprogram the timer before exiting the kernel */
مٿي_STATE_DEFINE(bool_t, ksReprogram);
/* the current kernel time (recorded on kernel entry) */
مٿي_STATE_DEFINE(ticks_t, ks_Cur_وقت);
/* current scheduling context pointer */
مٿي_STATE_DEFINE(sched_context_t *, ksCurSC);
مٿي_STATE_DEFINE(sched_context_t *, ksIdleSC);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
مٿي_STATE_DEFINE(tcb_t *, ksDebugTCBs);
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
#جيڪڏهن_وصف_ٿيل CONFIG_BENCHMARK_TRACK_UTILISATION
مٿي_STATE_DEFINE(bool_t, benchmark_log_utilisation_enabled);
مٿي_STATE_DEFINE(timestamp_t, benchmark_شروعات_وقت);
مٿي_STATE_DEFINE(timestamp_t, benchmark_پڇاڙي_وقت);
مٿي_STATE_DEFINE(timestamp_t, benchmark_kernel_وقت);
مٿي_STATE_DEFINE(timestamp_t, benchmark_kernel_عدد_entries);
مٿي_STATE_DEFINE(timestamp_t, benchmark_kernel_عدد_schedules);
#جيڪڏهن_پورو /* CONFIG_BENCHMARK_TRACK_UTILISATION */

/* Units of work we have completed since the last time we checked for
 * pending interrupts */
لفظ_t ksWorkUnitsCompleted;

irq_state_t intStateIRQTable[INT_STATE_ARRAY_سائيز];
/* CNode containing interrupt handler endpoints - like all seL4 objects, this CNode needs to be
 * of a size that is a power of 2 and aligned to its size. */
cte_t intStateIRQNode[ٻٽو_انگ(IRQ_CNODE_جڳهه_بٽ)] ALIGN(ٻٽو_انگ(IRQ_CNODE_جڳهه_بٽ + se_L4_جڳهه_بٽ));
compile_assert(irq_CNode_سائيز, حجم(intStateIRQNode) >= ((INT_STATE_ARRAY_سائيز) *حجم(cte_t)));

/* Currently active domain */
dom_t ksCurDomain;

/* Domain timeslice remaining */
ticks_t ks_Domain_وقت;

/* An index into ksDomSchedule for active domain and duration. */
لفظ_t ks_Dom_وقت_جدول_Idx;

/* The value ksDomScheduleIdx will be set to when reaching either the end of
 * ksDomSchedule, or an end marker (entry with zero domain and duration). */
لفظ_t ks_Dom_وقت_جدول_شروعات;

/* Idle thread. */
SECTION("._idle_thread") حرف ksIdleThreadTCB[CONFIG_MAX_انگ_NODES][ٻٽو_انگ(seL4_TCBBits)] ALIGN(ٻٽو_انگ(seL4_TCBBits));

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
/* Idle thread Schedcontexts */
حرف ksIdleThreadSC[CONFIG_MAX_انگ_NODES][ٻٽو_انگ(se_L4_Min_Sched_Context_بٽ)] ALIGN(ٻٽو_انگ(se_L4_Min_Sched_Context_بٽ));
#جيڪڏهن_پورو

#إذا (defined CONFIG_DEBUG_BUILD || defined CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES)
kernel_داخلا_t ks_Kernel_داخلا;
#جيڪڏهن_پورو /* DEBUG */

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_LOG_بفر
paddr_t ks_User_Log_بفر;
#جيڪڏهن_پورو /* CONFIG_KERNEL_LOG_BUFFER */
