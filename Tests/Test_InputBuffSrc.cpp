#include "MyProjectConfig.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>

#include "UnitTest++.h"

#include "InputBuffSrcIf.h"

SUITE(InputBuffSrc)
{
    struct InputBuffSrcData
    {
        InputBuffSrcData()  
        {
        }

        ~InputBuffSrcData() 
        {
        }
    };

    TEST_FIXTURE(InputBuffSrcData, DefaultTest)
    {
        CHECK(true);
    }
}

#endif //WITH_TESTS