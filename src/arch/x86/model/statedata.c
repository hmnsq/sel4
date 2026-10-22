/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <api/types.h>
#అన్తర్భావ <arch/types.h>
#అన్తర్భావ <arch/model/statedata.h>
#అన్తర్భావ <arch/object/structures.h>

/* ==== read/write kernel state not preserved across kernel entries ==== */

/* Interrupt currently being handled */
పైకి_STATE_DEFINE(అంతరాయం_t, x86_KScur_అంతరాయం VISIBLE);

పైకి_STATE_DEFINE(అంతరాయం_t, x86_KSPending_అంతరాయం);

/* ==== proper read/write kernel state ==== */

x86_arch_global_state_t x86KSGlobalState[CONFIG_MAX_అంకె_NODES] ALIGN(L1_CACHE_పంక్తి_కొలత) SKIM_BSS;

/* The top level ASID table */
asid_pool_t *x86KSASIDTable[nASIDPools];

/* Current user value of the fs/gs base */
పైకి_STATE_DEFINE(మాట_t, x86KSCurrentFSBase);
పైకి_STATE_DEFINE(మాట_t, x86KSCurrentGSBase);

పైకి_STATE_DEFINE(మాట_t, x86_KSGPExcept_Return_గమ్యం);

/* ==== read-only kernel state (only written during bootstrapping) ==== */

/* Defines a translation of cpu ids from an index of our actual CPUs */
SMP_STATE_DEFINE(cpu_id_mapping_t, cpu_mapping);

/* CPU Cache Line Size */
uint32_t x86_KScache_పంక్తి_కొలత_బిట్లు;

/* A valid initial FPU state, copied to every new thread. */
user_fpu_state_t x86KSnullFpuState ALIGN(MIN_FPU_ALIGNMENT);

/* Number of IOMMUs (DMA Remapping Hardware Units) */
uint32_t x86KSnumDrhu;

#నిర్వచితమైతే CONFIG_IOMMU
/* Intel VT-d Root Entry Table */
vtd_rte_t *x86_KSvtd_మూలం_పట్టిక;
uint32_t x86KSnumIOPTLevels;
uint32_t x86KSnumIODomainIDBits;
uint32_t x86KSFirstValidIODomain;
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_VTX
పైకి_STATE_DEFINE(vcpu_t *, x86KSCurrentVCPU);
పైకి_STATE_DEFINE(నిర్మాణం vmxon_region, x86KSVMXOnRegion);
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_PRINTING
uint16_t x86_KSconsole_పోర్టు;
#అయితే_ముగింపు
#ఒకవేళ defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
uint16_t x86_KSdebug_పోర్టు;
#అయితే_ముగింపు

/* State data tracking what IRQ source is related to each
 * CPU vector */
x86_irq_state_t x86KSIRQState[maxIRQ + 1];

మాట_t x86KSAllocatedIOPorts[అంకె_IO_PORTS / CONFIG_మాట_కొలత];
#నిర్వచితమైతే CONFIG_KERNEL_MCS
uint32_t x86KStscMhz;
uint32_t x86_KSapic_నిష్పత్తి;
#అయితే_ముగింపు
