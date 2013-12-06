
#include <cassert>
#include <iostream>

#include "MyProjectConfig.h"

// 3rd party includes
#ifdef WITH_SNDLIB
#include "sndlib.h"
#endif //WITH_SNDLIB


#include "Util.h"

#include "AudioFileIf.h"

using namespace std;

// consts
const int CAudioFileIf::m_kiDefBlockLength      = 1024;
const int CAudioFileIf::m_iNumOfBytesPerSample  = 2;

CAudioFileIf::CAudioFileIf() : 
    m_piTmpBuff(0),
    m_bWithClipping(true),
    m_bIsInitialized(false)
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
    m_sCurrFileSpec.iNumChannels    = 2;

    m_bIsInitialized    = false,
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
        m_bIsInitialized    = true;
    }

    // open file
    m_File.open (cAudioFileName, ios::binary | ((eIoType & kFileRead)? ios::in : 0) | ((eIoType & kFileWrite)? ios::out : 0));

    if (!m_File.is_open ())
    {
        resetInstance (true);
        return kFileOpenError;
    }

    // allocate internal memory
    return allocMemory ();
}

Error_t CAudioFileIf::closeFile()
{
    if (!m_File.is_open ())
    {
        return kNoError;
    }    

    m_File.close ();

    // free internal memory
    return freeMemory ();
}

Error_t CAudioFileIf::getFileSpec( FileSpec_t &sFileSpec )
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
    if (!m_bIsInitialized)
        return kNotInitializedError;

    // update iLength to the number of frames actually read
    iLength = readDataRaw (ppfAudioData, iLength);
    if (iLength < 0)
        return kFileAccessError;

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
    if (!m_bIsInitialized)
        return kNotInitializedError;

    // update iLength
    iLength = writeDataRaw (ppfAudioData, iLength);
    if (iLength < 0)
        return kFileAccessError;

    return kNoError;
}

Error_t CAudioFileIf::setClippingEnabled( bool bIsEnabled /*= true*/ )
{
    m_bWithClipping = bIsEnabled;
    return kNoError;
}

Error_t CAudioFileIf::setPosition( long long iFrame /*= 0*/ )
{
    // check file status
    if (!m_File.is_open () || m_File.bad ())
        return kUnknownError;

    // check file properties
    if (!m_bIsInitialized)
        return kNotInitializedError;

    if (iFrame <= 0 || iFrame >= getLengthRaw())
        return kFunctionInvalidArgsError;

    return setPositionRaw(iFrame);

}

Error_t CAudioFileIf::setPosition( double dTimeInS /*= .0*/ )
{
    long long iPosInFrames = CUtil::double2int<long long>(dTimeInS * m_sCurrFileSpec.fSampleRate);

    return setPosition (iPosInFrames);
}

Error_t CAudioFileIf::getLength( long long &iLengthInFrames ) 
{
    iLengthInFrames = -1;

    // check file status
    if (!m_File.is_open () || m_File.bad ())
        return kUnknownError;

    // check file properties
    if (!m_bIsInitialized)
        return kNotInitializedError;

    iLengthInFrames = getLengthRaw ();

    return kNoError;
}

Error_t CAudioFileIf::getPosition( long long &iFrame )
{
    iFrame = getPositionRaw();

    return kNoError;
}

Error_t CAudioFileIf::getPosition( double &dTimeInS )
{
    long long iFrame;
    dTimeInS = -1.;
    Error_t iErr = getPosition(iFrame);

    if (iErr != kNoError)
        return iErr;

    dTimeInS = iFrame * (1./m_sCurrFileSpec.fSampleRate);
    return kNoError;
}

bool CAudioFileIf::isEof()
{
    long long iPosition,
        iLength;

    return m_File.eof();
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

long long CAudioFileIf::getLengthRaw() 
{
    assert(m_File);

    //static const int    iNumOfBytesPerSample = 2;
    long long iCurrPos  = getPositionRaw();
    long long iLength   = 0;

    m_File.seekg (0, m_File.end);
    
    iLength = static_cast<long long>(m_File.tellg())/m_iNumOfBytesPerSample/m_sCurrFileSpec.iNumChannels;

    setPositionRaw(iCurrPos);
    //m_File.seekg (iCurrPos);

    return iLength;
}

long long CAudioFileIf::getPositionRaw()
{
    //assert(m_File);
    return static_cast<long long>(m_File.tellg())/m_iNumOfBytesPerSample/m_sCurrFileSpec.iNumChannels;
}

Error_t CAudioFileIf::setPositionRaw( long long iFrame )
{
    // convert frame to bytes
    //static const int iNumOfBytesPerSample = 2;
    
    iFrame  *= m_iNumOfBytesPerSample * m_sCurrFileSpec.iNumChannels;
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

int CAudioFileIf::readDataRaw( float **ppfAudioData, int &iLength )
{
    int iNumFrames2Read = min (iLength, m_kiDefBlockLength/m_sCurrFileSpec.iNumChannels);
    int iNumFrames      = 0;

    // sanity check
    assert (ppfAudioData || ppfAudioData[0]);

    // ugly hack
    // a) only for 16 bit input
    // b) only for little endian
    while (iNumFrames2Read > 0)
    {
        //static const int    iNumOfBytesPerSample    = 2;
        static const float  fScale                  = 32768.F;

        int iCurrFrames = iNumFrames2Read;
        m_File.read (reinterpret_cast<char*>(m_piTmpBuff), m_iNumOfBytesPerSample*iNumFrames2Read*m_sCurrFileSpec.iNumChannels);

        iNumFrames2Read = min (iLength-iCurrFrames, m_kiDefBlockLength/m_sCurrFileSpec.iNumChannels);

        if (!m_File)
        {
            iCurrFrames     = (static_cast<int>(m_File.gcount ())/m_iNumOfBytesPerSample)/m_sCurrFileSpec.iNumChannels;
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

    // sanity check
    assert (ppfAudioData || ppfAudioData[0]);

    // very ugly hack
    // a) only for 16 bit output
    // b) disregarded endianess
    while (iIdx < iLength)
    {
        //static const int    iNumOfBytesPerSample    = 2;
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

        m_File.write (reinterpret_cast<char*>(m_piTmpBuff), m_iNumOfBytesPerSample*iNumFrames2Write*m_sCurrFileSpec.iNumChannels);

        if (!m_File)
        {
            break;
        }

        // update frame counter
        iIdx   += iNumFrames2Write;
    }
    return iIdx;
}
