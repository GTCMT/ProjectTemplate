#if !defined(__AudioFileIf_hdr__)
#define __AudioFileIf_hdr__

#include "ErrorDef.h"

class CAudioFileIf
{
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
        kFileBitStreamInt32,
        kFileBitStreamFloat32,

        kNumWordLengths
    };
    struct FileSpec_t
    {
        FileFormat_t    eFormat;
        BitStream_t     eBitStreamType;
        int             iNumChannels;
        float           fSampleRate;
    };
    Error_t openFile (std::string cAudioFileName, FileIoType_t eIoType);
    Error_t closeFile ();

    Error_t setFileSpecs (FileSpec_t const sFileSpec);
    Error_t getFileSpecs (FileSpec_t &sFileSpec);

    Error_t setPosition (long long iFrame = 0);
    Error_t setPosition (double dTimeInS = .0);

    Error_t setClippingEnabled (bool bIsEnabled = true);

    Error_t getPosition (long long &iFrame) const;
    Error_t getPosition (double &dTimeInS) const;
    //Error_t getNumOfChannels (int &iNumChannels) const;
    //Error_t getSampleRate (float &fSampleRate) const;
    Error_t getLength (long long &iLengthInFrames) const;
    Error_t getLength (double &dLengthInFrames) const;

    Error_t readData (float **ppfAudioData, long long &iNumFrames);
    Error_t writeData (float **ppfAudioData, long long iNumFrames);
};

#endif // #if !defined(__AudioFileIf_hdr__)
