/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

/*
 * Kernel Profiler
 *
 * 2007 David Greenaway
 * 2007 Ported to seL4 C kernel by Philip Derrin
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PROFILER

#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <machine.h>
#ಅನ್ತರ್ಭಾವ <machine/profiler.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECKPOINT_PROFILER
/* The current checkpoint value */
ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ checkpoint VISIBLE;
ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ max_checkpoint;

/* Event count for each checkpoint value */
profiler_ನಮೂದು_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#ಇಲ್ಲದಿದ್ದರೆ
/* Number of entries the profiler currently keeps track of */
ಪೂರ್ಣಾಂಕ profiler_ಅಂಕೆ_entries;

/* Number of instructions the profiler could not record */
ದೀರ್ಘ ದೀರ್ಘ profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_ನಮೂದು_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#ಆದರೆ_ಅಂತ್ಯ

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECKPOINT_PROFILER
ಶೂನ್ಯ profiler_reset(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t i;

    ಗಾಗಿ (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].ಎಣಿಕೆ_2 = 0;
    }
    checkpoint = 0;
}

ಶೂನ್ಯ profiler_list(ಶೂನ್ಯ)
{
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ samples, i, ಎಣಿಕೆ_2;

    ಬರೆ("checkpoint count\n");

    samples = 0;
    ಎಣಿಕೆ_2 = 0;
    ಗಾಗಿ (i = 0; i <= max_checkpoint; i++) {
        ಆದರೆ (profiler_entries[i].pc != 0) {
            ಬರೆ("%u %u\n", i, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)profiler_entries[i].ಎಣಿಕೆ_2);
            samples += profiler_entries[i].ಎಣಿಕೆ_2;
            ಎಣಿಕೆ_2++;
        }
    }

    ಬರೆ("%u checkpoints, %u sample(s)\n", ಎಣಿಕೆ_2, samples);
}

ಶೂನ್ಯ profiler_ದಾಖಲೆ_sample(ಶಬ್ದ_t pc)
{
    ಆದರೆ (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    ಆದರೆ (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].ಎಣಿಕೆ_2++;
}
#ಇಲ್ಲದಿದ್ದರೆ
/*
 * Reset all counters
 */
ಶೂನ್ಯ profiler_reset(ಶೂನ್ಯ)
{
    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].ಎಣಿಕೆ_2 = 0;
    }
    profiler_ಅಂಕೆ_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
ಶೂನ್ಯ profiler_list(ಶೂನ್ಯ)
{
    ದೀರ್ಘ ದೀರ್ಘ samples;

    /* Print header */
    ಬರೆ("addr     count\n");

    /* Print out each address */
    samples = 0;
    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        ಆದರೆ (profiler_entries[i].pc != 0) {
            ಬರೆ("%x %d\n", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)profiler_entries[i].pc,
                   (ಪೂರ್ಣಾಂಕ)profiler_entries[i].ಎಣಿಕೆ_2);
            samples += profiler_entries[i].ಎಣಿಕೆ_2;
        }
    }

    /* Print statistics */
    ಬರೆ("\n%d unique address(es), %d sample(s)\n",
           (ಪೂರ್ಣಾಂಕ)profiler_ಅಂಕೆ_entries, (ಪೂರ್ಣಾಂಕ)samples);
    ಆದರೆ (profiler_dropped_instructions > 0) {
        ಬರೆ("*** WARNING : %d instructions dropped\n",
               (ಪೂರ್ಣಾಂಕ)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
ಶೂನ್ಯ profiler_ದಾಖಲೆ_sample(ಶಬ್ದ_t pc)
{
    /* Number used for hashing such that the gcd of MAX_UNIQUE_INSTRUCTIONS and
     * (1 .. hashVal) is 1.
     *
     * As MAX_UNIQUE_INSTRUCTIONS is prime, this can be ensured mearly by
     * having hashVal < MAX_UNIQUE_INSTRUCTIONS. */
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    ಶಬ್ದ_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    ಶಬ್ದ_t hash2 = ((pc >> 2) % hashVal) + 1;

    ಆದರೆ (!profiler_enabled) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆಗ (true) {

        ಆದರೆ (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].ಎಣಿಕೆ_2++;
            ನಿಲ್ಲಿಸು;

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            ಆದರೆ (profiler_ಅಂಕೆ_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].ಎಣಿಕೆ_2 = 1;
                profiler_ಅಂಕೆ_entries++;
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* Too many entries. Abort the record. */
                profiler_dropped_instructions++;
                ನಿಲ್ಲಿಸು;
            }
        }

        /* Keep searching */
        hash += hash2;
        hash %= MAX_UNIQUE_INSTRUCTIONS;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KDB_PROFILER */
