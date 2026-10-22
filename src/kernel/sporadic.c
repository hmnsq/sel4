/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <object/structures.h>

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
ساكن مضمن word_t refill_next(sched_context_t *sc, word_t index)
{
    أرجع (index == sc->scRefillMax - 1u) ? (0) : index + 1u;
}

#ifdef CONFIG_PRINTING
/* for debugging */
UNUSED ساكن مضمن فراغ اطبع_index(sched_context_t *sc, word_t index)
{

    printf("index %lu, Amount: %llx, time %llx\n", index, refill_index(sc, index)->rAmount,
           refill_index(sc, index)->rTime);
}

UNUSED ساكن مضمن فراغ refill_اطبع(sched_context_t *sc)
{
    printf("Head %lu tail %lu\n", sc->scRefillHead, sc->scRefillTail);
    word_t current = sc->scRefillHead;
    /* always print the head */
    اطبع_index(sc, current);

    طالما (current != sc->scRefillTail) {
        current = refill_next(sc, current);
        اطبع_index(sc, current);
    }

}
#endif /* CONFIG_PRINTING */
#ifdef CONFIG_DEBUG_BUILD
/* check a refill queue is ordered correctly */
ساكن UNUSED bool_t refill_ordered(sched_context_t *sc)
{
    إذا (isRoundRobin(sc)) {
        أرجع true;
    }

    word_t current = sc->scRefillHead;
    word_t next = refill_next(sc, sc->scRefillHead);

    طالما (current != sc->scRefillTail) {
        إذا (!(refill_index(sc, current)->rTime + refill_index(sc, current)->rAmount <= refill_index(sc, next)->rTime)) {
#ifdef CONFIG_PRINTING
            refill_اطبع(sc);
#endif
            أرجع false;
        }
        current = next;
        next = refill_next(sc, current);
    }

    أرجع true;
}

#define REFILL_SANITY_START(sc) ticks_t _sum = refill_sum(sc); أكد(isRoundRobin(sc) || refill_ordered(sc));
#define REFILL_SANITY_CHECK(sc, budget) \
    افعل { \
        أكد(refill_sum(sc) == budget); أكد(isRoundRobin(sc) || refill_ordered(sc)); \
    } طالما (0)

#define REFILL_SANITY_END(sc) \
    افعل {\
        REFILL_SANITY_CHECK(sc, _sum);\
    } طالما (0)
#وإلا
#define REFILL_SANITY_START(sc)
#define REFILL_SANITY_CHECK(sc, budget)
#define REFILL_SANITY_END(sc)
#endif /* CONFIG_DEBUG_BUILD */

/* compute the sum of a refill queue */
ساكن UNUSED ticks_t refill_sum(sched_context_t *sc)
{
    ticks_t sum = refill_head(sc)->rAmount;
    word_t current = sc->scRefillHead;

    طالما (current != sc->scRefillTail) {
        current = refill_next(sc, current);
        sum += refill_index(sc, current)->rAmount;
    }

    أرجع sum;
}

/* pop head of refill queue */
ساكن مضمن refill_t refill_اسحب_head(sched_context_t *sc)
{
    /* queues cannot be smaller than 1 */
    أكد(!refill_single(sc));

    UNUSED word_t prev_size = refill_size(sc);
    refill_t refill = *refill_head(sc);
    sc->scRefillHead = refill_next(sc, sc->scRefillHead);

    /* sanity */
    أكد(prev_size == (refill_size(sc) + 1));
    أكد(sc->scRefillHead < sc->scRefillMax);
    أرجع refill;
}

/* add item to tail of refill queue */
ساكن مضمن فراغ refill_add_tail(sched_context_t *sc, refill_t refill)
{
    /* cannot add beyond queue size */
    أكد(refill_size(sc) < sc->scRefillMax);

    word_t new_tail = refill_next(sc, sc->scRefillTail);
    sc->scRefillTail = new_tail;
    *refill_tail(sc) = refill;

    /* sanity */
    أكد(new_tail < sc->scRefillMax);
}

ساكن مضمن فراغ maybe_add_empty_tail(sched_context_t *sc)
{
    إذا (isRoundRobin(sc)) {
        /* add an empty refill - we track the used up time here */
        refill_t empty_tail = { .rTime = refill_head(sc)->rTime };
        refill_add_tail(sc, empty_tail);
        أكد(refill_size(sc) == MIN_REFILLS);
    }
}

فراغ refill_new(sched_context_t *sc, word_t max_refills, ticks_t budget, ticks_t period)
{
    sc->scPeriod = period;
    sc->scRefillHead = 0;
    sc->scRefillTail = 0;
    sc->scRefillMax = max_refills;
    أكد(budget >= MIN_BUDGET);
    /* full budget available */
    refill_head(sc)->rAmount = budget;
    /* budget can be used from now */
    refill_head(sc)->rTime = عقدة_STATE(ksCurTime);
    maybe_add_empty_tail(sc);
    REFILL_SANITY_CHECK(sc, budget);
}

فراغ refill_update(sched_context_t *sc, ticks_t new_period, ticks_t new_budget, word_t new_max_refills)
{

    /* refill must be initialised in order to be updated - otherwise refill_new should be used */
    أكد(sc->scRefillMax > 0);

    /* this is called on an active thread. We want to preserve the sliding window constraint -
     * so over new_period, new_budget should not be exceeded even temporarily */

    /* move the head refill to the start of the list - it's ok as we're going to truncate the
     * list to size 1 - and this way we can't be in an invalid list position once new_max_refills
     * is updated */
    *refill_index(sc, 0) = *refill_head(sc);
    sc->scRefillHead = 0;
    /* truncate refill list to size 1 */
    sc->scRefillTail = sc->scRefillHead;
    /* update max refills */
    sc->scRefillMax = new_max_refills;
    /* update period */
    sc->scPeriod = new_period;

    إذا (refill_ready(sc)) {
        refill_head(sc)->rTime = عقدة_STATE(ksCurTime);
    }

    إذا (refill_head(sc)->rAmount >= new_budget) {
        /* if the heads budget exceeds the new budget just trim it */
        refill_head(sc)->rAmount = new_budget;
        maybe_add_empty_tail(sc);
    } وإلا {
        /* otherwise schedule the rest for the next period */
        refill_t new = { .rAmount = (new_budget - refill_head(sc)->rAmount),
                         .rTime = refill_head(sc)->rTime + new_period
                       };
        refill_add_tail(sc, new);
    }

    REFILL_SANITY_CHECK(sc, new_budget);
}

ساكن مضمن فراغ schedule_used(sched_context_t *sc, refill_t new)
{
    refill_t tail = *refill_tail(sc);
    إذا (unlikely(tail.rTime + tail.rAmount >= new.rTime)) {
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
        refill_tail(sc)->rAmount = tail.rAmount + new.rAmount;
    } وإلا إذا (likely(!refill_full(sc))) {
        /* Add tail normally */
        refill_add_tail(sc, new);
    } وإلا {
        /* Delay existing tail to merge */
        refill_tail(sc)->rTime = new.rTime - tail.rAmount;
        refill_tail(sc)->rAmount = tail.rAmount + new.rAmount;
    }
}

ساكن bool_t refill_head_overlapping(sched_context_t *sc)
{
    إذا (!refill_single(sc)) {
        refill_t head = *refill_head(sc);
        ticks_t tail = head.rTime + head.rAmount;
        أرجع refill_index(sc, refill_next(sc, sc->scRefillHead))->rTime <= tail;
    } وإلا {
        أرجع false;
    }
}

ساكن مضمن bool_t head_refill_overrun(sched_context_t *sc, ticks_t usage)
{
    refill_t head = *refill_head(sc);
    أرجع head.rAmount <= usage && head.rTime < MAX_RELEASE_TIME;
}

ساكن مضمن ticks_t charge_entire_head_refill(sched_context_t *sc, ticks_t usage)
{
    refill_t head = *refill_head(sc);

    إذا (refill_single(sc)) {
        refill_head(sc)->rTime = head.rTime + sc->scPeriod;
    } وإلا {
        refill_t old_head = refill_اسحب_head(sc);
        old_head.rTime += sc->scPeriod;
        schedule_used(sc, old_head);
    }

    أرجع usage - head.rAmount;
}

ساكن مضمن ticks_t handle_overrun(sched_context_t *sc, ticks_t usage)
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
    طالما (head_refill_overrun(sc, current_usage)) {
        current_usage = charge_entire_head_refill(sc, current_usage);
    }

    أرجع current_usage;
}

ساكن مضمن bool_t head_refill_insufficient(sched_context_t *sc)
{
    أرجع refill_head(sc)->rAmount < MIN_BUDGET;
}

ساكن مضمن فراغ merge_nonoverlapping_head_refill(sched_context_t *sc)
{
    refill_t head = refill_اسحب_head(sc);
    refill_head(sc)->rAmount += head.rAmount;
    /* Delay head to ensure the subsequent refill doesn't end any
     * later (rather than simply combining refills). */
    refill_head(sc)->rTime -= head.rAmount;
}

فراغ refill_budget_check(ticks_t usage)
{
    sched_context_t *sc = عقدة_STATE(ksCurSC);
    أكد(!isRoundRobin(sc));
    REFILL_SANITY_START(sc);

    usage = handle_overrun(sc, usage);

    /*
     * If the head time is still sufficiently far from the point of
     * integer overflow then the usage must be smaller than the head.
     */
    refill_t head = *refill_head(sc);
    إذا (usage > 0 && head.rTime < MAX_RELEASE_TIME) {
        أكد(head.rAmount > usage);
        refill_t used = (refill_t) {
            .rAmount = usage,
            .rTime = head.rTime + sc->scPeriod,
        };

        refill_head(sc)->rAmount = head.rAmount - usage;
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
        refill_head(sc)->rTime = head.rTime + usage;
        schedule_used(sc, used);
    }

    /* Ensure the head refill has the minimum budget */
    طالما (head_refill_insufficient(sc)) {
        merge_nonoverlapping_head_refill(sc);
    }

    REFILL_SANITY_END(sc);
}

ساكن مضمن فراغ merge_overlapping_head_refill(sched_context_t *sc)
{
    refill_t old_head = refill_اسحب_head(sc);
    refill_head(sc)->rTime = old_head.rTime;
    refill_head(sc)->rAmount += old_head.rAmount;
}

فراغ refill_unblock_check(sched_context_t *sc)
{

    إذا (isRoundRobin(sc)) {
        /* nothing to do */
        أرجع;
    }

    /* advance earliest activation time to now */
    REFILL_SANITY_START(sc);
    إذا (refill_ready(sc)) {
        refill_head(sc)->rTime = عقدة_STATE(ksCurTime);
        عقدة_STATE(ksReprogram) = true;

        /* merge available replenishments */
        طالما (refill_head_overlapping(sc)) {
            merge_overlapping_head_refill(sc);
        }

        أكد(refill_sufficient(sc, 0));
    }
    REFILL_SANITY_END(sc);
}
