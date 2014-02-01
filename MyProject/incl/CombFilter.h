#if !defined(__CombFilter_hdr__)
#define __CombFilter_hdr__

#include "ErrorDef.h"
#include "MyProject.h"

class CCombFilterBase
{
public:
    CCombFilterBase (int iMaxDelayInFrames, int iNumChannels);
    virtual ~CCombFilterBase ();

    Error_t resetInstance ();

    Error_t setParam (CMyProject::FilterParam_t eParam, float fParamValue);
    float   getParam (CMyProject::FilterParam_t eParam) const;

    virtual Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) = 0;

protected:
    CRingBuffer<float>  **m_ppCRingBuffer;  

    float   m_afParam[CMyProject::kNumFilterParams];
    float   m_aafParamRange[CMyProject::kNumFilterParams][2];

    int     m_iNumChannels;

private:
    bool    isInParamRange (CMyProject::FilterParam_t eParam, float fValue);
};


class CCombFilterFir : public CCombFilterBase
{
public:
    CCombFilterFir (int iMaxDelayInFrames, int iNumChannels):CCombFilterBase(iMaxDelayInFrames, iNumChannels){};
    virtual ~CCombFilterFir (){};

    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
};

class CCombFilterIir : public CCombFilterBase
{
public:
    CCombFilterIir (int iMaxDelayInFrames, int iNumChannels);
    virtual ~CCombFilterIir (){};

    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
};

#endif // #if !defined(__CombFilter_hdr__)
