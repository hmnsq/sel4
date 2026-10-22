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
चञ्चल अचिन्हित पूर्णाङ्क checkpoint VISIBLE;
अचिन्हित पूर्णाङ्क max_checkpoint;

/* Event count for each checkpoint value */
profiler_entry_t profiler_entries[MAX_UNIQUE_CHECKPOINTS];
#अन्यथा
/* Number of entries the profiler currently keeps track of */
पूर्णाङ्क profiler_अङ्क_entries;

/* Number of instructions the profiler could not record */
दीर्घ दीर्घ profiler_dropped_instructions;

/* The instructions recorded by the profiler */
profiler_entry_t profiler_entries[MAX_UNIQUE_INSTRUCTIONS];
#endif

/* Should we be profiling the system? */
bool_t profiler_enabled VISIBLE = true;

#ifdef CHECKPOINT_PROFILER
शून्य profiler_reset(शून्य)
{
    word_t i;

    लागि (i = 0; i < MAX_UNIQUE_CHECKPOINTS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].गन्ती = 0;
    }
    checkpoint = 0;
}

शून्य profiler_list(शून्य)
{
    अचिन्हित पूर्णाङ्क samples, i, गन्ती;

    म_लिखो("checkpoint count\n");

    samples = 0;
    गन्ती = 0;
    लागि (i = 0; i <= max_checkpoint; i++) {
        यदि (profiler_entries[i].pc != 0) {
            म_लिखो("%u %u\n", i, (अचिन्हित पूर्णाङ्क)profiler_entries[i].गन्ती);
            samples += profiler_entries[i].गन्ती;
            गन्ती++;
        }
    }

    म_लिखो("%u checkpoints, %u sample(s)\n", गन्ती, samples);
}

शून्य profiler_record_sample(word_t pc)
{
    यदि (checkpoint > max_checkpoint) {
        max_checkpoint = checkpoint;
    }

    यदि (!profiler_entries[checkpoint].pc) {
        profiler_entries[checkpoint].pc = 1;
    }
    profiler_entries[checkpoint].गन्ती++;
}
#अन्यथा
/*
 * Reset all counters
 */
शून्य profiler_reset(शून्य)
{
    लागि (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        profiler_entries[i].pc = 0;
        profiler_entries[i].गन्ती = 0;
    }
    profiler_अङ्क_entries = 0;
    profiler_dropped_instructions = 0;
}

/*
 * Dump out recorded values to stdout
 */
शून्य profiler_list(शून्य)
{
    दीर्घ दीर्घ samples;

    /* Print header */
    म_लिखो("addr     count\n");

    /* Print out each address */
    samples = 0;
    लागि (word_t i = 0; i < MAX_UNIQUE_INSTRUCTIONS; i++) {
        यदि (profiler_entries[i].pc != 0) {
            म_लिखो("%x %d\n", (अचिन्हित पूर्णाङ्क)profiler_entries[i].pc,
                   (पूर्णाङ्क)profiler_entries[i].गन्ती);
            samples += profiler_entries[i].गन्ती;
        }
    }

    /* Print statistics */
    म_लिखो("\n%d unique address(es), %d sample(s)\n",
           (पूर्णाङ्क)profiler_अङ्क_entries, (पूर्णाङ्क)samples);
    यदि (profiler_dropped_instructions > 0) {
        म_लिखो("*** WARNING : %d instructions dropped\n",
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
    अचर पूर्णाङ्क hashVal = 1024;

    /* Hash optimised for valid ARM instruction addresses, which are always
     * word aligned. */
    word_t hash = (pc >> 2) % MAX_UNIQUE_INSTRUCTIONS;
    word_t hash2 = ((pc >> 2) % hashVal) + 1;

    यदि (!profiler_enabled) {
        फर्काऊ;
    }

    जबसम्म (true) {

        यदि (profiler_entries[hash].pc == pc) {

            /* Found the correct entry */
            profiler_entries[hash].गन्ती++;
            रोक;

        } अन्यथा यदि (profiler_entries[hash].pc == 0) {

            /* Found a spot for a new entry */
            यदि (profiler_अङ्क_entries < (MAX_UNIQUE_INSTRUCTIONS / 4) * 3) {
                profiler_entries[hash].pc = pc;
                profiler_entries[hash].गन्ती = 1;
                profiler_अङ्क_entries++;
                रोक;
            } अन्यथा {
                /* Too many entries. Abort the record. */
                profiler_dropped_instructions++;
                रोक;
            }
        }

        /* Keep searching */
        hash += hash2;
        hash %= MAX_UNIQUE_INSTRUCTIONS;
    }
}
#endif

#endif /* CONFIG_KDB_PROFILER */
