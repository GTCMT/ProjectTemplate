
#include <UnitTest++.h>
#include "TestReporterStdout.h"

// include project headers
#include "AudioFileIf.h"

std::string cTestDataDir;

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    // argument 2 contains the working dir
    if (argc > 2)
        cTestDataDir.assign(argv[2]);

    // see http://stackoverflow.com/questions/3546054/how-do-i-run-a-single-test-with-unittest
    if( argc > 1 )
    {
        //walk list of all tests, add those with a name that
        //matches one of the arguments  to a new TestList
        const UnitTest::TestList& allTests( UnitTest::Test::GetTestList() );
        UnitTest::TestList selectedTests;
        UnitTest::Test* p = allTests.GetHead();
        while( p )
        {
            if( strcmp( p->m_details.suiteName, argv[ 1 ] ) == 0 )
            {
                selectedTests.Add( p );
            }
            p = p->m_nextTest;
        }
        selectedTests.Add(0);

        //run selected test(s) only
        UnitTest::TestReporterStdout reporter;
        UnitTest::TestRunner runner( reporter );
        return runner.RunTestsIf( selectedTests, 0, UnitTest::True(), 0 );
    }
    else
    {
        return UnitTest::RunAllTests();
    }
}

