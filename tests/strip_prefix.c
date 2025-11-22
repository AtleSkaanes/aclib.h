#define ACLIB_STRIP_PREFIX
#define ACLIB_IMPLEMENTATION
#include "../aclib.h"
#include "test.h"

typedef VecDef(int) IntVec;
typedef SliceDef(int) IntSlc;

int main(void)
{
    TEST_INIT;

    TEST(compiles, {
        String str = str_from("foobar");
        str_appendf(&str, "foobar %d", str.len);
    });

    TEST_END;
}
