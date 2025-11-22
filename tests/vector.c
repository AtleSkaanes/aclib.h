#include "../aclib.h"
#include "test.h"

typedef Al_VecDef(int) IntVec;

int main(void)
{
    TEST_INIT;

    TEST(simple_vec_from, {
        int arr[5] = ((int[]){1, 2, 3, 4, 5});
        IntVec ivec = al_vec_from(arr, 5);

        ASSERT_ARR_EQ(arr, ivec.items, ivec.len, "%d");
        ASSERT_EQ((size_t)5, ivec.len, "%zu");
        ASSERT_NEQ(arr, ivec.items, "%p");

        al_vec_free(ivec);
    });

    TEST(create_push_and_free, {
        IntVec ivec = {0};
        ASSERT_EQ((size_t)0, ivec.len, "%zu");
        ASSERT_EQ((size_t)0, ivec.cap, "%zu");
        ASSERT_EQ((void *)0, ivec.items, "%p");

        al_vec_push(&ivec, 1);
        ASSERT_EQ(1, ivec.items[0], "%d");
        ASSERT_EQ((size_t)1, ivec.len, "%zu");
        ASSERT_GTE((size_t)1, ivec.cap, "%zu");
        ASSERT_NEQ((void *)0, ivec.items, "%p");

        al_vec_free(ivec);
        ASSERT_EQ((size_t)0, ivec.len, "%zu");
        ASSERT_EQ((size_t)0, ivec.cap, "%zu");
        ASSERT_EQ((void *)0, ivec.items, "%p");
    });

    TEST(push_pop, {
        IntVec ivec = {0};

        al_vec_push(&ivec, 1);
        ASSERT_EQ(1, ivec.items[0], "%d");
        ASSERT_EQ((size_t)1, ivec.len, "%zu");

        int popped = al_vec_pop(&ivec);
        ASSERT_EQ(1, popped, "%d");
        ASSERT_EQ((size_t)0, ivec.len, "%zu");
        al_vec_free(ivec);
    });


    TEST(simple_append, {
        IntVec ivec = {0};

        int list[5] = ((int[]){1, 2, 3, 4, 5});

        al_vec_append(&ivec, list, 5);
        ASSERT_EQ((size_t)5, ivec.len, "%zu");

        for (size_t i = 0; i < 5; i++)
            ASSERT_EQ(list[i], ivec.items[i], "%d");

        al_vec_free(ivec);
    });

    TEST(use_after_append, {
        IntVec ivec = {0};

        int list[5] = ((int[]){1, 2, 3, 4, 5});

        al_vec_append(&ivec, list, 5);
        ASSERT_EQ((size_t)5, ivec.len, "%zu");

        for (size_t i = 0; i < 5; i++)
            ASSERT_EQ(list[i], ivec.items[i], "%d");

        for (size_t i = 0; i < 5; i++)
            list[i] *= 2;

        for (size_t i = 0; i < 5; i++)
            ASSERT_NEQ(list[i], ivec.items[i], "%d");

        al_vec_free(ivec);
    });

    TEST(unshift_shift, {
        IntVec ivec = {0};

        al_vec_push(&ivec, 1);
        al_vec_unshift(&ivec, 2);

        ASSERT_EQ((size_t)2, ivec.len, "%zu");
        ASSERT_EQ(2, ivec.items[0], "%d");
        ASSERT_EQ(1, ivec.items[1], "%d");

        int shifted;
        al_vec_shift(&ivec, &shifted);

        ASSERT_EQ((size_t)1, ivec.len, "%zu");
        ASSERT_EQ(1, ivec.items[0], "%d");
        ASSERT_EQ(2, shifted, "%d");
    });

    TEST(simple_prepend, {
        IntVec ivec = {0};

        al_vec_push(&ivec, 5);

        al_vec_prepend(&ivec, ((int[]){1, 2, 3, 4}), 4);

        ASSERT_EQ((size_t)5, ivec.len, "%zu");

        for (size_t i = 0; i < ivec.len; i++)
            TEST_LOG("[%zu]: %d\n", i, ivec.items[i]);

        for (size_t i = 0; i < ivec.len; i++)
            ASSERT_EQ((int)i + 1, ivec.items[i], "%d");

        al_vec_free(ivec);
    });

    TEST(ensure_cap, {
        IntVec ivec = {0};

        ASSERT_EQ((void *)0, ivec.items, "%p");

        al_vec_ensure_cap(&ivec, 10);
        ASSERT_NEQ((void *)0, ivec.items, "%p");

        ASSERT_EQ((size_t)10, ivec.cap, "%zu");
        ASSERT_EQ((size_t)0, ivec.len, "%zu");

        al_vec_ensure_cap(&ivec, 0);
        // An ensured cap with a lesser value than the current cap, shouldn't do anything
        ASSERT_EQ((size_t)10, ivec.cap, "%zu");
    });

    TEST(simple_foreach, {
        IntVec ivec = al_vec_from(((int[]){1, 2, 3, 4, 5}), 5);

        int i = 1;

        AL_VEC_FOREACH(int, ivec, x)
        {
            ASSERT_EQ(i, *x, "%d");
            i++;
        }

        al_vec_free(ivec);
    });

    TEST_END;
}
