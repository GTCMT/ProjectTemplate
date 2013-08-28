
#include <iostream>

// include project headers
#include "MyProject.h"

#define WITH_FLOATEXCEPTIONS
#define WITH_MEMORYCHECK

// include exception header
#if (defined(WITH_FLOATEXCEPTIONS) && !defined(NDEBUG) && defined (WIN32))
#include <float.h>
#endif // #ifndef WITHOUT_EXCEPTIONS

// include memory leak header
#if (defined(WITH_MEMORYCHECK) && !defined(NDEBUG) && defined (WIN32))
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#else
#include <stdlib.h>
#endif

using std::cout;
using std::endl;

// local function declarations
void     CLShowProgInfo();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{

    CMyProject             *pInstanceHandle    = 0;

#if (defined(WITH_MEMORYCHECK) && !defined(NDEBUG) && defined (WIN32))
    // set memory checking flags
    int iDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    iDbgFlag       |= _CRTDBG_CHECK_ALWAYS_DF;
    iDbgFlag       |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag( iDbgFlag );
#endif

#if (defined(WITH_FLOATEXCEPTIONS) && !defined(NDEBUG) && defined (WIN32))
    // enable check for exceptions (don't forget to enable stop in MSVC!)
    _controlfp(~(_EM_INVALID | _EM_ZERODIVIDE | _EM_OVERFLOW | _EM_UNDERFLOW | _EM_DENORMAL), _MCW_EM) ;
#endif // #ifndef WITHOUT_EXCEPTIONS

    // parse command line arguments

    // open files
   
    //allocate memory 

    // create class instance
    CMyProject::CreateInstance (pInstanceHandle);

    // process
 
    // write results

    // free memory    

    // delete instance
    CMyProject::DestroyInstance (pInstanceHandle);
    
    return 0;
    
}


void     CLShowProgInfo()
{
    cout << "GTCMT template app" << endl;
    cout << "(c) 2013 by Alexander Lerch" << endl;
    cout    << "V" 
        << CMyProject::GetVersion (CMyProject::kMajor) << "." 
        << CMyProject::GetVersion (CMyProject::kMinor) << "." 
        << CMyProject::GetVersion (CMyProject::kPatch) << ", date: " 
        << CMyProject::GetBuildDate () << endl;
    cout  << endl;

    return;
}
