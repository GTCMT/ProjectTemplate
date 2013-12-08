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
        kFileFormatAiff,
        kFileFormatRaw,
        kFileFormatUnknown,

        kNumFileFormats
    };
    enum BitStream_t
    {
        kFileBitStreamInt16,
        kFileBitStreamFloat32,
        kFileBitStreamUnknown,

        kNumWordLengths
    };
    struct FileSpec_t
    {
        FileFormat_t    eFormat;
        BitStream_t     eBitStreamType;
        int             iNumChannels;
        float           fSampleRate;
    };

    static Error_t createInstance (CAudioFileIf*& pCInstance);
    static Error_t destroyInstance (CAudioFileIf*& pCInstance);

    virtual Error_t openFile (std::string cAudioFileName, FileIoType_t eIoType, FileSpec_t const *psFileSpec = 0) = 0;
    virtual Error_t closeFile () = 0;

    virtual Error_t readData (float **ppfAudioData, int &iNumFrames);
    virtual Error_t writeData (float **ppfAudioData, int iNumFrames);

    Error_t getFileSpec (FileSpec_t &sFileSpec);

    virtual Error_t setPosition (long long iFrame = 0);
    Error_t setPosition (double dTimeInS = .0);

    Error_t setClippingEnabled (bool bIsEnabled = true);
    bool isClippingEnabled () {return m_bWithClipping;};

    Error_t getPosition (long long &iFrame);
    Error_t getPosition (double &dTimeInS);
    Error_t getLength (long long &iLengthInFrames) ;
    Error_t getLength (double &dLengthInSeconds) ;

    virtual bool isEof () = 0;
    virtual bool isOpen () = 0;
    virtual bool isInitialized ();
    virtual Error_t resetInstance (bool bFreeMemory = false);

protected:

    CAudioFileIf ();
    virtual ~CAudioFileIf ();
    virtual Error_t freeMemory ();
    virtual Error_t allocMemory ();

    Error_t setInitialized (bool bInitialized = true);
    Error_t setIoType (FileIoType_t eIoType);
    FileIoType_t getIoType () const;
    Error_t setFileSpec (const FileSpec_t *pFileSpec);
    int getNumChannels () const;
    long long convFrames2Bytes (long long iNumFrames);
    long long convBytes2Frames (long long iNumFrames);
    Error_t setNumBytesPerSample (int iNumBytes);
    int getNumBytesPerSample () const;
    int getNumBitsPerSample () const {return (m_iNumBytesPerSample<<3);}

    static const int    m_kiDefBlockLength;     //!< buffer length for read and write operations

private:
    virtual Error_t initDefaults ();

    virtual int readDataIntern (float **ppfAudioData, int iNumFrames) = 0;
    virtual int writeDataIntern (float **ppfAudioData, int iNumFrames) = 0;
    virtual long long getLengthIntern() = 0;
    virtual long long getPositionIntern() = 0;
    virtual Error_t setPositionIntern( long long iFrame ) = 0;

    FileSpec_t      m_sCurrFileSpec;            //!< current file specifications
    FileIoType_t    m_eIoType;                  //!< read or write

    bool            m_bWithClipping;            //!< true if abs(values ) > 1 should be clipped
    bool            m_bIsInitialized;           //!< true if initialized
    int             m_iNumBytesPerSample;     //!< number of bytes per sample for the raw pcm IO option without sndlib

};

class CAudioFileRaw : public CAudioFileIf
{
public:
    CAudioFileRaw ();
    virtual ~CAudioFileRaw ();

    Error_t openFile (std::string cAudioFileName, FileIoType_t eIoType, FileSpec_t const *psFileSpec = 0);
    Error_t closeFile ();
    bool isEof ();
    bool isOpen ();

private:
    Error_t freeMemory ();
    Error_t allocMemory ();
    int readDataIntern (float **ppfAudioData, int iNumFrames);
    int writeDataIntern (float **ppfAudioData, int iNumFrames);
    long long getLengthIntern();
    long long getPositionIntern();
    Error_t setPositionIntern( long long iFrame );

    float scaleUp (float fSample2Clip);
    float scaleDown (float fSample2Scale);

    std::fstream        m_File;                 //!< raw pcm file handle
    short               *m_piTmpBuff;           //!< temporary buffer for 16 bit integer values
};


class CAudioFileSndLib : public CAudioFileIf
{
public:
    CAudioFileSndLib ();
    virtual ~CAudioFileSndLib ();

    Error_t openFile (std::string cAudioFileName, FileIoType_t eIoType, FileSpec_t const *psFileSpec = 0);
    Error_t closeFile ();
    bool isEof ();
    bool isOpen ();
    Error_t freeMemory ();
    Error_t allocMemory ();

private:
    int readDataIntern (float **ppfAudioData, int iNumFrames);
    int writeDataIntern (float **ppfAudioData, int iNumFrames);
    long long getLengthIntern();
    long long getPositionIntern();
    Error_t setPositionIntern( long long iFrame );

    int m_FileHandle;                           //!< sndlib file handle

    long long m_lFrameCnt;
    long long m_lFileLength;
    double    **m_ppdTmpBuff;                     //!< temporary buffer for double values

};
#endif // #if !defined(__AudioFileIf_hdr__)
