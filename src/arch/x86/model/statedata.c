/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <util.h>
#ضمن <api/types.h>
#ضمن <arch/types.h>
#ضمن <arch/model/statedata.h>
#ضمن <arch/object/structures.h>

/* ==== read/write kernel state not preserved across kernel entries ==== */

/* Interrupt currently being handled */
پورته_STATE_DEFINE(مداخله_t, x86_KScur_مداخله VISIBLE);

پورته_STATE_DEFINE(مداخله_t, x86_KSPending_مداخله);

/* ==== proper read/write kernel state ==== */

x86_arch_global_state_t x86KSGlobalState[CONFIG_MAX_عدد_NODES] ALIGN(L1_CACHE_کرښه_اندازه) SKIM_BSS;

/* The top level ASID table */
asid_pool_t *x86KSASIDTable[nASIDPools];

/* Current user value of the fs/gs base */
پورته_STATE_DEFINE(کلمه_t, x86KSCurrentFSBase);
پورته_STATE_DEFINE(کلمه_t, x86KSCurrentGSBase);

پورته_STATE_DEFINE(کلمه_t, x86_KSGPExcept_Return_منزل);

/* ==== read-only kernel state (only written during bootstrapping) ==== */

/* Defines a translation of cpu ids from an index of our actual CPUs */
SMP_STATE_DEFINE(cpu_id_mapping_t, cpu_mapping);

/* CPU Cache Line Size */
uint32_t x86_KScache_کرښه_اندازه_بټونه;

/* A valid initial FPU state, copied to every new thread. */
user_fpu_state_t x86KSnullFpuState ALIGN(MIN_FPU_ALIGNMENT);

/* Number of IOMMUs (DMA Remapping Hardware Units) */
uint32_t x86KSnumDrhu;

#که_تعریف_شوی CONFIG_IOMMU
/* Intel VT-d Root Entry Table */
vtd_rte_t *x86_KSvtd_ریښه_جدول;
uint32_t x86KSnumIOPTLevels;
uint32_t x86KSnumIODomainIDBits;
uint32_t x86KSFirstValidIODomain;
#د_که_پای

#که_تعریف_شوی CONFIG_VTX
پورته_STATE_DEFINE(vcpu_t *, x86KSCurrentVCPU);
پورته_STATE_DEFINE(بنية vmxon_region, x86KSVMXOnRegion);
#د_که_پای

#که_تعریف_شوی CONFIG_PRINTING
uint16_t x86_KSconsole_پورټ;
#د_که_پای
#إذا defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
uint16_t x86_KSdebug_پورټ;
#د_که_پای

/* State data tracking what IRQ source is related to each
 * CPU vector */
x86_irq_state_t x86KSIRQState[maxIRQ + 1];

کلمه_t x86KSAllocatedIOPorts[عدد_IO_PORTS / CONFIG_کلمه_اندازه];
#که_تعریف_شوی CONFIG_KERNEL_MCS
uint32_t x86KStscMhz;
uint32_t x86_KSapic_نسبت;
#د_که_پای
