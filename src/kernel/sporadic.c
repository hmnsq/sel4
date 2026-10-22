/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <object/structures.h>

/* functions to manage the circular buffer of
 * sporadic budget replenishments (refills for short).
 *
 * The circular buffer always has at least one item in it.
 *
 * Items are appended at the tail (the back) and
 * removed from the head (the front). Below is
 * an example of a queue with 4 items (h = head, t = tail, x = item, [] = slot)
 * and max size 8.
 *
 * [][h][x][x][t][][][]
 *
 * and another example of a queue with 5 items
 *
 * [x][t][][][][h][x][x]
 *
 * The queue has a minimum size of 1, so it is possible that h == t.
 *
 * The queue is implemented as head + tail rather than head + size as
 * we cannot use the mod operator on all architectures without accessing
 * the fpu or implementing divide.
 */

/* return the index of the next item in the refill queue */
स्थिर अन्तःस्थ word_t refill_अग्र(sched_context_t *sc, word_t सूचकांक)
{
    प्रत्यागम (सूचकांक == sc->scRefillMax - 1u) ? (0) : सूचकांक + 1u;
}

#यदि_परिभाषितम् CONFIG_PRINTING
/* for debugging */
अप्रयुक्तम् स्थिर अन्तःस्थ शून्य print_सूचकांक(sched_context_t *sc, word_t सूचकांक)
{

    प्रारूप_लिख("index %lu, Amount: %llx, time %llx\n", सूचकांक, refill_सूचकांक(sc, सूचकांक)->rAmount,
           refill_सूचकांक(sc, सूचकांक)->rTime);
}

अप्रयुक्तम् स्थिर अन्तःस्थ शून्य refill_print(sched_context_t *sc)
{
    प्रारूप_लिख("Head %lu tail %lu\n", sc->sc_Refill_शीर्ष, sc->sc_Refill_पुच्छ);
    word_t current = sc->sc_Refill_शीर्ष;
    /* always print the head */
    print_सूचकांक(sc, current);

    यावत् (current != sc->sc_Refill_पुच्छ) {
        current = refill_अग्र(sc, current);
        print_सूचकांक(sc, current);
    }

}
#यदि_अन्तः /* CONFIG_PRINTING */
#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
/* check a refill queue is ordered correctly */
स्थिर अप्रयुक्तम् bool_t refill_ordered(sched_context_t *sc)
{
    यदि (is_आवर्तनम्_Robin(sc)) {
        प्रत्यागम true;
    }

    word_t current = sc->sc_Refill_शीर्ष;
    word_t अग्र = refill_अग्र(sc, sc->sc_Refill_शीर्ष);

    यावत् (current != sc->sc_Refill_पुच्छ) {
        यदि (!(refill_सूचकांक(sc, current)->rTime + refill_सूचकांक(sc, current)->rAmount <= refill_सूचकांक(sc, अग्र)->rTime)) {
#यदि_परिभाषितम् CONFIG_PRINTING
            refill_print(sc);
#यदि_अन्तः
            प्रत्यागम false;
        }
        current = अग्र;
        अग्र = refill_अग्र(sc, current);
    }

    प्रत्यागम true;
}

#परिभाषय REFILL_SANITY_START(sc) ticks_t योग = refill_योग(sc); प्रतिज्ञा(is_आवर्तनम्_Robin(sc) || refill_ordered(sc));
#परिभाषय REFILL_SANITY_CHECK(sc, budget) \
    कुरु { \
        प्रतिज्ञा(refill_योग(sc) == budget); प्रतिज्ञा(is_आवर्तनम्_Robin(sc) || refill_ordered(sc)); \
    } यावत् (0)

#परिभाषय REFILL_SANITY_अन्तः(sc) \
    कुरु {\
        REFILL_SANITY_CHECK(sc, योग);\
    } यावत् (0)
#अन्यथा
#परिभाषय REFILL_SANITY_START(sc)
#परिभाषय REFILL_SANITY_CHECK(sc, budget)
#परिभाषय REFILL_SANITY_अन्तः(sc)
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */

/* compute the sum of a refill queue */
स्थिर अप्रयुक्तम् ticks_t refill_योग(sched_context_t *sc)
{
    ticks_t योग_2 = refill_शीर्ष(sc)->rAmount;
    word_t current = sc->sc_Refill_शीर्ष;

    यावत् (current != sc->sc_Refill_पुच्छ) {
        current = refill_अग्र(sc, current);
        योग_2 += refill_सूचकांक(sc, current)->rAmount;
    }

    प्रत्यागम योग_2;
}

/* pop head of refill queue */
स्थिर अन्तःस्थ refill_t refill_निष्कासय_शीर्ष(sched_context_t *sc)
{
    /* queues cannot be smaller than 1 */
    प्रतिज्ञा(!refill_single(sc));

    अप्रयुक्तम् word_t पूर्व_आकार = refill_आकार(sc);
    refill_t refill = *refill_शीर्ष(sc);
    sc->sc_Refill_शीर्ष = refill_अग्र(sc, sc->sc_Refill_शीर्ष);

    /* sanity */
    प्रतिज्ञा(पूर्व_आकार == (refill_आकार(sc) + 1));
    प्रतिज्ञा(sc->sc_Refill_शीर्ष < sc->scRefillMax);
    प्रत्यागम refill;
}

/* add item to tail of refill queue */
स्थिर अन्तःस्थ शून्य refill_सङ्कलय_पुच्छ(sched_context_t *sc, refill_t refill)
{
    /* cannot add beyond queue size */
    प्रतिज्ञा(refill_आकार(sc) < sc->scRefillMax);

    word_t new_पुच्छ = refill_अग्र(sc, sc->sc_Refill_पुच्छ);
    sc->sc_Refill_पुच्छ = new_पुच्छ;
    *refill_पुच्छ(sc) = refill;

    /* sanity */
    प्रतिज्ञा(new_पुच्छ < sc->scRefillMax);
}

स्थिर अन्तःस्थ शून्य maybe_सङ्कलय_रिक्तानि_पुच्छ(sched_context_t *sc)
{
    यदि (is_आवर्तनम्_Robin(sc)) {
        /* add an empty refill - we track the used up time here */
        refill_t रिक्तानि_पुच्छ = { .rTime = refill_शीर्ष(sc)->rTime };
        refill_सङ्कलय_पुच्छ(sc, रिक्तानि_पुच्छ);
        प्रतिज्ञा(refill_आकार(sc) == MIN_REFILLS);
    }
}

शून्य refill_new(sched_context_t *sc, word_t max_refills, ticks_t budget, ticks_t period)
{
    sc->scPeriod = period;
    sc->sc_Refill_शीर्ष = 0;
    sc->sc_Refill_पुच्छ = 0;
    sc->scRefillMax = max_refills;
    प्रतिज्ञा(budget >= MIN_BUDGET);
    /* full budget available */
    refill_शीर्ष(sc)->rAmount = budget;
    /* budget can be used from now */
    refill_शीर्ष(sc)->rTime = ग्रन्थिः_STATE(ksCurTime);
    maybe_सङ्कलय_रिक्तानि_पुच्छ(sc);
    REFILL_SANITY_CHECK(sc, budget);
}

शून्य refill_update(sched_context_t *sc, ticks_t new_period, ticks_t new_budget, word_t new_max_refills)
{

    /* refill must be initialised in order to be updated - otherwise refill_new should be used */
    प्रतिज्ञा(sc->scRefillMax > 0);

    /* this is called on an active thread. We want to preserve the sliding window constraint -
     * so over new_period, new_budget should not be exceeded even temporarily */

    /* move the head refill to the start of the list - it's ok as we're going to truncate the
     * list to size 1 - and this way we can't be in an invalid list position once new_max_refills
     * is updated */
    *refill_सूचकांक(sc, 0) = *refill_शीर्ष(sc);
    sc->sc_Refill_शीर्ष = 0;
    /* truncate refill list to size 1 */
    sc->sc_Refill_पुच्छ = sc->sc_Refill_शीर्ष;
    /* update max refills */
    sc->scRefillMax = new_max_refills;
    /* update period */
    sc->scPeriod = new_period;

    यदि (refill_ready(sc)) {
        refill_शीर्ष(sc)->rTime = ग्रन्थिः_STATE(ksCurTime);
    }

    यदि (refill_शीर्ष(sc)->rAmount >= new_budget) {
        /* if the heads budget exceeds the new budget just trim it */
        refill_शीर्ष(sc)->rAmount = new_budget;
        maybe_सङ्कलय_रिक्तानि_पुच्छ(sc);
    } अन्यथा {
        /* otherwise schedule the rest for the next period */
        refill_t new = { .rAmount = (new_budget - refill_शीर्ष(sc)->rAmount),
                         .rTime = refill_शीर्ष(sc)->rTime + new_period
                       };
        refill_सङ्कलय_पुच्छ(sc, new);
    }

    REFILL_SANITY_CHECK(sc, new_budget);
}

स्थिर अन्तःस्थ शून्य कालसारणी_प्रयुक्तम्(sched_context_t *sc, refill_t new)
{
    refill_t पुच्छ = *refill_पुच्छ(sc);
    यदि (unlikely(पुच्छ.rTime + पुच्छ.rAmount >= new.rTime)) {
        /* Merge overlapping or adjacent refill.
         *
         * refill_update can produce a tail refill that will overlap
         * with new refills when time is charged to the head refill.
         *
         * Preemption will cause the head refill to be partially
         * charged. When the head refill is again later charged the
         * additionally charged amount will be added where the new
         * refill ended such that they are merged here. This ensures
         * that (beyond a refill being split as it is charged
         * incrementally) a refill split is only caused by a thread
         * blocking. */
        refill_पुच्छ(sc)->rAmount = पुच्छ.rAmount + new.rAmount;
    } अन्यथा यदि (likely(!refill_पूर्णम्(sc))) {
        /* Add tail normally */
        refill_सङ्कलय_पुच्छ(sc, new);
    } अन्यथा {
        /* Delay existing tail to merge */
        refill_पुच्छ(sc)->rTime = new.rTime - पुच्छ.rAmount;
        refill_पुच्छ(sc)->rAmount = पुच्छ.rAmount + new.rAmount;
    }
}

स्थिर bool_t refill_शीर्ष_overlapping(sched_context_t *sc)
{
    यदि (!refill_single(sc)) {
        refill_t शीर्ष = *refill_शीर्ष(sc);
        ticks_t पुच्छ = शीर्ष.rTime + शीर्ष.rAmount;
        प्रत्यागम refill_सूचकांक(sc, refill_अग्र(sc, sc->sc_Refill_शीर्ष))->rTime <= पुच्छ;
    } अन्यथा {
        प्रत्यागम false;
    }
}

स्थिर अन्तःस्थ bool_t शीर्ष_refill_overrun(sched_context_t *sc, ticks_t usage)
{
    refill_t शीर्ष = *refill_शीर्ष(sc);
    प्रत्यागम शीर्ष.rAmount <= usage && शीर्ष.rTime < MAX_RELEASE_TIME;
}

स्थिर अन्तःस्थ ticks_t charge_entire_शीर्ष_refill(sched_context_t *sc, ticks_t usage)
{
    refill_t शीर्ष = *refill_शीर्ष(sc);

    यदि (refill_single(sc)) {
        refill_शीर्ष(sc)->rTime = शीर्ष.rTime + sc->scPeriod;
    } अन्यथा {
        refill_t old_शीर्ष = refill_निष्कासय_शीर्ष(sc);
        old_शीर्ष.rTime += sc->scPeriod;
        कालसारणी_प्रयुक्तम्(sc, old_शीर्ष);
    }

    प्रत्यागम usage - शीर्ष.rAmount;
}

स्थिर अन्तःस्थ ticks_t handle_overrun(sched_context_t *sc, ticks_t usage)
{
    ticks_t current_usage = usage;

    /*
     * We repeatedly charge the head refill in its entirety until we end up
     * with a partial refill, or at a point where we can't place refills
     * into the future without integer overflow.
     *
     * Verification actually requires that the current time is at least
     * 3 * MAX_PERIOD from the INT64_MAX value, so to ease relation to
     * that assertion we ensure that we never delay a refill past this
     * point in the future.
     */
    यावत् (शीर्ष_refill_overrun(sc, current_usage)) {
        current_usage = charge_entire_शीर्ष_refill(sc, current_usage);
    }

    प्रत्यागम current_usage;
}

स्थिर अन्तःस्थ bool_t शीर्ष_refill_insufficient(sched_context_t *sc)
{
    प्रत्यागम refill_शीर्ष(sc)->rAmount < MIN_BUDGET;
}

स्थिर अन्तःस्थ शून्य merge_nonoverlapping_शीर्ष_refill(sched_context_t *sc)
{
    refill_t शीर्ष = refill_निष्कासय_शीर्ष(sc);
    refill_शीर्ष(sc)->rAmount += शीर्ष.rAmount;
    /* Delay head to ensure the subsequent refill doesn't end any
     * later (rather than simply combining refills). */
    refill_शीर्ष(sc)->rTime -= शीर्ष.rAmount;
}

शून्य refill_budget_check(ticks_t usage)
{
    sched_context_t *sc = ग्रन्थिः_STATE(ksCurSC);
    प्रतिज्ञा(!is_आवर्तनम्_Robin(sc));
    REFILL_SANITY_START(sc);

    usage = handle_overrun(sc, usage);

    /*
     * If the head time is still sufficiently far from the point of
     * integer overflow then the usage must be smaller than the head.
     */
    refill_t शीर्ष = *refill_शीर्ष(sc);
    यदि (usage > 0 && शीर्ष.rTime < MAX_RELEASE_TIME) {
        प्रतिज्ञा(शीर्ष.rAmount > usage);
        refill_t प्रयुक्तम्_2 = (refill_t) {
            .rAmount = usage,
            .rTime = शीर्ष.rTime + sc->scPeriod,
        };

        refill_शीर्ष(sc)->rAmount = शीर्ष.rAmount - usage;
        /* We need to keep the head refill no more than a period before
         * the start of the tail refill. This ensures that new refills
         * are never added before the tail refill (breaking the ordered
         * invariant). This code actually keeps the head refill no more
         * than a period before the end of the tail refill (which is
         * stronger than necessary) but is what is used in the current
         * proofs. In combination with the merging behaviour of
         * schedule_used, the following will still ensure that
         * incremental charging of a refill across preemptions only
         * produces a single new refill one period in the future. */
        refill_शीर्ष(sc)->rTime = शीर्ष.rTime + usage;
        कालसारणी_प्रयुक्तम्(sc, प्रयुक्तम्_2);
    }

    /* Ensure the head refill has the minimum budget */
    यावत् (शीर्ष_refill_insufficient(sc)) {
        merge_nonoverlapping_शीर्ष_refill(sc);
    }

    REFILL_SANITY_अन्तः(sc);
}

स्थिर अन्तःस्थ शून्य merge_overlapping_शीर्ष_refill(sched_context_t *sc)
{
    refill_t old_शीर्ष = refill_निष्कासय_शीर्ष(sc);
    refill_शीर्ष(sc)->rTime = old_शीर्ष.rTime;
    refill_शीर्ष(sc)->rAmount += old_शीर्ष.rAmount;
}

शून्य refill_unblock_check(sched_context_t *sc)
{

    यदि (is_आवर्तनम्_Robin(sc)) {
        /* nothing to do */
        प्रत्यागम;
    }

    /* advance earliest activation time to now */
    REFILL_SANITY_START(sc);
    यदि (refill_ready(sc)) {
        refill_शीर्ष(sc)->rTime = ग्रन्थिः_STATE(ksCurTime);
        ग्रन्थिः_STATE(ksReprogram) = true;

        /* merge available replenishments */
        यावत् (refill_शीर्ष_overlapping(sc)) {
            merge_overlapping_शीर्ष_refill(sc);
        }

        प्रतिज्ञा(refill_sufficient(sc, 0));
    }
    REFILL_SANITY_अन्तः(sc);
}
