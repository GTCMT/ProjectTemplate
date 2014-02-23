#if !defined(__InputBuffSrcIf_HEADER_INCLUDED__)
#define __InputBuffSrcIf_HEADER_INCLUDED__


#include "ErrorDef.h"

/*! added buffer class for faster implementation of variable input block size vs. constant processing block size

    USAGE

        // add new input data of any length
        pInputBuffSrcIf->setDataPtr2Hold(apfTmpPtr, iInpLength);
        while (pInputBuffSrcIf->getBlock(ppfProcBuff, kiProcBuffLength))
        {
            // do processing on ppfProcBuff with FIXED length kiProcBuffLength          
        }
        // store remaining data
        pInputBuffSrcIf->releaseDataPtr();
 
        // at the end of the processing - get remaining frames from the internal buffer if needed
        pInputBuffSrcIf->flush(ppfProcBuff);
*/

class CInputBuffSrcIf
{
public:
    CInputBuffSrcIf (int iNumOfChannels, int iMaxOutputLength = 1024, int iInitialLatency = 0){};
    virtual ~CInputBuffSrcIf (){};

    virtual Error_t setDataPtr2Hold (float **ppNewData, int iNumOfElements) = 0;
    virtual Error_t releaseDataPtr () = 0;
    
    virtual Error_t getBlock (float **ppBlock, int iOutputBufferSize, int iIncIdxBy = 0) = 0;

    virtual Error_t reset () = 0;

    virtual int  flush (float **ppBlock) = 0;
};



#endif //#define __InputBuffSrcIf_HEADER_INCLUDED__

