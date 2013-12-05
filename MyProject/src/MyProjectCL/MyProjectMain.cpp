
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

void    getClArgs (std::string &sInputFilePath, std::string &sOutputFilePath, int argc, char* argv[]);

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,
                            sOutputFilePath;
    CAudioFileIf::FileSpec_t stFileSpec;

    float                   **ppfAudioData;
    static const int        kBlockSize = 17;
    int                     iBlockSize = kBlockSize;
    Error_t                 iErr;

    CAudioFileIf            *phInputFile    = new CAudioFileIf (),
                            *phOutputFile   = new CAudioFileIf ();
    CMyProject              *phMyProject    = 0;

#if (defined(WITH_MEMORYCHECK) && !defined(NDEBUG) && defined (GTCMT_WIN32))
    // set memory checking flags
    int iDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    iDbgFlag       |= _CRTDBG_CHECK_ALWAYS_DF;
    iDbgFlag       |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag( iDbgFlag );
#endif

#if (defined(WITH_FLOATEXCEPTIONS) && !defined(NDEBUG) && defined (GTCMT_WIN32))
    // enable check for exceptions (don't forget to enable stop in MSVC!)
    _controlfp(~(_EM_INVALID | _EM_ZERODIVIDE | _EM_OVERFLOW | _EM_UNDERFLOW | _EM_DENORMAL), _MCW_EM) ;
#endif // #ifndef WITHOUT_EXCEPTIONS

    showClInfo ();

    // parse command line arguments
    getClArgs (sInputFilePath, sOutputFilePath, argc, argv);

    // open files
    stFileSpec.eBitStreamType   = CAudioFileIf::kFileBitStreamInt16;
    stFileSpec.eFormat          = CAudioFileIf::kFileFormatRaw;
    stFileSpec.fSampleRate      = 44100;
    stFileSpec.iNumChannels     = 2;

    iErr = phInputFile->openFile (sInputFilePath, 
        CAudioFileIf::kFileRead,
        &stFileSpec);
    if (iErr != kNoError)
    {
        cout << "Input file open error!" << endl << endl;
        return -1;
    }
    iErr = phOutputFile->openFile (sOutputFilePath, 
        CAudioFileIf::kFileWrite,
        &stFileSpec);
    if (iErr != kNoError)
    {
        cout << "Input file open error!" << endl << endl;
        return -1;
    }

    //allocate memory 
    ppfAudioData = new float* [stFileSpec.iNumChannels];
    for (int c=0; c<stFileSpec.iNumChannels; c++)
    {
        ppfAudioData[c] = new float [iBlockSize];
    }

    int i = 1;
    while (iBlockSize == kBlockSize)
    {
        // read and write data
        phInputFile->readData (ppfAudioData, iBlockSize);
        phOutputFile->writeData (ppfAudioData, iBlockSize);
        if (i==1)
        {
            phInputFile->setPosition (.5);
            i =0;
        }
    }

    // free memory
    for (int c=0; c<stFileSpec.iNumChannels; c++)
    {
        delete [] ppfAudioData[c];
        ppfAudioData[c] = 0;
    }
    delete [] ppfAudioData;
    ppfAudioData    = 0;

    phInputFile->closeFile ();
    phOutputFile->closeFile ();
    delete phInputFile;
    delete phOutputFile;

    // create class instance
    CMyProject::createInstance (phMyProject);

    // process
 
    // write results

    // free memory    

    // delete instance
    CMyProject::destroyInstance (phMyProject);
    
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

void getClArgs( std::string &sInputFilePath, std::string &sOutputFilePath, int argc, char* argv[] )
{
    if (argc > 1)
        sInputFilePath.assign (argv[1]);
    if (argc > 2)
        sOutputFilePath.assign (argv[2]);
}
