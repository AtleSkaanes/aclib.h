#include "test.h"

#define ACLIB_IMPLEMENTATION
#include "../aclib.h"

int main(void)
{
    TEST_INIT;

    // ALPHABETIC

    TEST(alphabet_is_alphabetic, {
        char* alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        for (int i = 0; i < strlen(alphabet); i++)
            ASSERT_FN(al_ascii_is_alphabetic, alphabet[i], "'%c'");
    });

    TEST(nonalphabet_isnt_alphabetic, {
        char* non_alphabet = "0123456789_!#-;´'¨{}@[] \t\r\n";
        for (int i = 0; i < strlen(non_alphabet); i++)
            ASSERT_NFN(al_ascii_is_alphabetic, non_alphabet[i], "'%c'");
    });


    // NUMERIC

    TEST(number_is_numeric, {
        char* numbers = "0123456789";
        for (int i = 0; i < strlen(numbers); i++)
            ASSERT_FN(al_ascii_is_numeric, numbers[i], "'%c'");
    });

    TEST(nonnumber_isnt_numeric, {
        char* non_numbers = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ./:; \t\r\n";
        for (int i = 0; i < strlen(non_numbers); i++)
            ASSERT_NFN(al_ascii_is_numeric, non_numbers[i], "'%c'");
    });

    // ALPHANUMERIC

    TEST(alphanumeric_when_alphanumeric, {
        for (int ch = 0; ch < 127; ch++)
            if (al_ascii_is_alphabetic(ch) || al_ascii_is_numeric(ch))
            {
                ASSERT_FN(al_ascii_is_alphanumeric, ch, "'%c'");
            }
            else
            {
                ASSERT_NFN(al_ascii_is_alphanumeric, ch, "'%c'");
            }
    });

    //  WHITESPACE

    TEST(whitespace_is_whitespace, {
        char* chs = " \r\n\t\v\f";
        for (int i = 0; i < strlen(chs); i++)
            ASSERT_FN(al_ascii_is_whitespace, chs[i], "'%c'");
    });

    // UPPER AND LOWERCASE CHECK

    TEST(lower_is_lowercase, {
        char* chs = "abcdefghijklmnopqrstuvwxyz";
        for (int i = 0; i < strlen(chs); i++)
            ASSERT_FN(al_ascii_is_lowercase, chs[i], "'%c'");
    });

    TEST(lower_isnt_uppercase, {
        char* chs = "abcdefghijklmnopqrstuvwxyz";
        for (int i = 0; i < strlen(chs); i++)
            ASSERT_NFN(al_ascii_is_uppercase, chs[i], "'%c'");
    });

    TEST(upper_is_uppercase, {
        char* chs = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        for (int i = 0; i < strlen(chs); i++)
            ASSERT_FN(al_ascii_is_uppercase, chs[i], "'%c'");
    });

    TEST(upper_isnt_lowercase, {
        char* chs = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        for (int i = 0; i < strlen(chs); i++)
            ASSERT_NFN(al_ascii_is_lowercase, chs[i], "'%c'");
    });

    TEST(number_isnt_lowercase_or_uppercase, {
        char* chs = "0123456789";
        for (int i = 0; i < strlen(chs); i++)
        {
            ASSERT_NFN(al_ascii_is_lowercase, chs[i], "'%c'");
            ASSERT_NFN(al_ascii_is_uppercase, chs[i], "'%c'");
        }
    });

    // LOWER AND UPPERCASE CONVERT

    TEST(convert_to_other_case, {
        char* lower = "abcdefghijklmnopqrstuvwxyz";
        char* upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        for (int i = 0; i < strlen(lower); i++)
        {
            ASSERT_EQ(upper[i], al_ascii_to_uppercase(lower[i]), "%c");
            ASSERT_EQ(lower[i], al_ascii_to_lowercase(upper[i]), "%c");
        }
    });

    TEST(convert_to_same_case, {
        char* lower = "abcdefghijklmnopqrstuvwxyz";
        char* upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        for (int i = 0; i < strlen(upper); i++)
        {
            ASSERT_EQ(lower[i], al_ascii_to_lowercase(lower[i]), "%c");
            ASSERT_EQ(upper[i], al_ascii_to_uppercase(upper[i]), "%c");
        }
    });

    TEST(nonalphabet_convert_case, {
        for (int ch = 0; ch < 127; ch++)
        {
            if (!al_ascii_is_alphabetic(ch))
            {
                ASSERT_EQ(ch, al_ascii_to_uppercase(ch), "%c");
            }
        }
    });


    TEST_END;
}
