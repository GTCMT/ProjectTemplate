
#include <cassert>
#include <iostream>

#include "MyProjectConfig.h"

//#ifdef WITH_SNDLIB
//#include "sndlib.h"
//#endif //WITH_SNDLIB
#include "Util.h"

#include "AudioFileIf.h"

using namespace std;

// consts
const int CAudioFileIf::m_kiDefBlockLength = 1024;

CAudioFileIf::CAudioFileIf() : 
    m_piTmpBuff(0),
    m_bWithClipping(true),
    m_iFileLengthInFrames(-1)
{
    resetInstance (true);
}

CAudioFileIf::~CAudioFileIf()
{
    resetInstance (true);
}

Error_t CAudioFileIf::resetInstance( bool bFreeMemory /*= false*/ )
{
    Error_t eErr = closeFile ();
    if (eErr != kNoError)
        return eErr;

    if (bFreeMemory)
    {
        eErr = freeMemory ();
        if (eErr != kNoError)
            return eErr;
    }
  
    eErr = initDefaults ();
    if (eErr != kNoError)
        return eErr;

    return eErr;
}

Error_t CAudioFileIf::freeMemory()
{
    if (!m_piTmpBuff)
        return kNoError;

    delete [] m_piTmpBuff;
    m_piTmpBuff  = 0;

    return kNoError;
}

Error_t CAudioFileIf::allocMemory()
{
    freeMemory ();

    m_piTmpBuff = new short [m_kiDefBlockLength];

    if (!m_piTmpBuff)
        return kMemError;
    else
        return kNoError;
}

Error_t CAudioFileIf::initDefaults()
{
    m_sCurrFileSpec.eBitStreamType  = kFileBitStreamInt16;
    m_sCurrFileSpec.eFormat         = kFileFormatRaw;
    m_sCurrFileSpec.fSampleRate     = 48000;
    m_sCurrFileSpec.iNumChannels    = -1;

    m_iFileLengthInFrames           = -1;

    setClippingEnabled ();

    return kNoError;
}

Error_t CAudioFileIf::openFile( std::string cAudioFileName, FileIoType_t eIoType, FileSpec_t const *psFileSpec /*= 0*/ )
{
    resetInstance (true);

    // set file spec
    if (psFileSpec)
    {
        memcpy (&m_sCurrFileSpec, psFileSpec, sizeof (FileSpec_t));
    }

    // open file
    m_File.open (cAudioFileName, ios::binary | ((eIoType & kFileRead)? ios::in : 0) | ((eIoType & kFileWrite)? ios::out : 0));

    if (!m_File.is_open ())
    {
        resetInstance (true);
        return kFileOpenError;
    }
    long long iLength;
    getLength (iLength);

    // allocate internal memory
    allocMemory ();
    
    return kNoError;
}

Error_t CAudioFileIf::closeFile()
{
    if (!m_File.is_open ())
    {
        return kNoError;
    }    

    m_File.close ();

    // free internal memory
    freeMemory ();

    return kNoError;
}

Error_t CAudioFileIf::getFileSpecs( FileSpec_t &sFileSpec )
{
    memcpy (&sFileSpec, &m_sCurrFileSpec, sizeof(FileSpec_t));

    return kNoError;
}

Error_t CAudioFileIf::readData( float **ppfAudioData, int &iLength )
{
    // check parameters
    if (!ppfAudioData || !ppfAudioData[0] || iLength < 0)
        return kFunctionInvalidArgsError;

    // check file status
    if (!m_File.is_open () || m_File.bad ())
        return kUnknownError;

    // check file properties
    if (m_sCurrFileSpec.iNumChannels <= 0)
        return kNotInitializedError;

    // update iLength to the number of frames actually read
    iLength = readDataRaw (ppfAudioData, iLength);

    return kNoError;
}

Error_t CAudioFileIf::writeData( float **ppfAudioData, int iLength )
{

    // check parameters
    if (!ppfAudioData || !ppfAudioData[0] || iLength < 0)
        return kFunctionInvalidArgsError;

    // check file status
    if (!m_File.is_open () || m_File.bad ())
        return kUnknownError;

    // check file properties
    if (m_sCurrFileSpec.iNumChannels <= 0)
        return kNotInitializedError;

    writeDataRaw (ppfAudioData, iLength);

    return kNoError;
}

Error_t CAudioFileIf::setClippingEnabled( bool bIsEnabled /*= true*/ )
{
    m_bWithClipping = bIsEnabled;
    return kNoError;
}

int CAudioFileIf::readDataRaw( float **ppfAudioData, int &iLength )
{
    int iNumFrames2Read = min (iLength, m_kiDefBlockLength/m_sCurrFileSpec.iNumChannels);
    int iNumFrames      = 0;

    // very ugly hack
    // a) only for 16 bit input
    // b) only for little endian
    while (iNumFrames2Read > 0)
    {
        static const int    iNumOfBytesPerSample    = 2;
        static const float  fScale                  = 32768.F;

        int iCurrFrames = iNumFrames2Read;
        m_File.read (reinterpret_cast<char*>(m_piTmpBuff), iNumOfBytesPerSample*iNumFrames2Read*m_sCurrFileSpec.iNumChannels);

        iNumFrames2Read = min (iLength-iCurrFrames, m_kiDefBlockLength/m_sCurrFileSpec.iNumChannels);

        if (!m_File)
        {
            iCurrFrames     = (m_File.gcount ()/iNumOfBytesPerSample)/m_sCurrFileSpec.iNumChannels;
            iNumFrames2Read = 0;
        }

        // copy the data
        for (int i = 0; i < iCurrFrames; i++)
        {
            for (int iCh = 0; iCh < m_sCurrFileSpec.iNumChannels; iCh++)
            {
                ppfAudioData[iCh][iNumFrames+i] = static_cast<float> (m_piTmpBuff[i*m_sCurrFileSpec.iNumChannels+iCh])/fScale;
            }

        }
        // update frame counters
        iLength        -= iCurrFrames;
        iNumFrames     += iCurrFrames;

        assert (iLength >= 0);
    }

    // update iLength to the number of frames actually read
    return iNumFrames;

}

int CAudioFileIf::writeDataRaw( float **ppfAudioData, int iLength )
{
    int iIdx                = 0;
    
    // very ugly hack
    // a) only for 16 bit output
    // b) disregarded endianess
    while (iIdx < iLength)
    {
        static const int    iNumOfBytesPerSample    = 2;
        static const float  fScale                  = 32768.F;

        int iNumFrames2Write = min (iLength-iIdx, m_kiDefBlockLength/m_sCurrFileSpec.iNumChannels);

        // copy the data
        for (int i = 0; i < iNumFrames2Write; i++)
        {
            for (int iCh = 0; iCh < m_sCurrFileSpec.iNumChannels; iCh++)
            {
                float fTmp = ppfAudioData[iCh][iIdx+i] * 32768.F;
                if (m_bWithClipping)
                {
                    fTmp = min (fTmp, 32767.F);
                    fTmp = max (fTmp, -32768.F);
                }
                m_piTmpBuff[i*m_sCurrFileSpec.iNumChannels+iCh] = CUtil::float2int<short>(fTmp);
            }
        }

        m_File.write (reinterpret_cast<char*>(m_piTmpBuff), iNumOfBytesPerSample*iNumFrames2Write*m_sCurrFileSpec.iNumChannels);

        if (!m_File)
        {
            break;
        }

        // update frame counter
        iIdx   += iNumFrames2Write;
    }
    return iIdx;
}

Error_t CAudioFileIf::setPosition( long long iFrame /*= 0*/ )
{
    // check file status
    if (!m_File.is_open () || m_File.bad ())
        return kUnknownError;

    // check file properties
    if (m_sCurrFileSpec.iNumChannels <= 0)
        return kNotInitializedError;

    if (iFrame <= 0 || iFrame >= m_iFileLengthInFrames)
        return kFunctionInvalidArgsError;

    return setPositionRaw(iFrame);

}

Error_t CAudioFileIf::setPosition( double dTimeInS /*= .0*/ )
{
    long long iPosInFrames = CUtil::float2int<long long>(dTimeInS * m_sCurrFileSpec.fSampleRate);

    return setPosition (iPosInFrames);
}

Error_t CAudioFileIf::getLength( long long &iLengthInFrames )
{
    iLengthInFrames = -1;

    // check file status
    if (!m_File.is_open () || m_File.bad ())
        return kUnknownError;

    // check file properties
    if (m_sCurrFileSpec.iNumChannels <= 0)
        return kNotInitializedError;

    if (m_iFileLengthInFrames < 0)
    {
        m_iFileLengthInFrames   = getLengthRaw();
    }

    iLengthInFrames = m_iFileLengthInFrames;

    return kNoError;
}

Error_t CAudioFileIf::getLength( double &dLengthInSeconds )
{
    long long iLengthInFrames;
    dLengthInSeconds = -1.;
    Error_t iErr = getLength (iLengthInFrames);

    if (iErr != kNoError)
        return iErr;

    dLengthInSeconds = iLengthInFrames * (1./m_sCurrFileSpec.fSampleRate);
    return kNoError;
}

int CAudioFileIf::getLengthRaw()
{
    assert(m_File);

    static const int    iNumOfBytesPerSample = 2;
    int iCurrPos    = m_File.tellg();
    int iLength     = 0;

    m_File.seekg (0, m_File.end);
    
    iLength = m_File.tellg()/iNumOfBytesPerSample/m_sCurrFileSpec.iNumChannels;

    m_File.seekg (iCurrPos);

    return iLength;
}

Error_t CAudioFileIf::setPositionRaw( long long iFrame )
{
    // convert frame to bytes
    static const int iNumOfBytesPerSample = 2;
    
    iFrame  *= iNumOfBytesPerSample * m_sCurrFileSpec.iNumChannels;
    m_File.seekg (iFrame, m_File.beg);
    
    if (!m_File)
    {
        return kFileAccessError;
    }
    else
    {
        return kNoError;
    }
}
