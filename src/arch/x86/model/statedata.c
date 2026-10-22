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
به_بالا_STATE_DEFINE(وقفه_t, x86_KScur_وقفه VISIBLE);

به_بالا_STATE_DEFINE(وقفه_t, x86_KSPending_وقفه);

/* ==== proper read/write kernel state ==== */

x86_arch_global_state_t x86KSGlobalState[CONFIG_MAX_رقم_NODES] ALIGN(L1_CACHE_سطر_اندازه) SKIM_BSS;

/* The top level ASID table */
asid_pool_t *x86KSASIDTable[nASIDPools];

/* Current user value of the fs/gs base */
به_بالا_STATE_DEFINE(کلمه_t, x86KSCurrentFSBase);
به_بالا_STATE_DEFINE(کلمه_t, x86KSCurrentGSBase);

به_بالا_STATE_DEFINE(کلمه_t, x86_KSGPExcept_Return_مقصد);

/* ==== read-only kernel state (only written during bootstrapping) ==== */

/* Defines a translation of cpu ids from an index of our actual CPUs */
SMP_STATE_DEFINE(cpu_id_mapping_t, cpu_mapping);

/* CPU Cache Line Size */
uint32_t x86_KScache_سطر_اندازه_بیتها;

/* A valid initial FPU state, copied to every new thread. */
user_fpu_state_t x86KSnullFpuState ALIGN(MIN_FPU_ALIGNMENT);

/* Number of IOMMUs (DMA Remapping Hardware Units) */
uint32_t x86KSnumDrhu;

#اگر_تعریف_شده CONFIG_IOMMU
/* Intel VT-d Root Entry Table */
vtd_rte_t *x86_KSvtd_ریشه_جدول;
uint32_t x86KSnumIOPTLevels;
uint32_t x86KSnumIODomainIDBits;
uint32_t x86KSFirstValidIODomain;
#پایان_اگر

#اگر_تعریف_شده CONFIG_VTX
به_بالا_STATE_DEFINE(vcpu_t *, x86KSCurrentVCPU);
به_بالا_STATE_DEFINE(بنية vmxon_region, x86KSVMXOnRegion);
#پایان_اگر

#اگر_تعریف_شده CONFIG_PRINTING
uint16_t x86_KSconsole_پورت;
#پایان_اگر
#إذا defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
uint16_t x86_KSdebug_پورت;
#پایان_اگر

/* State data tracking what IRQ source is related to each
 * CPU vector */
x86_irq_state_t x86KSIRQState[maxIRQ + 1];

کلمه_t x86KSAllocatedIOPorts[رقم_IO_PORTS / CONFIG_کلمه_اندازه];
#اگر_تعریف_شده CONFIG_KERNEL_MCS
uint32_t x86KStscMhz;
uint32_t x86_KSapic_نسبت;
#پایان_اگر
