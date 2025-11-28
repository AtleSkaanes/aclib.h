#define ACLIB_IMPLEMENTATION
#include "../aclib.h"
#include "test.h"

typedef Ac_VecDef(int) IntVec;
typedef Ac_OptDef(int) IntOpt;

int main(void)
{
    TEST_INIT;

    TEST(simple_vec_from, {
        int arr[5] = ((int[]){1, 2, 3, 4, 5});
        IntVec ivec = ac_vec_from(arr, 5);

        ASSERT_ARR_EQ(arr, ivec.items, ivec.len, "%d");
        ASSERT_EQ((size_t)5, ivec.len, "%zu");
        ASSERT_NEQ(arr, ivec.items, "%p");

        ac_vec_free(ivec);
    });

    TEST(slice_member_eql, {
        int arr[5] = ((int[]){1, 2, 3, 4, 5});
        IntVec ivec = ac_vec_from(arr, 5);

        ASSERT_ARR_EQ(ivec.items, ivec.slice.items, ivec.len, "%d");
        ASSERT_EQ(ivec.len, ivec.slice.len, "%zu");
        ASSERT_EQ(ivec.items, ivec.slice.items, "%p");

        ac_vec_free(ivec);
    });

    TEST(create_push_and_free, {
        IntVec ivec = {0};
        ASSERT_EQ((size_t)0, ivec.len, "%zu");
        ASSERT_EQ((size_t)0, ivec.cap, "%zu");
        ASSERT_EQ((void *)0, ivec.items, "%p");

        ac_vec_push(&ivec, 1);
        ASSERT_EQ(1, ivec.items[0], "%d");
        ASSERT_EQ((size_t)1, ivec.len, "%zu");
        ASSERT_GTE((size_t)1, ivec.cap, "%zu");
        ASSERT_NEQ((void *)0, ivec.items, "%p");

        ac_vec_free(ivec);
        ASSERT_EQ((size_t)0, ivec.len, "%zu");
        ASSERT_EQ((size_t)0, ivec.cap, "%zu");
        ASSERT_EQ((void *)0, ivec.items, "%p");
    });

    TEST(push_pop, {
        IntVec ivec = {0};

        ac_vec_push(&ivec, 1);
        ASSERT_EQ(1, ivec.items[0], "%d");
        ASSERT_EQ((size_t)1, ivec.len, "%zu");

        int popped = ac_vec_pop(&ivec);
        ASSERT_EQ(1, popped, "%d");
        ASSERT_EQ((size_t)0, ivec.len, "%zu");
        ac_vec_free(ivec);
    });

    TEST(pop_opt_some, {
        IntVec ivec = ac_vec_from(((int[]){1, 2, 3}), 3);
        ASSERT_EQ((size_t)3, ivec.len, "%zu");

        IntOpt opt = ac_vec_pop_opt(IntOpt, &ivec);

        ASSERT_EQ((size_t)2, ivec.len, "%zu");
        ASSERT_ARR_EQ(((int[]){1, 2}), ivec.items, ivec.len, "%d");

        ASSERT_EQ(AC_OPT_SOME, opt.tag, "{tag: %d}");
        ASSERT_EQ(3, opt.some, "%d");

        ac_vec_free(ivec);
    });

    TEST(pop_opt_empty, {
        IntVec ivec = {0};
        ASSERT_EQ((size_t)0, ivec.len, "%zu");

        IntOpt opt = ac_vec_pop_opt(IntOpt, &ivec);

        ASSERT_EQ((size_t)0, ivec.len, "%zu");

        ASSERT_EQ(AC_OPT_NONE, opt.tag, "{tag: %d}");

        ac_vec_free(ivec);
    });


    TEST(simple_append, {
        IntVec ivec = {0};

        int list[5] = ((int[]){1, 2, 3, 4, 5});

        ac_vec_append(&ivec, list, 5);
        ASSERT_EQ((size_t)5, ivec.len, "%zu");

        for (size_t i = 0; i < 5; i++)
            ASSERT_EQ(list[i], ivec.items[i], "%d");

        ac_vec_free(ivec);
    });

    TEST(use_after_append, {
        IntVec ivec = {0};

        int list[5] = ((int[]){1, 2, 3, 4, 5});

        ac_vec_append(&ivec, list, 5);
        ASSERT_EQ((size_t)5, ivec.len, "%zu");

        for (size_t i = 0; i < 5; i++)
            ASSERT_EQ(list[i], ivec.items[i], "%d");

        for (size_t i = 0; i < 5; i++)
            list[i] *= 2;

        for (size_t i = 0; i < 5; i++)
            ASSERT_NEQ(list[i], ivec.items[i], "%d");

        ac_vec_free(ivec);
    });

    TEST(unshift_shift, {
        IntVec ivec = {0};

        ac_vec_push(&ivec, 1);
        ac_vec_unshift(&ivec, 2);

        ASSERT_EQ((size_t)2, ivec.len, "%zu");
        ASSERT_EQ(2, ivec.items[0], "%d");
        ASSERT_EQ(1, ivec.items[1], "%d");

        int shifted;
        ac_vec_shift(&ivec, &shifted);

        ASSERT_EQ((size_t)1, ivec.len, "%zu");
        ASSERT_EQ(1, ivec.items[0], "%d");
        ASSERT_EQ(2, shifted, "%d");
    });

    TEST(simple_prepend, {
        IntVec ivec = {0};

        ac_vec_push(&ivec, 5);

        ac_vec_prepend(&ivec, ((int[]){1, 2, 3, 4}), 4);

        ASSERT_EQ((size_t)5, ivec.len, "%zu");

        for (size_t i = 0; i < ivec.len; i++)
            TEST_LOG("[%zu]: %d\n", i, ivec.items[i]);

        for (size_t i = 0; i < ivec.len; i++)
            ASSERT_EQ((int)i + 1, ivec.items[i], "%d");

        ac_vec_free(ivec);
    });

    TEST(ensure_cap, {
        IntVec ivec = {0};

        ASSERT_EQ((void *)0, ivec.items, "%p");

        ac_vec_ensure_cap(&ivec, 10);
        ASSERT_NEQ((void *)0, ivec.items, "%p");

        ASSERT_EQ((size_t)10, ivec.cap, "%zu");
        ASSERT_EQ((size_t)0, ivec.len, "%zu");

        ac_vec_ensure_cap(&ivec, 0);
        // An ensured cap with a lesser value than the current cap, shouldn't do anything
        ASSERT_EQ((size_t)10, ivec.cap, "%zu");
    });

    TEST(simple_foreach, {
        IntVec ivec = ac_vec_from(((int[]){1, 2, 3, 4, 5}), 5);

        int i = 1;

        AC_VEC_FOREACH(int, ivec, x)
        {
            ASSERT_EQ(i, *x, "%d");
            i++;
        }

        ac_vec_free(ivec);
    });

    TEST_END;
}
