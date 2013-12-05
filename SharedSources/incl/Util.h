#if !defined(__Util_hdr__)
#define __Util_hdr__

class CUtil
{
public:
    template<typename T>
    static T float2int (float fInput)
    {
        if (fInput >= 0.F)
            return static_cast<T>(fInput + .5F);
        else
            return static_cast<T>(fInput - .5F);
    }
    template<typename T>
    static T double2int (double fInput)
    {
        if (fInput >= 0)
            return static_cast<T>(fInput + .5F);
        else
            return static_cast<T>(fInput - .5F);
    }
};
#endif // __Util_hdr__