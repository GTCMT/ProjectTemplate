
// standard headers

// project headers
#include "MyProjectConfig.h"

#include "ErrorDef.h"

#include "AudioFileIf.h"
#include "ConvBlock.h"


CConvBlock::CConvBlock (std::string cFilePath, int iStartIdx, int iLengthOfBlock) :
    m_ppfOutputData(0),
    m_ppfProcessData(0),
    m_ppFir(0),
    m_pCAudioFile(0),
    m_iStartIdx(0), 
    m_iLengthOfBlock(0),
    m_iLengthOfIr(0),
    m_iNumChannels(0)

{
    CAudioFileIf::createInstance(m_pCAudioFile);
    m_pCAudioFile->openFile(cFilePath, CAudioFileIf::kFileRead);

}


CConvBlock::~CConvBlock ()
{
}

Error_t CConvBlock::setIr( float **ppfImpulseResponse, int iLengthOfIr )
{
    return kNoError;
}

Error_t CConvBlock::getResult( float **&ppfOutputData, int &iNumChannels, int &iLength )
{
    return kNoError;
}

Error_t CConvBlock::process()
{
    return kNoError;
}
