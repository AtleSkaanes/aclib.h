#include <stdio.h>

#define ASSERT_FD stdin
#define ACLIB_ASSERT_FN(x) (x ? fprintf(ASSERT_FD, "T\n") : fprintf(ASSERT_FD, "F\n"))
#define ACLIB_IMPLEMENTATION
#include "../aclib.h"
#include "test.h"

bool check_assert(Al_StrSlice slc)
{
    if (slc.len < 2)
        return false;

    if (slc.chars[slc.len - 2] == 'T')
        return true;

    return false;
}

typedef enum
{
    ERROR_EXAMPLE,
} ErrorT;

typedef Al_ResDef(int, ErrorT) IntRes;
typedef Al_ResDef(char, ErrorT) CharRes;

IntRes try_or_times_2(IntRes result);

int map_times_2(int val);
char map_to_alphabet(int val);

#undef ASSERT_FD
#define ASSERT_FD assert_fd
int main(void)
{
    TEST_INIT;

    Al_StrSlice assert_buf = {0};
    FILE* assert_fd = open_memstream(&assert_buf.chars, &assert_buf.len);

    TEST(simple_construct, {
        IntRes res = (IntRes)al_res_ok(5);
        ASSERT_EQ(AL_RES_OK, res.tag, "{tag: %d}");
        ASSERT_EQ(5, res.ok, "%d");
    });

    TEST(simple_ok_unwrap, {
        IntRes res = (IntRes)al_res_ok(5);

        ASSERT_EQ(AL_RES_OK, res.tag, "{tag: %d}");
        ASSERT_EQ(5, res.ok, "%d");

        int val = al_res_unwrap(res);
        ASSERT_EQ(5, val, "%d");
    });

    TEST(simple_err_unwrap, {
        IntRes res = (IntRes)al_res_err(ERROR_EXAMPLE);

        ASSERT_EQ(AL_RES_ERR, res.tag, "{tag: %d}");
        ASSERT_EQ(ERROR_EXAMPLE, res.err, "%d");

        int val = al_res_unwrap(res);

        ASSERT(!check_assert(assert_buf));
        ASSERT_EQ(ERROR_EXAMPLE, val, "%d");
    });

    TEST(simple_unwrap_or_on_ok, {
        IntRes res = (IntRes)al_res_ok(2);

        int val = al_res_unwrap_or(res, 5);

        ASSERT_EQ(2, val, "%d");
    });

    TEST(simple_unwrap_or_on_err, {
        IntRes res = (IntRes)al_res_err(ERROR_EXAMPLE);

        int val = al_res_unwrap_or(res, 5);

        ASSERT_EQ(5, val, "%d");
    });

    TEST(simple_try_ok, {
        IntRes res = (IntRes)al_res_ok(5);

        IntRes new_res = try_or_times_2(res);

        ASSERT_EQ(AL_RES_OK, new_res.tag, "{tag: %d}");
        ASSERT_EQ(10, new_res.ok, "%d");
    });

    TEST(simple_try_err, {
        IntRes res = (IntRes)al_res_err(ERROR_EXAMPLE);

        IntRes new_res = try_or_times_2(res);

        ASSERT_EQ(AL_RES_ERR, new_res.tag, "{tag: %d}");
        ASSERT_EQ(ERROR_EXAMPLE, new_res.err, "%d");
    });

    TEST(simple_map_on_ok, {
        IntRes res = (IntRes)al_res_ok(5);
        IntRes new_res = al_res_map(IntRes, res, map_times_2);

        ASSERT_EQ(AL_RES_OK, new_res.tag, "{tag: %d}");
        ASSERT_EQ(10, new_res.ok, "%d");

        ASSERT_EQ(AL_RES_OK, res.tag, "{tag: %d}");
        ASSERT_EQ(5, res.ok, "%d");
    });

    TEST(simple_map_on_err, {
        IntRes res = (IntRes)al_res_err(ERROR_EXAMPLE);
        IntRes new_res = al_res_map(IntRes, res, map_times_2);

        ASSERT_EQ(AL_RES_ERR, new_res.tag, "{tag: %d}");
        ASSERT_EQ(ERROR_EXAMPLE, new_res.err, "%d");

        ASSERT_EQ(AL_RES_ERR, res.tag, "{tag: %d}");
        ASSERT_EQ(ERROR_EXAMPLE, res.err, "%d");
    });

    TEST(map_change_type, {
        IntRes res = (IntRes)al_res_ok(5);
        CharRes new_res = al_res_map(CharRes, res, map_to_alphabet);

        ASSERT_EQ(AL_RES_OK, new_res.tag, "{tag: %d}");
        ASSERT_EQ('F', new_res.ok, "'%c'");

        ASSERT_EQ(AL_RES_OK, res.tag, "{tag: %d}");
        ASSERT_EQ(5, res.ok, "%d");
    });

    TEST_END;
}

IntRes try_or_times_2(IntRes result)
{
    int val = al_res_try(IntRes, result);

    return (IntRes)al_res_ok(val * 2);
}

int map_times_2(int val)
{
    return val * 2;
}

char map_to_alphabet(int val)
{
    return (char)(val + 65);
}
