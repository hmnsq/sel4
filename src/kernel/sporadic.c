/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <types.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <object/structures.h>

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
స్థిర అంతఃవరుస మాట_t refill_తదుపరి(sched_context_t *sc, మాట_t సూచీ)
{
    తిరిగి (సూచీ == sc->scRefillMax - 1u) ? (0) : సూచీ + 1u;
}

#నిర్వచితమైతే CONFIG_PRINTING
/* for debugging */
వాడని స్థిర అంతఃవరుస శూన్య print_సూచీ(sched_context_t *sc, మాట_t సూచీ)
{

    రాయి("index %lu, Amount: %llx, time %llx\n", సూచీ, refill_సూచీ(sc, సూచీ)->rAmount,
           refill_సూచీ(sc, సూచీ)->r_సమయం);
}

వాడని స్థిర అంతఃవరుస శూన్య refill_print(sched_context_t *sc)
{
    రాయి("Head %lu tail %lu\n", sc->sc_Refill_తల, sc->sc_Refill_తోక);
    మాట_t current = sc->sc_Refill_తల;
    /* always print the head */
    print_సూచీ(sc, current);

    అయితే (current != sc->sc_Refill_తోక) {
        current = refill_తదుపరి(sc, current);
        print_సూచీ(sc, current);
    }

}
#అయితే_ముగింపు /* CONFIG_PRINTING */
#నిర్వచితమైతే CONFIG_DEBUG_BUILD
/* check a refill queue is ordered correctly */
స్థిర వాడని bool_t refill_ordered(sched_context_t *sc)
{
    ఒకవేళ (is_విడత_Robin(sc)) {
        తిరిగి true;
    }

    మాట_t current = sc->sc_Refill_తల;
    మాట_t తదుపరి = refill_తదుపరి(sc, sc->sc_Refill_తల);

    అయితే (current != sc->sc_Refill_తోక) {
        ఒకవేళ (!(refill_సూచీ(sc, current)->r_సమయం + refill_సూచీ(sc, current)->rAmount <= refill_సూచీ(sc, తదుపరి)->r_సమయం)) {
#నిర్వచితమైతే CONFIG_PRINTING
            refill_print(sc);
#అయితే_ముగింపు
            తిరిగి false;
        }
        current = తదుపరి;
        తదుపరి = refill_తదుపరి(sc, current);
    }

    తిరిగి true;
}

#నిర్వచించు REFILL_SANITY_ప్రారంభం(sc) ticks_t కూడిక = refill_కూడిక(sc); ప్రతిజ్ఞా(is_విడత_Robin(sc) || refill_ordered(sc));
#నిర్వచించు REFILL_SANITY_CHECK(sc, budget) \
    చేయి { \
        ప్రతిజ్ఞా(refill_కూడిక(sc) == budget); ప్రతిజ్ఞా(is_విడత_Robin(sc) || refill_ordered(sc)); \
    } అయితే (0)

#నిర్వచించు REFILL_SANITY_చివర(sc) \
    చేయి {\
        REFILL_SANITY_CHECK(sc, కూడిక);\
    } అయితే (0)
#లేకపోతే
#నిర్వచించు REFILL_SANITY_ప్రారంభం(sc)
#నిర్వచించు REFILL_SANITY_CHECK(sc, budget)
#నిర్వచించు REFILL_SANITY_చివర(sc)
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */

/* compute the sum of a refill queue */
స్థిర వాడని ticks_t refill_కూడిక(sched_context_t *sc)
{
    ticks_t కూడిక_2 = refill_తల(sc)->rAmount;
    మాట_t current = sc->sc_Refill_తల;

    అయితే (current != sc->sc_Refill_తోక) {
        current = refill_తదుపరి(sc, current);
        కూడిక_2 += refill_సూచీ(sc, current)->rAmount;
    }

    తిరిగి కూడిక_2;
}

/* pop head of refill queue */
స్థిర అంతఃవరుస refill_t refill_తీయి_తల(sched_context_t *sc)
{
    /* queues cannot be smaller than 1 */
    ప్రతిజ్ఞా(!refill_single(sc));

    వాడని మాట_t prev_కొలత = refill_కొలత(sc);
    refill_t refill = *refill_తల(sc);
    sc->sc_Refill_తల = refill_తదుపరి(sc, sc->sc_Refill_తల);

    /* sanity */
    ప్రతిజ్ఞా(prev_కొలత == (refill_కొలత(sc) + 1));
    ప్రతిజ్ఞా(sc->sc_Refill_తల < sc->scRefillMax);
    తిరిగి refill;
}

/* add item to tail of refill queue */
స్థిర అంతఃవరుస శూన్య refill_కూడు_తోక(sched_context_t *sc, refill_t refill)
{
    /* cannot add beyond queue size */
    ప్రతిజ్ఞా(refill_కొలత(sc) < sc->scRefillMax);

    మాట_t new_తోక = refill_తదుపరి(sc, sc->sc_Refill_తోక);
    sc->sc_Refill_తోక = new_తోక;
    *refill_తోక(sc) = refill;

    /* sanity */
    ప్రతిజ్ఞా(new_తోక < sc->scRefillMax);
}

స్థిర అంతఃవరుస శూన్య maybe_కూడు_ఖాళీలు_తోక(sched_context_t *sc)
{
    ఒకవేళ (is_విడత_Robin(sc)) {
        /* add an empty refill - we track the used up time here */
        refill_t ఖాళీలు_తోక = { .r_సమయం = refill_తల(sc)->r_సమయం };
        refill_కూడు_తోక(sc, ఖాళీలు_తోక);
        ప్రతిజ్ఞా(refill_కొలత(sc) == MIN_REFILLS);
    }
}

శూన్య refill_new(sched_context_t *sc, మాట_t max_refills, ticks_t budget, ticks_t period)
{
    sc->scPeriod = period;
    sc->sc_Refill_తల = 0;
    sc->sc_Refill_తోక = 0;
    sc->scRefillMax = max_refills;
    ప్రతిజ్ఞా(budget >= MIN_BUDGET);
    /* full budget available */
    refill_తల(sc)->rAmount = budget;
    /* budget can be used from now */
    refill_తల(sc)->r_సమయం = కణుపు_STATE(ks_Cur_సమయం);
    maybe_కూడు_ఖాళీలు_తోక(sc);
    REFILL_SANITY_CHECK(sc, budget);
}

శూన్య refill_update(sched_context_t *sc, ticks_t new_period, ticks_t new_budget, మాట_t new_max_refills)
{

    /* refill must be initialised in order to be updated - otherwise refill_new should be used */
    ప్రతిజ్ఞా(sc->scRefillMax > 0);

    /* this is called on an active thread. We want to preserve the sliding window constraint -
     * so over new_period, new_budget should not be exceeded even temporarily */

    /* move the head refill to the start of the list - it's ok as we're going to truncate the
     * list to size 1 - and this way we can't be in an invalid list position once new_max_refills
     * is updated */
    *refill_సూచీ(sc, 0) = *refill_తల(sc);
    sc->sc_Refill_తల = 0;
    /* truncate refill list to size 1 */
    sc->sc_Refill_తోక = sc->sc_Refill_తల;
    /* update max refills */
    sc->scRefillMax = new_max_refills;
    /* update period */
    sc->scPeriod = new_period;

    ఒకవేళ (refill_ready(sc)) {
        refill_తల(sc)->r_సమయం = కణుపు_STATE(ks_Cur_సమయం);
    }

    ఒకవేళ (refill_తల(sc)->rAmount >= new_budget) {
        /* if the heads budget exceeds the new budget just trim it */
        refill_తల(sc)->rAmount = new_budget;
        maybe_కూడు_ఖాళీలు_తోక(sc);
    } లేకపోతే {
        /* otherwise schedule the rest for the next period */
        refill_t new = { .rAmount = (new_budget - refill_తల(sc)->rAmount),
                         .r_సమయం = refill_తల(sc)->r_సమయం + new_period
                       };
        refill_కూడు_తోక(sc, new);
    }

    REFILL_SANITY_CHECK(sc, new_budget);
}

స్థిర అంతఃవరుస శూన్య కాలపట్టిక_వాడిన(sched_context_t *sc, refill_t new)
{
    refill_t తోక = *refill_తోక(sc);
    ఒకవేళ (unlikely(తోక.r_సమయం + తోక.rAmount >= new.r_సమయం)) {
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
        refill_తోక(sc)->rAmount = తోక.rAmount + new.rAmount;
    } లేకపోతే ఒకవేళ (likely(!refill_నిండిన(sc))) {
        /* Add tail normally */
        refill_కూడు_తోక(sc, new);
    } లేకపోతే {
        /* Delay existing tail to merge */
        refill_తోక(sc)->r_సమయం = new.r_సమయం - తోక.rAmount;
        refill_తోక(sc)->rAmount = తోక.rAmount + new.rAmount;
    }
}

స్థిర bool_t refill_తల_overlapping(sched_context_t *sc)
{
    ఒకవేళ (!refill_single(sc)) {
        refill_t తల = *refill_తల(sc);
        ticks_t తోక = తల.r_సమయం + తల.rAmount;
        తిరిగి refill_సూచీ(sc, refill_తదుపరి(sc, sc->sc_Refill_తల))->r_సమయం <= తోక;
    } లేకపోతే {
        తిరిగి false;
    }
}

స్థిర అంతఃవరుస bool_t తల_refill_overrun(sched_context_t *sc, ticks_t usage)
{
    refill_t తల = *refill_తల(sc);
    తిరిగి తల.rAmount <= usage && తల.r_సమయం < MAX_RELEASE_సమయం;
}

స్థిర అంతఃవరుస ticks_t charge_entire_తల_refill(sched_context_t *sc, ticks_t usage)
{
    refill_t తల = *refill_తల(sc);

    ఒకవేళ (refill_single(sc)) {
        refill_తల(sc)->r_సమయం = తల.r_సమయం + sc->scPeriod;
    } లేకపోతే {
        refill_t old_తల = refill_తీయి_తల(sc);
        old_తల.r_సమయం += sc->scPeriod;
        కాలపట్టిక_వాడిన(sc, old_తల);
    }

    తిరిగి usage - తల.rAmount;
}

స్థిర అంతఃవరుస ticks_t handle_overrun(sched_context_t *sc, ticks_t usage)
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
    అయితే (తల_refill_overrun(sc, current_usage)) {
        current_usage = charge_entire_తల_refill(sc, current_usage);
    }

    తిరిగి current_usage;
}

స్థిర అంతఃవరుస bool_t తల_refill_insufficient(sched_context_t *sc)
{
    తిరిగి refill_తల(sc)->rAmount < MIN_BUDGET;
}

స్థిర అంతఃవరుస శూన్య విలీనం_nonoverlapping_తల_refill(sched_context_t *sc)
{
    refill_t తల = refill_తీయి_తల(sc);
    refill_తల(sc)->rAmount += తల.rAmount;
    /* Delay head to ensure the subsequent refill doesn't end any
     * later (rather than simply combining refills). */
    refill_తల(sc)->r_సమయం -= తల.rAmount;
}

శూన్య refill_budget_check(ticks_t usage)
{
    sched_context_t *sc = కణుపు_STATE(ksCurSC);
    ప్రతిజ్ఞా(!is_విడత_Robin(sc));
    REFILL_SANITY_ప్రారంభం(sc);

    usage = handle_overrun(sc, usage);

    /*
     * If the head time is still sufficiently far from the point of
     * integer overflow then the usage must be smaller than the head.
     */
    refill_t తల = *refill_తల(sc);
    ఒకవేళ (usage > 0 && తల.r_సమయం < MAX_RELEASE_సమయం) {
        ప్రతిజ్ఞా(తల.rAmount > usage);
        refill_t వాడిన_2 = (refill_t) {
            .rAmount = usage,
            .r_సమయం = తల.r_సమయం + sc->scPeriod,
        };

        refill_తల(sc)->rAmount = తల.rAmount - usage;
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
        refill_తల(sc)->r_సమయం = తల.r_సమయం + usage;
        కాలపట్టిక_వాడిన(sc, వాడిన_2);
    }

    /* Ensure the head refill has the minimum budget */
    అయితే (తల_refill_insufficient(sc)) {
        విలీనం_nonoverlapping_తల_refill(sc);
    }

    REFILL_SANITY_చివర(sc);
}

స్థిర అంతఃవరుస శూన్య విలీనం_overlapping_తల_refill(sched_context_t *sc)
{
    refill_t old_తల = refill_తీయి_తల(sc);
    refill_తల(sc)->r_సమయం = old_తల.r_సమయం;
    refill_తల(sc)->rAmount += old_తల.rAmount;
}

శూన్య refill_unblock_check(sched_context_t *sc)
{

    ఒకవేళ (is_విడత_Robin(sc)) {
        /* nothing to do */
        తిరిగి;
    }

    /* advance earliest activation time to now */
    REFILL_SANITY_ప్రారంభం(sc);
    ఒకవేళ (refill_ready(sc)) {
        refill_తల(sc)->r_సమయం = కణుపు_STATE(ks_Cur_సమయం);
        కణుపు_STATE(ksReprogram) = true;

        /* merge available replenishments */
        అయితే (refill_తల_overlapping(sc)) {
            విలీనం_overlapping_తల_refill(sc);
        }

        ప్రతిజ్ఞా(refill_sufficient(sc, 0));
    }
    REFILL_SANITY_చివర(sc);
}
