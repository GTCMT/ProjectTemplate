#if !defined(__Fft_hdr__)
#define __Fft_hdr__

#include "ErrorDef.h"

class CFft
{
public:
    typedef float complex_t;

    /*! defines the window function to use -- a custom window can also be used (\sa overrideWindow) */
    enum WindowFunction_t
    {
        //kWindowNone,            //!< rectangular window (see Windowing_t::kNoWindow)
        kWindowSine,            //!< sinusoidal window
        kWindowHann,            //!< von-Hann window
        kWindowHamming,         //!< Hamming window

        kNumWindows
    };
    enum Windowing_t
    {
        kNoWindow   = 0x00L,    //!< do not apply any window
        kPreWindow  = 0x01L,    //!< apply window function before FFT
        kPostWindow = 0x02L     //!< apply window function after IFFT
    };

    static Error_t createInstance (CFft*& pCFft);
    static Error_t destroyInstance (CFft*& pCFft);
    
    Error_t initInstance (int iBlockLength, int iZeroPadFactor, WindowFunction_t eWindow = kWindowHann, Windowing_t eWindowing = kPreWindow);
    Error_t resetInstance ();
 
    Error_t overrideWindow (const float *pfNewWindow);
    Error_t getWindow (float *pfWindow) const;

    Error_t doFft (complex_t *pfSpectrum, const float *pfInput);
    Error_t doInvFft (float *pfOutput, const complex_t *pfSpectrum);

    Error_t getMagnitude (float *pfMag, const complex_t *pfSpectrum) const;
    Error_t getPhase(float *pfPhase, const complex_t *pfSpectrum) const;
    Error_t splitRealImag(float *pfReal, float *pfImag, const complex_t *pfSpectrum) const;
    Error_t mergeRealImag(complex_t *pfSpectrum, const float *pfReal, const float *pfImag) const;

    float freq2bin (float fFreqInHz, float fSampleRateInHz) const;
    float bin2freq (int iBinIdx, float fSampleRateInHz) const;

protected:
    CFft ();
    virtual ~CFft () {};

private:
    Error_t allocMemory ();
    Error_t freeMemory ();
    Error_t computeWindow (WindowFunction_t eWindow);

    float   *m_pfProcessBuff;
    float   *m_pfWindowBuff;

    int     m_iDataLength;
    int     m_iFftLength;

    Windowing_t m_ePrePostWindowOpt;

    bool    m_bIsInitialized;

    static const float m_Pi;
    static const float m_Pi2;
};

#endif // #if !defined(__Fft_hdr__)



