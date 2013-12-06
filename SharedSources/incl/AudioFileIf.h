#if !defined(__AudioFileIf_hdr__)
#define __AudioFileIf_hdr__

#include <string>
#include <fstream>
#include "ErrorDef.h"

class CAudioFileIf
{
public:
    enum FileIoType_t
    {
        kFileRead   = 0x1L,
        kFileWrite  = 0x2L,

        kNumFileOpenTypes = 2
    };
    enum FileFormat_t
    {
        kFileFormatWav,
        kFileFormatRaw,

        kNumFileFormats
    };
    enum BitStream_t
    {
        kFileBitStreamInt16,
        //kFileBitStreamInt32,
        //kFileBitStreamFloat32,

        kNumWordLengths
    };
    struct FileSpec_t
    {
        FileFormat_t    eFormat;
        BitStream_t     eBitStreamType;
        int             iNumChannels;
        float           fSampleRate;
    };

    CAudioFileIf ();;
    virtual ~CAudioFileIf ();

    Error_t openFile (std::string cAudioFileName, FileIoType_t eIoType, FileSpec_t const *psFileSpec = 0);
    Error_t closeFile ();

    Error_t getFileSpec (FileSpec_t &sFileSpec);

    Error_t setPosition (long long iFrame = 0);
    Error_t setPosition (double dTimeInS = .0);

    Error_t setClippingEnabled (bool bIsEnabled = true);

    Error_t getPosition (long long &iFrame);
    Error_t getPosition (double &dTimeInS);
    Error_t getLength (long long &iLengthInFrames) ;
    Error_t getLength (double &dLengthInSeconds) ;

    Error_t readData (float **ppfAudioData, int &iNumFrames);
    Error_t writeData (float **ppfAudioData, int iNumFrames);

    bool isEof ();

    Error_t resetInstance (bool bFreeMemory = false);

private:
    Error_t freeMemory ();
    Error_t allocMemory ();
    Error_t initDefaults ();

    int readDataRaw (float **ppfAudioData, int &iNumFrames);
    int writeDataRaw (float **ppfAudioData, int iNumFrames);
    long long getLengthRaw() ;
    long long getPositionRaw();
    Error_t setPositionRaw( long long iFrame );


    FileSpec_t      m_sCurrFileSpec;

    short           *m_piTmpBuff;

    std::fstream    m_File;

    bool            m_bWithClipping;
    bool            m_bIsInitialized;

    static const int    m_kiDefBlockLength;     //!< buffer length for read and write operations
    static const int    m_iNumOfBytesPerSample; //!< number of bytes per sample for the raw pcm IO option without sndlib

};

#endif // #if !defined(__AudioFileIf_hdr__)
