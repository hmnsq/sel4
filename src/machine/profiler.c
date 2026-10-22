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

#অন্তর্ভাব <util.h>
#অন্তর্ভাব <machine.h>
#অন্তর্ভাব <machine/profiler.h>

#ifdef CHECKPOINT_PROFILER
/* The current checkpoint value */
চঞ্চল unsigned পূর্ণাঙ্ক checkpoint VISIBLE;
unsigned পূর্ণাঙ্ক max_checkpoint;

/* Event count for each checkpoint value */
profiler_entry_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#অন্যথা
/* Number of entries the profiler currently keeps track of */
পূর্ণাঙ্ক profiler_অঙ্ক_entries;

/* Number of instructions the profiler could not record */
long long profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_entry_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#endif

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#ifdef CHECKPOINT_PROFILER
শূন্য profiler_reset(শূন্য)
{
    word_t i;

    প্রতি (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].গুনতি = 0;
    }
    checkpoint = 0;
}

শূন্য profiler_list(শূন্য)
{
    unsigned পূর্ণাঙ্ক samples, i, গুনতি;

    printf("checkpoint count\n");

    samples = 0;
    গুনতি = 0;
    প্রতি (i = 0; i <= max_checkpoint; i++) {
        যদি (profiler_entries[i].pc != 0) {
            printf("%u %u\n", i, (unsigned পূর্ণাঙ্ক)profiler_entries[i].গুনতি);
            samples += profiler_entries[i].গুনতি;
            গুনতি++;
        }
    }

    printf("%u checkpoints, %u sample(s)\n", গুনতি, samples);
}

শূন্য profiler_record_sample(word_t pc)
{
    যদি (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    যদি (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].গুনতি++;
}
#অন্যথা
/*
 * Reset all counters
 */
শূন্য profiler_reset(শূন্য)
{
    প্রতি (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].গুনতি = 0;
    }
    profiler_অঙ্ক_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
শূন্য profiler_list(শূন্য)
{
    long long samples;

    /* Print header */
    printf("addr     count\n");

    /* Print out each address */
    samples = 0;
    প্রতি (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        যদি (profiler_entries[i].pc != 0) {
            printf("%x %d\n", (unsigned পূর্ণাঙ্ক)profiler_entries[i].pc,
                   (পূর্ণাঙ্ক)profiler_entries[i].গুনতি);
            samples += profiler_entries[i].গুনতি;
        }
    }

    /* Print statistics */
    printf("\n%d unique address(es), %d sample(s)\n",
           (পূর্ণাঙ্ক)profiler_অঙ্ক_entries, (পূর্ণাঙ্ক)samples);
    যদি (profiler_dropped_instructions > 0) {
        printf("*** WARNING : %d instructions dropped\n",
               (পূর্ণাঙ্ক)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
শূন্য profiler_record_sample(word_t pc)
{
    /* Number used for hashing such that the gcd of MAX_UNIQUE_INSTRUCTIONS and
     * (1 .. hashVal) is 1.
     *
     * As MAX_UNIQUE_INSTRUCTIONS is prime, this can be ensured mearly by
     * having hashVal < MAX_UNIQUE_INSTRUCTIONS. */
    ধ্রুব পূর্ণাঙ্ক hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    word_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    word_t hash2 = ((pc >> 2) % hashVal) + 1;

    যদি (!profiler_enabled) {
        প্রত্যাগম;
    }

    যাবত্ (true) {

        যদি (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].গুনতি++;
            ভঙ্গ;

        } অন্যথা যদি (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            যদি (profiler_অঙ্ক_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].গুনতি = 1;
                profiler_অঙ্ক_entries++;
                ভঙ্গ;
            } অন্যথা {
                /* Too many entries. Abort the record. */
                profiler_dropped_instructions++;
                ভঙ্গ;
            }
        }

        /* Keep searching */
        hash += hash2;
        hash %= MAX_UNIQUE_INSTRUCTIONS;
    }
}
#endif

#endif /* CONFIG_KDB_PROFILER */
