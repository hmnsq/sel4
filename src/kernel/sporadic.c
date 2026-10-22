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
ساكن مضمن کلمه_t refill_بل(sched_context_t *sc, کلمه_t شاخص)
{
    أرجع (شاخص == sc->scRefillMax - 1u) ? (0) : شاخص + 1u;
}

#که_تعریف_شوی CONFIG_PRINTING
/* for debugging */
ناکارول_شوی ساكن مضمن فراغ print_شاخص(sched_context_t *sc, کلمه_t شاخص)
{

    چاپ_کړه("index %lu, Amount: %llx, time %llx\n", شاخص, refill_شاخص(sc, شاخص)->rAmount,
           refill_شاخص(sc, شاخص)->r_وخت);
}

ناکارول_شوی ساكن مضمن فراغ refill_print(sched_context_t *sc)
{
    چاپ_کړه("Head %lu tail %lu\n", sc->sc_Refill_سر, sc->sc_Refill_لکۍ);
    کلمه_t current = sc->sc_Refill_سر;
    /* always print the head */
    print_شاخص(sc, current);

    طالما (current != sc->sc_Refill_لکۍ) {
        current = refill_بل(sc, current);
        print_شاخص(sc, current);
    }

}
#د_که_پای /* CONFIG_PRINTING */
#که_تعریف_شوی CONFIG_DEBUG_BUILD
/* check a refill queue is ordered correctly */
ساكن ناکارول_شوی bool_t refill_ordered(sched_context_t *sc)
{
    إذا (is_پړاو_Robin(sc)) {
        أرجع true;
    }

    کلمه_t current = sc->sc_Refill_سر;
    کلمه_t بل = refill_بل(sc, sc->sc_Refill_سر);

    طالما (current != sc->sc_Refill_لکۍ) {
        إذا (!(refill_شاخص(sc, current)->r_وخت + refill_شاخص(sc, current)->rAmount <= refill_شاخص(sc, بل)->r_وخت)) {
#که_تعریف_شوی CONFIG_PRINTING
            refill_print(sc);
#د_که_پای
            أرجع false;
        }
        current = بل;
        بل = refill_بل(sc, current);
    }

    أرجع true;
}

#تعریف_کړه REFILL_SANITY_پیل(sc) ticks_t مجموعه = refill_مجموعه(sc); أكد(is_پړاو_Robin(sc) || refill_ordered(sc));
#تعریف_کړه REFILL_SANITY_CHECK(sc, budget) \
    افعل { \
        أكد(refill_مجموعه(sc) == budget); أكد(is_پړاو_Robin(sc) || refill_ordered(sc)); \
    } طالما (0)

#تعریف_کړه REFILL_SANITY_پای_ټکی(sc) \
    افعل {\
        REFILL_SANITY_CHECK(sc, مجموعه);\
    } طالما (0)
#وإلا
#تعریف_کړه REFILL_SANITY_پیل(sc)
#تعریف_کړه REFILL_SANITY_CHECK(sc, budget)
#تعریف_کړه REFILL_SANITY_پای_ټکی(sc)
#د_که_پای /* CONFIG_DEBUG_BUILD */

/* compute the sum of a refill queue */
ساكن ناکارول_شوی ticks_t refill_مجموعه(sched_context_t *sc)
{
    ticks_t مجموعه_2 = refill_سر(sc)->rAmount;
    کلمه_t current = sc->sc_Refill_سر;

    طالما (current != sc->sc_Refill_لکۍ) {
        current = refill_بل(sc, current);
        مجموعه_2 += refill_شاخص(sc, current)->rAmount;
    }

    أرجع مجموعه_2;
}

/* pop head of refill queue */
ساكن مضمن refill_t refill_راوباسه_سر(sched_context_t *sc)
{
    /* queues cannot be smaller than 1 */
    أكد(!refill_single(sc));

    ناکارول_شوی کلمه_t prev_اندازه = refill_اندازه(sc);
    refill_t refill = *refill_سر(sc);
    sc->sc_Refill_سر = refill_بل(sc, sc->sc_Refill_سر);

    /* sanity */
    أكد(prev_اندازه == (refill_اندازه(sc) + 1));
    أكد(sc->sc_Refill_سر < sc->scRefillMax);
    أرجع refill;
}

/* add item to tail of refill queue */
ساكن مضمن فراغ refill_جمع_کړه_لکۍ(sched_context_t *sc, refill_t refill)
{
    /* cannot add beyond queue size */
    أكد(refill_اندازه(sc) < sc->scRefillMax);

    کلمه_t new_لکۍ = refill_بل(sc, sc->sc_Refill_لکۍ);
    sc->sc_Refill_لکۍ = new_لکۍ;
    *refill_لکۍ(sc) = refill;

    /* sanity */
    أكد(new_لکۍ < sc->scRefillMax);
}

ساكن مضمن فراغ maybe_جمع_کړه_تشه_لکۍ(sched_context_t *sc)
{
    إذا (is_پړاو_Robin(sc)) {
        /* add an empty refill - we track the used up time here */
        refill_t تشه_لکۍ = { .r_وخت = refill_سر(sc)->r_وخت };
        refill_جمع_کړه_لکۍ(sc, تشه_لکۍ);
        أكد(refill_اندازه(sc) == MIN_REFILLS);
    }
}

فراغ refill_new(sched_context_t *sc, کلمه_t max_refills, ticks_t budget, ticks_t period)
{
    sc->scPeriod = period;
    sc->sc_Refill_سر = 0;
    sc->sc_Refill_لکۍ = 0;
    sc->scRefillMax = max_refills;
    أكد(budget >= MIN_BUDGET);
    /* full budget available */
    refill_سر(sc)->rAmount = budget;
    /* budget can be used from now */
    refill_سر(sc)->r_وخت = غوټه_STATE(ks_Cur_وخت);
    maybe_جمع_کړه_تشه_لکۍ(sc);
    REFILL_SANITY_CHECK(sc, budget);
}

فراغ refill_update(sched_context_t *sc, ticks_t new_period, ticks_t new_budget, کلمه_t new_max_refills)
{

    /* refill must be initialised in order to be updated - otherwise refill_new should be used */
    أكد(sc->scRefillMax > 0);

    /* this is called on an active thread. We want to preserve the sliding window constraint -
     * so over new_period, new_budget should not be exceeded even temporarily */

    /* move the head refill to the start of the list - it's ok as we're going to truncate the
     * list to size 1 - and this way we can't be in an invalid list position once new_max_refills
     * is updated */
    *refill_شاخص(sc, 0) = *refill_سر(sc);
    sc->sc_Refill_سر = 0;
    /* truncate refill list to size 1 */
    sc->sc_Refill_لکۍ = sc->sc_Refill_سر;
    /* update max refills */
    sc->scRefillMax = new_max_refills;
    /* update period */
    sc->scPeriod = new_period;

    إذا (refill_ready(sc)) {
        refill_سر(sc)->r_وخت = غوټه_STATE(ks_Cur_وخت);
    }

    إذا (refill_سر(sc)->rAmount >= new_budget) {
        /* if the heads budget exceeds the new budget just trim it */
        refill_سر(sc)->rAmount = new_budget;
        maybe_جمع_کړه_تشه_لکۍ(sc);
    } وإلا {
        /* otherwise schedule the rest for the next period */
        refill_t new = { .rAmount = (new_budget - refill_سر(sc)->rAmount),
                         .r_وخت = refill_سر(sc)->r_وخت + new_period
                       };
        refill_جمع_کړه_لکۍ(sc, new);
    }

    REFILL_SANITY_CHECK(sc, new_budget);
}

ساكن مضمن فراغ مهالوېش_کارول_شوی(sched_context_t *sc, refill_t new)
{
    refill_t لکۍ = *refill_لکۍ(sc);
    إذا (unlikely(لکۍ.r_وخت + لکۍ.rAmount >= new.r_وخت)) {
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
        refill_لکۍ(sc)->rAmount = لکۍ.rAmount + new.rAmount;
    } وإلا إذا (likely(!refill_ډک(sc))) {
        /* Add tail normally */
        refill_جمع_کړه_لکۍ(sc, new);
    } وإلا {
        /* Delay existing tail to merge */
        refill_لکۍ(sc)->r_وخت = new.r_وخت - لکۍ.rAmount;
        refill_لکۍ(sc)->rAmount = لکۍ.rAmount + new.rAmount;
    }
}

ساكن bool_t refill_سر_overlapping(sched_context_t *sc)
{
    إذا (!refill_single(sc)) {
        refill_t سر = *refill_سر(sc);
        ticks_t لکۍ = سر.r_وخت + سر.rAmount;
        أرجع refill_شاخص(sc, refill_بل(sc, sc->sc_Refill_سر))->r_وخت <= لکۍ;
    } وإلا {
        أرجع false;
    }
}

ساكن مضمن bool_t سر_refill_overrun(sched_context_t *sc, ticks_t usage)
{
    refill_t سر = *refill_سر(sc);
    أرجع سر.rAmount <= usage && سر.r_وخت < MAX_RELEASE_وخت;
}

ساكن مضمن ticks_t charge_entire_سر_refill(sched_context_t *sc, ticks_t usage)
{
    refill_t سر = *refill_سر(sc);

    إذا (refill_single(sc)) {
        refill_سر(sc)->r_وخت = سر.r_وخت + sc->scPeriod;
    } وإلا {
        refill_t old_سر = refill_راوباسه_سر(sc);
        old_سر.r_وخت += sc->scPeriod;
        مهالوېش_کارول_شوی(sc, old_سر);
    }

    أرجع usage - سر.rAmount;
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
    طالما (سر_refill_overrun(sc, current_usage)) {
        current_usage = charge_entire_سر_refill(sc, current_usage);
    }

    أرجع current_usage;
}

ساكن مضمن bool_t سر_refill_insufficient(sched_context_t *sc)
{
    أرجع refill_سر(sc)->rAmount < MIN_BUDGET;
}

ساكن مضمن فراغ یوځای_nonoverlapping_سر_refill(sched_context_t *sc)
{
    refill_t سر = refill_راوباسه_سر(sc);
    refill_سر(sc)->rAmount += سر.rAmount;
    /* Delay head to ensure the subsequent refill doesn't end any
     * later (rather than simply combining refills). */
    refill_سر(sc)->r_وخت -= سر.rAmount;
}

فراغ refill_budget_check(ticks_t usage)
{
    sched_context_t *sc = غوټه_STATE(ksCurSC);
    أكد(!is_پړاو_Robin(sc));
    REFILL_SANITY_پیل(sc);

    usage = handle_overrun(sc, usage);

    /*
     * If the head time is still sufficiently far from the point of
     * integer overflow then the usage must be smaller than the head.
     */
    refill_t سر = *refill_سر(sc);
    إذا (usage > 0 && سر.r_وخت < MAX_RELEASE_وخت) {
        أكد(سر.rAmount > usage);
        refill_t کارول_شوی_2 = (refill_t) {
            .rAmount = usage,
            .r_وخت = سر.r_وخت + sc->scPeriod,
        };

        refill_سر(sc)->rAmount = سر.rAmount - usage;
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
        refill_سر(sc)->r_وخت = سر.r_وخت + usage;
        مهالوېش_کارول_شوی(sc, کارول_شوی_2);
    }

    /* Ensure the head refill has the minimum budget */
    طالما (سر_refill_insufficient(sc)) {
        یوځای_nonoverlapping_سر_refill(sc);
    }

    REFILL_SANITY_پای_ټکی(sc);
}

ساكن مضمن فراغ یوځای_overlapping_سر_refill(sched_context_t *sc)
{
    refill_t old_سر = refill_راوباسه_سر(sc);
    refill_سر(sc)->r_وخت = old_سر.r_وخت;
    refill_سر(sc)->rAmount += old_سر.rAmount;
}

فراغ refill_unblock_check(sched_context_t *sc)
{

    إذا (is_پړاو_Robin(sc)) {
        /* nothing to do */
        أرجع;
    }

    /* advance earliest activation time to now */
    REFILL_SANITY_پیل(sc);
    إذا (refill_ready(sc)) {
        refill_سر(sc)->r_وخت = غوټه_STATE(ks_Cur_وخت);
        غوټه_STATE(ksReprogram) = true;

        /* merge available replenishments */
        طالما (refill_سر_overlapping(sc)) {
            یوځای_overlapping_سر_refill(sc);
        }

        أكد(refill_sufficient(sc, 0));
    }
    REFILL_SANITY_پای_ټکی(sc);
}
