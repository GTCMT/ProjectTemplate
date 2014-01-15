#include "MyProjectConfig.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>

#include "UnitTest++.h"

#include "MyProject.h"

SUITE(MyProject)
{
    struct MyProject
    {
        MyProject() 
        {
        }

        ~MyProject() 
        {
        }
    };

    TEST_FIXTURE(MyProject, NoTest)
    {
        CHECK_EQUAL (0 ,0);
    }
}

#endif //WITH_TESTS