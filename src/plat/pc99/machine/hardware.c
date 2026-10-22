/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/apic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/pic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/ioapic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/intel-vtd.h>

BOOT_ਕੋਡ bool_t plat_ਜੋੜੋ_Devices(ਖਾਲੀ)
{
    /* remove the MSI region as poking at this is undefined and may allow for
     * the user to generate arbitrary MSI interrupts. Only need to consider
     * this if it would actually be in the user device region */
    ਜੇ (CONFIG_PADDR_USER_DEVICE_ਸਿਖਰ > 0xFFFFFFF8) {
        ਜੇ (!reserve_region((p_region_t) {
        (ਸ਼ਬਦ_t)0xFFFFFFF8, (ਸ਼ਬਦ_t)0xFFFFFFF8 + 8
        })) {
            ਵਾਪਸ false;
        }
    }
    ਵਾਪਸ true;
}

/* ============================== timer ============================== */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TSC_FREQ_RETRIES 10

BOOT_ਕੋਡ ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t measure_tsc_khz(ਖਾਲੀ)
{
    /* The frequency is repeatedly measured until the number of TSC
     * ticks in the pit wraparound interval (~50ms) fits in 32 bits.
     * On bare metal, this should succeed immediately, since x86
     * guarantees the number of TSC ticks in a second can be stored
     * in 32 bits. When running in a simulator, it's possible for
     * the emulation (or not) of the PIT and TSC to occasionally
     * allow too many TSC ticks per PIT wraparound. This loop
     * repeatedly measures the TSC ticks per PIT wraparound under
     * the expectation that it will eventually yield a sensible
     * result.
     */
    ਲਈ (ਪੂਰਨ i = 0; i < TSC_FREQ_RETRIES; i++) {

        /* read tsc */
        uint64_t old_ticks = x86_rdtsc();

        /* measure how many tsc cycles pass while PIT wraps around */
        pit_wait_wraparound();

        uint64_t new_ticks = x86_rdtsc();

        uint64_t diff = new_ticks - old_ticks;

        ਜੇ ((uint32_t)diff == diff && new_ticks > old_ticks) {
            ਵਾਪਸ (uint32_t)diff / PIT_WRAPAROUND_MS;
        }

        ਲਿਖੋ("warning: TSC frequency too high (%d retries remaining)\n",
               TSC_FREQ_RETRIES - i - 1);
    }

    fail("TSC frequency too high");

    /* should never get here */
    ਵਾਪਸ 0;
}

BOOT_ਕੋਡ uint32_t tsc_init(ਖਾਲੀ)
{

#ਜੇ CONFIG_PC99_TSC_FREQUENCY > 0
    ਵਾਪਸ CONFIG_PC99_TSC_FREQUENCY / 1000000;
#ਜੇ_ਅੰਤ

    x86_cpu_ਪਛਾਣ_ਤੱਤ_t *model_info = x86_cpuid_get_model_info();
    uint32_t valid_models[] = {
        NEHALEM_1_MODEL_ID, NEHALEM_2_MODEL_ID, NEHALEM_3_MODEL_ID,
        SANDY_BRIDGE_1_MODEL_ID, SANDY_BRIDGE_2_MODEL_ID,
        HASWELL_1_MODEL_ID, HASWELL_2_MODEL_ID, HASWELL_3_MODEL_ID, HASWELL_4_MODEL_ID,
        IVY_BRIDGE_1_MODEL_ID, IVY_BRIDGE_2_MODEL_ID, IVY_BRIDGE_3_MODEL_ID,
        /* BROADWELL_1_MODEL_ID is an Atom that doesn't support the MSR */
        BROADWELL_2_MODEL_ID, BROADWELL_3_MODEL_ID, BROADWELL_4_MODEL_ID, BROADWELL_5_MODEL_ID,
        SKYLAKE_1_MODEL_ID, SKYLAKE_2_MODEL_ID
    };

    /* try to read the frequency from the platform info MSR */
    ਜੇ (model_info->family == IA32_PREFETCHER_COMPATIBLE_FAMILIES_ID) {
        ਲਈ (ਪੂਰਨ i = 0; i < ARRAY_ਮਾਪ(valid_models); i++) {
            ਜੇ (model_info->model == valid_models[i]) {

                /* read tsc freq from the platform info msr. Under some environments such
                 * as KVM this MSR will cause a GP fault even though it should be there.
                 * As such we perform a 'safe' rdmsr, which will catch a GP fault and
                 * tells through .success whether or not one happened */
                rdmsr_ਸੁਰੱਖਿਅਤ_ਨਤੀਜਾ_t info = x86_rdmsr_ਸੁਰੱਖਿਅਤ(IA32_PLATFORM_INFO_MSR);
                ਜੇ (info.success) {
                    uint32_t ਅਨੁਪਾਤ = (((uint32_t) info.ਮੁੱਲ) & 0xFF00) >> 8u;
                    /* Ignore hardware that claims a tsc frequency of zero */
                    ਜੇ (ਅਨੁਪਾਤ != 0) {
                        /* Convert to MHz */
                        ਜੇ (model_info->model == NEHALEM_1_MODEL_ID ||
                            model_info->model == NEHALEM_2_MODEL_ID ||
                            model_info->model == NEHALEM_3_MODEL_ID) {
                            ਵਾਪਸ ਅਨੁਪਾਤ * 13333u / 100u;
                        } ਨਹੀਂਤਾਂ {
                            ਵਾਪਸ ਅਨੁਪਾਤ * 100u;
                        }
                    }
                }
                /* We just found the matching model, so no point continuing the search */
                ਰੋਕੋ;
            }
        }
    }

    /* otherwise use the pit to find out the tsc freq */
    pit_init();

    /* wait for pit to wraparound */
    pit_wait_wraparound();

    /* count tsc ticks per ms */
    uint32_t tsc_khz = measure_tsc_khz();

    /* finally, return mhz */
    ਵਾਪਸ tsc_khz / 1000u;
}
