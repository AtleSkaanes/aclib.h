#ifndef __TEST_H
#define __TEST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define _RED "\x1b[31m"
#define _GREEN "\x1b[32m"
#define _BLUE "\x1b[34m"
#define _CLEAR "\x1b[0m"

#define TEST_MIN(a, b) ((a) <= (b) ? (a) : (b))
#define TEST_MAX(a, b) ((a) >= (b) ? (a) : (b))

typedef enum
{
    TEST_OK,
    TEST_ERR,
} TestResult;

#define TEST_INIT                           \
    TestResult __all_test_result = TEST_OK; \
    size_t __tests_ran_ok = 0;              \
    size_t __tests_ran_err = 0;             \
    char* __log_stream_ptr = NULL;          \
    size_t __log_stream_len = 0;            \
    FILE* __log_stream = NULL;

#define TEST_END                                   \
    {                                              \
        fprintf(stdout, "%zu\n", __tests_ran_ok);  \
        fprintf(stdout, "%zu\n", __tests_ran_err); \
        return __all_test_result;                  \
    }

#define TEST(name, blck)                                                                     \
    {                                                                                        \
        TestResult __test_result = TEST_OK;                                                  \
        char* __test_name = #name;                                                           \
        __log_stream = open_memstream(&__log_stream_ptr, &__log_stream_len);                 \
        do                                                                                   \
        {                                                                                    \
            blck;                                                                            \
        } while (0);                                                                         \
        if (__test_result == TEST_OK)                                                        \
        {                                                                                    \
            fprintf(stderr, _BLUE "test " _CLEAR "%s: " _GREEN "succeeded" _CLEAR "\n",      \
                    __test_name);                                                            \
            __tests_ran_ok += 1;                                                             \
        }                                                                                    \
        else                                                                                 \
        {                                                                                    \
            __tests_ran_err += 1;                                                            \
            fflush(__log_stream);                                                            \
            if (__log_stream_len > 0)                                                        \
                fprintf(stderr, "=== LOGS ===\n%.*s\n=== LOGS ===\n", (int)__log_stream_len, \
                        __log_stream_ptr);                                                   \
        }                                                                                    \
        fclose(__log_stream);                                                                \
        free(__log_stream_ptr);                                                              \
        __log_stream_ptr = NULL;                                                             \
        __log_stream_len = 0;                                                                \
    }

#define ASSERT(cond)                                                                          \
    if (!(cond))                                                                              \
    {                                                                                         \
        fprintf(stderr,                                                                       \
                _BLUE "test " _CLEAR "%s: " _RED "failed" _CLEAR "\n\twith assertion '" #cond \
                      "'\n",                                                                  \
                __test_name);                                                                 \
        __test_result = TEST_ERR;                                                             \
        __all_test_result = TEST_ERR;                                                         \
        break;                                                                                \
    }

#define ASSERT_EQ(expected, got, fmt)                                                  \
    if ((expected) != (got))                                                           \
    {                                                                                  \
        fprintf(stderr,                                                                \
                _BLUE "test " _CLEAR "%s: " _RED "failed" _CLEAR                       \
                      "\n\ton line %d with equality assertion '" #expected " == " #got \
                      "'\n\texpected: " fmt "\n\tgot: " fmt "\n",                      \
                __test_name, __LINE__, expected, got);                                 \
        __test_result = TEST_ERR;                                                      \
        __all_test_result = TEST_ERR;                                                  \
        break;                                                                         \
    }

#define ASSERT_NEQ(expected, got, fmt)                                                   \
    if ((expected) == (got))                                                             \
    {                                                                                    \
        fprintf(stderr,                                                                  \
                _BLUE "test " _CLEAR "%s: " _RED "failed" _CLEAR                         \
                      "\n\ton line %d with inequality assertion '" #expected " != " #got \
                      "'\n\texpected: " fmt "\n\tgot: " fmt "\n",                        \
                __test_name, __LINE__, expected, got);                                   \
        __test_result = TEST_ERR;                                                        \
        __all_test_result = TEST_ERR;                                                    \
        break;                                                                           \
    }

#define ASSERT_GT(expected, got, fmt)                                                       \
    if ((expected) <= (got))                                                                \
    {                                                                                       \
        fprintf(stderr,                                                                     \
                _BLUE "test " _CLEAR "%s: " _RED "failed" _CLEAR                            \
                      "\n\ton line %d with gte comparison assertion '" #expected " > " #got \
                      "'\n\texpected: " fmt "\n\tgot: " fmt "\n",                           \
                __test_name, __LINE__, expected, got);                                      \
        __test_result = TEST_ERR;                                                           \
        __all_test_result = TEST_ERR;                                                       \
        break;                                                                              \
    }

#define ASSERT_GTE(expected, got, fmt)                                                       \
    if ((expected) < (got))                                                                  \
    {                                                                                        \
        fprintf(stderr,                                                                      \
                _BLUE "test " _CLEAR "%s: " _RED "failed" _CLEAR                             \
                      "\n\ton line %d with gte comparison assertion '" #expected " >= " #got \
                      "'\n\texpected: " fmt "\n\tgot: " fmt "\n",                            \
                __test_name, __LINE__, expected, got);                                       \
        __test_result = TEST_ERR;                                                            \
        __all_test_result = TEST_ERR;                                                        \
        break;                                                                               \
    }

#define ASSERT_LT(expected, got, fmt)                                                       \
    if ((expected) >= (got))                                                                \
    {                                                                                       \
        fprintf(stderr,                                                                     \
                _BLUE "test " _CLEAR "%s: " _RED "failed" _CLEAR                            \
                      "\n\ton line %d with gte comparison assertion '" #expected " < " #got \
                      "'\n\texpected: " fmt "\n\tgot: " fmt "\n",                           \
                __test_name, __LINE__, expected, got);                                      \
        __test_result = TEST_ERR;                                                           \
        __all_test_result = TEST_ERR;                                                       \
        break;                                                                              \
    }

#define ASSERT_LTE(expected, got, fmt)                                                       \
    if ((expected) > (got))                                                                  \
    {                                                                                        \
        fprintf(stderr,                                                                      \
                _BLUE "test " _CLEAR "%s: " _RED "failed" _CLEAR                             \
                      "\n\ton line %d with gte comparison assertion '" #expected " <= " #got \
                      "'\n\texpected: " fmt "\n\tgot: " fmt "\n",                            \
                __test_name, __LINE__, (expected), (got));                                   \
        __test_result = TEST_ERR;                                                            \
        __all_test_result = TEST_ERR;                                                        \
        break;                                                                               \
    }

/// Assert that a function call with an argument evaluates to true.
/// `ASSERT_FN(foo, bar)` is equal to `ASSERT(foo(bar))`, but `ASSERT_FN` can print the failing
/// `arg` value with `fmt`, whereas `ASSERT` cannot.
#define ASSERT_FN(fn, arg, fmt)                                                                 \
    if (!((fn)(arg)))                                                                           \
    {                                                                                           \
        fprintf(stderr,                                                                         \
                _BLUE "test " _CLEAR "%s: " _RED "failed" _CLEAR                                \
                      "\n\ton line %d with function assertion '" #fn "(" #arg ")'\n\targ: " fmt \
                      "\n",                                                                     \
                __test_name, __LINE__, arg);                                                    \
        __test_result = TEST_ERR;                                                               \
        __all_test_result = TEST_ERR;                                                           \
        break;                                                                                  \
    }

/// Asssert that a function call with an argument evaulates to false.
/// `ASSERT_NFN(foo, bar)` is equal to `ASSERT(!foo(bar))`, but `ASSERT_FN` can print the failing
/// `arg` value with `fmt`, whereas `ASSERT` cannot.
#define ASSERT_NFN(fn, arg, fmt)                                                                 \
    if (((fn)(arg)))                                                                             \
    {                                                                                            \
        fprintf(stderr,                                                                          \
                _BLUE "test " _CLEAR "%s: " _RED "failed" _CLEAR                                 \
                      "\n\ton line %d with function assertion '!" #fn "(" #arg ")'\n\targ: " fmt \
                      "\n",                                                                      \
                __test_name, __LINE__, arg);                                                     \
        __test_result = TEST_ERR;                                                                \
        __all_test_result = TEST_ERR;                                                            \
        break;                                                                                   \
    }

#define ASSERT_STR_EQ(expected, got)                                                          \
    if (strcmp((expected), (got)) != 0)                                                       \
    {                                                                                         \
        fprintf(stderr,                                                                       \
                _BLUE "test " _CLEAR "%s: " _RED "failed" _CLEAR                              \
                      "\n\ton line %d with string equality assertion '" #expected " == " #got \
                      "'\n\texpected: \"%s\""                                                 \
                      "\n\tgot: \"%s\"\n",                                                    \
                __test_name, __LINE__, expected, got);                                        \
        __test_result = TEST_ERR;                                                             \
        __all_test_result = TEST_ERR;                                                         \
        break;                                                                                \
    }

#define ASSERT_STR_LEN_EQ(expected, got, n)                                                   \
    if (strncmp((expected), (got), (n)) != 0)                                                 \
    {                                                                                         \
        fprintf(stderr,                                                                       \
                _BLUE "test " _CLEAR "%s: " _RED "failed" _CLEAR                              \
                      "\n\ton line %d with string equality assertion '" #expected " == " #got \
                      "'\n\texpected: \"%s\""                                                 \
                      "\n\tgot: \"%s\"\n",                                                    \
                __test_name, __LINE__, expected, got);                                        \
        __test_result = TEST_ERR;                                                             \
        __all_test_result = TEST_ERR;                                                         \
        break;                                                                                \
    }

#define ASSERT_ARR_EQ(expected, got, n, fmt)                                                     \
    {                                                                                            \
        bool is_equal = true;                                                                    \
        for (size_t i = 0; i < (n); i++)                                                         \
        {                                                                                        \
            if ((expected)[i] != (got)[i])                                                       \
                is_equal = false;                                                                \
        }                                                                                        \
        if (!is_equal)                                                                           \
        {                                                                                        \
            char* expected_str = NULL;                                                           \
            STRINGIFY_ARRAY(&expected_str, (expected), (n), fmt);                                \
            char* got_str = NULL;                                                                \
            STRINGIFY_ARRAY(&got_str, (got), (n), fmt);                                          \
            fprintf(stderr,                                                                      \
                    _BLUE "test " _CLEAR "%s: " _RED "failed" _CLEAR                             \
                          "\n\ton line %d with array equality assertion '" #expected " == " #got \
                          "'\n\texpected: %s"                                                    \
                          "\n\tgot: %s\n",                                                       \
                    __test_name, __LINE__, expected_str, got_str);                               \
            free(expected_str);                                                                  \
            free(got_str);                                                                       \
            __test_result = TEST_ERR;                                                            \
            __all_test_result = TEST_ERR;                                                        \
            break;                                                                               \
        }                                                                                        \
    }

#define TEST_LOG(...) fprintf(__log_stream, __VA_ARGS__)

#define STRINGIFY_ARRAY(out, arr, n, fmt)                              \
    {                                                                  \
        size_t size = (4 + 3 * (n));                                   \
        *(out) = realloc(*(out), size * sizeof(char));                 \
        (*(out))[0] = '{';                                             \
        (*(out))[1] = ' ';                                             \
        size_t idx = 2;                                                \
        for (size_t i = 0; i < (n); i++)                               \
        {                                                              \
            size_t needed = snprintf(NULL, 0, fmt ", ", (arr)[i]) + 1; \
            size = TEST_MAX(size, idx + needed);                       \
            *(out) = realloc(*(out), size * sizeof(char));             \
            sprintf(*(out) + idx, fmt ", ", (arr)[i]);                 \
            idx += needed - 1;                                         \
        }                                                              \
        idx = TEST_MAX(3, idx);                                        \
        (*(out))[idx - 2] = ' ';                                       \
        (*(out))[idx - 1] = '}';                                       \
        (*(out))[idx] = '\0';                                          \
    }


#endif // __TEST_H
