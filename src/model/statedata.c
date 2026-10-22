/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <api/debug.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <plat/machine.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <model/smp.h>
#అన్తర్భావ <object/structures.h>
#అన్తర్భావ <object/tcb.h>
#అన్తర్భావ <benchmark/benchmark_track.h>

/* Collective cpu states, including both pre core architecture dependant and independent data */
SMP_STATE_DEFINE(smpStatedata_t, ksSMP[CONFIG_MAX_అంకె_NODES]);

/* Global count of how many cpus there are */
మాట_t ks_అంకె_CPUs;

/* Pointer to the head of the scheduler queue for each priority */
పైకి_STATE_DEFINE(tcb_వరుస_t, ksReadyQueues[అంకె_READY_QUEUES]);
పైకి_STATE_DEFINE(మాట_t, ksReadyQueuesL1Bitmap[CONFIG_అంకె_DOMAINS]);
పైకి_STATE_DEFINE(మాట_t, ksReadyQueuesL2Bitmap[CONFIG_అంకె_DOMAINS][L2_BITMAP_కొలత]);
compile_assert(ksReadyQueuesL1BitmapBigEnough, (L2_BITMAP_కొలత - 1) <= మాట_బిట్లు)
#నిర్వచితమైతే CONFIG_KERNEL_MCS
/* Head of the queue of threads waiting for their budget to be replenished */
పైకి_STATE_DEFINE(tcb_వరుస_t, ks_Release_వరుస);
#అయితే_ముగింపు

/* Current thread TCB pointer */
పైకి_STATE_DEFINE(tcb_t *, ksCurThread);

/* Idle thread TCB pointer */
పైకి_STATE_DEFINE(tcb_t *, ksIdleThread);

/* Values of 0 and ~0 encode ResumeCurrentThread and ChooseNewThread
 * respectively; other values encode SwitchToThread and must be valid
 * tcb pointers */
పైకి_STATE_DEFINE(tcb_t *, ksSchedulerAction);

#నిర్వచితమైతే CONFIG_HAVE_FPU
/* The thread using the FPU, or NULL if FPU state is invalid */
పైకి_STATE_DEFINE(tcb_t *, ksCurFPUOwner);
#అయితే_ముగింపు /* CONFIG_HAVE_FPU */

#నిర్వచితమైతే CONFIG_KERNEL_MCS
/* the amount of time passed since the kernel time was last updated */
పైకి_STATE_DEFINE(ticks_t, ksConsumed);
/* whether we need to reprogram the timer before exiting the kernel */
పైకి_STATE_DEFINE(bool_t, ksReprogram);
/* the current kernel time (recorded on kernel entry) */
పైకి_STATE_DEFINE(ticks_t, ks_Cur_సమయం);
/* current scheduling context pointer */
పైకి_STATE_DEFINE(sched_context_t *, ksCurSC);
పైకి_STATE_DEFINE(sched_context_t *, ksIdleSC);
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
పైకి_STATE_DEFINE(tcb_t *, ksDebugTCBs);
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_UTILISATION
పైకి_STATE_DEFINE(bool_t, benchmark_log_utilisation_enabled);
పైకి_STATE_DEFINE(timestamp_t, benchmark_ప్రారంభం_సమయం);
పైకి_STATE_DEFINE(timestamp_t, benchmark_చివర_సమయం);
పైకి_STATE_DEFINE(timestamp_t, benchmark_kernel_సమయం);
పైకి_STATE_DEFINE(timestamp_t, benchmark_kernel_సంఖ్య_entries);
పైకి_STATE_DEFINE(timestamp_t, benchmark_kernel_సంఖ్య_schedules);
#అయితే_ముగింపు /* CONFIG_BENCHMARK_TRACK_UTILISATION */

/* Units of work we have completed since the last time we checked for
 * pending interrupts */
మాట_t ksWorkUnitsCompleted;

irq_state_t intStateIRQTable[INT_STATE_ARRAY_కొలత];
/* CNode containing interrupt handler endpoints - like all seL4 objects, this CNode needs to be
 * of a size that is a power of 2 and aligned to its size. */
cte_t intStateIRQNode[బిట్టు(IRQ_CNODE_స్థానం_బిట్లు)] ALIGN(బిట్టు(IRQ_CNODE_స్థానం_బిట్లు + se_L4_స్థానం_బిట్లు));
compile_assert(irq_CNode_కొలత, పరిమాణం(intStateIRQNode) >= ((INT_STATE_ARRAY_కొలత) *పరిమాణం(cte_t)));

/* Currently active domain */
dom_t ksCurDomain;

/* Domain timeslice remaining */
ticks_t ks_Domain_సమయం;

/* An index into ksDomSchedule for active domain and duration. */
మాట_t ks_Dom_కాలపట్టిక_Idx;

/* The value ksDomScheduleIdx will be set to when reaching either the end of
 * ksDomSchedule, or an end marker (entry with zero domain and duration). */
మాట_t ks_Dom_కాలపట్టిక_ప్రారంభం;

/* Idle thread. */
SECTION("._idle_thread") అక్షరం ksIdleThreadTCB[CONFIG_MAX_అంకె_NODES][బిట్టు(seL4_TCBBits)] ALIGN(బిట్టు(seL4_TCBBits));

#నిర్వచితమైతే CONFIG_KERNEL_MCS
/* Idle thread Schedcontexts */
అక్షరం ksIdleThreadSC[CONFIG_MAX_అంకె_NODES][బిట్టు(se_L4_Min_Sched_Context_బిట్లు)] ALIGN(బిట్టు(se_L4_Min_Sched_Context_బిట్లు));
#అయితే_ముగింపు

#ఒకవేళ (defined CONFIG_DEBUG_BUILD || defined CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES)
kernel_నమోదు_t ks_Kernel_నమోదు;
#అయితే_ముగింపు /* DEBUG */

#నిర్వచితమైతే CONFIG_KERNEL_LOG_బఫరు
paddr_t ks_User_Log_బఫరు;
#అయితే_ముగింపు /* CONFIG_KERNEL_LOG_BUFFER */
