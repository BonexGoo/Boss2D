#include <boss.h>

#if !defined(BOSS_NEED_ADDON_TESSERACT) || (BOSS_NEED_ADDON_TESSERACT != 0 && BOSS_NEED_ADDON_TESSERACT != 1)
    #error BOSS_NEED_ADDON_TESSERACT macro is invalid use
#endif
bool __LINK_ADDON_TESSERACT__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_TESSERACT

#include <iostream>
#include <functional>
#include <cctype>

// 도구준비
#include <addon/boss_fakewin.h>
#include <addon/boss_integration_tesseract-3.05.02.h>

#include <addon/tesseract-3.05.02_for_boss/api/baseapi.h>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/include/allheaders.h>
#include BOSS_OPENCV_U_opencv2__core_hpp
#include BOSS_OPENCV_U_opencv2__imgproc_hpp
#include BOSS_OPENCV_U_opencv2__imgcodecs_hpp
#include BOSS_OPENCV_U_opencv2__highgui_hpp

#include <boss.hpp>
#include "boss_addon_tesseract.hpp"

#include <platform/boss_platform.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(Tesseract, Create, id_tesseract, chars, chars, chars)
    BOSS_DECLARE_ADDON_FUNCTION(Tesseract, Release, void, id_tesseract)
    BOSS_DECLARE_ADDON_FUNCTION(Tesseract, Training, void, id_tesseract, chars)

    static autorun Bind_AddOn_Tesseract()
    {
        Core_AddOn_Tesseract_Create() = Customized_AddOn_Tesseract_Create;
        Core_AddOn_Tesseract_Release() = Customized_AddOn_Tesseract_Release;
        Core_AddOn_Tesseract_Training() = Customized_AddOn_Tesseract_Training;
        return true;
    }
    static autorun _ = Bind_AddOn_Tesseract();
}

bool TesseractProgram(int argc, const char **argv);
bool UnicharsetExtractorProgram(int argc, const char **argv);
bool MfTrainingProgram(int argc, const char **argv);
bool CnTrainingProgram(int argc, const char **argv);
bool CombineTessDataProgram(int argc, const char **argv);

// 구현부
namespace BOSS
{
    TesseractClass::TesseractClass()
    {
    }

    TesseractClass::TesseractClass(const TesseractClass& rhs)
    {
        operator=(rhs);
    }

    TesseractClass::~TesseractClass()
    {
    }

    TesseractClass& TesseractClass::operator=(const TesseractClass& rhs)
    {
        m_tifpath = rhs.m_tifpath;
        m_otherpath = rhs.m_otherpath;
        m_filepath = rhs.m_filepath;
        return *this;
    }

    void TesseractClass::Init(chars tifpath, chars boxpath, chars filepath)
    {
        BOSS_ASSERT("tifpath/boxpath/filepath는 nullptr가 될 수 없습니다", tifpath && boxpath && filepath);
        m_tifpath = tifpath;
        m_otherpath = boxpath;
        m_filepath = filepath;
    }

    id_tesseract Customized_AddOn_Tesseract_Create(chars tifpath, chars boxpath, chars filepath)
    {
        TesseractClass* NewTesseract = (TesseractClass*) Buffer::Alloc<TesseractClass>(BOSS_DBG 1);
        NewTesseract->Init(tifpath, boxpath, filepath);
        return (id_tesseract) NewTesseract;
    }

    void Customized_AddOn_Tesseract_Release(id_tesseract tesseract)
    {
        Buffer::Free((buffer) tesseract);
    }

    void Customized_AddOn_Tesseract_Training(id_tesseract tesseract, chars workname)
    {
        TesseractClass* CurTesseract = (TesseractClass*) tesseract;

        const String OutName = CurTesseract->GetTifPath().Sub(4);
        const sint32 FirstDot = OutName.Find(0, ".");
        const sint32 SecondDot = OutName.Find(FirstDot + 1, ".");
        BOSS_ASSERT("TifPath는 2개의 점기호가 필요합니다", -1 < FirstDot && -1 < SecondDot);
        if(FirstDot == -1 || SecondDot == -1) return;

        const String LangCode = OutName.Left(FirstDot);
        const String FontName = OutName.Left(SecondDot).Right(SecondDot - FirstDot - 1);
        const String FilePath = CurTesseract->GetFilePath();
        const String TifPath = FilePath + "/" + CurTesseract->GetTifPath();
        const String OutPath = FilePath + "/" + OutName;
        const String TrPath = FilePath + "/" + OutName + ".tr";

        // "<OutName>.box" 제작
        if(!String::Compare(workname, "tif to box"))
        {
            const String BoxPath = OutPath + ".box";
            if(Platform::File::Exist(BoxPath))
                Platform::File::Rename(WString::FromChars(BoxPath), WString::FromChars(BoxPath + ".old"));

            const char* Args[] = {"tesseract.exe", TifPath, OutPath,
                "--tessdata-dir", FilePath, "-l", LangCode,
                "-c", "tessedit_create_boxfile=true"};
            TesseractProgram(sizeof(Args) / sizeof(Args[0]), Args);
        }
        // "<OutName>.tr" 제작
        else if(!String::Compare(workname, "box to tr"))
        {
            const char* Args[] = {"tesseract.exe", TifPath, OutPath,
                "--tessdata-dir", FilePath, "-l", LangCode,
                "-c", "tessedit_resegment_from_boxes=true",
                "-c", "tessedit_train_from_boxes=true"};
            TesseractProgram(sizeof(Args) / sizeof(Args[0]), Args);
        }
        // "font_properties" 제작
        else if(!String::Compare(workname, "make blanked font_properties"))
        {
            id_file PropertiesFile = Platform::File::OpenForWrite(FilePath + "/font_properties");
            String PropertiesString = String::Format("%s 0 0 0 0 0", (chars) FontName);
            Platform::File::Write(PropertiesFile, (bytes)(chars) PropertiesString, PropertiesString.Length());
            Platform::File::Close(PropertiesFile);
        }
        // "unicharset" 제작
        else if(!String::Compare(workname, "box to unicharset"))
        {
            const String BoxPath = OutPath + ".box";
            const char* Args[] = {"unicharset_extractor.exe", "-D", FilePath, BoxPath};
            UnicharsetExtractorProgram(sizeof(Args) / sizeof(Args[0]), Args);
        }
        // "<LangCode>.inttemp", "<LangCode>.pffmtable", "<LangCode>.shapetable", "<LangCode>.unicharset" 제작
        else if(!String::Compare(workname, "font_properties/unicharset to inttemp/pffmtable/shapetable/unicharset"))
        {
            const String FontPropertiesPath = FilePath + "/font_properties";
            const String InUnicharsetPath = FilePath + "/unicharset";
            const String OutUnicharsetPath = FilePath + "/" + LangCode + ".unicharset";
            const char* Args[] = {"mftraining.exe", "-D", FilePath,
                "-F", FontPropertiesPath, "-U", InUnicharsetPath, "-O", OutUnicharsetPath, TrPath};
            MfTrainingProgram(sizeof(Args) / sizeof(Args[0]), Args);
            Platform::File::Rename(WString::FromChars(FilePath + "/inttemp"), WString::FromChars(FilePath + "/" + LangCode + ".inttemp"));
            Platform::File::Rename(WString::FromChars(FilePath + "/pffmtable"), WString::FromChars(FilePath + "/" + LangCode + ".pffmtable"));
            Platform::File::Rename(WString::FromChars(FilePath + "/shapetable"), WString::FromChars(FilePath + "/" + LangCode + ".shapetable"));
        }
        // "<LangCode>.normproto" 제작
        else if(!String::Compare(workname, "tr to normproto"))
        {
            const char* Args[] = {"cntraining.exe", "-D", FilePath, TrPath};
            CnTrainingProgram(sizeof(Args) / sizeof(Args[0]), Args);
            Platform::File::Rename(WString::FromChars(FilePath + "/normproto"), WString::FromChars(FilePath + "/" + LangCode + ".normproto"));
        }
        // "<LangCode>.traineddata" 제작
        else if(!String::Compare(workname, "all to traineddata"))
        {
            const String LangPathAndDot = FilePath + "/" + LangCode + ".";
            const char* Args[] = {"combine_tessdata.exe", LangPathAndDot};
            CombineTessDataProgram(sizeof(Args) / sizeof(Args[0]), Args);
        }
        // 상대방의 traineddata에서 모든 컴포넌트 추출
        else if(!String::Compare(workname, "otherpath to all components"))
        {
            const String OtherTrainedDataPath = FilePath + "/" + CurTesseract->GetOtherPath();
            const String OtherTrainedDataPathAndDot = OtherTrainedDataPath + ".";
            const char* Args[] = {"combine_tessdata.exe", "-u", OtherTrainedDataPath, OtherTrainedDataPathAndDot};
            CombineTessDataProgram(sizeof(Args) / sizeof(Args[0]), Args);
        }
        // 나의 traineddata에 상대방의 config 컴포넌트 병합
        else if(!String::Compare(workname, "config components to traineddata"))
        {
            const String TrainedDataPath = FilePath + "/" + LangCode + ".traineddata";
            const String ConfigComponentPath = FilePath + "/" + CurTesseract->GetOtherPath() + ".config";
            const char* Args[] = {"combine_tessdata.exe", "-o", TrainedDataPath, ConfigComponentPath};
            CombineTessDataProgram(sizeof(Args) / sizeof(Args[0]), Args);
        }
    }
}

#endif
