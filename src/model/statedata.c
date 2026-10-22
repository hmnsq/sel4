/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <api/debug.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <plat/machine.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <model/smp.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <object/tcb.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_track.h>

/* Collective cpu states, including both pre core architecture dependant and independent data */
SMP_STATE_DEFINE(smpStatedata_t, ksSMP[CONFIG_MAX_ಅಂಕೆ_NODES]);

/* Global count of how many cpus there are */
ಶಬ್ದ_t ks_ಅಂಕೆ_CPUs;

/* Pointer to the head of the scheduler queue for each priority */
ಮೇಲೆ_STATE_DEFINE(tcb_ಸರತಿ_t, ksReadyQueues[ಅಂಕೆ_READY_QUEUES]);
ಮೇಲೆ_STATE_DEFINE(ಶಬ್ದ_t, ksReadyQueuesL1Bitmap[CONFIG_ಅಂಕೆ_DOMAINS]);
ಮೇಲೆ_STATE_DEFINE(ಶಬ್ದ_t, ksReadyQueuesL2Bitmap[CONFIG_ಅಂಕೆ_DOMAINS][L2_BITMAP_ಗಾತ್ರ]);
compile_assert(ksReadyQueuesL1BitmapBigEnough, (L2_BITMAP_ಗಾತ್ರ - 1) <= ಶಬ್ದ_ಬಿಟ್ಟುಗಳು)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
/* Head of the queue of threads waiting for their budget to be replenished */
ಮೇಲೆ_STATE_DEFINE(tcb_ಸರತಿ_t, ks_Release_ಸರತಿ);
#ಆದರೆ_ಅಂತ್ಯ

/* Current thread TCB pointer */
ಮೇಲೆ_STATE_DEFINE(tcb_t *, ksCurThread);

/* Idle thread TCB pointer */
ಮೇಲೆ_STATE_DEFINE(tcb_t *, ksIdleThread);

/* Values of 0 and ~0 encode ResumeCurrentThread and ChooseNewThread
 * respectively; other values encode SwitchToThread and must be valid
 * tcb pointers */
ಮೇಲೆ_STATE_DEFINE(tcb_t *, ksSchedulerAction);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
/* The thread using the FPU, or NULL if FPU state is invalid */
ಮೇಲೆ_STATE_DEFINE(tcb_t *, ksCurFPUOwner);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_HAVE_FPU */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
/* the amount of time passed since the kernel time was last updated */
ಮೇಲೆ_STATE_DEFINE(ticks_t, ksConsumed);
/* whether we need to reprogram the timer before exiting the kernel */
ಮೇಲೆ_STATE_DEFINE(bool_t, ksReprogram);
/* the current kernel time (recorded on kernel entry) */
ಮೇಲೆ_STATE_DEFINE(ticks_t, ks_Cur_ಸಮಯ);
/* current scheduling context pointer */
ಮೇಲೆ_STATE_DEFINE(sched_context_t *, ksCurSC);
ಮೇಲೆ_STATE_DEFINE(sched_context_t *, ksIdleSC);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
ಮೇಲೆ_STATE_DEFINE(tcb_t *, ksDebugTCBs);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_UTILISATION
ಮೇಲೆ_STATE_DEFINE(bool_t, benchmark_log_utilisation_enabled);
ಮೇಲೆ_STATE_DEFINE(timestamp_t, benchmark_ಆರಂಭ_ಸಮಯ);
ಮೇಲೆ_STATE_DEFINE(timestamp_t, benchmark_ಕೊನೆ_ಸಮಯ);
ಮೇಲೆ_STATE_DEFINE(timestamp_t, benchmark_kernel_ಸಮಯ);
ಮೇಲೆ_STATE_DEFINE(timestamp_t, benchmark_kernel_ಸಂಖ್ಯೆ_entries);
ಮೇಲೆ_STATE_DEFINE(timestamp_t, benchmark_kernel_ಸಂಖ್ಯೆ_schedules);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_BENCHMARK_TRACK_UTILISATION */

/* Units of work we have completed since the last time we checked for
 * pending interrupts */
ಶಬ್ದ_t ksWorkUnitsCompleted;

irq_state_t intStateIRQTable[INT_STATE_ARRAY_ಗಾತ್ರ];
/* CNode containing interrupt handler endpoints - like all seL4 objects, this CNode needs to be
 * of a size that is a power of 2 and aligned to its size. */
cte_t intStateIRQNode[ಬಿಟ್(IRQ_CNODE_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(IRQ_CNODE_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು + se_L4_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು));
compile_assert(irq_CNode_ಗಾತ್ರ, ಗಾತ್ರ(intStateIRQNode) >= ((INT_STATE_ARRAY_ಗಾತ್ರ) *ಗಾತ್ರ(cte_t)));

/* Currently active domain */
dom_t ksCurDomain;

/* Domain timeslice remaining */
ticks_t ks_Domain_ಸಮಯ;

/* An index into ksDomSchedule for active domain and duration. */
ಶಬ್ದ_t ks_Dom_ವೇಳಾಪಟ್ಟಿ_Idx;

/* The value ksDomScheduleIdx will be set to when reaching either the end of
 * ksDomSchedule, or an end marker (entry with zero domain and duration). */
ಶಬ್ದ_t ks_Dom_ವೇಳಾಪಟ್ಟಿ_ಆರಂಭ;

/* Idle thread. */
SECTION("._idle_thread") ಅಕ್ಷರ ksIdleThreadTCB[CONFIG_MAX_ಅಂಕೆ_NODES][ಬಿಟ್(seL4_TCBBits)] ALIGN(ಬಿಟ್(seL4_TCBBits));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
/* Idle thread Schedcontexts */
ಅಕ್ಷರ ksIdleThreadSC[CONFIG_MAX_ಅಂಕೆ_NODES][ಬಿಟ್(se_L4_Min_Sched_Context_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(se_L4_Min_Sched_Context_ಬಿಟ್ಟುಗಳು));
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ (defined CONFIG_DEBUG_BUILD || defined CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES)
kernel_ನಮೂದು_t ks_Kernel_ನಮೂದು;
#ಆದರೆ_ಅಂತ್ಯ /* DEBUG */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_LOG_ಬಫರ್
paddr_t ks_User_Log_ಬಫರ್;
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_LOG_BUFFER */
