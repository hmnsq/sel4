/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <api/debug.h>
#شامل <types.h>
#شامل <plat/machine.h>
#شامل <model/statedata.h>
#شامل <model/smp.h>
#شامل <object/structures.h>
#شامل <object/tcb.h>
#شامل <benchmark/benchmark_track.h>

/* Collective cpu states, including both pre core architecture dependant and independent data */
SMP_STATE_DEFINE(smpStatedata_t, ksSMP[CONFIG_MAX_عدد_NODES]);

/* Global count of how many cpus there are */
لفظ_t ks_عدد_CPUs;

/* Pointer to the head of the scheduler queue for each priority */
اوپر_STATE_DEFINE(tcb_قطار_t, ksReadyQueues[عدد_READY_QUEUES]);
اوپر_STATE_DEFINE(لفظ_t, ksReadyQueuesL1Bitmap[CONFIG_عدد_DOMAINS]);
اوپر_STATE_DEFINE(لفظ_t, ksReadyQueuesL2Bitmap[CONFIG_عدد_DOMAINS][L2_BITMAP_سائز]);
compile_assert(ksReadyQueuesL1BitmapBigEnough, (L2_BITMAP_سائز - 1) <= لفظ_بٹ)
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
/* Head of the queue of threads waiting for their budget to be replenished */
اوپر_STATE_DEFINE(tcb_قطار_t, ks_Release_قطار);
#اگر_ختم

/* Current thread TCB pointer */
اوپر_STATE_DEFINE(tcb_t *, ksCurThread);

/* Idle thread TCB pointer */
اوپر_STATE_DEFINE(tcb_t *, ksIdleThread);

/* Values of 0 and ~0 encode ResumeCurrentThread and ChooseNewThread
 * respectively; other values encode SwitchToThread and must be valid
 * tcb pointers */
اوپر_STATE_DEFINE(tcb_t *, ksSchedulerAction);

#اگر_تعریف_شدہ CONFIG_HAVE_FPU
/* The thread using the FPU, or NULL if FPU state is invalid */
اوپر_STATE_DEFINE(tcb_t *, ksCurFPUOwner);
#اگر_ختم /* CONFIG_HAVE_FPU */

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
/* the amount of time passed since the kernel time was last updated */
اوپر_STATE_DEFINE(ticks_t, ksConsumed);
/* whether we need to reprogram the timer before exiting the kernel */
اوپر_STATE_DEFINE(bool_t, ksReprogram);
/* the current kernel time (recorded on kernel entry) */
اوپر_STATE_DEFINE(ticks_t, ks_Cur_وقت);
/* current scheduling context pointer */
اوپر_STATE_DEFINE(sched_context_t *, ksCurSC);
اوپر_STATE_DEFINE(sched_context_t *, ksIdleSC);
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
اوپر_STATE_DEFINE(tcb_t *, ksDebugTCBs);
#اگر_ختم /* CONFIG_DEBUG_BUILD */
#اگر_تعریف_شدہ CONFIG_BENCHMARK_TRACK_UTILISATION
اوپر_STATE_DEFINE(bool_t, benchmark_log_utilisation_enabled);
اوپر_STATE_DEFINE(timestamp_t, benchmark_آغاز_وقت);
اوپر_STATE_DEFINE(timestamp_t, benchmark_انتہا_وقت);
اوپر_STATE_DEFINE(timestamp_t, benchmark_kernel_وقت);
اوپر_STATE_DEFINE(timestamp_t, benchmark_kernel_نمبر_entries);
اوپر_STATE_DEFINE(timestamp_t, benchmark_kernel_نمبر_schedules);
#اگر_ختم /* CONFIG_BENCHMARK_TRACK_UTILISATION */

/* Units of work we have completed since the last time we checked for
 * pending interrupts */
لفظ_t ksWorkUnitsCompleted;

irq_state_t intStateIRQTable[INT_STATE_ARRAY_سائز];
/* CNode containing interrupt handler endpoints - like all seL4 objects, this CNode needs to be
 * of a size that is a power of 2 and aligned to its size. */
cte_t intStateIRQNode[ثنائی_ہندسہ(IRQ_CNODE_خانہ_بٹ)] ALIGN(ثنائی_ہندسہ(IRQ_CNODE_خانہ_بٹ + se_L4_خانہ_بٹ));
compile_assert(irq_CNode_سائز, حجم(intStateIRQNode) >= ((INT_STATE_ARRAY_سائز) *حجم(cte_t)));

/* Currently active domain */
dom_t ksCurDomain;

/* Domain timeslice remaining */
ticks_t ks_Domain_وقت;

/* An index into ksDomSchedule for active domain and duration. */
لفظ_t ks_Dom_جدول_وقت_Idx;

/* The value ksDomScheduleIdx will be set to when reaching either the end of
 * ksDomSchedule, or an end marker (entry with zero domain and duration). */
لفظ_t ks_Dom_جدول_وقت_آغاز;

/* Idle thread. */
SECTION("._idle_thread") حرف ksIdleThreadTCB[CONFIG_MAX_عدد_NODES][ثنائی_ہندسہ(seL4_TCBBits)] ALIGN(ثنائی_ہندسہ(seL4_TCBBits));

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
/* Idle thread Schedcontexts */
حرف ksIdleThreadSC[CONFIG_MAX_عدد_NODES][ثنائی_ہندسہ(se_L4_Min_Sched_Context_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_Min_Sched_Context_بٹ));
#اگر_ختم

#اگر (defined CONFIG_DEBUG_BUILD || defined CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES)
kernel_اندراج_t ks_Kernel_اندراج;
#اگر_ختم /* DEBUG */

#اگر_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
paddr_t ks_User_Log_بفر;
#اگر_ختم /* CONFIG_KERNEL_LOG_BUFFER */
