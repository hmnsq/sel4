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

#அந்தர்பாவ <util.h>
#அந்தர்பாவ <machine.h>
#அந்தர்பாவ <machine/profiler.h>

#ifdef CHECKPOINT_PROFILER
/* The current checkpoint value */
நிலையற்ற unsigned முழு checkpoint VISIBLE;
unsigned முழு max_checkpoint;

/* Event count for each checkpoint value */
profiler_entry_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#இல்லையெனில்
/* Number of entries the profiler currently keeps track of */
முழு profiler_இலக்கம்_entries;

/* Number of instructions the profiler could not record */
long long profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_entry_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#endif

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#ifdef CHECKPOINT_PROFILER
வெறுமை profiler_reset(வெறுமை)
{
    word_t i;

    க்கு (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].மொத்தம் = 0;
    }
    checkpoint = 0;
}

வெறுமை profiler_list(வெறுமை)
{
    unsigned முழு samples, i, மொத்தம்;

    printf("checkpoint count\n");

    samples = 0;
    மொத்தம் = 0;
    க்கு (i = 0; i <= max_checkpoint; i++) {
        எனில் (profiler_entries[i].pc != 0) {
            printf("%u %u\n", i, (unsigned முழு)profiler_entries[i].மொத்தம்);
            samples += profiler_entries[i].மொத்தம்;
            மொத்தம்++;
        }
    }

    printf("%u checkpoints, %u sample(s)\n", மொத்தம், samples);
}

வெறுமை profiler_record_sample(word_t pc)
{
    எனில் (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    எனில் (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].மொத்தம்++;
}
#இல்லையெனில்
/*
 * Reset all counters
 */
வெறுமை profiler_reset(வெறுமை)
{
    க்கு (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].மொத்தம் = 0;
    }
    profiler_இலக்கம்_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
வெறுமை profiler_list(வெறுமை)
{
    long long samples;

    /* Print header */
    printf("addr     count\n");

    /* Print out each address */
    samples = 0;
    க்கு (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        எனில் (profiler_entries[i].pc != 0) {
            printf("%x %d\n", (unsigned முழு)profiler_entries[i].pc,
                   (முழு)profiler_entries[i].மொத்தம்);
            samples += profiler_entries[i].மொத்தம்;
        }
    }

    /* Print statistics */
    printf("\n%d unique address(es), %d sample(s)\n",
           (முழு)profiler_இலக்கம்_entries, (முழு)samples);
    எனில் (profiler_dropped_instructions > 0) {
        printf("*** WARNING : %d instructions dropped\n",
               (முழு)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
வெறுமை profiler_record_sample(word_t pc)
{
    /* Number used for hashing such that the gcd of MAX_UNIQUE_INSTRUCTIONS and
     * (1 .. hashVal) is 1.
     *
     * As MAX_UNIQUE_INSTRUCTIONS is prime, this can be ensured mearly by
     * having hashVal < MAX_UNIQUE_INSTRUCTIONS. */
    மாறிலி முழு hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    word_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    word_t hash2 = ((pc >> 2) % hashVal) + 1;

    எனில் (!profiler_enabled) {
        திரும்பு;
    }

    போது (true) {

        எனில் (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].மொத்தம்++;
            நிறுத்து;

        } இல்லையெனில் எனில் (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            எனில் (profiler_இலக்கம்_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].மொத்தம் = 1;
                profiler_இலக்கம்_entries++;
                நிறுத்து;
            } இல்லையெனில் {
                /* Too many entries. Abort the record. */
                profiler_dropped_instructions++;
                நிறுத்து;
            }
        }

        /* Keep searching */
        hash += hash2;
        hash %= MAX_UNIQUE_INSTRUCTIONS;
    }
}
#endif

#endif /* CONFIG_KDB_PROFILER */
