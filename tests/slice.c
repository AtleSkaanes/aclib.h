#include <stdio.h>
#define ACLIB_IMPLEMENTATION
#include "../aclib.h"
#include "test.h"

typedef Al_VecDef(int) IntVec;
typedef Al_SliceDef(int) IntSlc;

int main(void)
{
    TEST_INIT;

    TEST(simple_slice_range, {
        int arr[5] = ((int[]){1, 2, 3, 4, 5});
        IntVec ivec = al_vec_from(arr, 5);
        IntSlc slc = al_vec_slice_range(IntSlc, ivec, 2, 4);

        ASSERT_EQ((size_t)2, slc.len, "%zu");
        ASSERT_ARR_EQ(((int[]){3, 4}), slc.items, 2, "%d");

        al_vec_free(ivec);
    });

    TEST_END;
}
