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

#അന്തര്ഭാവ <util.h>
#അന്തര്ഭാവ <machine.h>
#അന്തര്ഭാവ <machine/profiler.h>

#ifdef CHECKPOINT_PROFILER
/* The current checkpoint value */
അസ്ഥിര unsigned പൂർണ്ണസംഖ്യ checkpoint VISIBLE;
unsigned പൂർണ്ണസംഖ്യ max_checkpoint;

/* Event count for each checkpoint value */
profiler_entry_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#അല്ലെങ്കിൽ
/* Number of entries the profiler currently keeps track of */
പൂർണ്ണസംഖ്യ profiler_അക്കം_entries;

/* Number of instructions the profiler could not record */
long long profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_entry_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#endif

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#ifdef CHECKPOINT_PROFILER
ശൂന്യം profiler_reset(ശൂന്യം)
{
    word_t i;

    വേണ്ടി (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].എണ്ണം = 0;
    }
    checkpoint = 0;
}

ശൂന്യം profiler_list(ശൂന്യം)
{
    unsigned പൂർണ്ണസംഖ്യ samples, i, എണ്ണം;

    printf("checkpoint count\n");

    samples = 0;
    എണ്ണം = 0;
    വേണ്ടി (i = 0; i <= max_checkpoint; i++) {
        എങ്കിൽ (profiler_entries[i].pc != 0) {
            printf("%u %u\n", i, (unsigned പൂർണ്ണസംഖ്യ)profiler_entries[i].എണ്ണം);
            samples += profiler_entries[i].എണ്ണം;
            എണ്ണം++;
        }
    }

    printf("%u checkpoints, %u sample(s)\n", എണ്ണം, samples);
}

ശൂന്യം profiler_record_sample(word_t pc)
{
    എങ്കിൽ (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    എങ്കിൽ (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].എണ്ണം++;
}
#അല്ലെങ്കിൽ
/*
 * Reset all counters
 */
ശൂന്യം profiler_reset(ശൂന്യം)
{
    വേണ്ടി (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].എണ്ണം = 0;
    }
    profiler_അക്കം_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
ശൂന്യം profiler_list(ശൂന്യം)
{
    long long samples;

    /* Print header */
    printf("addr     count\n");

    /* Print out each address */
    samples = 0;
    വേണ്ടി (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        എങ്കിൽ (profiler_entries[i].pc != 0) {
            printf("%x %d\n", (unsigned പൂർണ്ണസംഖ്യ)profiler_entries[i].pc,
                   (പൂർണ്ണസംഖ്യ)profiler_entries[i].എണ്ണം);
            samples += profiler_entries[i].എണ്ണം;
        }
    }

    /* Print statistics */
    printf("\n%d unique address(es), %d sample(s)\n",
           (പൂർണ്ണസംഖ്യ)profiler_അക്കം_entries, (പൂർണ്ണസംഖ്യ)samples);
    എങ്കിൽ (profiler_dropped_instructions > 0) {
        printf("*** WARNING : %d instructions dropped\n",
               (പൂർണ്ണസംഖ്യ)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
ശൂന്യം profiler_record_sample(word_t pc)
{
    /* Number used for hashing such that the gcd of MAX_UNIQUE_INSTRUCTIONS and
     * (1 .. hashVal) is 1.
     *
     * As MAX_UNIQUE_INSTRUCTIONS is prime, this can be ensured mearly by
     * having hashVal < MAX_UNIQUE_INSTRUCTIONS. */
    സ്ഥിരം പൂർണ്ണസംഖ്യ hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    word_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    word_t hash2 = ((pc >> 2) % hashVal) + 1;

    എങ്കിൽ (!profiler_enabled) {
        തിരികെ;
    }

    ആകുമ്പോൾ (true) {

        എങ്കിൽ (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].എണ്ണം++;
            നിർത്തുക;

        } അല്ലെങ്കിൽ എങ്കിൽ (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            എങ്കിൽ (profiler_അക്കം_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].എണ്ണം = 1;
                profiler_അക്കം_entries++;
                നിർത്തുക;
            } അല്ലെങ്കിൽ {
                /* Too many entries. Abort the record. */
                profiler_dropped_instructions++;
                നിർത്തുക;
            }
        }

        /* Keep searching */
        hash += hash2;
        hash %= MAX_UNIQUE_INSTRUCTIONS;
    }
}
#endif

#endif /* CONFIG_KDB_PROFILER */
