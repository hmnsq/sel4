/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>

/* The privileged kernel mapping PD & PT */
pml4e_t x64KSKernelPML4[ਦੋਆਧਾਰੀ_ਅੰਕ(PML4_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_PML4_ਬਿੱਟ)) VISIBLE;
pdpte_t x64KSKernelPDPT[ਦੋਆਧਾਰੀ_ਅੰਕ(PDPT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(seL4_PDPTBits));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HUGE_ਪੰਨਾ
pde_t x64KSKernelPD[ਦੋਆਧਾਰੀ_ਅੰਕ(PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_Dir_ਬਿੱਟ));
#ਨਹੀਂਤਾਂ
pde_t x64KSKernelPDs[ਦੋਆਧਾਰੀ_ਅੰਕ(PDPT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)][ਦੋਆਧਾਰੀ_ਅੰਕ(PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_Dir_ਬਿੱਟ));
#ਜੇ_ਅੰਤ
pte_t x64KSKernelPT[ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_SKIM_ਖਿੜਕੀ
pml4e_t x64KSSKIMPML4[ਦੋਆਧਾਰੀ_ਅੰਕ(PML4_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_PML4_ਬਿੱਟ));
pdpte_t x64KSSKIMPDPT[ਦੋਆਧਾਰੀ_ਅੰਕ(PDPT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(seL4_PDPTBits));
pde_t x64KSSKIMPD[ਦੋਆਧਾਰੀ_ਅੰਕ(PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_Dir_ਬਿੱਟ));
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_SKIM_ਖਿੜਕੀ
ਉੱਪਰ_STATE_DEFINE(ਸ਼ਬਦ_t, x64KSCurrentUserCR3);
#ਨਹੀਂਤਾਂ
ਉੱਪਰ_STATE_DEFINE(cr3_t, x64KSCurrentCR3);
#ਜੇ_ਅੰਤ

ਸ਼ਬਦ_t x64KSIRQStack[CONFIG_MAX_ਅੰਕ_NODES][IRQ_ਢੇਰ_ਮਾਪ + 2] ALIGN(64) VISIBLE SKIM_BSS;
