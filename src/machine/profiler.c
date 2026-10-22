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

#ifdef PROFILER

#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <machine.h>
#ಅನ್ತರ್ಭಾವ <machine/profiler.h>

#ifdef CHECKPOINT_PROFILER
/* The current checkpoint value */
ಚಞ್ಚಲ unsigned ಪೂರ್ಣಾಙ್ಕ checkpoint VISIBLE;
unsigned ಪೂರ್ಣಾಙ್ಕ max_checkpoint;

/* Event count for each checkpoint value */
profiler_entry_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#ಅನ್ಯಥಾ
/* Number of entries the profiler currently keeps track of */
ಪೂರ್ಣಾಙ್ಕ profiler_ಅಂಕೆ_entries;

/* Number of instructions the profiler could not record */
long long profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_entry_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#endif

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#ifdef CHECKPOINT_PROFILER
ಶೂನ್ಯ profiler_reset(ಶೂನ್ಯ)
{
    word_t i;

    ಪ್ರತಿ (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].ಎಣಿಕೆ = 0;
    }
    checkpoint = 0;
}

ಶೂನ್ಯ profiler_list(ಶೂನ್ಯ)
{
    unsigned ಪೂರ್ಣಾಙ್ಕ samples, i, ಎಣಿಕೆ;

    printf("checkpoint count\n");

    samples = 0;
    ಎಣಿಕೆ = 0;
    ಪ್ರತಿ (i = 0; i <= max_checkpoint; i++) {
        ಯದಿ (profiler_entries[i].pc != 0) {
            printf("%u %u\n", i, (unsigned ಪೂರ್ಣಾಙ್ಕ)profiler_entries[i].ಎಣಿಕೆ);
            samples += profiler_entries[i].ಎಣಿಕೆ;
            ಎಣಿಕೆ++;
        }
    }

    printf("%u checkpoints, %u sample(s)\n", ಎಣಿಕೆ, samples);
}

ಶೂನ್ಯ profiler_record_sample(word_t pc)
{
    ಯದಿ (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    ಯದಿ (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].ಎಣಿಕೆ++;
}
#ಅನ್ಯಥಾ
/*
 * Reset all counters
 */
ಶೂನ್ಯ profiler_reset(ಶೂನ್ಯ)
{
    ಪ್ರತಿ (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].ಎಣಿಕೆ = 0;
    }
    profiler_ಅಂಕೆ_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
ಶೂನ್ಯ profiler_list(ಶೂನ್ಯ)
{
    long long samples;

    /* Print header */
    printf("addr     count\n");

    /* Print out each address */
    samples = 0;
    ಪ್ರತಿ (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        ಯದಿ (profiler_entries[i].pc != 0) {
            printf("%x %d\n", (unsigned ಪೂರ್ಣಾಙ್ಕ)profiler_entries[i].pc,
                   (ಪೂರ್ಣಾಙ್ಕ)profiler_entries[i].ಎಣಿಕೆ);
            samples += profiler_entries[i].ಎಣಿಕೆ;
        }
    }

    /* Print statistics */
    printf("\n%d unique address(es), %d sample(s)\n",
           (ಪೂರ್ಣಾಙ್ಕ)profiler_ಅಂಕೆ_entries, (ಪೂರ್ಣಾಙ್ಕ)samples);
    ಯದಿ (profiler_dropped_instructions > 0) {
        printf("*** WARNING : %d instructions dropped\n",
               (ಪೂರ್ಣಾಙ್ಕ)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
ಶೂನ್ಯ profiler_record_sample(word_t pc)
{
    /* Number used for hashing such that the gcd of MAX_UNIQUE_INSTRUCTIONS and
     * (1 .. hashVal) is 1.
     *
     * As MAX_UNIQUE_INSTRUCTIONS is prime, this can be ensured mearly by
     * having hashVal < MAX_UNIQUE_INSTRUCTIONS. */
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    word_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    word_t hash2 = ((pc >> 2) % hashVal) + 1;

    ಯದಿ (!profiler_enabled) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯಾವತ್ (true) {

        ಯದಿ (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].ಎಣಿಕೆ++;
            ಭಙ್ಗ;

        } ಅನ್ಯಥಾ ಯದಿ (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            ಯದಿ (profiler_ಅಂಕೆ_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].ಎಣಿಕೆ = 1;
                profiler_ಅಂಕೆ_entries++;
                ಭಙ್ಗ;
            } ಅನ್ಯಥಾ {
                /* Too many entries. Abort the record. */
                profiler_dropped_instructions++;
                ಭಙ್ಗ;
            }
        }

        /* Keep searching */
        hash += hash2;
        hash %= MAX_UNIQUE_INSTRUCTIONS;
    }
}
#endif

#endif /* CONFIG_KDB_PROFILER */
