#include <stdio.h>
#define ACLIB_IMPLEMENTATION
#include "../aclib.h"
#include "test.h"

typedef Ac_VecDef(int) IntVec;
typedef Ac_SliceDef(int) IntSlc;

int main(void)
{
    TEST_INIT;

    TEST(simple_slice_range, {
        int arr[5] = ((int[]){1, 2, 3, 4, 5});
        IntVec ivec = ac_vec_from(arr, 5);
        IntSlc slc = ac_vec_slice_range(IntSlc, ivec, 2, 4);

        ASSERT_EQ((size_t)2, slc.len, "%zu");
        ASSERT_ARR_EQ(((int[]){3, 4}), slc.items, 2, "%d");

        ac_vec_free(ivec);
    });

    TEST_END;
}
