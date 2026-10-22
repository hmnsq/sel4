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

#અન્તર્ભાવ <util.h>
#અન્તર્ભાવ <machine.h>
#અન્તર્ભાવ <machine/profiler.h>

#ifdef CHECKPOINT_PROFILER
/* The current checkpoint value */
અસ્થિર unsigned પૂર્ણાંક checkpoint VISIBLE;
unsigned પૂર્ણાંક max_checkpoint;

/* Event count for each checkpoint value */
profiler_entry_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#નહીંતર
/* Number of entries the profiler currently keeps track of */
પૂર્ણાંક profiler_અંક_entries;

/* Number of instructions the profiler could not record */
long long profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_entry_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#endif

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#ifdef CHECKPOINT_PROFILER
શૂન્ય profiler_reset(શૂન્ય)
{
    word_t i;

    માટે (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].ગણતરી = 0;
    }
    checkpoint = 0;
}

શૂન્ય profiler_list(શૂન્ય)
{
    unsigned પૂર્ણાંક samples, i, ગણતરી;

    printf("checkpoint count\n");

    samples = 0;
    ગણતરી = 0;
    માટે (i = 0; i <= max_checkpoint; i++) {
        જો (profiler_entries[i].pc != 0) {
            printf("%u %u\n", i, (unsigned પૂર્ણાંક)profiler_entries[i].ગણતરી);
            samples += profiler_entries[i].ગણતરી;
            ગણતરી++;
        }
    }

    printf("%u checkpoints, %u sample(s)\n", ગણતરી, samples);
}

શૂન્ય profiler_record_sample(word_t pc)
{
    જો (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    જો (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].ગણતરી++;
}
#નહીંતર
/*
 * Reset all counters
 */
શૂન્ય profiler_reset(શૂન્ય)
{
    માટે (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].ગણતરી = 0;
    }
    profiler_અંક_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
શૂન્ય profiler_list(શૂન્ય)
{
    long long samples;

    /* Print header */
    printf("addr     count\n");

    /* Print out each address */
    samples = 0;
    માટે (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        જો (profiler_entries[i].pc != 0) {
            printf("%x %d\n", (unsigned પૂર્ણાંક)profiler_entries[i].pc,
                   (પૂર્ણાંક)profiler_entries[i].ગણતરી);
            samples += profiler_entries[i].ગણતરી;
        }
    }

    /* Print statistics */
    printf("\n%d unique address(es), %d sample(s)\n",
           (પૂર્ણાંક)profiler_અંક_entries, (પૂર્ણાંક)samples);
    જો (profiler_dropped_instructions > 0) {
        printf("*** WARNING : %d instructions dropped\n",
               (પૂર્ણાંક)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
શૂન્ય profiler_record_sample(word_t pc)
{
    /* Number used for hashing such that the gcd of MAX_UNIQUE_INSTRUCTIONS and
     * (1 .. hashVal) is 1.
     *
     * As MAX_UNIQUE_INSTRUCTIONS is prime, this can be ensured mearly by
     * having hashVal < MAX_UNIQUE_INSTRUCTIONS. */
    અચલ પૂર્ણાંક hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    word_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    word_t hash2 = ((pc >> 2) % hashVal) + 1;

    જો (!profiler_enabled) {
        પાછા;
    }

    જ્યારે (true) {

        જો (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].ગણતરી++;
            તોડો;

        } નહીંતર જો (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            જો (profiler_અંક_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].ગણતરી = 1;
                profiler_અંક_entries++;
                તોડો;
            } નહીંતર {
                /* Too many entries. Abort the record. */
                profiler_dropped_instructions++;
                તોડો;
            }
        }

        /* Keep searching */
        hash += hash2;
        hash %= MAX_UNIQUE_INSTRUCTIONS;
    }
}
#endif

#endif /* CONFIG_KDB_PROFILER */
