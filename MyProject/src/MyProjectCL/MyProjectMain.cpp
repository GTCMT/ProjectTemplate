
#include <iostream>

#include "MyProjectConfig.h"

// include project headers
#include "AudioFileIf.h"

#include "MyProject.h"

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

void getClArgs (std::string &sInputFilePath, std::string &sOutputFilePath, CMyProject::CombFilterType_t &eFilterType, float &fGain, float &fDelayInS, int argc, char* argv[]);

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,
                            sOutputFilePath;

    float                   **ppfAudioData  = 0;
    static const int        kBlockSize      = 1024;

    float                   fParamGain      = .5F,
                            fParamDelayInS  = .5F;
    float                   fMaxDelayLength = 3.F;
                            
    CMyProject::CombFilterType_t eFilterType= CMyProject::kCombFIR;

    CAudioFileIf            *phInputFile    = 0,
                            *phOutputFile   = 0;
    CAudioFileIf::FileSpec_t stFileSpec;

    CMyProject              *phMyProject    = 0;

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
    getClArgs (sInputFilePath, sOutputFilePath, eFilterType, fParamGain, fParamDelayInS, argc, argv);

    // open the input wave file
    CAudioFileIf::createInstance(phInputFile);
    phInputFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phInputFile->isOpen())
    {
        cout << "Input wave file open error!" << endl;
        return -1;
    }
    phInputFile->getFileSpec(stFileSpec);

    // open the output file
    CAudioFileIf::createInstance(phOutputFile);
    phOutputFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
    if (!phOutputFile->isOpen())
    {
        cout << "Output wave file open error!" << endl;
        return -1;
    }

    // allocate audio data buffer
    ppfAudioData        = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioData[i] = new float [kBlockSize];

    // create instance of effect and set parameters
    CMyProject::createInstance(phMyProject);
    phMyProject->initInstance(eFilterType, fMaxDelayLength, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    if (phMyProject->setParam(CMyProject::kParamGain, fParamGain) != kNoError)
        cout << "illegal gain" << endl;
    if (phMyProject->setParam(CMyProject::kParamDelay, fParamDelayInS) != kNoError)
        cout << "illegal delay" << endl;


    // read wave
    while (!phInputFile->isEof())
    {
        int iNumFrames = kBlockSize;
        phInputFile->readData(ppfAudioData, iNumFrames);

        phMyProject->process(ppfAudioData, ppfAudioData, iNumFrames);

        phOutputFile->writeData(ppfAudioData, iNumFrames);
    }

    // destroy instance
    CMyProject::destroyInstance(phMyProject);

    // close the files
    CAudioFileIf::destroyInstance(phInputFile);
    CAudioFileIf::destroyInstance(phOutputFile);

    // free memory
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete [] ppfAudioData[i];
    delete [] ppfAudioData;
    ppfAudioData = 0;

    return 0;
    
}


void     showClInfo()
{
    cout << "GTCMT template app" << endl;
    cout << "(c) 2013 by Alexander Lerch" << endl;
    cout    << "V" 
        << CMyProject::getVersion (CMyProject::kMajor) << "." 
        << CMyProject::getVersion (CMyProject::kMinor) << "." 
        << CMyProject::getVersion (CMyProject::kPatch) << ", date: " 
        << CMyProject::getBuildDate () << endl;
    cout  << endl;

    return;
}

void getClArgs( std::string &sInputFilePath, std::string &sOutputFilePath, CMyProject::CombFilterType_t &eFilterType, float &fGain, float &fDelayInS, int argc, char* argv[] )
{
    if (argc > 1)
        sInputFilePath.assign (argv[1]);
    if (argc > 2)
        sOutputFilePath.assign (argv[2]);
    if (argc > 3)
        eFilterType = static_cast<CMyProject::CombFilterType_t>(atoi(argv[3]));
    if (argc > 4)
        fGain       = static_cast<float>(atof(argv[4]));
    if (argc > 5)
        fDelayInS   = static_cast<float>(atof(argv[5]));
}
