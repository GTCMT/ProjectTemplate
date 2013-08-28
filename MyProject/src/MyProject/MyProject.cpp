
// standard headers

// project headers
#include "MyProjectConfig.h"

#include "MyProject.h"

static const char*  kCMyProjectBuildDate             = __DATE__;


CMyProject::CMyProject ()
{
    // this never hurts
    this->ResetInstance ();
}


CMyProject::~CMyProject ()
{
    this->ResetInstance ();
}

const int  CMyProject::GetVersion (const Version_t eVersionIdx)
{
    int iVersion = 0;

    switch (eVersionIdx)
    {
    case kMajor:
        iVersion    = MyProject_VERSION_MAJOR; 
        break;
    case kMinor:
        iVersion    = MyProject_VERSION_MINOR; 
        break;
    case kPatch:
        iVersion    = MyProject_VERSION_PATCH; 
        break;
    }

    return iVersion;
}
const char*  CMyProject::GetBuildDate ()
{
    return kCMyProjectBuildDate;
}

CMyProject::Error_t CMyProject::CreateInstance (CMyProject*& pCMyProject)
{
    pCMyProject = new CMyProject ();

    if (!pCMyProject)
        return kUnknownError;


    return kNoError;
}

CMyProject::Error_t CMyProject::DestroyInstance (CMyProject*& pCMyProject)
{
    if (!pCMyProject)
        return kUnknownError;
    
    pCMyProject->ResetInstance ();
    
    delete pCMyProject;
    pCMyProject = 0;

    return kNoError;

}

CMyProject::Error_t CMyProject::InitInstance()
{
    // allocate memory

    // initialize variables and buffers

    return kNoError;
}

CMyProject::Error_t CMyProject::ResetInstance ()
{
    // reset buffers and variables to default values

    return kNoError;
}
