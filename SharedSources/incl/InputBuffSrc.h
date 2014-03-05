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

    /*! set new external data pointer
    \param T * * ppNewData
    \param int iNumOfElements
    \return Error_t
    */
    Error_t setDataPtr2Hold (T **ppNewData, int iNumOfElements)
    {
        return kNoError;
    };
    
    /*! store remaining frames and release external data pointer
    \return Error_t
    */
    Error_t releaseDataPtr ()
    {
        return kNoError;
    };
    
    /*! return current block of frames if available
    \param T * * ppBlock
    \param int iOutputBufferSize
    \param int iIncIdxBy
    \return bool
    */
    bool getBlock (T **ppBlock, int iOutputBufferSize, int iIncIdxBy = 0)
    {
        return false;
    };

    /*! clear all internal buffer and reset member variables
    \return Error_t
    */
    Error_t reset ()
    {
        return kNoError;
    };

    /*! return the remaining frames from internal buffers
    \param T * * ppBlock
    \return int
    */
    int  flush (T **ppBlock)
    {
        return 0;
    };

private:
    int     m_iNumChannels;                 //!< number of channels in data
    T       **m_pptInternalBuffer;          //!< internal buffer
    int     m_iMaxInternalBufferLength;     //!< maximum number of output frames
    int     m_iNumFramesInternalBuffer;     //!< number of frames currently in internal buffer

    T       **m_pphtExternalData;           //!< pointer to external data to be managed
    int     m_iExternalDataReadIdx;         //!< current read index in m_pphtExternalData
    int     m_iNumFramesExternalData;       //!< number of frames in m_pphtExternalData
};


#endif //#define __InputBuffSrcIf_HEADER_INCLUDED__

