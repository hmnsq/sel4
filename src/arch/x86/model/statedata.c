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
مٿي_STATE_DEFINE(مداخلت_t, x86_KScur_مداخلت VISIBLE);

مٿي_STATE_DEFINE(مداخلت_t, x86_KSPending_مداخلت);

/* ==== proper read/write kernel state ==== */

x86_arch_global_state_t x86KSGlobalState[CONFIG_MAX_انگ_NODES] ALIGN(L1_CACHE_سٽ_سائيز) SKIM_BSS;

/* The top level ASID table */
asid_pool_t *x86KSASIDTable[nASIDPools];

/* Current user value of the fs/gs base */
مٿي_STATE_DEFINE(لفظ_t, x86KSCurrentFSBase);
مٿي_STATE_DEFINE(لفظ_t, x86KSCurrentGSBase);

مٿي_STATE_DEFINE(لفظ_t, x86_KSGPExcept_Return_منزل);

/* ==== read-only kernel state (only written during bootstrapping) ==== */

/* Defines a translation of cpu ids from an index of our actual CPUs */
SMP_STATE_DEFINE(cpu_id_mapping_t, cpu_mapping);

/* CPU Cache Line Size */
uint32_t x86_KScache_سٽ_سائيز_بٽ;

/* A valid initial FPU state, copied to every new thread. */
user_fpu_state_t x86KSnullFpuState ALIGN(MIN_FPU_ALIGNMENT);

/* Number of IOMMUs (DMA Remapping Hardware Units) */
uint32_t x86KSnumDrhu;

#جيڪڏهن_وصف_ٿيل CONFIG_IOMMU
/* Intel VT-d Root Entry Table */
vtd_rte_t *x86_KSvtd_پاڙ_جدول;
uint32_t x86KSnumIOPTLevels;
uint32_t x86KSnumIODomainIDBits;
uint32_t x86KSFirstValidIODomain;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_VTX
مٿي_STATE_DEFINE(vcpu_t *, x86KSCurrentVCPU);
مٿي_STATE_DEFINE(بنية vmxon_region, x86KSVMXOnRegion);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
uint16_t x86_KSconsole_پورٽ;
#جيڪڏهن_پورو
#إذا defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
uint16_t x86_KSdebug_پورٽ;
#جيڪڏهن_پورو

/* State data tracking what IRQ source is related to each
 * CPU vector */
x86_irq_state_t x86KSIRQState[maxIRQ + 1];

لفظ_t x86KSAllocatedIOPorts[انگ_IO_PORTS / CONFIG_لفظ_سائيز];
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
uint32_t x86KStscMhz;
uint32_t x86_KSapic_تناسب;
#جيڪڏهن_پورو
