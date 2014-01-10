#if !defined(__RingBuffer_hdr__)
#define __RingBuffer_hdr__

#include <cassert>

template <class T> 
class CRingBuffer
{
public:
    CRingBuffer (int iBufferLengthInSamples) :
        m_iBuffLength(iBufferLengthInSamples),
        m_iReadIdx(0),                          // note: when the read index equals the write index you actually don't know whether the buffer is full or empty -- you are on your own!
        m_iWriteIdx(0),
        m_ptBuff(0)
    {
        assert(iBufferLengthInSamples > 0);
        m_ptBuff        = new T [m_iBuffLength];
        resetInstance();
    };

    virtual ~CRingBuffer ()
    {
        delete m_ptBuff;
        m_ptBuff    = 0;
    };

    void putValuePostInc (T tNewValue)
    {
        m_ptBuff[m_iWriteIdx]   = tNewValue;
        incIdx(m_iWriteIdx);
    };

    float getValuePostInc ()
    {
        int iCurrIdx    = m_iReadIdx;
        incIdx(m_iReadIdx);
        return m_ptBuff[iCurrIdx];
    };

    void resetInstance ()
    {
        CUtil::setZero(m_ptBuff, m_iBuffLength);
        m_iReadIdx  = 0;
        m_iWriteIdx = 0;
    }

    int getNumValuesInBuffer ()
    {
        return (m_iWriteIdx - m_iReadIdx + m_iBuffLength)%m_iBuffLength;
    }
private:
    void incIdx (int &iIdx)
    {
        iIdx    = (iIdx + 1) % m_iBuffLength;
    };

    int     m_iBuffLength,
        m_iReadIdx,
        m_iWriteIdx;

    T       *m_ptBuff;
};
#endif // __RingBuffer_hdr__
