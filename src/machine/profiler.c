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

#ଅନ୍ତର୍ଭାଵ <util.h>
#ଅନ୍ତର୍ଭାଵ <machine.h>
#ଅନ୍ତର୍ଭାଵ <machine/profiler.h>

#ifdef CHECKPOINT_PROFILER
/* The current checkpoint value */
ଅସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା checkpoint VISIBLE;
unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_checkpoint;

/* Event count for each checkpoint value */
profiler_entry_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#ଅନ୍ୟଥା
/* Number of entries the profiler currently keeps track of */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା profiler_ଅଙ୍କ_entries;

/* Number of instructions the profiler could not record */
long long profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_entry_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#endif

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#ifdef CHECKPOINT_PROFILER
ଶୂନ୍ୟ profiler_reset(ଶୂନ୍ୟ)
{
    word_t i;

    ପାଇଁ (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].ଗଣନା_2 = 0;
    }
    checkpoint = 0;
}

ଶୂନ୍ୟ profiler_list(ଶୂନ୍ୟ)
{
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା samples, i, ଗଣନା_2;

    printf("checkpoint count\n");

    samples = 0;
    ଗଣନା_2 = 0;
    ପାଇଁ (i = 0; i <= max_checkpoint; i++) {
        ଯଦି (profiler_entries[i].pc != 0) {
            printf("%u %u\n", i, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)profiler_entries[i].ଗଣନା_2);
            samples += profiler_entries[i].ଗଣନା_2;
            ଗଣନା_2++;
        }
    }

    printf("%u checkpoints, %u sample(s)\n", ଗଣନା_2, samples);
}

ଶୂନ୍ୟ profiler_record_sample(word_t pc)
{
    ଯଦି (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    ଯଦି (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].ଗଣନା_2++;
}
#ଅନ୍ୟଥା
/*
 * Reset all counters
 */
ଶୂନ୍ୟ profiler_reset(ଶୂନ୍ୟ)
{
    ପାଇଁ (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].ଗଣନା_2 = 0;
    }
    profiler_ଅଙ୍କ_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
ଶୂନ୍ୟ profiler_list(ଶୂନ୍ୟ)
{
    long long samples;

    /* Print header */
    printf("addr     count\n");

    /* Print out each address */
    samples = 0;
    ପାଇଁ (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        ଯଦି (profiler_entries[i].pc != 0) {
            printf("%x %d\n", (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)profiler_entries[i].pc,
                   (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)profiler_entries[i].ଗଣନା_2);
            samples += profiler_entries[i].ଗଣନା_2;
        }
    }

    /* Print statistics */
    printf("\n%d unique address(es), %d sample(s)\n",
           (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)profiler_ଅଙ୍କ_entries, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)samples);
    ଯଦି (profiler_dropped_instructions > 0) {
        printf("*** WARNING : %d instructions dropped\n",
               (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
ଶୂନ୍ୟ profiler_record_sample(word_t pc)
{
    /* Number used for hashing such that the gcd of MAX_UNIQUE_INSTRUCTIONS and
     * (1 .. hashVal) is 1.
     *
     * As MAX_UNIQUE_INSTRUCTIONS is prime, this can be ensured mearly by
     * having hashVal < MAX_UNIQUE_INSTRUCTIONS. */
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    word_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    word_t hash2 = ((pc >> 2) % hashVal) + 1;

    ଯଦି (!profiler_enabled) {
        ଫେରନ୍ତୁ;
    }

    ଯେତେବେଳେ (true) {

        ଯଦି (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].ଗଣନା_2++;
            ଭାଙ୍ଗ;

        } ଅନ୍ୟଥା ଯଦି (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            ଯଦି (profiler_ଅଙ୍କ_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].ଗଣନା_2 = 1;
                profiler_ଅଙ୍କ_entries++;
                ଭାଙ୍ଗ;
            } ଅନ୍ୟଥା {
                /* Too many entries. Abort the record. */
                profiler_dropped_instructions++;
                ଭାଙ୍ଗ;
            }
        }

        /* Keep searching */
        hash += hash2;
        hash %= MAX_UNIQUE_INSTRUCTIONS;
    }
}
#endif

#endif /* CONFIG_KDB_PROFILER */
