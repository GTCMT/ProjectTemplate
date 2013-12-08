#include "MyProjectConfig.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>

#include "UnitTest++.h"

#include "SignalGen.h"

#include "AudioFileIf.h"

//TEST(Fail)
//{
//    CHECK(false);
//}

extern std::string cTestDataDir;

SUITE(AudioIo)
{
    struct AudioIoData
    {
        AudioIoData() 
        {
            float fPhase    = 0;

            stFileSpec.eBitStreamType  = CAudioFileIf::kFileBitStreamInt16;
            stFileSpec.eFormat         = CAudioFileIf::kFileFormatRaw;
            stFileSpec.fSampleRate     = 44100.F;
            stFileSpec.iNumChannels    = m_iNumChannels;

            CAudioFileIf::createInstance(m_pCAudioFile);
            m_ppfAudioData  = 0;
            m_ppfTmp        = 0;
            m_ppfAudioData  = new float*[m_iNumChannels];
            m_ppfTmp        = new float*[m_iNumChannels];
            for (int i = 0; i < m_iNumChannels; i++)
                m_ppfAudioData[i]   = new float[m_iBuffLength];

            for (int i = 0; i < m_iNumChannels; i++)
            {
                CSignalGen::generateSine(m_ppfAudioData[i], 441.F, stFileSpec.fSampleRate, m_iBuffLength, .6F, fPhase);
                fPhase  += static_cast<float>(M_PI_2);
            }

        }
        
        ~AudioIoData() 
        {
            assert (m_ppfAudioData != 0);
            for (int i = 0; i < m_iNumChannels; i++)
                delete [] m_ppfAudioData[i];
            delete [] m_ppfAudioData;
            delete [] m_ppfTmp;

            CAudioFileIf::destroyInstance(m_pCAudioFile);
       }

        float **m_ppfAudioData;
        float **m_ppfTmp;
        CAudioFileIf    *m_pCAudioFile;
        CAudioFileIf::FileSpec_t stFileSpec;

        static const int m_iBuffLength  = 1027;
        static const int m_iBlockLength = 17;
        static const int m_iNumChannels = 2;
    };


    TEST_FIXTURE(AudioIoData, FileReadRaw)
    {
        if (cTestDataDir.empty())
        {
            CHECK(false);
        }
        int iReadIdx = 0;
        float **ppfReadData  = new float*[m_iNumChannels];
        for (int i = 0; i < m_iNumChannels; i++)
            ppfReadData[i]   = new float[m_iBuffLength];
 
        m_pCAudioFile->openFile (cTestDataDir+"/ref.pcm", CAudioFileIf::kFileRead, &stFileSpec);

        while (!m_pCAudioFile->isEof ())
        {
            int iNumRead = m_iBlockLength;
            for (int i = 0; i < m_iNumChannels; i++)
                m_ppfTmp[i] = &ppfReadData[i][iReadIdx];

            m_pCAudioFile->readData (m_ppfTmp, iNumRead);
            iReadIdx   += iNumRead;
        }

        // check identity
        for (int i = 0; i < m_iNumChannels; i++)
            CHECK_ARRAY_CLOSE(m_ppfAudioData[i], ppfReadData[i], m_iBuffLength, 1e-3); // succeeds

        // free allocated memory
        for (int i = 0; i < m_iNumChannels; i++)
            delete [] ppfReadData[i];
        delete [] ppfReadData;
    }

    TEST_FIXTURE(AudioIoData, FileReadRawOffset)
    {
        if (cTestDataDir.empty())
        {
            CHECK(false);
            return;
        }
        const long long iOffset = 327;
        int iReadIdx = 0;
        float **ppfReadData  = new float*[m_iNumChannels];
        for (int i = 0; i < m_iNumChannels; i++)
            ppfReadData[i]   = new float[m_iBuffLength];

        m_pCAudioFile->openFile (cTestDataDir+"/ref.pcm", CAudioFileIf::kFileRead, &stFileSpec);

        m_pCAudioFile->setPosition (iOffset);

        while (!m_pCAudioFile->isEof ())
        {
            int iNumRead = m_iBlockLength;
            for (int i = 0; i < m_iNumChannels; i++)
                m_ppfTmp[i] = &ppfReadData[i][iReadIdx];

            m_pCAudioFile->readData (m_ppfTmp, iNumRead);
            iReadIdx   += iNumRead;
        }

        // check identity
        for (int i = 0; i < m_iNumChannels; i++)
            CHECK_ARRAY_CLOSE(&m_ppfAudioData[i][iOffset], ppfReadData[i], m_iBuffLength-iOffset, 1e-3); // succeeds

        // free allocated memory
        for (int i = 0; i < m_iNumChannels; i++)
            delete [] ppfReadData[i];
        delete [] ppfReadData;

    }

    TEST_FIXTURE(AudioIoData, FileReadAllAtOnce)
    {
        if (cTestDataDir.empty())
        {
            CHECK(false);
            return;
        }
       //note that the file length is longer than the internal read block size
        long long   iFileLength = 0;
        int         iNumRead    = 0;
        float **ppfReadData     = new float*[m_iNumChannels];
        
        for (int i = 0; i < m_iNumChannels; i++)
            ppfReadData[i]      = new float[m_iBuffLength];

        m_pCAudioFile->openFile (cTestDataDir+"/ref.pcm", CAudioFileIf::kFileRead, &stFileSpec);
        m_pCAudioFile->getLength (iFileLength);
        iNumRead    = static_cast<int>(iFileLength);

        m_pCAudioFile->readData (ppfReadData, iNumRead);

        // check identity
        CHECK(iNumRead == iFileLength);
        for (int i = 0; i < m_iNumChannels; i++)
            CHECK_ARRAY_CLOSE(m_ppfAudioData[i], ppfReadData[i], m_iBuffLength, 1e-3); // succeeds

        // free allocated memory
        for (int i = 0; i < m_iNumChannels; i++)
            delete [] ppfReadData[i];
        delete [] ppfReadData;
    }

    TEST_FIXTURE(AudioIoData, FileWriteReadRaw)
    {
        if (cTestDataDir.empty())
        {
            CHECK(false);
            return;
        }
        int iNumRemainingFrames = m_iBuffLength;
        Error_t err = kUnknownError;
        
        err = m_pCAudioFile->openFile (cTestDataDir+"/test.pcm", CAudioFileIf::kFileWrite, &stFileSpec);
        CHECK (err == kNoError);

        // put data
        while (iNumRemainingFrames > 0)
        {
            for (int i = 0; i < m_iNumChannels; i++)
                m_ppfTmp[i] = &m_ppfAudioData[i][m_iBuffLength - iNumRemainingFrames];
            int iPutFrames = std::min(m_iBlockLength, iNumRemainingFrames);

            m_pCAudioFile->writeData (m_ppfTmp, iPutFrames);

            iNumRemainingFrames -= iPutFrames;
        }
        m_pCAudioFile->closeFile ();
        m_pCAudioFile->resetInstance ();

        // read the file and compare
        int iReadIdx = 0;
        float **ppfReadData  = new float*[m_iNumChannels];
        for (int i = 0; i < m_iNumChannels; i++)
            ppfReadData[i]   = new float[m_iBuffLength];

        err = m_pCAudioFile->openFile (cTestDataDir+"/test.pcm", CAudioFileIf::kFileRead, &stFileSpec);
        CHECK (err == kNoError);

        while (!m_pCAudioFile->isEof ())
        {
            int iNumRead = m_iBlockLength;
            for (int i = 0; i < m_iNumChannels; i++)
                m_ppfTmp[i] = &ppfReadData[i][iReadIdx];

            m_pCAudioFile->readData (m_ppfTmp, iNumRead);
            iReadIdx   += iNumRead;
        }

        // check identity
        for (int i = 0; i < m_iNumChannels; i++)
            CHECK_ARRAY_CLOSE(m_ppfAudioData[i], ppfReadData[i], m_iBuffLength, 1e-3); // succeeds

        // free allocated memory
        for (int i = 0; i < m_iNumChannels; i++)
            delete [] ppfReadData[i];
        delete [] ppfReadData;


        // cleanup: delete file from disk -- permissions problem under win
        remove ((cTestDataDir+"/test.pcm").c_str());
        
    }
#ifdef WITH_SNDLIB
    TEST_FIXTURE(AudioIoData, FileWriteReadWav)
    {
        if (cTestDataDir.empty())
        {
            CHECK(false);
            return;
        }
        const std::string cExt(".wav");
        int iNumRemainingFrames = m_iBuffLength;
        Error_t err = kUnknownError;

        stFileSpec.eFormat         = CAudioFileIf::kFileFormatWav;

        err = m_pCAudioFile->openFile (cTestDataDir+"/test"+cExt, CAudioFileIf::kFileWrite, &stFileSpec);
        CHECK (err == kNoError);

        // put data
        while (iNumRemainingFrames > 0)
        {
            for (int i = 0; i < m_iNumChannels; i++)
                m_ppfTmp[i] = &m_ppfAudioData[i][m_iBuffLength - iNumRemainingFrames];
            int iPutFrames = std::min(m_iBlockLength, iNumRemainingFrames);

            m_pCAudioFile->writeData (m_ppfTmp, iPutFrames);

            iNumRemainingFrames -= iPutFrames;
        }
        m_pCAudioFile->closeFile ();
        m_pCAudioFile->resetInstance ();

        // read the file and compare
        int iReadIdx = 0;
        float **ppfReadData  = new float*[m_iNumChannels];
        for (int i = 0; i < m_iNumChannels; i++)
            ppfReadData[i]   = new float[m_iBuffLength];

        err = m_pCAudioFile->openFile (cTestDataDir+"/test"+cExt, CAudioFileIf::kFileRead, &stFileSpec);
        CHECK (err == kNoError);

        while (!m_pCAudioFile->isEof ())
        {
            int iNumRead = m_iBlockLength;
            for (int i = 0; i < m_iNumChannels; i++)
                m_ppfTmp[i] = &ppfReadData[i][iReadIdx];

            m_pCAudioFile->readData (m_ppfTmp, iNumRead);
            iReadIdx   += iNumRead;
        }

        // check identity
        for (int i = 0; i < m_iNumChannels; i++)
            CHECK_ARRAY_CLOSE(m_ppfAudioData[i], ppfReadData[i], m_iBuffLength, 1e-3); // succeeds

        // free allocated memory
        for (int i = 0; i < m_iNumChannels; i++)
            delete [] ppfReadData[i];
        delete [] ppfReadData;


        // cleanup: delete file from disk -- permissions problem under win
        remove ((cTestDataDir+"/test"+cExt).c_str());

    }
#endif // WITH_SNDLIB
}

#endif //WITH_TESTS