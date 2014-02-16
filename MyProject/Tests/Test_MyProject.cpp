#include "MyProjectConfig.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>

#include "UnitTest++.h"

#include "SignalGen.h"

#include "MyProject.h"

SUITE(MyProject)
{
    struct MyProjectData
    {
        MyProjectData()  :
            m_pMyProject(0),
            m_ppfInputData(0),
            m_ppfOutputData(0),
            m_kiDataLength(35131),
            m_fMaxModWidth(.5F),
            m_iBlockLength(171),
            m_iNumChannels(3),
            m_fSampleRate(31271),
            m_fModWidth(.1F),
            m_fModFreq(1.F)
        {
            CMyProject::createInstance(m_pMyProject);
            m_pMyProject->initInstance(m_fMaxModWidth,m_fSampleRate,m_iNumChannels);

            m_ppfInputData = new float*[m_iNumChannels];
            m_ppfOutputData = new float*[m_iNumChannels];
            m_ppfInputTmp = new float*[m_iNumChannels];
            m_ppfOutputTmp = new float*[m_iNumChannels];
            for (int i = 0; i < m_iNumChannels; i++)
            {
                m_ppfInputData[i] = new float [m_kiDataLength];
                CSignalGen::generateSine(m_ppfInputData[i], 800, m_fSampleRate, m_kiDataLength, .6F, 0);
                m_ppfOutputData[i] = new float [m_kiDataLength];
                CUtil::setZero(m_ppfOutputData[i], m_kiDataLength); 
            }

        }

        ~MyProjectData() 
        {
            for (int i = 0; i < m_iNumChannels; i++)
            {
                delete [] m_ppfOutputData[i];
                delete [] m_ppfInputData[i];
            }
            delete [] m_ppfOutputTmp;
            delete [] m_ppfInputTmp;
            delete [] m_ppfOutputData;
            delete [] m_ppfInputData;

            CMyProject::destroyInstance(m_pMyProject);
        }

        void process ()
        {
            int iNumFramesRemaining = m_kiDataLength;
            while (iNumFramesRemaining > 0)
            {
                int iNumFrames = std::min(iNumFramesRemaining, m_iBlockLength);

                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c] = &m_ppfInputData[c][m_kiDataLength - iNumFramesRemaining];
                    m_ppfOutputTmp[c] = &m_ppfOutputData[c][m_kiDataLength - iNumFramesRemaining];
                }
                m_pMyProject->process(m_ppfInputTmp, m_ppfOutputTmp, iNumFrames);

                iNumFramesRemaining -= iNumFrames;
            }
        }

        CMyProject *m_pMyProject;
        float **m_ppfInputData,
            **m_ppfOutputData,
            **m_ppfInputTmp,
            **m_ppfOutputTmp;
        const int m_kiDataLength;
        float m_fMaxModWidth;
        int m_iBlockLength;
        int m_iNumChannels;
        float m_fSampleRate;
        float m_fModWidth,
            m_fModFreq;

    };

    TEST_FIXTURE(MyProjectData, VibModWidthZero)
    {
        m_pMyProject->setParam(CMyProject::kParamModFreqInHz, 20);
        m_pMyProject->setParam(CMyProject::kParamModWidthInS, 0);

        process();

        int iDelay = CUtil::float2int<int>(m_fMaxModWidth*m_fSampleRate+1);
        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], &m_ppfOutputData[c][iDelay], m_kiDataLength-iDelay, 1e-3F);
    }

    TEST_FIXTURE(MyProjectData, VibDc)
    {
        m_pMyProject->setParam(CMyProject::kParamModFreqInHz, 2);
        m_pMyProject->setParam(CMyProject::kParamModWidthInS, .1);
        for (int c = 0; c < m_iNumChannels; c++)
        {
            CSignalGen::generateDc(m_ppfInputData[c], m_kiDataLength, (c+1)*.1);
        }

        process();

        int iDelay = CUtil::float2int<int>(m_fMaxModWidth*m_fSampleRate+1);
        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], &m_ppfOutputData[c][iDelay], m_kiDataLength-iDelay, 1e-3F);
    }

    TEST_FIXTURE(MyProjectData, VibVaryingBlocksize)
    {
        m_pMyProject->setParam(CMyProject::kParamModFreqInHz, 2);
        m_pMyProject->setParam(CMyProject::kParamModWidthInS, .1);

        process();

        m_pMyProject->resetInstance();
        m_pMyProject->initInstance(m_fMaxModWidth,m_fSampleRate,m_iNumChannels);
        m_pMyProject->setParam(CMyProject::kParamModFreqInHz, 2);
        m_pMyProject->setParam(CMyProject::kParamModWidthInS, .1);
        {
            int iNumFramesRemaining = m_kiDataLength;
            while (iNumFramesRemaining > 0)
            {

                int iNumFrames = std::min(iNumFramesRemaining, rand()/RAND_MAX*17000);

                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c] = &m_ppfInputData[c][m_kiDataLength - iNumFramesRemaining];
                }
                m_pMyProject->process(m_ppfInputTmp, m_ppfInputTmp, iNumFrames);

                iNumFramesRemaining -= iNumFrames;
            }
        }

        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], m_ppfOutputData[c], m_kiDataLength, 1e-3);
    }

    TEST_FIXTURE(MyProjectData, VibParamRange)
    {
        CHECK_EQUAL(Error_t::kFunctionInvalidArgsError, m_pMyProject->setParam(CMyProject::kParamModFreqInHz, -1));
        CHECK_EQUAL(Error_t::kFunctionInvalidArgsError, m_pMyProject->setParam(CMyProject::kParamModWidthInS, -0.001));

        CHECK_EQUAL(Error_t::kFunctionInvalidArgsError, m_pMyProject->setParam(CMyProject::kParamModFreqInHz, m_fSampleRate));
        CHECK_EQUAL(Error_t::kFunctionInvalidArgsError, m_pMyProject->setParam(CMyProject::kParamModWidthInS, m_fMaxModWidth+.1));
    }

    TEST_FIXTURE(MyProjectData, VibZeroInput)
    {
        m_pMyProject->setParam(CMyProject::kParamModFreqInHz, 20);
        m_pMyProject->setParam(CMyProject::kParamModWidthInS, 0);
        
        for (int c = 0; c < m_iNumChannels; c++)
            CUtil::setZero(m_ppfInputData[c], m_kiDataLength);

        process();

        int iDelay = CUtil::float2int<int>(m_fMaxModWidth*m_fSampleRate+1);
        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], &m_ppfOutputData[c][iDelay], m_kiDataLength-iDelay, 1e-3F);
    }

}

#endif //WITH_TESTS