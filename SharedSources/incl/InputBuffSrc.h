#if !defined(__InputBuffSrcIf_HEADER_INCLUDED__)
#define __InputBuffSrcIf_HEADER_INCLUDED__


#include "ErrorDef.h"

/*! buffer class for faster implementation of variable input block size vs. constant processing block size

    USAGE

        // add new input data of any length
        pInputBuffSrcIf->setDataPtr2Hold(apfTmpPtr, iInpLength);
        while (pInputBuffSrcIf->getBlock(ppfProcBuff, kiProcBuffLength, kiProcBuffLength))
        {
            // do processing on ppfProcBuff with FIXED length kiProcBuffLength          
        }
        // store remaining data
        pInputBuffSrcIf->releaseDataPtr();
 
        // at the end of the processing - get remaining frames from the internal buffer if needed
        pInputBuffSrcIf->flush(ppfProcBuff);
*/

template <class T> 
class CInputBuffSrc
{
public:
    CInputBuffSrc (int iNumOfChannels, int iMaxOutputLength = 1024, int iInitialLatency = 0) : m_iNumChannels(iNumOfChannels),
        m_iMaxInternalBufferLength(iMaxOutputLength),
        m_iNumFramesInternalBuffer(iInitialLatency),
        m_pptInternalBuffer(0),
        m_pphtExternalData(0)
    {
    };
    virtual ~CInputBuffSrc ()
    {};

    Error_t setDataPtr2Hold (T **ppNewData, int iNumOfElements)
    {
        return kNoError;
    };
    
    Error_t releaseDataPtr ()
    {
        return kNoError;
    };
    
    bool getBlock (T **ppBlock, int iOutputBufferSize, int iIncIdxBy = 0)
    {
        return false;
    };

    Error_t reset ()
    {
        return kNoError;
    };

    int  flush (T **ppBlock)
    {
        return 0;
    };

private:
    int     m_iNumChannels;             //!< number of channels in data
    int     m_iMaxInternalBufferLength;            //!< maximum number of output frames
    int     m_iNumFramesInternalBuffer;       //!< number of frames currently in internal buffer
    T       **m_pptInternalBuffer;        //!< internal buffer
    T       **m_pphtExternalData;             //!< pointer to external data to be managed
    int     m_iExternalDataReadIdx;
    int     m_iNumFramesExternalData;
};


#endif //#define __InputBuffSrcIf_HEADER_INCLUDED__

