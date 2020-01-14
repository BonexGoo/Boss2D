#pragma once
#include <boss.hpp>

namespace BOSS
{
    class TesseractClass
    {
    public:
        TesseractClass();
        TesseractClass(const TesseractClass& rhs);
        ~TesseractClass();
        TesseractClass& operator=(const TesseractClass& rhs);

    public:
        void Init(chars tifpath, chars boxpath, chars filepath);
        inline String GetTifPath() {return m_tifpath;}
        inline String GetOtherPath() {return m_otherpath;}
        inline String GetFilePath() {return m_filepath;}

    private:
        String m_tifpath;
        String m_otherpath;
        String m_filepath;
    };
}
