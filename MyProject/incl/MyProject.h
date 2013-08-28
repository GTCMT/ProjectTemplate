#if !defined(__MyProject_hdr__)
#define __MyProject_hdr__

// do not include any headers here if possible -- this is the interface

class CMyProject
{
public:
    /*! error codes */
    enum Error_t
    {
        kNoError,                       //!< no error occurred
        kUnknownError                   //!< unknown error occurred
    };

    /*! version number */
    enum Version_t
    {
        kMajor,                         //!< major version number
        kMinor,                         //!< minor version number
        kPatch,                         //!< patch version number

        kNumVersionInts
    };

    static const int  GetVersion (const Version_t eVersionIdx);
    static const char* GetBuildDate ();

    static Error_t CreateInstance (CMyProject*& pCKortIf);
    static Error_t DestroyInstance (CMyProject*& pCKortIf);
    
    Error_t InitInstance (/*enter parameters here*/);
    Error_t ResetInstance ();
    
    //Error_t Process (float **ppfInputBuffer, int iNumberOfFrames) = 0;

protected:
    CMyProject ();
    virtual ~CMyProject ();
};

#endif // #if !defined(__MyProject_hdr__)



