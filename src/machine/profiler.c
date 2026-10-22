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

#अन्तर्भाव <util.h>
#अन्तर्भाव <machine.h>
#अन्तर्भाव <machine/profiler.h>

#ifdef CHECKPOINT_PROFILER
/* The current checkpoint value */
चञ्चल unsigned पूर्णाङ्क checkpoint VISIBLE;
unsigned पूर्णाङ्क max_checkpoint;

/* Event count for each checkpoint value */
profiler_entry_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#अण्णहा
/* Number of entries the profiler currently keeps track of */
पूर्णाङ्क profiler_अङ्कः_entries;

/* Number of instructions the profiler could not record */
long long profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_entry_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#endif

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#ifdef CHECKPOINT_PROFILER
शून्य profiler_reset(शून्य)
{
    word_t i;

    प्रति (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].गणनम् = 0;
    }
    checkpoint = 0;
}

शून्य profiler_list(शून्य)
{
    unsigned पूर्णाङ्क samples, i, गणनम्;

    printf("checkpoint count\n");

    samples = 0;
    गणनम् = 0;
    प्रति (i = 0; i <= max_checkpoint; i++) {
        जइ (profiler_entries[i].pc != 0) {
            printf("%u %u\n", i, (unsigned पूर्णाङ्क)profiler_entries[i].गणनम्);
            samples += profiler_entries[i].गणनम्;
            गणनम्++;
        }
    }

    printf("%u checkpoints, %u sample(s)\n", गणनम्, samples);
}

शून्य profiler_record_sample(word_t pc)
{
    जइ (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    जइ (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].गणनम्++;
}
#अण्णहा
/*
 * Reset all counters
 */
शून्य profiler_reset(शून्य)
{
    प्रति (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].गणनम् = 0;
    }
    profiler_अङ्कः_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
शून्य profiler_list(शून्य)
{
    long long samples;

    /* Print header */
    printf("addr     count\n");

    /* Print out each address */
    samples = 0;
    प्रति (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        जइ (profiler_entries[i].pc != 0) {
            printf("%x %d\n", (unsigned पूर्णाङ्क)profiler_entries[i].pc,
                   (पूर्णाङ्क)profiler_entries[i].गणनम्);
            samples += profiler_entries[i].गणनम्;
        }
    }

    /* Print statistics */
    printf("\n%d unique address(es), %d sample(s)\n",
           (पूर्णाङ्क)profiler_अङ्कः_entries, (पूर्णाङ्क)samples);
    जइ (profiler_dropped_instructions > 0) {
        printf("*** WARNING : %d instructions dropped\n",
               (पूर्णाङ्क)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
शून्य profiler_record_sample(word_t pc)
{
    /* Number used for hashing such that the gcd of MAX_UNIQUE_INSTRUCTIONS and
     * (1 .. hashVal) is 1.
     *
     * As MAX_UNIQUE_INSTRUCTIONS is prime, this can be ensured mearly by
     * having hashVal < MAX_UNIQUE_INSTRUCTIONS. */
    ध्रुव पूर्णाङ्क hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    word_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    word_t hash2 = ((pc >> 2) % hashVal) + 1;

    जइ (!profiler_enabled) {
        प्रत्यागम;
    }

    जाव (true) {

        जइ (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].गणनम्++;
            भङ्ग;

        } अण्णहा जइ (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            जइ (profiler_अङ्कः_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].गणनम् = 1;
                profiler_अङ्कः_entries++;
                भङ्ग;
            } अण्णहा {
                /* Too many entries. Abort the record. */
                profiler_dropped_instructions++;
                भङ्ग;
            }
        }

        /* Keep searching */
        hash += hash2;
        hash %= MAX_UNIQUE_INSTRUCTIONS;
    }
}
#endif

#endif /* CONFIG_KDB_PROFILER */
