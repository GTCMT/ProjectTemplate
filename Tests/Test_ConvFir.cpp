#include "MyProjectConfig.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>

#include "UnitTest++.h"

#include "Dsp.h"

SUITE(ConvFir)
{
    struct ConvFirData
    {
        ConvFirData() : m_pCConvFir(0)
        {
            m_pfImpulseResponse   = new float [m_iIrLength];
            m_pfOutputData  = new float [m_iIrLength];

            m_pCConvFir   = new CConvFir();

            for (int i = 0; i < m_iIrLength; i++)
                m_pfImpulseResponse[i]  = static_cast<float>(2*rand()/(RAND_MAX) - 1.);
        }

        ~ConvFirData() 
        {
            delete [] m_pfImpulseResponse;
            delete [] m_pfOutputData;
            delete m_pCConvFir;
        }

        CConvFir *m_pCConvFir;
        static const int m_iIrLength;
        float   *m_pfOutputData;
        float   *m_pfImpulseResponse;
    };
    const int ConvFirData::m_iIrLength = 5013;

    TEST_FIXTURE(ConvFirData, IrIdentity)
    {
        float fIn = 1;
        m_pCConvFir->setImpulseResponse(m_pfImpulseResponse, m_iIrLength);

        m_pCConvFir->process(&fIn, m_pfOutputData, 1);
        CHECK_EQUAL(m_iIrLength, m_pCConvFir->getTailLength ());

        m_pCConvFir->flush (m_pfOutputData);

        CHECK_ARRAY_CLOSE(m_pfImpulseResponse, m_pfOutputData, m_iIrLength, 1e-3F);
    }
}

#endif //WITH_TESTS