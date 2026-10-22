/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <types.h>
#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <object/structures.h>

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
স্থির অন্তঃরেখ word_t refill_পরবর্তী(sched_context_t *sc, word_t সূচক)
{
    ফেরত (সূচক == sc->scRefillMax - 1u) ? (0) : সূচক + 1u;
}

#ifdef CONFIG_PRINTING
/* for debugging */
UNUSED স্থির অন্তঃরেখ শূন্য print_সূচক(sched_context_t *sc, word_t সূচক)
{

    printf("index %lu, Amount: %llx, time %llx\n", সূচক, refill_সূচক(sc, সূচক)->rAmount,
           refill_সূচক(sc, সূচক)->rTime);
}

UNUSED স্থির অন্তঃরেখ শূন্য refill_print(sched_context_t *sc)
{
    printf("Head %lu tail %lu\n", sc->sc_Refill_শীর্ষ, sc->sc_Refill_লেজ);
    word_t current = sc->sc_Refill_শীর্ষ;
    /* always print the head */
    print_সূচক(sc, current);

    যখন (current != sc->sc_Refill_লেজ) {
        current = refill_পরবর্তী(sc, current);
        print_সূচক(sc, current);
    }

}
#endif /* CONFIG_PRINTING */
#ifdef CONFIG_DEBUG_BUILD
/* check a refill queue is ordered correctly */
স্থির UNUSED bool_t refill_ordered(sched_context_t *sc)
{
    যদি (isRoundRobin(sc)) {
        ফেরত true;
    }

    word_t current = sc->sc_Refill_শীর্ষ;
    word_t পরবর্তী = refill_পরবর্তী(sc, sc->sc_Refill_শীর্ষ);

    যখন (current != sc->sc_Refill_লেজ) {
        যদি (!(refill_সূচক(sc, current)->rTime + refill_সূচক(sc, current)->rAmount <= refill_সূচক(sc, পরবর্তী)->rTime)) {
#ifdef CONFIG_PRINTING
            refill_print(sc);
#endif
            ফেরত false;
        }
        current = পরবর্তী;
        পরবর্তী = refill_পরবর্তী(sc, current);
    }

    ফেরত true;
}

#define REFILL_SANITY_START(sc) ticks_t যোগ = refill_যোগ(sc); প্রতিজ্ঞা(isRoundRobin(sc) || refill_ordered(sc));
#define REFILL_SANITY_CHECK(sc, budget) \
    করো { \
        প্রতিজ্ঞা(refill_যোগ(sc) == budget); প্রতিজ্ঞা(isRoundRobin(sc) || refill_ordered(sc)); \
    } যখন (0)

#define REFILL_SANITY_END(sc) \
    করো {\
        REFILL_SANITY_CHECK(sc, যোগ);\
    } যখন (0)
#অন্যথা
#define REFILL_SANITY_START(sc)
#define REFILL_SANITY_CHECK(sc, budget)
#define REFILL_SANITY_END(sc)
#endif /* CONFIG_DEBUG_BUILD */

/* compute the sum of a refill queue */
স্থির UNUSED ticks_t refill_যোগ(sched_context_t *sc)
{
    ticks_t যোগ_2 = refill_শীর্ষ(sc)->rAmount;
    word_t current = sc->sc_Refill_শীর্ষ;

    যখন (current != sc->sc_Refill_লেজ) {
        current = refill_পরবর্তী(sc, current);
        যোগ_2 += refill_সূচক(sc, current)->rAmount;
    }

    ফেরত যোগ_2;
}

/* pop head of refill queue */
স্থির অন্তঃরেখ refill_t refill_বের_করো_শীর্ষ(sched_context_t *sc)
{
    /* queues cannot be smaller than 1 */
    প্রতিজ্ঞা(!refill_single(sc));

    UNUSED word_t পূর্ব_আকার = refill_আকার(sc);
    refill_t refill = *refill_শীর্ষ(sc);
    sc->sc_Refill_শীর্ষ = refill_পরবর্তী(sc, sc->sc_Refill_শীর্ষ);

    /* sanity */
    প্রতিজ্ঞা(পূর্ব_আকার == (refill_আকার(sc) + 1));
    প্রতিজ্ঞা(sc->sc_Refill_শীর্ষ < sc->scRefillMax);
    ফেরত refill;
}

/* add item to tail of refill queue */
স্থির অন্তঃরেখ শূন্য refill_add_লেজ(sched_context_t *sc, refill_t refill)
{
    /* cannot add beyond queue size */
    প্রতিজ্ঞা(refill_আকার(sc) < sc->scRefillMax);

    word_t new_লেজ = refill_পরবর্তী(sc, sc->sc_Refill_লেজ);
    sc->sc_Refill_লেজ = new_লেজ;
    *refill_লেজ(sc) = refill;

    /* sanity */
    প্রতিজ্ঞা(new_লেজ < sc->scRefillMax);
}

স্থির অন্তঃরেখ শূন্য maybe_add_empty_লেজ(sched_context_t *sc)
{
    যদি (isRoundRobin(sc)) {
        /* add an empty refill - we track the used up time here */
        refill_t empty_লেজ = { .rTime = refill_শীর্ষ(sc)->rTime };
        refill_add_লেজ(sc, empty_লেজ);
        প্রতিজ্ঞা(refill_আকার(sc) == MIN_REFILLS);
    }
}

শূন্য refill_new(sched_context_t *sc, word_t max_refills, ticks_t budget, ticks_t period)
{
    sc->scPeriod = period;
    sc->sc_Refill_শীর্ষ = 0;
    sc->sc_Refill_লেজ = 0;
    sc->scRefillMax = max_refills;
    প্রতিজ্ঞা(budget >= MIN_BUDGET);
    /* full budget available */
    refill_শীর্ষ(sc)->rAmount = budget;
    /* budget can be used from now */
    refill_শীর্ষ(sc)->rTime = গ্রন্থি_STATE(ksCurTime);
    maybe_add_empty_লেজ(sc);
    REFILL_SANITY_CHECK(sc, budget);
}

শূন্য refill_update(sched_context_t *sc, ticks_t new_period, ticks_t new_budget, word_t new_max_refills)
{

    /* refill must be initialised in order to be updated - otherwise refill_new should be used */
    প্রতিজ্ঞা(sc->scRefillMax > 0);

    /* this is called on an active thread. We want to preserve the sliding window constraint -
     * so over new_period, new_budget should not be exceeded even temporarily */

    /* move the head refill to the start of the list - it's ok as we're going to truncate the
     * list to size 1 - and this way we can't be in an invalid list position once new_max_refills
     * is updated */
    *refill_সূচক(sc, 0) = *refill_শীর্ষ(sc);
    sc->sc_Refill_শীর্ষ = 0;
    /* truncate refill list to size 1 */
    sc->sc_Refill_লেজ = sc->sc_Refill_শীর্ষ;
    /* update max refills */
    sc->scRefillMax = new_max_refills;
    /* update period */
    sc->scPeriod = new_period;

    যদি (refill_ready(sc)) {
        refill_শীর্ষ(sc)->rTime = গ্রন্থি_STATE(ksCurTime);
    }

    যদি (refill_শীর্ষ(sc)->rAmount >= new_budget) {
        /* if the heads budget exceeds the new budget just trim it */
        refill_শীর্ষ(sc)->rAmount = new_budget;
        maybe_add_empty_লেজ(sc);
    } অন্যথা {
        /* otherwise schedule the rest for the next period */
        refill_t new = { .rAmount = (new_budget - refill_শীর্ষ(sc)->rAmount),
                         .rTime = refill_শীর্ষ(sc)->rTime + new_period
                       };
        refill_add_লেজ(sc, new);
    }

    REFILL_SANITY_CHECK(sc, new_budget);
}

স্থির অন্তঃরেখ শূন্য schedule_used(sched_context_t *sc, refill_t new)
{
    refill_t লেজ = *refill_লেজ(sc);
    যদি (unlikely(লেজ.rTime + লেজ.rAmount >= new.rTime)) {
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
        refill_লেজ(sc)->rAmount = লেজ.rAmount + new.rAmount;
    } অন্যথা যদি (likely(!refill_full(sc))) {
        /* Add tail normally */
        refill_add_লেজ(sc, new);
    } অন্যথা {
        /* Delay existing tail to merge */
        refill_লেজ(sc)->rTime = new.rTime - লেজ.rAmount;
        refill_লেজ(sc)->rAmount = লেজ.rAmount + new.rAmount;
    }
}

স্থির bool_t refill_শীর্ষ_overlapping(sched_context_t *sc)
{
    যদি (!refill_single(sc)) {
        refill_t শীর্ষ = *refill_শীর্ষ(sc);
        ticks_t লেজ = শীর্ষ.rTime + শীর্ষ.rAmount;
        ফেরত refill_সূচক(sc, refill_পরবর্তী(sc, sc->sc_Refill_শীর্ষ))->rTime <= লেজ;
    } অন্যথা {
        ফেরত false;
    }
}

স্থির অন্তঃরেখ bool_t শীর্ষ_refill_overrun(sched_context_t *sc, ticks_t usage)
{
    refill_t শীর্ষ = *refill_শীর্ষ(sc);
    ফেরত শীর্ষ.rAmount <= usage && শীর্ষ.rTime < MAX_RELEASE_TIME;
}

স্থির অন্তঃরেখ ticks_t charge_entire_শীর্ষ_refill(sched_context_t *sc, ticks_t usage)
{
    refill_t শীর্ষ = *refill_শীর্ষ(sc);

    যদি (refill_single(sc)) {
        refill_শীর্ষ(sc)->rTime = শীর্ষ.rTime + sc->scPeriod;
    } অন্যথা {
        refill_t old_শীর্ষ = refill_বের_করো_শীর্ষ(sc);
        old_শীর্ষ.rTime += sc->scPeriod;
        schedule_used(sc, old_শীর্ষ);
    }

    ফেরত usage - শীর্ষ.rAmount;
}

স্থির অন্তঃরেখ ticks_t handle_overrun(sched_context_t *sc, ticks_t usage)
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
    যখন (শীর্ষ_refill_overrun(sc, current_usage)) {
        current_usage = charge_entire_শীর্ষ_refill(sc, current_usage);
    }

    ফেরত current_usage;
}

স্থির অন্তঃরেখ bool_t শীর্ষ_refill_insufficient(sched_context_t *sc)
{
    ফেরত refill_শীর্ষ(sc)->rAmount < MIN_BUDGET;
}

স্থির অন্তঃরেখ শূন্য merge_nonoverlapping_শীর্ষ_refill(sched_context_t *sc)
{
    refill_t শীর্ষ = refill_বের_করো_শীর্ষ(sc);
    refill_শীর্ষ(sc)->rAmount += শীর্ষ.rAmount;
    /* Delay head to ensure the subsequent refill doesn't end any
     * later (rather than simply combining refills). */
    refill_শীর্ষ(sc)->rTime -= শীর্ষ.rAmount;
}

শূন্য refill_budget_check(ticks_t usage)
{
    sched_context_t *sc = গ্রন্থি_STATE(ksCurSC);
    প্রতিজ্ঞা(!isRoundRobin(sc));
    REFILL_SANITY_START(sc);

    usage = handle_overrun(sc, usage);

    /*
     * If the head time is still sufficiently far from the point of
     * integer overflow then the usage must be smaller than the head.
     */
    refill_t শীর্ষ = *refill_শীর্ষ(sc);
    যদি (usage > 0 && শীর্ষ.rTime < MAX_RELEASE_TIME) {
        প্রতিজ্ঞা(শীর্ষ.rAmount > usage);
        refill_t used = (refill_t) {
            .rAmount = usage,
            .rTime = শীর্ষ.rTime + sc->scPeriod,
        };

        refill_শীর্ষ(sc)->rAmount = শীর্ষ.rAmount - usage;
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
        refill_শীর্ষ(sc)->rTime = শীর্ষ.rTime + usage;
        schedule_used(sc, used);
    }

    /* Ensure the head refill has the minimum budget */
    যখন (শীর্ষ_refill_insufficient(sc)) {
        merge_nonoverlapping_শীর্ষ_refill(sc);
    }

    REFILL_SANITY_END(sc);
}

স্থির অন্তঃরেখ শূন্য merge_overlapping_শীর্ষ_refill(sched_context_t *sc)
{
    refill_t old_শীর্ষ = refill_বের_করো_শীর্ষ(sc);
    refill_শীর্ষ(sc)->rTime = old_শীর্ষ.rTime;
    refill_শীর্ষ(sc)->rAmount += old_শীর্ষ.rAmount;
}

শূন্য refill_unblock_check(sched_context_t *sc)
{

    যদি (isRoundRobin(sc)) {
        /* nothing to do */
        ফেরত;
    }

    /* advance earliest activation time to now */
    REFILL_SANITY_START(sc);
    যদি (refill_ready(sc)) {
        refill_শীর্ষ(sc)->rTime = গ্রন্থি_STATE(ksCurTime);
        গ্রন্থি_STATE(ksReprogram) = true;

        /* merge available replenishments */
        যখন (refill_শীর্ষ_overlapping(sc)) {
            merge_overlapping_শীর্ষ_refill(sc);
        }

        প্রতিজ্ঞা(refill_sufficient(sc, 0));
    }
    REFILL_SANITY_END(sc);
}
