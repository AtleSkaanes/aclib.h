#include "test.h"

#define ACLIB_IMPLEMENTATION
#include "../aclib.h"

#include <stdio.h>

void simple_log(Ac_LogLevel loglvl, const char* fmt, ...);
void reset_file(FILE** fd, char* buf, size_t buf_size);

int main(void)
{
    TEST_INIT;

    char outbuf[256] = {0};
    aclib_log_fd = fmemopen(outbuf, 255, "w");
    aclib_log_fn_ptr = simple_log;

    // fprintf(outfile, "hello :)");
    // printf("CONTENTS: '%s'\n", outbuf);

    TEST(log_printing, {
        for (int i = 0; i < 5; i++)
        {
            Ac_LogLevel lvl = (Ac_LogLevel)i;
            aclib_log_level = lvl;
            ac_log(lvl, "foobar");
            ASSERT_STR_EQ("foobar", outbuf);
            reset_file(&aclib_log_fd, outbuf, 256);
        }
    });

    reset_file(&aclib_log_fd, outbuf, 256);
    TEST(no_logs, {
        aclib_log_level = ACLIB_NO_LOGS;
        for (int i = 0; i < ACLIB_NO_LOGS; i++)
            ac_log((Ac_LogLevel)i, "foobar");

        ASSERT_EQ((unsigned long)0, strlen(outbuf), "%lu");
    });


    TEST_END;
}

void simple_log(Ac_LogLevel loglvl, const char* fmt, ...)
{
    if (loglvl < aclib_log_level)
        return;

    va_list args;
    va_start(args, fmt);
    vfprintf(aclib_log_fd, fmt, args);
    va_end(args);
    fflush(aclib_log_fd);
}

void reset_file(FILE** fd, char* buf, size_t buf_size)
{
    fclose(*fd);
    buf[0] = '\0';
    *fd = fmemopen(buf, buf_size - 1, "w");
}
