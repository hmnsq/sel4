/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <arch/kernel/apic.h>
#అన్తర్భావ <arch/model/statedata.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/pic.h>
#అన్తర్భావ <plat/machine/ioapic.h>
#అన్తర్భావ <plat/machine.h>

#అన్తర్భావ <plat/machine/intel-vtd.h>

BOOT_సంకేతం bool_t plat_కూడు_Devices(శూన్య)
{
    /* remove the MSI region as poking at this is undefined and may allow for
     * the user to generate arbitrary MSI interrupts. Only need to consider
     * this if it would actually be in the user device region */
    ఒకవేళ (CONFIG_PADDR_USER_DEVICE_శిఖరం > 0xFFFFFFF8) {
        ఒకవేళ (!reserve_region((p_region_t) {
        (మాట_t)0xFFFFFFF8, (మాట_t)0xFFFFFFF8 + 8
        })) {
            తిరిగి false;
        }
    }
    తిరిగి true;
}

/* ============================== timer ============================== */

#నిర్వచించు TSC_FREQ_RETRIES 10

BOOT_సంకేతం స్థిర అంతఃవరుస uint32_t measure_tsc_khz(శూన్య)
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
    కొరకు (పూర్ణాంకం i = 0; i < TSC_FREQ_RETRIES; i++) {

        /* read tsc */
        uint64_t old_ticks = x86_rdtsc();

        /* measure how many tsc cycles pass while PIT wraps around */
        pit_wait_wraparound();

        uint64_t new_ticks = x86_rdtsc();

        uint64_t diff = new_ticks - old_ticks;

        ఒకవేళ ((uint32_t)diff == diff && new_ticks > old_ticks) {
            తిరిగి (uint32_t)diff / PIT_WRAPAROUND_MS;
        }

        రాయి("warning: TSC frequency too high (%d retries remaining)\n",
               TSC_FREQ_RETRIES - i - 1);
    }

    fail("TSC frequency too high");

    /* should never get here */
    తిరిగి 0;
}

BOOT_సంకేతం uint32_t tsc_init(శూన్య)
{

#ఒకవేళ CONFIG_PC99_TSC_FREQUENCY > 0
    తిరిగి CONFIG_PC99_TSC_FREQUENCY / 1000000;
#అయితే_ముగింపు

    x86_cpu_తటస్థం_t *model_info = x86_cpuid_get_model_info();
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
    ఒకవేళ (model_info->family == IA32_PREFETCHER_COMPATIBLE_FAMILIES_ID) {
        కొరకు (పూర్ణాంకం i = 0; i < ARRAY_కొలత(valid_models); i++) {
            ఒకవేళ (model_info->model == valid_models[i]) {

                /* read tsc freq from the platform info msr. Under some environments such
                 * as KVM this MSR will cause a GP fault even though it should be there.
                 * As such we perform a 'safe' rdmsr, which will catch a GP fault and
                 * tells through .success whether or not one happened */
                rdmsr_సురక్షిత_ఫలితం_t info = x86_rdmsr_సురక్షిత(IA32_PLATFORM_INFO_MSR);
                ఒకవేళ (info.success) {
                    uint32_t నిష్పత్తి = (((uint32_t) info.విలువ) & 0xFF00) >> 8u;
                    /* Ignore hardware that claims a tsc frequency of zero */
                    ఒకవేళ (నిష్పత్తి != 0) {
                        /* Convert to MHz */
                        ఒకవేళ (model_info->model == NEHALEM_1_MODEL_ID ||
                            model_info->model == NEHALEM_2_MODEL_ID ||
                            model_info->model == NEHALEM_3_MODEL_ID) {
                            తిరిగి నిష్పత్తి * 13333u / 100u;
                        } లేకపోతే {
                            తిరిగి నిష్పత్తి * 100u;
                        }
                    }
                }
                /* We just found the matching model, so no point continuing the search */
                ఆపు;
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
    తిరిగి tsc_khz / 1000u;
}
