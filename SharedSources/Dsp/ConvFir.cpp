

// standard headers

// project headers
#include "MyProjectConfig.h"

#include "ErrorDef.h"
#include "RingBuffer.h"

#include "ConvFir.h"

CConvFir::CConvFir(  ) :     m_pfImpulseResponse(0),
m_pCRingBuff(0),
m_iLengthOfIr(0)
{
    this->reset ();
}

CConvFir::~CConvFir()
{
    this->reset ();
}

Error_t CConvFir::setImpulseResponse( float *pfImpulseResponse, int iLengthOfIr )
{
    if (!pfImpulseResponse || iLengthOfIr <= 0)
        return kFunctionInvalidArgsError;

    this->reset ();

    m_pfImpulseResponse = new float [iLengthOfIr];
    m_iLengthOfIr       = iLengthOfIr;
    CUtil::copyBuff(m_pfImpulseResponse, pfImpulseResponse, iLengthOfIr);

    m_pCRingBuff        = new CRingBuffer<float> (m_iLengthOfIr);

    return kNoError;
}

Error_t CConvFir::reset()
{
    delete m_pCRingBuff;
    delete [] m_pfImpulseResponse;

    m_pCRingBuff        = 0;
    m_pfImpulseResponse = 0;
    m_iLengthOfIr       = 0;

    return kNoError;
}

int CConvFir::getTailLength() const
{
    return m_iLengthOfIr - 1;
}

Error_t CConvFir::process( const float *pfIn, float *pfOut, int iNumSamples )
{
    if (!pfIn || !pfOut || !m_pfImpulseResponse || !m_pCRingBuff)
        return kFunctionInvalidArgsError;

    for (int i = 0; i < iNumSamples; i++)
    {
        m_pCRingBuff->putPostInc(pfIn[i]);
        
        pfOut[i] = 0;
        for (int j = 0; j < m_iLengthOfIr; j++)
        {
            pfOut[i]    += m_pCRingBuff->get(static_cast<float>(-j)) * m_pfImpulseResponse[j];
        }
        m_pCRingBuff->setReadIdx(m_pCRingBuff->getReadIdx() + 1);
    }
    return kNoError;
}

Error_t CConvFir::flush( float *pfOut )
{
    float fZero = 0;

    if (!pfOut)
        return kFunctionInvalidArgsError;

    for (int i = 0; i < m_iLengthOfIr-1; i++)
        process(&fZero, &pfOut[i], 1);

    return kNoError;
}
