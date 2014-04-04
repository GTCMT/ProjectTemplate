
#include <iostream>
#include <ctime>

#include "MyProjectConfig.h"

// include project headers
#include "AudioFileIf.h"

#include "MyProject.h"

#define WITH_FLOATEXCEPTIONS
#define WITH_MEMORYCHECK

// include exception header
#if (defined(WITH_FLOATEXCEPTIONS) && !defined(NDEBUG) && defined (GTCMT_WIN32))
#include <float.h>
#endif // #ifndef WITHOUT_EXCEPTIONS

// include memory leak header
#if (defined(WITH_MEMORYCHECK) && !defined(NDEBUG) && defined (GTCMT_WIN32))
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#else
#include <stdlib.h>
#endif

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int /*argc*/, char* /*argv[]*/)
{
    CMyProject  *pCMyProject = 0;

    clock_t time  = 0;

    // detect memory leaks in win32
#if (defined(WITH_MEMORYCHECK) && !defined(NDEBUG) && defined (GTCMT_WIN32))
    // set memory checking flags
    int iDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    iDbgFlag       |= _CRTDBG_CHECK_ALWAYS_DF;
    iDbgFlag       |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag( iDbgFlag );
#endif

    // enable floating point exceptions in win32
#if (defined(WITH_FLOATEXCEPTIONS) && !defined(NDEBUG) && defined (GTCMT_WIN32))
    // enable check for exceptions (don't forget to enable stop in MSVC!)
    _controlfp(~(_EM_INVALID | _EM_ZERODIVIDE | _EM_OVERFLOW | _EM_UNDERFLOW | _EM_DENORMAL), _MCW_EM) ;
#endif // #ifndef WITHOUT_EXCEPTIONS

    showClInfo ();

    CMyProject::createInstance(pCMyProject);

    cout << "running native code..." << endl;
    time = clock();
    pCMyProject->benchmark(CMyProject::kUnoptimized);
    cout << "native time:\t" << double(clock() - time) / CLOCKS_PER_SEC << endl << endl;

    cout << "running intrinsics code..." << endl;
    time = clock();
    pCMyProject->benchmark(CMyProject::kIntrinsics);
    cout << "sse time:\t" << double(clock() - time) / CLOCKS_PER_SEC << endl;

    CMyProject::destroyInstance(pCMyProject);

    return 0;
    
}


void     showClInfo()
{
    cout << "GTCMT template app" << endl;
    cout << "(c) 2013 by Alexander Lerch" << endl;
    cout    << "V" 
        << CMyProject::getVersion (CMyProject::kMajor) << "." 
        << CMyProject::getVersion (CMyProject::kMinor) << "." 
        << CMyProject::getVersion (CMyProject::kPatch) << ", date: " 
        << CMyProject::getBuildDate () << endl;
    cout  << endl;

    return;
}
