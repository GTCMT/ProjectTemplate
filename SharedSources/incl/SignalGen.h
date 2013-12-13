#if !defined(__SignalGen_hdr__)
#define __SignalGen_hdr__

#define _USE_MATH_DEFINES
#include <math.h>

#include "ErrorDef.h"
class CSignalGen
{
public:
    static Error_t generateSine (float *pfOutBuf, float fFreqInHz, float fSampleFreqInHz, int iLength, float fAmplitude = 1.F, float fStartPhaseInRad = 0.F)
    {
        if (!pfOutBuf)
            return kFunctionInvalidArgsError;

        for (int i = 0; i < iLength; i++)
        {
            pfOutBuf[i] = fAmplitude * static_cast<float>(sin (2*M_PI*fFreqInHz * i/fSampleFreqInHz + fStartPhaseInRad));
        }

        return kNoError;
    }
    static Error_t generateDc (float *pfOutBuf, int iLength, float fAmplitude = 1.F)
    {
        if (!pfOutBuf)
            return kFunctionInvalidArgsError;

        for (int i = 0; i < iLength; i++)
        {
            pfOutBuf[i] = fAmplitude;
        }

        return kNoError;
    }
    static Error_t generateNoise (float *pfOutBuf, int iLength, float fAmplitude = 1.F)
    {
        if (!pfOutBuf)
            return kFunctionInvalidArgsError;

        for (int i = 0; i < iLength; i++)
        {
            pfOutBuf[i] = rand()*2*fAmplitude/RAND_MAX;
        }

        return kNoError;
    }
};
#endif // __SignalGen_hdr__