#include <boss.h>

#if !defined(BOSS_NEED_ADDON_PSD) || (BOSS_NEED_ADDON_PSD != 0 && BOSS_NEED_ADDON_PSD != 1)
    #error BOSS_NEED_ADDON_PSD macro is invalid use
#endif
bool __LINK_ADDON_PSD__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_PSD

#include "boss_addon_psd.hpp"

#include <boss.hpp>
#include <format/boss_bmp.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(Psd, Create, id_psd, uint32, uint32)
    BOSS_DECLARE_ADDON_FUNCTION(Psd, Release, void, id_psd)
    BOSS_DECLARE_ADDON_FUNCTION(Psd, Insert, void, id_psd, chars, id_bitmap_read, sint32, sint32)
    BOSS_DECLARE_ADDON_FUNCTION(Psd, Build, buffer, id_psd)

    static autorun Bind_AddOn_Psd()
    {
        Core_AddOn_Psd_Create() = Customized_AddOn_Psd_Create;
        Core_AddOn_Psd_Release() = Customized_AddOn_Psd_Release;
        Core_AddOn_Psd_Insert() = Customized_AddOn_Psd_Insert;
        Core_AddOn_Psd_Build() = Customized_AddOn_Psd_Build;
        return true;
    }
    static autorun _ = Bind_AddOn_Psd();
}

namespace psd
{
    class NativeBuffer : public File
    {
    public:
	    explicit NativeBuffer(Allocator* allocator) : File(allocator)
        {
        }

    public:
        buffer Build() const
        {
            buffer Result = Buffer::Alloc(BOSS_DBG mBuffer.Count());
            Memory::Copy((void*) Result, &mBuffer[0], mBuffer.Count());
            return Result;
        }

    private:
	    virtual bool DoOpenRead(const wchar_t* filename) override
        {
            return false;
        }

	    virtual bool DoOpenWrite(const wchar_t* filename) override
        {
            return true;
        }

	    virtual bool DoClose(void) override
        {
            mBuffer.Clear();
            return true;
        }

	    virtual File::ReadOperation DoRead(void* buffer, uint32_t count, uint64_t position) override
        {
            return nullptr;
        }

	    virtual bool DoWaitForRead(File::ReadOperation& operation) override
        {
            return false;
        }

	    virtual File::WriteOperation DoWrite(const void* buffer, uint32_t count, uint64_t position) override
        {
            OVERLAPPED* NewOperation = memoryUtil::Allocate<OVERLAPPED>(m_allocator);
	        NewOperation->hEvent = nullptr;
	        NewOperation->Offset = static_cast<DWORD>(position & 0xFFFFFFFFull);
	        NewOperation->OffsetHigh = static_cast<DWORD>((position >> 32u) & 0xFFFFFFFFull);

            if(auto CurBuffer = mBuffer.AtDumping((sint32) position, count))
                Memory::Copy(CurBuffer, buffer, count);
            else
            {
                memoryUtil::Free(m_allocator, NewOperation);
                return nullptr;
            }
            return static_cast<File::WriteOperation>(NewOperation);
        }

	    virtual bool DoWaitForWrite(File::WriteOperation& operation) override
        {
            OVERLAPPED* OldOverlapped = static_cast<OVERLAPPED*>(operation);
	        if(!OldOverlapped)
		        return false;
            memoryUtil::Free(m_allocator, OldOverlapped);
            return true;
        }

	    virtual uint64_t DoGetSize(void) const override
        {
            return mBuffer.Count();
        }

    private:
        uint08s mBuffer;
    };
}

// 구현부
namespace BOSS
{
    class PsdClass
    {
    public:
        PsdClass(uint32 width = 0, uint32 height = 0)
        {
            mDocument = CreateExportDocument(&mAllocator, width, height, 8u, exportColorMode::RGB);
            AddMetaData(mDocument, &mAllocator, "MyAttribute", "MyValue");
        }
        ~PsdClass()
        {
            DestroyExportDocument(mDocument, &mAllocator);
        }

    public:
        MallocAllocator mAllocator;
        ExportDocument* mDocument;
    };

    id_psd Customized_AddOn_Psd_Create(uint32 width, uint32 height)
    {
        buffer NewBuffer = Buffer::AllocNoConstructorOnce<PsdClass>(BOSS_DBG 1);
        BOSS_CONSTRUCTOR(NewBuffer, 0, PsdClass, width, height);
        return (id_psd) NewBuffer;
    }

    void Customized_AddOn_Psd_Release(id_psd psd)
    {
        Buffer::Free((buffer) psd);
    }

    void Customized_AddOn_Psd_Insert(id_psd psd, chars layername, id_bitmap_read bmp, sint32 x, sint32 y)
    {
        if(auto CurPsd = (PsdClass*) psd)
        if(Bmp::GetBitCount(bmp) == 32)
        {
            const sint32 BmpWidth = Bmp::GetWidth(bmp);
            const sint32 BmpHeight = Bmp::GetHeight(bmp);
            auto BmpBits = (const Bmp::bitmappixel*) Bmp::GetBits(bmp);

            const uint32 Newlayer = AddLayer(CurPsd->mDocument, &CurPsd->mAllocator, layername);
            const exportChannel::Enum ChannelCode[4] = {exportChannel::BLUE, exportChannel::GREEN, exportChannel::RED, exportChannel::ALPHA};

            uint08* ChannelBits = new uint08[BmpWidth * BmpHeight];
            for(sint32 i = 0; i < 4; ++i)
            {
                for(sint32 y = 0; y < BmpHeight; ++y)
                for(sint32 x = 0; x < BmpWidth; ++x)
                    ChannelBits[x + y * BmpWidth] = (BmpBits[x + (BmpHeight - 1 - y) * BmpWidth].argb >> (i * 8)) & 0xFF;
                UpdateLayer(CurPsd->mDocument, &CurPsd->mAllocator, Newlayer,
                    ChannelCode[i], x, y, x + BmpWidth, y + BmpHeight, ChannelBits, compressionType::ZIP);
            }
            delete[] ChannelBits;
        }
    }

    buffer Customized_AddOn_Psd_Build(id_psd psd)
    {
        buffer Result = nullptr;
        if(auto CurPsd = (PsdClass*) psd)
        {
            NativeBuffer NewBuffer(&CurPsd->mAllocator);
            WriteDocument(CurPsd->mDocument, &CurPsd->mAllocator, &NewBuffer);
            Result = NewBuffer.Build();
            NewBuffer.Close();
        }
        return Result;
    }
}

#endif
