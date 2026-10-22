/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/structures.h>

/* ==== read/write kernel state not preserved across kernel entries ==== */

/* Interrupt currently being handled */
ਉੱਪਰ_STATE_DEFINE(ਰੁਕਾਵਟ_t, x86_KScur_ਰੁਕਾਵਟ VISIBLE);

ਉੱਪਰ_STATE_DEFINE(ਰੁਕਾਵਟ_t, x86_KSPending_ਰੁਕਾਵਟ);

/* ==== proper read/write kernel state ==== */

x86_arch_global_state_t x86KSGlobalState[CONFIG_MAX_ਅੰਕ_NODES] ALIGN(L1_CACHE_ਸਤਰ_ਮਾਪ) SKIM_BSS;

/* The top level ASID table */
asid_pool_t *x86KSASIDTable[nASIDPools];

/* Current user value of the fs/gs base */
ਉੱਪਰ_STATE_DEFINE(ਸ਼ਬਦ_t, x86KSCurrentFSBase);
ਉੱਪਰ_STATE_DEFINE(ਸ਼ਬਦ_t, x86KSCurrentGSBase);

ਉੱਪਰ_STATE_DEFINE(ਸ਼ਬਦ_t, x86_KSGPExcept_Return_ਮੰਜ਼ਿਲ);

/* ==== read-only kernel state (only written during bootstrapping) ==== */

/* Defines a translation of cpu ids from an index of our actual CPUs */
SMP_STATE_DEFINE(cpu_id_mapping_t, cpu_mapping);

/* CPU Cache Line Size */
uint32_t x86_KScache_ਸਤਰ_ਮਾਪ_ਬਿੱਟ;

/* A valid initial FPU state, copied to every new thread. */
user_fpu_state_t x86KSnullFpuState ALIGN(MIN_FPU_ALIGNMENT);

/* Number of IOMMUs (DMA Remapping Hardware Units) */
uint32_t x86KSnumDrhu;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
/* Intel VT-d Root Entry Table */
vtd_rte_t *x86_KSvtd_ਜੜ੍ਹ_ਸਾਰਣੀ;
uint32_t x86KSnumIOPTLevels;
uint32_t x86KSnumIODomainIDBits;
uint32_t x86KSFirstValidIODomain;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
ਉੱਪਰ_STATE_DEFINE(vcpu_t *, x86KSCurrentVCPU);
ਉੱਪਰ_STATE_DEFINE(ਬਣਤਰ vmxon_region, x86KSVMXOnRegion);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
uint16_t x86_KSconsole_ਪੋਰਟ;
#ਜੇ_ਅੰਤ
#ਜੇ defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
uint16_t x86_KSdebug_ਪੋਰਟ;
#ਜੇ_ਅੰਤ

/* State data tracking what IRQ source is related to each
 * CPU vector */
x86_irq_state_t x86KSIRQState[maxIRQ + 1];

ਸ਼ਬਦ_t x86KSAllocatedIOPorts[ਅੰਕ_IO_PORTS / CONFIG_ਸ਼ਬਦ_ਮਾਪ];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
uint32_t x86KStscMhz;
uint32_t x86_KSapic_ਅਨੁਪਾਤ;
#ਜੇ_ਅੰਤ
