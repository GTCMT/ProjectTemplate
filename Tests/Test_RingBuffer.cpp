#include "MyProjectConfig.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>

#include "UnitTest++.h"

#include "SignalGen.h"

#include "Util.h"
#include "RingBuffer.h"

SUITE(RingBuff)
{
    struct RingBuffer
    {
        RingBuffer() 
        {
            const float fSampleFreq  = 44100;

            m_pCRingBuffer  = new CRingBuffer<float> (m_iRingBuffLength);
            m_pfData        = new float [m_iDataLength];
            CSignalGen::generateSine(m_pfData, 20.F, fSampleFreq, m_iRingBuffLength, .7F, static_cast<float>(M_PI_2));
        }

        ~RingBuffer() 
        {
            delete [] m_pfData;
            delete m_pCRingBuffer;
        }

 
        static const int m_iRingBuffLength;
        static const int m_iDataLength;
        static const int m_iDelay;

        CRingBuffer<float> *m_pCRingBuffer;

        float   *m_pfData;
    };

    const int RingBuffer::m_iRingBuffLength = 16;
    const int RingBuffer::m_iDataLength     = 55;
    const int RingBuffer::m_iDelay          = 9;

    TEST_FIXTURE(RingBuffer, RbDelay)
    {
        // put initial values into ring buffer
        for (int i = 0; i < m_iDelay; i++)
        {
            m_pCRingBuffer->putPostInc (m_pfData[i]);
        }

        for (int i = m_iDelay; i < m_iDataLength; i++)
        {
            CHECK_EQUAL(m_iDelay, m_pCRingBuffer->getNumValuesInBuffer());
            m_pCRingBuffer->getPostInc (); // just to increment - ignore the value
            m_pCRingBuffer->putPostInc (m_pfData[i]);
        }
    }

    TEST_FIXTURE(RingBuffer, RbValues)
    {
        int iDataBuffIdx    = 0;

        // put initial values into ring buffer
        for (int i = 0; i < m_iDelay; i++)
        {
            m_pCRingBuffer->putPostInc (m_pfData[i]);
        }

        for (int i = m_iDelay; i < m_iDataLength; i++, iDataBuffIdx++)
        {
            float fValue = m_pCRingBuffer->getPostInc ();
            CHECK_CLOSE(fValue, m_pfData[iDataBuffIdx], 1e-3F);
            m_pCRingBuffer->putPostInc (m_pfData[i]);
        }
    }

    TEST_FIXTURE(RingBuffer, RbReset)
    {
        // put initial values into ring buffer
        for (int i = 0; i < m_iDelay; i++)
        {
            m_pCRingBuffer->putPostInc (m_pfData[i]);
        }
        m_pCRingBuffer->resetInstance ();
        CHECK_EQUAL(0, m_pCRingBuffer->getNumValuesInBuffer());

        float fValue = m_pCRingBuffer->getPostInc ();
        CHECK_EQUAL (0.F, fValue);

        CHECK_EQUAL(m_iRingBuffLength-1, m_pCRingBuffer->getNumValuesInBuffer());
    }
}

#endif //WITH_TESTS