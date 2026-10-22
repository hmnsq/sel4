/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <api/debug.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <plat/machine.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <model/smp.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <object/tcb.h>
#अन्तर्भाव <benchmark/benchmark_track.h>

/* Collective cpu states, including both pre core architecture dependant and independent data */
SMP_STATE_DEFINE(smpStatedata_t, ksSMP[CONFIG_MAX_अङ्कः_NODES]);

/* Global count of how many cpus there are */
word_t ks_अङ्कः_CPUs;

/* Pointer to the head of the scheduler queue for each priority */
ऊर्ध्वम्_STATE_DEFINE(tcb_queue_t, ksReadyQueues[अङ्कः_READY_QUEUES]);
ऊर्ध्वम्_STATE_DEFINE(word_t, ksReadyQueuesL1Bitmap[CONFIG_अङ्कः_DOMAINS]);
ऊर्ध्वम्_STATE_DEFINE(word_t, ksReadyQueuesL2Bitmap[CONFIG_अङ्कः_DOMAINS][L2_BITMAP_आकार]);
compile_assert(ksReadyQueuesL1BitmapBigEnough, (L2_BITMAP_आकार - 1) <= word_द्वयङ्काः)
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
/* Head of the queue of threads waiting for their budget to be replenished */
ऊर्ध्वम्_STATE_DEFINE(tcb_queue_t, ksReleaseQueue);
#यदि_अन्तः

/* Current thread TCB pointer */
ऊर्ध्वम्_STATE_DEFINE(tcb_t *, ksCurThread);

/* Idle thread TCB pointer */
ऊर्ध्वम्_STATE_DEFINE(tcb_t *, ksIdleThread);

/* Values of 0 and ~0 encode ResumeCurrentThread and ChooseNewThread
 * respectively; other values encode SwitchToThread and must be valid
 * tcb pointers */
ऊर्ध्वम्_STATE_DEFINE(tcb_t *, ksSchedulerAction);

#यदि_परिभाषितम् CONFIG_HAVE_FPU
/* The thread using the FPU, or NULL if FPU state is invalid */
ऊर्ध्वम्_STATE_DEFINE(tcb_t *, ksCurFPUOwner);
#यदि_अन्तः /* CONFIG_HAVE_FPU */

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
/* the amount of time passed since the kernel time was last updated */
ऊर्ध्वम्_STATE_DEFINE(ticks_t, ksConsumed);
/* whether we need to reprogram the timer before exiting the kernel */
ऊर्ध्वम्_STATE_DEFINE(bool_t, ksReprogram);
/* the current kernel time (recorded on kernel entry) */
ऊर्ध्वम्_STATE_DEFINE(ticks_t, ksCurTime);
/* current scheduling context pointer */
ऊर्ध्वम्_STATE_DEFINE(sched_context_t *, ksCurSC);
ऊर्ध्वम्_STATE_DEFINE(sched_context_t *, ksIdleSC);
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
ऊर्ध्वम्_STATE_DEFINE(tcb_t *, ksDebugTCBs);
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */
#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_UTILISATION
ऊर्ध्वम्_STATE_DEFINE(bool_t, benchmark_log_utilisation_enabled);
ऊर्ध्वम्_STATE_DEFINE(timestamp_t, benchmark_start_time);
ऊर्ध्वम्_STATE_DEFINE(timestamp_t, benchmark_अन्तः_time);
ऊर्ध्वम्_STATE_DEFINE(timestamp_t, benchmark_kernel_time);
ऊर्ध्वम्_STATE_DEFINE(timestamp_t, benchmark_kernel_सङ्ख्या_entries);
ऊर्ध्वम्_STATE_DEFINE(timestamp_t, benchmark_kernel_सङ्ख्या_schedules);
#यदि_अन्तः /* CONFIG_BENCHMARK_TRACK_UTILISATION */

/* Units of work we have completed since the last time we checked for
 * pending interrupts */
word_t ksWorkUnitsCompleted;

irq_state_t intStateIRQTable[INT_STATE_ARRAY_आकार];
/* CNode containing interrupt handler endpoints - like all seL4 objects, this CNode needs to be
 * of a size that is a power of 2 and aligned to its size. */
cte_t intStateIRQNode[BIT(IRQ_CNODE_SLOT_द्वयङ्काः)] ALIGN(BIT(IRQ_CNODE_SLOT_द्वयङ्काः + se_L4_Slot_द्वयङ्काः));
compile_assert(irq_CNode_आकार, परिमाण(intStateIRQNode) >= ((INT_STATE_ARRAY_आकार) *परिमाण(cte_t)));

/* Currently active domain */
dom_t ksCurDomain;

/* Domain timeslice remaining */
ticks_t ksDomainTime;

/* An index into ksDomSchedule for active domain and duration. */
word_t ks_Dom_कालसारणी_Idx;

/* The value ksDomScheduleIdx will be set to when reaching either the end of
 * ksDomSchedule, or an end marker (entry with zero domain and duration). */
word_t ks_Dom_कालसारणी_Start;

/* Idle thread. */
SECTION("._idle_thread") वर्ण ksIdleThreadTCB[CONFIG_MAX_अङ्कः_NODES][BIT(seL4_TCBBits)] ALIGN(BIT(seL4_TCBBits));

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
/* Idle thread Schedcontexts */
वर्ण ksIdleThreadSC[CONFIG_MAX_अङ्कः_NODES][BIT(se_L4_Min_Sched_Context_द्वयङ्काः)] ALIGN(BIT(se_L4_Min_Sched_Context_द्वयङ्काः));
#यदि_अन्तः

#यदि (defined CONFIG_DEBUG_BUILD || defined CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES)
kernel_प्रविष्टिः_t ks_Kernel_प्रविष्टिः;
#यदि_अन्तः /* DEBUG */

#यदि_परिभाषितम् CONFIG_KERNEL_LOG_बफर
paddr_t ks_User_Log_बफर;
#यदि_अन्तः /* CONFIG_KERNEL_LOG_BUFFER */
