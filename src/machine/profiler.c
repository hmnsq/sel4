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

#अगर_घोषित PROFILER

#समावेश <util.h>
#समावेश <machine.h>
#समावेश <machine/profiler.h>

#अगर_घोषित CHECKPOINT_PROFILER
/* The current checkpoint value */
अस्थिर अचिन्हित पूर्णांक checkpoint VISIBLE;
अचिन्हित पूर्णांक max_checkpoint;

/* Event count for each checkpoint value */
profiler_entry_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#अन्यथा
/* Number of entries the profiler currently keeps track of */
पूर्णांक profiler_अंक_entries;

/* Number of instructions the profiler could not record */
दीर्घ दीर्घ profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_entry_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#पूर्ण_अगर

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#अगर_घोषित CHECKPOINT_PROFILER
व्योम profiler_reset(व्योम)
{
    word_t i;

    क्रम (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].गिनती = 0;
    }
    checkpoint = 0;
}

व्योम profiler_list(व्योम)
{
    अचिन्हित पूर्णांक samples, i, गिनती;

    म_लिखो("checkpoint count\n");

    samples = 0;
    गिनती = 0;
    क्रम (i = 0; i <= max_checkpoint; i++) {
        अगर (profiler_entries[i].pc != 0) {
            म_लिखो("%u %u\n", i, (अचिन्हित पूर्णांक)profiler_entries[i].गिनती);
            samples += profiler_entries[i].गिनती;
            गिनती++;
        }
    }

    म_लिखो("%u checkpoints, %u sample(s)\n", गिनती, samples);
}

व्योम profiler_record_sample(word_t pc)
{
    अगर (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    अगर (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].गिनती++;
}
#अन्यथा
/*
 * Reset all counters
 */
व्योम profiler_reset(व्योम)
{
    क्रम (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].गिनती = 0;
    }
    profiler_अंक_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
व्योम profiler_list(व्योम)
{
    दीर्घ दीर्घ samples;

    /* Print header */
    म_लिखो("addr     count\n");

    /* Print out each address */
    samples = 0;
    क्रम (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        अगर (profiler_entries[i].pc != 0) {
            म_लिखो("%x %d\n", (अचिन्हित पूर्णांक)profiler_entries[i].pc,
                   (पूर्णांक)profiler_entries[i].गिनती);
            samples += profiler_entries[i].गिनती;
        }
    }

    /* Print statistics */
    म_लिखो("\n%d unique address(es), %d sample(s)\n",
           (पूर्णांक)profiler_अंक_entries, (पूर्णांक)samples);
    अगर (profiler_dropped_instructions > 0) {
        म_लिखो("*** WARNING : %d instructions dropped\n",
               (पूर्णांक)profiler_dropped_instructions);
    }
}

/*
 * Record a sample
 */
व्योम profiler_record_sample(word_t pc)
{
    /* Number used for hashing such that the gcd of MAX_UNIQUE_INSTRUCTIONS and
     * (1 .. hashVal) is 1.
     *
     * As MAX_UNIQUE_INSTRUCTIONS is prime, this can be ensured mearly by
     * having hashVal < MAX_UNIQUE_INSTRUCTIONS. */
    स्थिर पूर्णांक hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    word_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    word_t hash2 = ((pc >> 2) % hashVal) + 1;

    अगर (!profiler_enabled) {
        वापस;
    }

    जबतक (true) {

        अगर (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].गिनती++;
            अवरोध;

        } अन्यथा अगर (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            अगर (profiler_अंक_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].गिनती = 1;
                profiler_अंक_entries++;
                अवरोध;
            } अन्यथा {
                /* Too many entries. Abort the record. */
                profiler_dropped_instructions++;
                अवरोध;
            }
        }

        /* Keep searching */
        hash += hash2;
        hash %= MAX_UNIQUE_INSTRUCTIONS;
    }
}
#पूर्ण_अगर

#पूर्ण_अगर /* CONFIG_KDB_PROFILER */
