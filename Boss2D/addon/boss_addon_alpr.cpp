#include <boss.h>

#if !defined(BOSS_NEED_ADDON_ALPR) || (BOSS_NEED_ADDON_ALPR != 0 && BOSS_NEED_ADDON_ALPR != 1)
    #error BOSS_NEED_ADDON_ALPR macro is invalid use
#endif
bool __LINK_ADDON_ALPR__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_ALPR

#include <iostream>
#include <functional>
#include <cctype>

// 도구준비
#include <addon/boss_fakewin.h>
#include <addon/boss_integration_openalpr-2.3.0.h>

#include <addon/openalpr-2.3.0_for_boss/src/openalpr/alpr.h>
#include <addon/opencv-3.1.0_for_boss/include/opencv2/opencv.hpp>

#include <boss.hpp>
#include "boss_addon_alpr.hpp"

#include <platform/boss_platform.hpp>
#include <format/boss_bmp.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(Alpr, Create, id_alpr, chars, h_view)
    BOSS_DECLARE_ADDON_FUNCTION(Alpr, Release, void, id_alpr)
    BOSS_DECLARE_ADDON_FUNCTION(Alpr, Recognize, chars, id_alpr, id_bitmap_read)
    BOSS_DECLARE_ADDON_FUNCTION(Alpr, Summary, chars, chars)

    static autorun Bind_AddOn_Alpr()
    {
        Core_AddOn_Alpr_Create() = Customized_AddOn_Alpr_Create;
        Core_AddOn_Alpr_Release() = Customized_AddOn_Alpr_Release;
        Core_AddOn_Alpr_Recognize() = Customized_AddOn_Alpr_Recognize;
        Core_AddOn_Alpr_Summary() = Customized_AddOn_Alpr_Summary;
        return true;
    }
    static autorun _ = Bind_AddOn_Alpr();
}

static h_view gCurListener;
static rect128 gLastRect;

extern "C" void AlprDebug_AddRect(int x, int y, int w, int h)
{
    if(gCurListener.get())
    {
        gLastRect.l = x;
        gLastRect.t = y;
        gLastRect.r = x + w;
        gLastRect.b = y + h;
    }
}

extern "C" void AlprDebug_Flush(const char* subject, const char* comment)
{
    if(gCurListener.get())
    {
        String NewMessage = String::Format("{\"subject\":\"%s\", \"comment\":\"%s\", "
            "\"coordinates\":[{\"l\":%d, \"t\":%d, \"r\":%d, \"b\":%d}]}",
            subject, comment, gLastRect.l, gLastRect.t, gLastRect.r, gLastRect.b);
        Platform::SendNotify(gCurListener, "AlprDebug", NewMessage);
    }
}

// 구현부
namespace BOSS
{
    class AlprInstance
    {
    public:
        AlprInstance(alpr::Alpr* alpr)
        {
            mObject = alpr;
            mCount = 1;
        }
        ~AlprInstance()
        {
            delete mObject;
        }

    public:
        inline alpr::Alpr* Get()
        {
            return mObject;
        }
        AlprInstance* CopyMe()
        {
            mCount++;
            return this;
        }
        void DestroyMe()
        {
            if(0 < mCount && --mCount == 0)
                delete this;
        }

    private:
        alpr::Alpr* mObject;
        sint32 mCount;
    };

    AlprClass::AlprClass()
    {
        m_instance = nullptr;
    }

    AlprClass::AlprClass(const AlprClass& rhs)
    {
        operator=(rhs);
    }

    AlprClass::~AlprClass()
    {
        if(m_instance)
            m_instance->DestroyMe();
    }

    AlprClass& AlprClass::operator=(const AlprClass& rhs)
    {
        m_listener = rhs.m_listener;
        m_instance = (rhs.m_instance)? rhs.m_instance->CopyMe() : nullptr;
        return *this;
    }

    bool AlprClass::Init(chars country, h_view listener)
    {
        BOSS_ASSERT("country는 nullptr가 될 수 없습니다", country);
        m_listener = listener;
        gCurListener = m_listener;

        if(m_instance)
        {
            m_instance->DestroyMe();
            m_instance = nullptr;
        }

        alpr::Alpr* NewAlpr = new alpr::Alpr((*country == '\0')? "us" : country);
        if(!NewAlpr->isLoaded())
        {
            delete NewAlpr;
            return false;
        }
        NewAlpr->setDetectRegion(true);
        NewAlpr->setDefaultRegion("");
        NewAlpr->setTopN(3);

        m_instance = new AlprInstance(NewAlpr);
        return true;
    }

    id_alpr Customized_AddOn_Alpr_Create(chars country, h_view listener)
    {
        AlprClass* NewAlpr = (AlprClass*) Buffer::Alloc<AlprClass>(BOSS_DBG 1);
        if(!NewAlpr->Init(country, listener))
        {
            Buffer::Free((buffer) NewAlpr);
            return nullptr;
        }
        return (id_alpr) NewAlpr;
    }

    void Customized_AddOn_Alpr_Release(id_alpr alpr)
    {
        Buffer::Free((buffer) alpr);
    }

    chars Customized_AddOn_Alpr_Recognize(id_alpr alpr, id_bitmap_read bitmap)
    {
        BOSS_ASSERT("alpr은 nullptr가 될 수 없습니다", alpr);
        BOSS_ASSERT("bitmap은 nullptr가 될 수 없습니다", bitmap);
        AlprClass* CurAlpr = (AlprClass*) alpr;
        gCurListener = CurAlpr->GetListener();

        alpr::Alpr* MyAlpr = CurAlpr->GetInstance()->Get();
        if(MyAlpr == nullptr)
            return "{\"results\":[]}";

        const sint32 BmpSize = Bmp::GetFileSizeWithoutBM(bitmap);
        std::vector<char> buffer;
        buffer.resize(2 + BmpSize);
        buffer.data()[0] = 'B';
        buffer.data()[1] = 'M';
        memcpy(&buffer.data()[2], (const void*) bitmap, BmpSize);
        alpr::AlprResults results = MyAlpr->recognize(buffer);

        static String Result;
        Result = MyAlpr->toJson(results).c_str();
        return Result;
    }

    chars Customized_AddOn_Alpr_Summary(chars recognizedtext)
    {
        String Text = recognizedtext;
        if(0 < Text.Length() && Text[0] != '(')
        {
            sint32 FinedPos = Text.Find(0, ':');
            if(-1 < FinedPos)
                Text = Text.Right(Text.Length() - FinedPos - 1);

            String ChildText[2];
            sint32 Focus = 0;
            for(sint32 i = 0, iend = Text.Length(); i < iend; ++i)
            {
                if('0' <= Text[i] && Text[i] <= '9')
                    ChildText[Focus] += Text[i];
                else if(Text[i] == 'I') // 학습효과를 늘리기 위해 '1'과 'I"를 분리하여 OCR제작
                    ChildText[Focus] += '1';
                else if(Text[i] == '~')
                    ChildText[Focus] += '~';
                else if(Focus < 1 && 0 < ChildText[Focus].Length())
                    Focus++;
            }

            // 보정과정
            if(ChildText[0].Length() + ChildText[1].Length() != 6)
            {
                for(sint32 i = 0; i < 2; ++i)
                {
                    String& CurText = ChildText[i];
                    sint32 BaseSize = (i == 0)? 2 : 4;
                    if(CurText.Length() == BaseSize + 1)
                    {
                        if((CurText[0] == '0' || CurText[0] == '1') && CurText[-1] != '0' && CurText[-1] != '1')
                            CurText = CurText.Right(BaseSize);
                        else if((CurText[-1] == '0' || CurText[-1] == '1') && CurText[0] != '0' && CurText[0] != '1')
                            CurText = CurText.Left(BaseSize);
                        else if(CurText[0] == '1' && CurText[-1] != '1')
                            CurText = CurText.Right(BaseSize);
                        else if(CurText[-1] == '1' && CurText[0] != '1')
                            CurText = CurText.Left(BaseSize);
                    }
                    else if(CurText.Length() == BaseSize + 2)
                    {
                        if((CurText[0] == '0' || CurText[0] == '1') && (CurText[-1] == '0' || CurText[-1] == '1'))
                            CurText = CurText.Left(BaseSize + 1).Right(BaseSize);
                    }
                }
            }

            static String Result;
            Result = ChildText[0] + ChildText[1];
            return Result;
        }
        return "";
    }
}

#endif
