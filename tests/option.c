#include <stdio.h>

#define ASSERT_FD stdin
#define ACLIB_ASSERT_FN(x) (x ? fprintf(ASSERT_FD, "T\n") : fprintf(ASSERT_FD, "F\n"))
#define ACLIB_IMPLEMENTATION
#include "../aclib.h"
#include "test.h"

bool check_assert(Ac_StrSlice slc)
{
    if (slc.len < 2)
        return false;

    if (slc.chars[slc.len - 2] == 'T')
        return true;

    return false;
}

typedef Ac_OptDef(int) IntOpt;
typedef Ac_OptDef(char) CharOpt;

int map_times_2(int val);
char map_to_alphabet(int val);

#undef ASSERT_FD
#define ASSERT_FD assert_fd
int main(void)
{
    TEST_INIT;

    Ac_StrSlice assert_buf = {0};
    FILE* assert_fd = open_memstream(&assert_buf.chars, &assert_buf.len);

    TEST(simple_construct, {
        IntOpt opt = (IntOpt)ac_opt_some(5);
        ASSERT_EQ(AC_OPT_SOME, opt.tag, "{tag: %d}");
        ASSERT_EQ(5, opt.some, "%d");

        IntOpt opt_none = (IntOpt)ac_opt_none();
        ASSERT_EQ(AC_OPT_NONE, opt_none.tag, "{tag: %d}");
    });

    TEST(simple_some_unwrap, {
        IntOpt opt = (IntOpt)ac_opt_some(5);

        ASSERT_EQ(AC_OPT_SOME, opt.tag, "{tag: %d}");
        ASSERT_EQ(5, opt.some, "%d");

        int val = ac_opt_unwrap(opt);
        ASSERT_EQ(5, val, "%d");
    });

    TEST(simple_none_unwrap, {
        IntOpt opt = (IntOpt)ac_opt_none();

        ASSERT_EQ(AC_OPT_NONE, opt.tag, "{tag: %d}");

        int val = ac_opt_unwrap(opt);

        ASSERT(!check_assert(assert_buf));
        ASSERT_EQ(0, val, "%d");
    });

    TEST(simple_unwrap_or_on_some, {
        IntOpt opt = (IntOpt)ac_opt_some(2);

        int val = ac_opt_unwrap_or(opt, 5);

        ASSERT_EQ(2, val, "%d");
    });

    TEST(simple_unwrap_or_on_none, {
        IntOpt opt = (IntOpt)ac_opt_none();

        int val = ac_opt_unwrap_or(opt, 5);

        ASSERT_EQ(5, val, "%d");
    });

    TEST(simple_map_on_some, {
        IntOpt opt = (IntOpt)ac_opt_some(5);
        IntOpt new_opt = ac_opt_map(IntOpt, opt, map_times_2);

        ASSERT_EQ(AC_OPT_SOME, new_opt.tag, "{tag: %d}");
        ASSERT_EQ(10, new_opt.some, "%d");

        ASSERT_EQ(AC_OPT_SOME, opt.tag, "{tag: %d}");
        ASSERT_EQ(5, opt.some, "%d");
    });

    TEST(simple_map_on_none, {
        IntOpt opt = (IntOpt)ac_opt_none();
        IntOpt new_opt = ac_opt_map(IntOpt, opt, map_times_2);

        ASSERT_EQ(AC_OPT_NONE, new_opt.tag, "{tag: %d}");

        ASSERT_EQ(AC_OPT_NONE, opt.tag, "{tag: %d}");
    });

    TEST(map_change_type, {
        IntOpt opt = (IntOpt)ac_opt_some(5);
        CharOpt new_opt = ac_opt_map(CharOpt, opt, map_to_alphabet);

        ASSERT_EQ(AC_OPT_SOME, new_opt.tag, "{tag: %d}");
        ASSERT_EQ('F', new_opt.some, "'%c'");

        ASSERT_EQ(AC_OPT_SOME, opt.tag, "{tag: %d}");
        ASSERT_EQ(5, opt.some, "%d");
    });

    TEST_END;
}

int map_times_2(int val)
{
    return val * 2;
}
char map_to_alphabet(int val)
{
    return (char)(65 + val);
}
