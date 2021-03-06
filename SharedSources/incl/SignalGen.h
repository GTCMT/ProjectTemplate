#if !defined(__SignalGen_hdr__)
#define __SignalGen_hdr__

#define _USE_MATH_DEFINES
#include <math.h>
#include <cstdlib>

#include "ErrorDef.h"
#include "Util.h"

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
    static Error_t generateRect (float *pfOutBuf, float fFreqInHz, float fSampleFreqInHz, int iLength, float fAmplitude = 1.F)
    {
        if (!pfOutBuf)
            return kFunctionInvalidArgsError;

        float fPeriodLength = fSampleFreqInHz / fFreqInHz;
        for (int i = 0; i < iLength; i++)
        {
            // note that this can, depending on the period length, lead to error accumulation
            // FIXME when there is time
            if (i%CUtil::float2int<int>(fPeriodLength) < .5*fPeriodLength)
            {            
                pfOutBuf[i] = fAmplitude;
            }
            else
            {
                pfOutBuf[i] = -fAmplitude;
            }
        }

        return kNoError;
    }
    static Error_t generateSaw (float *pfOutBuf, float fFreqInHz, float fSampleFreqInHz, int iLength, float fAmplitude = 1.F)
    {
        if (!pfOutBuf)
            return kFunctionInvalidArgsError;

        float fIncr = 2*fAmplitude / fSampleFreqInHz * fFreqInHz;
        pfOutBuf[0] = 0;
        for (int i = 1; i < iLength; i++)
        {
            pfOutBuf[i] = fmodf(pfOutBuf[i-1] + fIncr + fAmplitude, 2*fAmplitude) - fAmplitude;
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
            pfOutBuf[i] = rand()*2*fAmplitude/RAND_MAX - 1;
        }

        return kNoError;
    }
};
#endif // __SignalGen_hdr__