
// standard headers
#include <cmath>
#include <cstdlib>
#include <cstring>

// project headers
#include "MyProjectConfig.h"

#include "ErrorDef.h"

#include "MyProject.h"

#include <intrin.h>

static const char*  kCMyProjectBuildDate             = __DATE__;


CMyProject::CMyProject( int iVectorLength ): 
    m_pfTestBuffer(0), 
    m_pfAdd(0),
    m_pfResult(0),
    m_iLength(iVectorLength)
{
    // we have to allocate ALIGNED memory!
    m_pfTestBuffer  = (float*) _aligned_malloc(m_iLength * sizeof(float), 16);
    m_pfAdd         = (float*) _aligned_malloc(4 * sizeof(float), 16);
    m_pfResult      = (float*) _aligned_malloc(4 * sizeof(float), 16);

    //init 
    for (int i = 0; i < m_iLength; i++)
        m_pfTestBuffer[i]   = static_cast <float>(rand()) / static_cast <float>(RAND_MAX);
    for (int i = 0; i < 4; i++)
    {
        m_pfResult[i]   = 0;
        m_pfAdd[i]      = .5F;
    }
}


CMyProject::~CMyProject ()
{
    _aligned_free (m_pfTestBuffer);
    _aligned_free (m_pfAdd);
    _aligned_free (m_pfResult);
}

const int  CMyProject::getVersion (const Version_t eVersionIdx)
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
    case kNumVersionInts:
        iVersion    = -1;
        break;
    }

    return iVersion;
}
const char*  CMyProject::getBuildDate ()
{
    return kCMyProjectBuildDate;
}

Error_t CMyProject::createInstance( CMyProject*& pCMyProject, int iVectorLength )
{
    pCMyProject = new CMyProject (iVectorLength);

    if (!pCMyProject)
        return kUnknownError;


    return kNoError;
}

Error_t CMyProject::destroyInstance (CMyProject*& pCMyProject)
{
    if (!pCMyProject)
        return kUnknownError;
    
    delete pCMyProject;
    pCMyProject = 0;

    return kNoError;

}

void CMyProject::benchmark( BenchMarkMode_t eMode, int iNumOfIterations )
{
    if (eMode == kUnoptimized)
    {
        volatile float fResult;

        for (int k = 0; k < iNumOfIterations; k++)
        {
            fResult = 0;
            for (int i = 0; i < m_iLength; i++)
            {
                fResult += sqrtf((m_pfTestBuffer[i] + m_pfAdd[0])*(m_pfTestBuffer[i]));
            }
        }
    }
    else
    {
        int iLoopLength = m_iLength/4; // careful with non-multiples of 4!

        for (int k = 0; k < iNumOfIterations; k++)
        {
            memset (m_pfResult, 0, 4*sizeof(float));
            __m128* pBuffer = (__m128*) m_pfTestBuffer; // cast to format for SSE (aligned memory!)
            __m128* pAdd    = (__m128*) m_pfAdd;
            __m128* pResult = (__m128*) m_pfResult;
            for (int i = 0; i < iLoopLength; i++)
            {
                __m128 m1 = _mm_add_ps(pBuffer[i], *pAdd);      // add 4 buffer elements
                m1 = _mm_mul_ps(m1,pBuffer[i]);                 // multiply added and orig
                m1 = _mm_sqrt_ps(m1);                           // compute sqrt
                
                // now get the sum and write the result
                m1 = _mm_hadd_ps(m1, m1);                       // this works only with SSE3
                m1 = _mm_hadd_ps(m1, m1);                       // SSE is not optimized for horizontal sums...
                *pResult = _mm_add_ps(*pResult, m1); 
            }
        }
    }
}
