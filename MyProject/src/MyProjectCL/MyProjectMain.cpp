
#include <iostream>

#include "pthread.h"

#include "MyProjectConfig.h"

#include "Util.h"
// include project headers
#include "AudioFileIf.h"

#include "ConvBlock.h"


#define WITH_FLOATEXCEPTIONS
#define WITH_MEMORYCHECK

// include exception header
#if (defined(WITH_FLOATEXCEPTIONS) && !defined(NDEBUG) && defined (GTCMT_WIN32))
#include <float.h>
#endif // #ifndef WITHOUT_EXCEPTIONS

// include memory leak header
#if (defined(WITH_MEMORYCHECK) && !defined(NDEBUG) && defined (GTCMT_WIN32))
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#else
#include <stdlib.h>
#endif

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

void    getClArgs (std::string &sInputFilePath, std::string &sIrFilePath, std::string &sOutputFilePath, int &iNumThreads, int argc, char* argv[]);

void *TaskCode(void *argument)
{
    CConvBlock *pCurrInstance = static_cast<CConvBlock*> (argument);

    pCurrInstance->process();

    return NULL;
}

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,             //!< file paths
                            sIrFilePath,
                            sOutputFilePath;

    CAudioFileIf            *phOutputFile       = 0;        //!< output audio file
    CAudioFileIf            *phIrFile           = 0;        //!< impulse response file

    CAudioFileIf::FileSpec_t stFileSpec;                    //!< input/output file spec

    float                   **ppfIrData         = 0;        //!< buffer for IR data

    pthread_t               *phThreads          = 0;        //!< thread handles
    CConvBlock              **ppConvInstances   = 0;

    int                     iLengthOfBlock      = 0;        //!< length of one input block to process
    int                     iNumThreads         = 0;        //!< number of threads to start

    long long               iLengthOfIr         = 0;        //!< length of IR
    long long               iInFileLength       = 0;        //!< length of input file


    int                     iSampleCount        = 0;
    int                     iRc                 = 0;
    int                     iTmp                = 0;


    // detect memory leaks in win32
#if (defined(WITH_MEMORYCHECK) && !defined(NDEBUG) && defined (GTCMT_WIN32))
    // set memory checking flags
    int iDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    iDbgFlag       |= _CRTDBG_CHECK_ALWAYS_DF;
    iDbgFlag       |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag( iDbgFlag );
#endif

    // enable floating point exceptions in win32
#if (defined(WITH_FLOATEXCEPTIONS) && !defined(NDEBUG) && defined (GTCMT_WIN32))
    // enable check for exceptions (don't forget to enable stop in MSVC!)
    _controlfp(~(_EM_INVALID | _EM_ZERODIVIDE | _EM_OVERFLOW | _EM_UNDERFLOW | _EM_DENORMAL), _MCW_EM) ;
#endif // #ifndef WITHOUT_EXCEPTIONS

    showClInfo ();

    // parse command line arguments
    getClArgs (sInputFilePath, sIrFilePath, sOutputFilePath, iNumThreads, argc, argv);

    //////////////////////////////////////////////////////////////////////////////
    // open files
    {
        CAudioFileIf    *phInputFile    = 0;        //!< input audio file

        // open the input wave file
        CAudioFileIf::createInstance(phInputFile);
        phInputFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
        if (!phInputFile->isOpen())
        {
            cout << "Input wave file open error!";
            return -1;
        }
        phInputFile->getFileSpec(stFileSpec);
        phInputFile->getLength(iInFileLength);
        CAudioFileIf::destroyInstance(phInputFile);

        // open the IR wave file
        CAudioFileIf::createInstance(phIrFile);
        phIrFile->openFile(sIrFilePath, CAudioFileIf::kFileRead);
        if (!phIrFile->isOpen())
        {
            cout << "IR wave file open error!";
            return -1;
        }
        phIrFile->getLength(iLengthOfIr);

        // open the output wave file
        CAudioFileIf::createInstance(phOutputFile);
        phOutputFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
        if (!phOutputFile->isOpen())
        {
            cout << "Input wave file open error!";
            return -1;
        }
    }

    //////////////////////////////////////
    // allocate IR data buffer and read IR
    ppfIrData   = new float* [stFileSpec.iNumChannels];
    iTmp        = static_cast<int>(iLengthOfIr);
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfIrData[i] = new float [iTmp];
    phIrFile->readData(ppfIrData, iTmp);
    CAudioFileIf::destroyInstance(phIrFile);


    //////////////////////////////////////
    // allocate thread and convolution instances
    phThreads       = new pthread_t [iNumThreads];
    ppConvInstances = new CConvBlock*[iNumThreads];

    //////////////////////////////////////
    // compute the standard block length for each instance
    iLengthOfBlock  = static_cast<int>(ceilf(iInFileLength*1.F/iNumThreads)+.001F);

    //////////////////////////////////////
    // start the threads
    for (int i = 0; i < iNumThreads; i++)
    {
        iLengthOfBlock      = std::min(iLengthOfBlock, static_cast<int>(iInFileLength-iSampleCount));

        // create new instance
        ppConvInstances[i]  = new CConvBlock(sInputFilePath, iSampleCount, iLengthOfBlock);
        ppConvInstances[i]->setIr(ppfIrData, static_cast<int>(iLengthOfIr));

        // create new thread
        iRc = pthread_create(&phThreads[i], NULL, TaskCode, (void *) ppConvInstances[i]);

        iSampleCount       += iLengthOfBlock;
    }

    ////////////////////////////////////
    // wait for each thread to complete
    for (int i=0; i<iNumThreads; ++i) 
    {
        // block until thread i completes
        iRc = pthread_join(phThreads[i], NULL);
        cout << "In main: thread " << i << " is complete" << endl;
    }

    //////////////////////////////////////
    // overlap and add the output data and write the output file

    // reuse pIrData for tail overlap and add
    for (int c=0; c<stFileSpec.iNumChannels; c++)
        CUtil::setZero(ppfIrData[c], iLengthOfIr);

    for (int i=0; i<iNumThreads; ++i) 
    {
        float **ppfOutput   = 0;
        int     iNumChannels = 0,
            iLength = 0;

        //get result and write it to file until blocklength
        ppConvInstances[i]->getResult(ppfOutput, iNumChannels, iLength);

        // add tail to next result
        for (int c=0; c<stFileSpec.iNumChannels; c++)
        {
            for (int k = 0; k < iLengthOfIr-1; k++)
                ppfOutput[c][k] += ppfIrData[c][k];
        }

        phOutputFile->writeData(ppfOutput, iLength - iLengthOfIr + 1);

        // store tail
        for (int c=0; c<stFileSpec.iNumChannels; c++)
        {
            CUtil::copyBuff(ppfIrData[c], &ppfOutput[c][iLength - iLengthOfIr + 1], iLengthOfIr-1);
        }
    }

    // write remaining samples to file
    phOutputFile->writeData(ppfIrData, iLengthOfIr-1);

    //////////////////////////////////////
    // clean-up
    // close the files
    CAudioFileIf::destroyInstance(phOutputFile);

    // free instances
    delete [] phThreads;
    for (int i = 0; i < iNumThreads; i++)
        delete ppConvInstances[i];
    delete [] ppConvInstances;

    // free memory
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete [] ppfIrData[i];
    delete [] ppfIrData;
    ppfIrData = 0;

    return 0;
    
}


void     showClInfo()
{
    cout << "GTCMT template app" << endl;
    cout << "(c) 2013 by Alexander Lerch" << endl;
    cout    << "Thread Example" << endl;
    cout  << endl;

    return;
}

void getClArgs( std::string &sInputFilePath, std::string &sIrFilePath, std::string &sOutputFilePath, int &iNumThreads, int argc, char* argv[] )
{
    if (argc > 1)
        sInputFilePath.assign (argv[1]);
    if (argc > 2)
        sIrFilePath.assign (argv[2]);
    if (argc > 3)
        sOutputFilePath.assign (argv[3]);

    if (argc > 4)
        iNumThreads = atoi(argv[4]);
    else
    {
        iNumThreads = 5;
    }
}
