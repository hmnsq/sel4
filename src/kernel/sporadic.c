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
ساكن مضمن لفظ_t refill_اگلا(sched_context_t *sc, لفظ_t اشاریہ)
{
    أرجع (اشاریہ == sc->scRefillMax - 1u) ? (0) : اشاریہ + 1u;
}

#جے_تعریف_شدہ CONFIG_PRINTING
/* for debugging */
بن_ورتیا ساكن مضمن فراغ print_اشاریہ(sched_context_t *sc, لفظ_t اشاریہ)
{

    لکھو("index %lu, Amount: %llx, time %llx\n", اشاریہ, refill_اشاریہ(sc, اشاریہ)->rAmount,
           refill_اشاریہ(sc, اشاریہ)->r_ویلا);
}

بن_ورتیا ساكن مضمن فراغ refill_print(sched_context_t *sc)
{
    لکھو("Head %lu tail %lu\n", sc->sc_Refill_سر, sc->sc_Refill_پوچھ);
    لفظ_t current = sc->sc_Refill_سر;
    /* always print the head */
    print_اشاریہ(sc, current);

    طالما (current != sc->sc_Refill_پوچھ) {
        current = refill_اگلا(sc, current);
        print_اشاریہ(sc, current);
    }

}
#جے_انت /* CONFIG_PRINTING */
#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
/* check a refill queue is ordered correctly */
ساكن بن_ورتیا bool_t refill_ordered(sched_context_t *sc)
{
    إذا (is_گیڑا_Robin(sc)) {
        أرجع true;
    }

    لفظ_t current = sc->sc_Refill_سر;
    لفظ_t اگلا = refill_اگلا(sc, sc->sc_Refill_سر);

    طالما (current != sc->sc_Refill_پوچھ) {
        إذا (!(refill_اشاریہ(sc, current)->r_ویلا + refill_اشاریہ(sc, current)->rAmount <= refill_اشاریہ(sc, اگلا)->r_ویلا)) {
#جے_تعریف_شدہ CONFIG_PRINTING
            refill_print(sc);
#جے_انت
            أرجع false;
        }
        current = اگلا;
        اگلا = refill_اگلا(sc, current);
    }

    أرجع true;
}

#تعریف_کرو REFILL_SANITY_شروع(sc) ticks_t جوڑ = refill_جوڑ(sc); أكد(is_گیڑا_Robin(sc) || refill_ordered(sc));
#تعریف_کرو REFILL_SANITY_CHECK(sc, budget) \
    افعل { \
        أكد(refill_جوڑ(sc) == budget); أكد(is_گیڑا_Robin(sc) || refill_ordered(sc)); \
    } طالما (0)

#تعریف_کرو REFILL_SANITY_سرا(sc) \
    افعل {\
        REFILL_SANITY_CHECK(sc, جوڑ);\
    } طالما (0)
#وإلا
#تعریف_کرو REFILL_SANITY_شروع(sc)
#تعریف_کرو REFILL_SANITY_CHECK(sc, budget)
#تعریف_کرو REFILL_SANITY_سرا(sc)
#جے_انت /* CONFIG_DEBUG_BUILD */

/* compute the sum of a refill queue */
ساكن بن_ورتیا ticks_t refill_جوڑ(sched_context_t *sc)
{
    ticks_t جوڑ_2 = refill_سر(sc)->rAmount;
    لفظ_t current = sc->sc_Refill_سر;

    طالما (current != sc->sc_Refill_پوچھ) {
        current = refill_اگلا(sc, current);
        جوڑ_2 += refill_اشاریہ(sc, current)->rAmount;
    }

    أرجع جوڑ_2;
}

/* pop head of refill queue */
ساكن مضمن refill_t refill_نکالو_سر(sched_context_t *sc)
{
    /* queues cannot be smaller than 1 */
    أكد(!refill_single(sc));

    بن_ورتیا لفظ_t prev_ناپ = refill_ناپ(sc);
    refill_t refill = *refill_سر(sc);
    sc->sc_Refill_سر = refill_اگلا(sc, sc->sc_Refill_سر);

    /* sanity */
    أكد(prev_ناپ == (refill_ناپ(sc) + 1));
    أكد(sc->sc_Refill_سر < sc->scRefillMax);
    أرجع refill;
}

/* add item to tail of refill queue */
ساكن مضمن فراغ refill_جوڑو_پوچھ(sched_context_t *sc, refill_t refill)
{
    /* cannot add beyond queue size */
    أكد(refill_ناپ(sc) < sc->scRefillMax);

    لفظ_t new_پوچھ = refill_اگلا(sc, sc->sc_Refill_پوچھ);
    sc->sc_Refill_پوچھ = new_پوچھ;
    *refill_پوچھ(sc) = refill;

    /* sanity */
    أكد(new_پوچھ < sc->scRefillMax);
}

ساكن مضمن فراغ maybe_جوڑو_خالی_تھاواں_پوچھ(sched_context_t *sc)
{
    إذا (is_گیڑا_Robin(sc)) {
        /* add an empty refill - we track the used up time here */
        refill_t خالی_تھاواں_پوچھ = { .r_ویلا = refill_سر(sc)->r_ویلا };
        refill_جوڑو_پوچھ(sc, خالی_تھاواں_پوچھ);
        أكد(refill_ناپ(sc) == MIN_REFILLS);
    }
}

فراغ refill_new(sched_context_t *sc, لفظ_t max_refills, ticks_t budget, ticks_t period)
{
    sc->scPeriod = period;
    sc->sc_Refill_سر = 0;
    sc->sc_Refill_پوچھ = 0;
    sc->scRefillMax = max_refills;
    أكد(budget >= MIN_BUDGET);
    /* full budget available */
    refill_سر(sc)->rAmount = budget;
    /* budget can be used from now */
    refill_سر(sc)->r_ویلا = گرہ_STATE(ks_Cur_ویلا);
    maybe_جوڑو_خالی_تھاواں_پوچھ(sc);
    REFILL_SANITY_CHECK(sc, budget);
}

فراغ refill_update(sched_context_t *sc, ticks_t new_period, ticks_t new_budget, لفظ_t new_max_refills)
{

    /* refill must be initialised in order to be updated - otherwise refill_new should be used */
    أكد(sc->scRefillMax > 0);

    /* this is called on an active thread. We want to preserve the sliding window constraint -
     * so over new_period, new_budget should not be exceeded even temporarily */

    /* move the head refill to the start of the list - it's ok as we're going to truncate the
     * list to size 1 - and this way we can't be in an invalid list position once new_max_refills
     * is updated */
    *refill_اشاریہ(sc, 0) = *refill_سر(sc);
    sc->sc_Refill_سر = 0;
    /* truncate refill list to size 1 */
    sc->sc_Refill_پوچھ = sc->sc_Refill_سر;
    /* update max refills */
    sc->scRefillMax = new_max_refills;
    /* update period */
    sc->scPeriod = new_period;

    إذا (refill_ready(sc)) {
        refill_سر(sc)->r_ویلا = گرہ_STATE(ks_Cur_ویلا);
    }

    إذا (refill_سر(sc)->rAmount >= new_budget) {
        /* if the heads budget exceeds the new budget just trim it */
        refill_سر(sc)->rAmount = new_budget;
        maybe_جوڑو_خالی_تھاواں_پوچھ(sc);
    } وإلا {
        /* otherwise schedule the rest for the next period */
        refill_t new = { .rAmount = (new_budget - refill_سر(sc)->rAmount),
                         .r_ویلا = refill_سر(sc)->r_ویلا + new_period
                       };
        refill_جوڑو_پوچھ(sc, new);
    }

    REFILL_SANITY_CHECK(sc, new_budget);
}

ساكن مضمن فراغ ویلا_سارنی_ورتیا(sched_context_t *sc, refill_t new)
{
    refill_t پوچھ = *refill_پوچھ(sc);
    إذا (unlikely(پوچھ.r_ویلا + پوچھ.rAmount >= new.r_ویلا)) {
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
        refill_پوچھ(sc)->rAmount = پوچھ.rAmount + new.rAmount;
    } وإلا إذا (likely(!refill_بھریا(sc))) {
        /* Add tail normally */
        refill_جوڑو_پوچھ(sc, new);
    } وإلا {
        /* Delay existing tail to merge */
        refill_پوچھ(sc)->r_ویلا = new.r_ویلا - پوچھ.rAmount;
        refill_پوچھ(sc)->rAmount = پوچھ.rAmount + new.rAmount;
    }
}

ساكن bool_t refill_سر_overlapping(sched_context_t *sc)
{
    إذا (!refill_single(sc)) {
        refill_t سر = *refill_سر(sc);
        ticks_t پوچھ = سر.r_ویلا + سر.rAmount;
        أرجع refill_اشاریہ(sc, refill_اگلا(sc, sc->sc_Refill_سر))->r_ویلا <= پوچھ;
    } وإلا {
        أرجع false;
    }
}

ساكن مضمن bool_t سر_refill_overrun(sched_context_t *sc, ticks_t usage)
{
    refill_t سر = *refill_سر(sc);
    أرجع سر.rAmount <= usage && سر.r_ویلا < MAX_RELEASE_ویلا;
}

ساكن مضمن ticks_t charge_entire_سر_refill(sched_context_t *sc, ticks_t usage)
{
    refill_t سر = *refill_سر(sc);

    إذا (refill_single(sc)) {
        refill_سر(sc)->r_ویلا = سر.r_ویلا + sc->scPeriod;
    } وإلا {
        refill_t old_سر = refill_نکالو_سر(sc);
        old_سر.r_ویلا += sc->scPeriod;
        ویلا_سارنی_ورتیا(sc, old_سر);
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

ساكن مضمن فراغ ملاؤ_nonoverlapping_سر_refill(sched_context_t *sc)
{
    refill_t سر = refill_نکالو_سر(sc);
    refill_سر(sc)->rAmount += سر.rAmount;
    /* Delay head to ensure the subsequent refill doesn't end any
     * later (rather than simply combining refills). */
    refill_سر(sc)->r_ویلا -= سر.rAmount;
}

فراغ refill_budget_check(ticks_t usage)
{
    sched_context_t *sc = گرہ_STATE(ksCurSC);
    أكد(!is_گیڑا_Robin(sc));
    REFILL_SANITY_شروع(sc);

    usage = handle_overrun(sc, usage);

    /*
     * If the head time is still sufficiently far from the point of
     * integer overflow then the usage must be smaller than the head.
     */
    refill_t سر = *refill_سر(sc);
    إذا (usage > 0 && سر.r_ویلا < MAX_RELEASE_ویلا) {
        أكد(سر.rAmount > usage);
        refill_t ورتیا_2 = (refill_t) {
            .rAmount = usage,
            .r_ویلا = سر.r_ویلا + sc->scPeriod,
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
        refill_سر(sc)->r_ویلا = سر.r_ویلا + usage;
        ویلا_سارنی_ورتیا(sc, ورتیا_2);
    }

    /* Ensure the head refill has the minimum budget */
    طالما (سر_refill_insufficient(sc)) {
        ملاؤ_nonoverlapping_سر_refill(sc);
    }

    REFILL_SANITY_سرا(sc);
}

ساكن مضمن فراغ ملاؤ_overlapping_سر_refill(sched_context_t *sc)
{
    refill_t old_سر = refill_نکالو_سر(sc);
    refill_سر(sc)->r_ویلا = old_سر.r_ویلا;
    refill_سر(sc)->rAmount += old_سر.rAmount;
}

فراغ refill_unblock_check(sched_context_t *sc)
{

    إذا (is_گیڑا_Robin(sc)) {
        /* nothing to do */
        أرجع;
    }

    /* advance earliest activation time to now */
    REFILL_SANITY_شروع(sc);
    إذا (refill_ready(sc)) {
        refill_سر(sc)->r_ویلا = گرہ_STATE(ks_Cur_ویلا);
        گرہ_STATE(ksReprogram) = true;

        /* merge available replenishments */
        طالما (refill_سر_overlapping(sc)) {
            ملاؤ_overlapping_سر_refill(sc);
        }

        أكد(refill_sufficient(sc, 0));
    }
    REFILL_SANITY_سرا(sc);
}
