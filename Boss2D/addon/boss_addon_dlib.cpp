#include <boss.h>

#if !defined(BOSS_NEED_ADDON_DLIB) || (BOSS_NEED_ADDON_DLIB != 0 && BOSS_NEED_ADDON_DLIB != 1)
    #error BOSS_NEED_ADDON_DLIB macro is invalid use
#endif
bool __LINK_ADDON_DLIB__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_DLIB

#include "boss_addon_dlib.hpp"

#include <boss.hpp>
#include <format/boss_bmp.hpp>

using namespace dlib;

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(Dlib, Create, id_dlib, chars)
    BOSS_DECLARE_ADDON_FUNCTION(Dlib, Release, void, id_dlib)
    BOSS_DECLARE_ADDON_FUNCTION(Dlib, Update, sint32, id_dlib, id_bitmap_read)
    BOSS_DECLARE_ADDON_FUNCTION(Dlib, GetFaceLandmark, const point64*, id_dlib, sint32, AddOn::Dlib::FaceLandmark68Type)

    static autorun Bind_AddOn_Dlib()
    {
        Core_AddOn_Dlib_Create() = Customized_AddOn_Dlib_Create;
        Core_AddOn_Dlib_Release() = Customized_AddOn_Dlib_Release;
        Core_AddOn_Dlib_Update() = Customized_AddOn_Dlib_Update;
        Core_AddOn_Dlib_GetFaceLandmark() = Customized_AddOn_Dlib_GetFaceLandmark;
        return true;
    }
    static autorun _ = Bind_AddOn_Dlib();
}

// 구현부
namespace BOSS
{
    class DlibShape
    {
    public:
        static const sint32 Max = 68;
    public:
        point64 m_pos[Max];
    };
    typedef Array<DlibShape, datatype_pod_canmemcpy_zeroset> DlibShapes;

    class DlibClass
    {
    public:
        DlibClass() {}
        ~DlibClass() {}

    public:
        frontal_face_detector m_detector;
        shape_predictor m_sp;
        DlibShapes m_shapes;
    };

    id_dlib Customized_AddOn_Dlib_Create(chars trainpath)
    {
        auto NewDlib = (DlibClass*) Buffer::Alloc<DlibClass>(BOSS_DBG 1);
        NewDlib->m_detector = get_frontal_face_detector();
        deserialize(trainpath) >> NewDlib->m_sp;
        return (id_dlib) NewDlib;
    }

    void Customized_AddOn_Dlib_Release(id_dlib dlib)
    {
        Buffer::Free((buffer) dlib);
    }

    sint32 Customized_AddOn_Dlib_Update(id_dlib dlib, id_bitmap_read bmp24)
    {
        auto CurDlib = (DlibClass*) dlib;
        if(!CurDlib) return 0;
        CurDlib->m_shapes.Clear();

        struct BmpStream : public std::streambuf
        {
        public:
            BmpStream(sint32 size) : buffer(size)
            {
                write_pos = 0;
                char* begin = &buffer.front();
                char* end = begin + buffer.size();
                setg(begin, begin, end);
            }
        public:
            void copy(void* buf, int size)
            {
                std::memcpy(&buffer[write_pos], buf, size);
                write_pos += size;
            }
        private:
            std::vector<char> buffer;
            int write_pos;
        } NewBmp(2 + Bmp::GetFileSizeWithoutBM(bmp24));

        NewBmp.copy((void*) "BM", 2);
        NewBmp.copy((void*) bmp24, Bmp::GetFileSizeWithoutBM(bmp24));

        array2d<rgb_pixel> NewImg;
        std::istream NewStream(&NewBmp);
        load_bmp(NewImg, NewStream);
        const sint32 ImageWidth = NewImg.nc();
        const sint32 ImageHeight = NewImg.nr();

        pyramid_up(NewImg);
        const sint32 RatedWidth = NewImg.nc();
        const sint32 RatedHeight = NewImg.nr();

        std::vector<rectangle> dets = CurDlib->m_detector(NewImg);
        for(unsigned long i = 0; i < dets.size(); ++i)
        {
            full_object_detection shape = CurDlib->m_sp(NewImg, dets[i]);
            if(shape.num_parts() == DlibShape::Max)
            {
                auto& NewShape = CurDlib->m_shapes.AtAdding();
                for(int p = 0; p < DlibShape::Max; ++p)
                {
                    auto& CurPoint = shape.part(p);
                    NewShape.m_pos[p].x = Math::Clamp(CurPoint.x() * ImageWidth / RatedWidth, 0, ImageWidth - 1);
                    NewShape.m_pos[p].y = Math::Clamp(CurPoint.y() * ImageHeight / RatedHeight, 0, ImageHeight - 1);
                }
            }
        }
        return CurDlib->m_shapes.Count();
    }

    const point64* Customized_AddOn_Dlib_GetFaceLandmark(id_dlib dlib, sint32 index, AddOn::Dlib::FaceLandmark68Type type)
    {
        auto CurDlib = (DlibClass*) dlib;
        if(!CurDlib) return nullptr;
        if(CurDlib->m_shapes.Count() <= index)
            return nullptr;

        switch(type)
        {
        case AddOn::Dlib::FLT_All_68_Dots:
            return &CurDlib->m_shapes[index].m_pos[0];
        case AddOn::Dlib::FLT_Jaw_17_Dots:
            return &CurDlib->m_shapes[index].m_pos[0];
        case AddOn::Dlib::FLT_LeftEyeBrow_5_Dots:
            return &CurDlib->m_shapes[index].m_pos[17];
        case AddOn::Dlib::FLT_RightEyeBrow_5_Dots:
            return &CurDlib->m_shapes[index].m_pos[17 + 5];
        case AddOn::Dlib::FLT_Nose_9_Dots:
            return &CurDlib->m_shapes[index].m_pos[17 + 5 + 5];
        case AddOn::Dlib::FLT_LeftEye_6_Dots:
            return &CurDlib->m_shapes[index].m_pos[17 + 5 + 5 + 9];
        case AddOn::Dlib::FLT_RightEye_6_Dots:
            return &CurDlib->m_shapes[index].m_pos[17 + 5 + 5 + 9 + 6];
        case AddOn::Dlib::FLT_Mouth_20_Dots:
            return &CurDlib->m_shapes[index].m_pos[17 + 5 + 5 + 9 + 6 + 6];
        }
        return nullptr;
    }
}

#endif
