

// standard headers

// project headers
#include "MyProjectConfig.h"

#include "ErrorDef.h"

#include "ConvFir.h"

CConvFir::CConvFir( int iMaxFilterLength )
{

}

CConvFir::~CConvFir()
{

}

Error_t CConvFir::setImpulseResponse( float *pfImpulseResponse, int iLengthOfIr )
{
    return kNoError;
}

Error_t CConvFir::reset()
{
    return kNoError;
}

int CConvFir::getTailLength() const
{
    return -1;
}

Error_t CConvFir::process( const float *pfIn, float *pfOut, int iNumSamples )
{
    return kNoError;
}

Error_t CConvFir::flush( float *pfOut )
{
    return kNoError;
}
