#include <stdio.h>
#define ACLIB_IMPLEMENTATION
#include "../aclib.h"
#include "test.h"

int main(void)
{
    TEST_INIT;

    TEST(string_from_lit, {
        Ac_String str = ac_str_from("foo");

        ASSERT_EQ((size_t)3, str.len, "%zu");
        ASSERT_GTE((size_t)3, str.cap, "%zu");
        ASSERT_STR_EQ("foo", str.chars);

        ac_str_free(&str);
    });

    TEST(string_from_cstr, {
        char* cstr = "foo";
        Ac_String str = ac_str_from(cstr);

        ASSERT_EQ((size_t)3, str.len, "%zu");
        ASSERT_GTE((size_t)3, str.cap, "%zu");
        ASSERT_STR_EQ(cstr, str.chars);
        ASSERT_NEQ(cstr, str.chars, "%p");

        ac_str_free(&str);
    });

    TEST(string_from_string_slice, {
        Ac_StrSlice slice = ac_str_slice_from("foo");
        Ac_String str = ac_str_from(slice.chars);

        ASSERT_EQ((size_t)3, str.len, "%zu");
        ASSERT_GTE((size_t)3, str.cap, "%zu");
        ASSERT_STR_EQ(slice.chars, str.chars);
        ASSERT_NEQ(slice.chars, str.chars, "%p");

        ac_str_free(&str);
    });

    TEST(empty_string_push_pop, {
        Ac_String str = {0};
        ASSERT_EQ((size_t)0, str.len, "%zu");
        ASSERT_EQ((size_t)0, str.cap, "%zu");
        ASSERT_EQ((char*)0, str.chars, "%p");

        ac_str_push(&str, 'E');
        ASSERT_EQ((size_t)1, str.len, "%zu");
        ASSERT_GTE((size_t)1, str.cap, "%zu");
        ASSERT_STR_EQ("E", str.chars);
        size_t prev_cap = str.cap;

        char popped = ac_str_pop(&str);
        ASSERT_EQ('E', popped, "%c");

        ASSERT_EQ((size_t)0, str.len, "%zu");
        ASSERT_EQ(prev_cap, str.cap, "%zu");

        ac_str_free(&str);
        ASSERT_EQ((size_t)0, str.len, "%zu");
        ASSERT_EQ((size_t)0, str.cap, "%zu");
        ASSERT_EQ((char*)0, str.chars, "%p");
    });

    TEST(simple_append, {
        Ac_String str = {0};

        ac_str_push(&str, '[');
        ac_str_append(&str, "foobar");
        ac_str_push(&str, ']');

        ASSERT_STR_EQ("[foobar]", str.chars);
        ASSERT_EQ((size_t)8, str.len, "%zu");

        ac_str_free(&str);
    });

    TEST(append_format, {
        Ac_String str = {0};

        ac_str_appendf(&str, "NUMBER: %d", 4);

        ASSERT_STR_EQ("NUMBER: 4", str.chars);
        ASSERT_EQ((size_t)9, str.len, "%zu");

        ac_str_free(&str);
    });

    TEST(empty_string_unshift_shift, {
        Ac_String str = {0};
        ASSERT_EQ((size_t)0, str.len, "%zu");
        ASSERT_EQ((size_t)0, str.cap, "%zu");
        ASSERT_EQ((char*)0, str.chars, "%p");

        ac_str_unshift(&str, 'E');
        ASSERT_EQ((size_t)1, str.len, "%zu");
        ASSERT_GTE((size_t)1, str.cap, "%zu");
        ASSERT_STR_EQ("E", str.chars);
        size_t prev_cap = str.cap;

        char shifted = ac_str_shift(&str);
        ASSERT_EQ('E', shifted, "%c");

        ASSERT_EQ((size_t)0, str.len, "%zu");
        ASSERT_EQ(prev_cap, str.cap, "%zu");

        ac_str_free(&str);
        ASSERT_EQ((size_t)0, str.len, "%zu");
        ASSERT_EQ((size_t)0, str.cap, "%zu");
        ASSERT_EQ((char*)0, str.chars, "%p");
    });

    TEST(simple_prepend, {
        Ac_String str = {0};

        ac_str_push(&str, ']');
        ac_str_prepend(&str, "foobar");
        ac_str_unshift(&str, '[');

        ASSERT_STR_EQ("[foobar]", str.chars);
        ASSERT_EQ((size_t)8, str.len, "%zu");

        ac_str_free(&str);
    });

    TEST(prepend_format, {
        Ac_String str = {0};

        ac_str_push(&str, ']');

        ac_str_prependf(&str, "NUMBER: %d", 4);

        ac_str_unshift(&str, '[');

        ASSERT_STR_EQ("[NUMBER: 4]", str.chars);
        ASSERT_EQ((size_t)11, str.len, "%zu");

        ac_str_free(&str);
    });

    TEST(simple_empty, {
        Ac_String str = ac_str_from("foobar");

        char* old_ptr = str.chars;
        size_t old_cap = str.cap;

        ac_str_empty(&str);
        ASSERT_STR_EQ("", str.chars);
        ASSERT_EQ((size_t)0, str.len, "%zu");
        ASSERT_EQ(old_ptr, str.chars, "%p");
        ASSERT_EQ(old_cap, str.cap, "%zu");

        ac_str_free(&str);
    });

    TEST(simple_remove, {
        Ac_String str = ac_str_from("foolbar");

        ac_str_remove(&str, 3);

        ASSERT_STR_EQ("foobar", str.chars);
        ASSERT_EQ((size_t)6, str.len, "%zu");

        ac_str_free(&str);
    });

    TEST(remove_edges, {
        Ac_String str = ac_str_from("xfoobarx");

        ac_str_remove(&str, 0);
        ASSERT_STR_EQ("foobarx", str.chars);
        ASSERT_EQ((size_t)7, str.len, "%zu");

        ac_str_remove(&str, str.len - 1);
        ASSERT_STR_EQ("foobar", str.chars);
        ASSERT_EQ((size_t)6, str.len, "%zu");

        ac_str_free(&str);
    });

    TEST(remove_empty, {
        Ac_String str = ac_str_from("foobar");
        char* old_ptr = str.chars;

        ac_str_empty(&str);
        ASSERT_EQ((size_t)0, str.len, "%zu");
        ASSERT_EQ(old_ptr, str.chars, "%p");

        ac_str_remove(&str, 0);
        ASSERT_STR_EQ("", str.chars);
        ASSERT_EQ((size_t)0, str.len, "%zu");
        ASSERT_EQ(old_ptr, str.chars, "%p");

        ac_str_free(&str);
    });

    TEST(remove_out_of_bounds, {
        Ac_String str = ac_str_from("foobar");

        char* old_ptr = str.chars;

        ac_str_remove(&str, str.len + 4);
        ASSERT_STR_EQ("foobar", str.chars);
        ASSERT_EQ((size_t)6, str.len, "%zu");
        ASSERT_EQ(old_ptr, str.chars, "%p");

        ac_str_free(&str);
    });

    TEST(simple_remove_range, {
        Ac_String str = ac_str_from("fooloobar");

        ac_str_remove_range(&str, 3, 6);

        ASSERT_STR_EQ("foobar", str.chars);
        ASSERT_EQ((size_t)6, str.len, "%zu");

        ac_str_free(&str);
    });

    TEST(remove_range_edges, {
        Ac_String str = ac_str_from("xxfoobarxxxxx");

        ac_str_remove_range(&str, 0, 2);

        ASSERT_STR_EQ("foobarxxxxx", str.chars);
        ASSERT_EQ((size_t)11, str.len, "%zu");

        ac_str_remove_range(&str, 6, 11);

        ASSERT_STR_EQ("foobar", str.chars);
        ASSERT_EQ((size_t)6, str.len, "%zu");

        ac_str_free(&str);
    });

    TEST(simple_drain, {
        Ac_String str = {0};

        ac_str_append(&str, "foobar");
        ASSERT_EQ((size_t)6, str.len, "%zu");

        Ac_StrSlice slc = ac_str_drain(&str);
        ASSERT_EQ((size_t)0, str.len, "%zu");

        ASSERT_NEQ(str.chars, slc.chars, "%p");

        ASSERT_EQ((size_t)6, slc.len, "%zu");
        ASSERT_STR_LEN_EQ("foobar", slc.chars, slc.len);

        ac_str_free(&str);
        ac_str_slice_free(&slc);
    });

    TEST(simple_drain_range, {
        Ac_String str = {0};

        ac_str_append(&str, "foobazbar");
        ASSERT_EQ((size_t)9, str.len, "%zu");

        Ac_StrSlice slc = ac_str_drain_range(&str, 3, 6);
        ASSERT_EQ((size_t)3, slc.len, "%zu");
        ASSERT_STR_LEN_EQ("baz", slc.chars, slc.len);

        ASSERT_EQ((size_t)6, str.len, "%zu");
        ASSERT_STR_EQ("foobar", str.chars);

        ac_str_free(&str);
        ac_str_slice_free(&slc);
    });

    TEST(simple_trim_front, {
        Ac_String str = ac_str_from(" \t\v foo bar ");
        ac_str_trim_front(&str);

        ASSERT_STR_EQ("foo bar ", str.chars);
        ASSERT_EQ((size_t)8, str.len, "%zu");

        ac_str_free(&str);
    });

    TEST(simple_trim_back, {
        Ac_String str = ac_str_from(" \n\tfoo bar\r  ");
        ac_str_trim_back(&str);

        ASSERT_STR_EQ(" \n\tfoo bar", str.chars);
        ASSERT_EQ((size_t)10, str.len, "%zu");
    });

    TEST(simple_trim_both, {
        Ac_String str = ac_str_from(" \r\v foo bar  \t");
        ac_str_trim(&str);

        ASSERT_STR_EQ("foo bar", str.chars);
        ASSERT_EQ((size_t)7, str.len, "%zu");

        ac_str_free(&str);
    });

    TEST(trim_no_whitespace, {
        Ac_String str = ac_str_from("foo bar");

        ac_str_trim(&str);
        ASSERT_STR_EQ("foo bar", str.chars);
        ASSERT_EQ((size_t)7, str.len, "%zu");

        ac_str_free(&str);
    });

    TEST(trim_empty, {
        Ac_String str = ac_str_from("foo bar");
        char* old_ptr = str.chars;

        ac_str_empty(&str);
        ASSERT_STR_EQ("", str.chars);
        ASSERT_EQ((size_t)0, str.len, "%zu");
        ASSERT_EQ(old_ptr, str.chars, "%p");

        ac_str_trim(&str);
        ASSERT_STR_EQ("", str.chars);
        ASSERT_EQ((size_t)0, str.len, "%zu");
        ASSERT_EQ(old_ptr, str.chars, "%p");

        ac_str_free(&str);
    });

    TEST(simple_split_by, {
        Ac_String str = ac_str_from("foo bar baz");
        char* old_ptr = str.chars;
        size_t old_cap = str.cap;
        size_t old_len = str.len;

        Ac_StrVec parts = ac_str_split_by(str, ' ');

        ASSERT_STR_EQ("foo bar baz", str.chars);
        ASSERT_EQ(old_ptr, str.chars, "%p");
        ASSERT_EQ(old_cap, str.cap, "%zu");
        ASSERT_EQ(old_len, str.len, "%zu");

        ASSERT_EQ((size_t)3, parts.len, "%zu");

        ASSERT_STR_EQ("foo", parts.items[0].chars);
        ASSERT_EQ((size_t)3, parts.items[0].len, "%zu");
        ASSERT_NEQ(old_ptr, parts.items[0].chars, "%p");

        ASSERT_STR_EQ("bar", parts.items[1].chars);
        ASSERT_EQ((size_t)3, parts.items[1].len, "%zu");

        ASSERT_STR_EQ("baz", parts.items[2].chars);
        ASSERT_EQ((size_t)3, parts.items[2].len, "%zu");

        ac_str_free(&str);

        for (size_t i = 0; i < parts.len; i++)
            ac_str_slice_free(&parts.items[i]);
    });

    TEST(split_by_no_matches, {
        Ac_String str = ac_str_from("foobarbaz");

        Ac_StrVec parts = ac_str_split_by(str, ' ');

        ASSERT_EQ((size_t)1, parts.len, "%zu");
        ASSERT_STR_EQ(str.chars, parts.items[0].chars);
        ASSERT_NEQ(str.chars, parts.items[0].chars, "%p");
        ASSERT_EQ(str.len, parts.items[0].len, "%zu");
    });

    TEST(simple_split_by_once, {
        Ac_String str = ac_str_from("foo bar baz");
        char* old_ptr = str.chars;
        size_t old_cap = str.cap;
        size_t old_len = str.len;

        Ac_StrVec parts = ac_str_split_by_once(str, ' ');

        ASSERT_STR_EQ("foo bar baz", str.chars);
        ASSERT_EQ(old_ptr, str.chars, "%p");
        ASSERT_EQ(old_cap, str.cap, "%zu");
        ASSERT_EQ(old_len, str.len, "%zu");

        ASSERT_EQ((size_t)2, parts.len, "%zu");

        ASSERT_STR_EQ("foo", parts.items[0].chars);
        ASSERT_EQ((size_t)3, parts.items[0].len, "%zu");
        ASSERT_NEQ(old_ptr, parts.items[0].chars, "%p");

        ASSERT_STR_EQ("bar baz", parts.items[1].chars);
        ASSERT_EQ((size_t)7, parts.items[1].len, "%zu");

        ac_str_free(&str);

        for (size_t i = 0; i < parts.len; i++)
            ac_str_slice_free(&parts.items[i]);
    });

    TEST(simple_split_by_many, {
        Ac_String str = ac_str_from("foo bar\tbaz");
        char* old_ptr = str.chars;
        size_t old_cap = str.cap;
        size_t old_len = str.len;

        Ac_StrVec parts = ac_str_split_by_many(str, " \t");

        ASSERT_STR_EQ("foo bar\tbaz", str.chars);
        ASSERT_EQ(old_ptr, str.chars, "%p");
        ASSERT_EQ(old_cap, str.cap, "%zu");
        ASSERT_EQ(old_len, str.len, "%zu");

        ASSERT_EQ((size_t)3, parts.len, "%zu");

        ASSERT_STR_EQ("foo", parts.items[0].chars);
        ASSERT_EQ((size_t)3, parts.items[0].len, "%zu");
        ASSERT_NEQ(old_ptr, parts.items[0].chars, "%p");

        ASSERT_STR_EQ("bar", parts.items[1].chars);
        ASSERT_EQ((size_t)3, parts.items[1].len, "%zu");

        ASSERT_STR_EQ("baz", parts.items[2].chars);
        ASSERT_EQ((size_t)3, parts.items[2].len, "%zu");

        ac_str_free(&str);

        for (size_t i = 0; i < parts.len; i++)
            ac_str_slice_free(&parts.items[i]);
    });

    TEST(simple_split_at, {
        Ac_String str = ac_str_from("foobar");
        char* old_ptr = str.chars;
        size_t old_cap = str.cap;
        size_t old_len = str.len;

        Ac_StrVec parts = ac_str_split_at(str, 3);

        ASSERT_STR_EQ("foobar", str.chars);
        ASSERT_EQ(old_ptr, str.chars, "%p");
        ASSERT_EQ(old_cap, str.cap, "%zu");
        ASSERT_EQ(old_len, str.len, "%zu");

        ASSERT_EQ((size_t)2, parts.len, "%zu");

        ASSERT_STR_EQ("foo", parts.items[0].chars);
        ASSERT_EQ((size_t)3, parts.items[0].len, "%zu");
        ASSERT_NEQ(old_ptr, parts.items[0].chars, "%p");

        ASSERT_STR_EQ("bar", parts.items[1].chars);
        ASSERT_EQ((size_t)3, parts.items[1].len, "%zu");

        ac_str_free(&str);

        for (size_t i = 0; i < parts.len; i++)
            ac_str_slice_free(&parts.items[i]);
    });

    TEST(simple_split_at_out_of_bounds, {
        Ac_String str = ac_str_from("foobar");
        char* old_ptr = str.chars;
        size_t old_cap = str.cap;
        size_t old_len = str.len;

        Ac_StrVec parts = ac_str_split_at(str, str.len + 4);

        ASSERT_STR_EQ("foobar", str.chars);
        ASSERT_EQ(old_ptr, str.chars, "%p");
        ASSERT_EQ(old_cap, str.cap, "%zu");
        ASSERT_EQ(old_len, str.len, "%zu");

        ASSERT_EQ((size_t)1, parts.len, "%zu");

        ASSERT_STR_EQ(str.chars, parts.items[0].chars);
        ASSERT_EQ(str.len, parts.items[0].len, "%zu");
        ASSERT_NEQ(old_ptr, parts.items[0].chars, "%p");

        ac_str_free(&str);

        for (size_t i = 0; i < parts.len; i++)
            ac_str_slice_free(&parts.items[i]);
    });

    TEST(simple_read_file, {
        char* file_buf = NULL;
        size_t file_len = 0;
        FILE* file = open_memstream(&file_buf, &file_len);

        fprintf(file, "foo\nbar\nbaz\tbaz");
        fflush(file);

        Ac_String str = {0};

        ac_str_read_file(&str, file);

        ASSERT_STR_EQ("foo\nbar\nbaz\tbaz", str.chars);
        ASSERT_EQ((size_t)15, str.len, "%zu");

        ac_str_free(&str);
    });

    TEST(simple_read_file_lines, {
        char* file_buf = NULL;
        size_t file_len = 0;
        FILE* file = open_memstream(&file_buf, &file_len);

        fprintf(file, "foo bar \n bar baz\nbaz\tfoo ");
        fflush(file);

        Ac_StrVec lines = {0};

        ac_str_read_lines(&lines, file);

        ASSERT_EQ((size_t)3, lines.len, "%zu");

        ASSERT_STR_EQ("foo bar ", lines.items[0].chars);
        ASSERT_EQ((size_t)8, lines.items[0].len, "%zu");
        ASSERT_STR_EQ(" bar baz", lines.items[1].chars);
        ASSERT_EQ((size_t)8, lines.items[1].len, "%zu");
        ASSERT_STR_EQ("baz\tfoo ", lines.items[2].chars);
        ASSERT_EQ((size_t)8, lines.items[2].len, "%zu");

        for (size_t i = 0; i < lines.len; i++)
            ac_str_slice_free(&lines.items[i]);

        ac_vec_free(lines);
    });

    TEST(string_fmt, {
        Ac_String str = ac_str_from("foobar");

        char buf[128];
        sprintf(buf, "'" AC_STR_FMT "'", AC_STR_ARG(str));
        ASSERT_STR_EQ("'foobar'", buf);
        ASSERT_EQ(str.len + 2, strlen(buf), "%zu");

        ac_str_free(&str);
    });

    TEST(string_slice_fmt, {
        Ac_String str = ac_str_from("hello foo world!");
        Ac_StrSlice slc = ac_str_slice_range(str, 6, 9);
        ASSERT_NEQ('\0', slc.chars[slc.len], "%c");

        char buf[128];
        sprintf(buf, "'" AC_STR_FMT "'", AC_STR_ARG(slc));
        ASSERT_STR_EQ("'foo'", buf);
        ASSERT_EQ(slc.len + 2, strlen(buf), "%zu");

        ac_str_free(&str);
    });


    TEST_END;
}
