#if !defined(__MyProject_hdr__)
#define __MyProject_hdr__

#include "ErrorDef.h"

class CCombFilterBase;

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

    enum CombFilterType_t
    {
        kCombFIR,
        kCombIIR
    };

    enum FilterParam_t
    {
        kParamGain,
        kParamDelay,

        kNumFilterParams
    };
    static const int  getVersion (const Version_t eVersionIdx);
    static const char* getBuildDate ();

    static Error_t createInstance (CMyProject*& pCMyProject);
    static Error_t destroyInstance (CMyProject*& pCMyProject);
    
    Error_t initInstance (CombFilterType_t eFilterType, float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels);
    Error_t resetInstance ();

    Error_t setParam (FilterParam_t eParam, float fParamValue);
    float   getParam (FilterParam_t eParam) const;
    
    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);

protected:
    CMyProject ();
    virtual ~CMyProject ();

private:
    bool            m_bIsInitialized;
    CCombFilterBase *m_pCCombFilter;

    float           m_fSampleRate;
};

#endif // #if !defined(__MyProject_hdr__)



