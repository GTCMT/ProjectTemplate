#if !defined(__MyProject_hdr__)
#define __MyProject_hdr__

#include "ErrorDef.h"

class CMyProject
{
public:
    /*! version number */
    enum Version_t
    {
        kMajor,                         //!< major version number
        kMinor,                         //!< minor version number
        kPatch,                         //!< patch version number

        kNumVersionInts
    };

    enum BenchMarkMode_t
    {
        kUnoptimized,
        kIntrinsics,

        kNumBenchMarkModes
    };

    static const int  getVersion (const Version_t eVersionIdx);
    static const char* getBuildDate ();

    static Error_t createInstance (CMyProject*& pCMyProject, int iVectorLength = 65536);
    static Error_t destroyInstance (CMyProject*& pCMyProject);
    
    void benchmark (BenchMarkMode_t eMode, int iNumOfIterations = 10000);

protected:
    CMyProject (int iVectorLength);
    virtual ~CMyProject ();

    float *m_pfTestBuffer;
    float *m_pfAdd;
    float *m_pfResult;
    int m_iLength;
};

#endif // #if !defined(__MyProject_hdr__)



