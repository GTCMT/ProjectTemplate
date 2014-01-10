#if !defined(__RingBuffer_hdr__)
#define __RingBuffer_hdr__

#include <cassert>

template <class T> 
class CRingBuffer
{
public:
    CRingBuffer (int iBufferLengthInSamples) :
        m_iBuffLength(iBufferLengthInSamples),
        m_iReadIdx(0),                          
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

    /*! add a new value of type T and increment write index
    \param T tNewValue the new value
    \return void
    */
    void putValuePostInc (T tNewValue)
    {
        m_ptBuff[m_iWriteIdx]   = tNewValue;
        incIdx(m_iWriteIdx);
    };

    /*! return the value of the current read index and increment the read pointer
    \return float the value from the read index
    */
    float getValuePostInc ()
    {
        int iCurrIdx    = m_iReadIdx;
        incIdx(m_iReadIdx);
        return m_ptBuff[iCurrIdx];
    };

    /*! set buffer content and indices to 0
    \return void
    */
    void resetInstance ()
    {
        CUtil::setZero(m_ptBuff, m_iBuffLength);
        m_iReadIdx  = 0;
        m_iWriteIdx = 0;
    }

    /*! returns the number of values currently buffered (note: 0 could also mean the buffer is full!)
    \return int
    */
    int getNumValuesInBuffer ()
    {
        return (m_iWriteIdx - m_iReadIdx + m_iBuffLength)%m_iBuffLength;
    }
private:
    void incIdx (int &iIdx)
    {
        iIdx    = (iIdx + 1) % m_iBuffLength;
    };

    int     m_iBuffLength,          //!< length of the internal buffer
        m_iReadIdx,                 //!< current read index
        m_iWriteIdx;                //!< current write index

    T       *m_ptBuff;              //!< data buffer
};
#endif // __RingBuffer_hdr__
