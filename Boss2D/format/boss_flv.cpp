#include <boss.hpp>
#include "boss_flv.hpp"

#include <platform/boss_platform.hpp>

class FlvPrivate
{
public:
    typedef Array<uint08, datatype_pod_canmemcpy, 256> FlvBits;

public:
    FlvPrivate() {mReadFocus = 0; mReadTimeStamp = 0;}
    ~FlvPrivate() {}

public:
    FlvPrivate(const FlvPrivate&)
    {BOSS_ASSERT("잘못된 시나리오입니다", false);}
    FlvPrivate& operator=(const FlvPrivate&)
    {BOSS_ASSERT("잘못된 시나리오입니다", false); return *this;}

public:
    void WriteECMA(FlvBits& dst, chars name, sint32 value)
    {
        WriteScriptData(dst, name);
        dst.AtAdding() = 0x08; // type: ecma
        Memory::Copy(dst.AtDumpingAdded(4), Flv::ToBE4((id_flash) this, value), 4); // value
    }
    void WriteNumber(FlvBits& dst, chars name, double value)
    {
        WriteScriptData(dst, name);
        dst.AtAdding() = 0x00; // type: number
        Memory::Copy(dst.AtDumpingAdded(8), Flv::ToBE8_Double((id_flash) this, value), 8); // value
    }
    void WriteBoolean(FlvBits& dst, chars name, bool value)
    {
        WriteScriptData(dst, name);
        dst.AtAdding() = 0x01; // type: boolean
        dst.AtAdding() = value; // value
    }
    void WriteString(FlvBits& dst, chars name, chars value)
    {
        WriteScriptData(dst, name);
        dst.AtAdding() = 0x02; // type: string
        WriteScriptData(dst, value);
    }

private:
    void WriteScriptData(FlvBits& dst, chars name)
    {
        const sint32 Length = boss_strlen(name);
        Memory::Copy(dst.AtDumpingAdded(2), Flv::ToBE2((id_flash) this, Length), 2); // stringsize
        Memory::Copy(dst.AtDumpingAdded(Length), name, Length); // string
    }

public:
    FlvBits mBits;
    sint32 mReadFocus;
    sint32 mReadTimeStamp;
    uint32 mTempBE2;
    uint32 mTempBE3;
    uint32 mTempBE4;
    uint64 mTempBE8_Double;
};

namespace BOSS
{
    id_flash Flv::Create(sint32 width, sint32 height)
    {
        auto NewFlash = (FlvPrivate*) Buffer::Alloc<FlvPrivate>(BOSS_DBG 1);
        FlvPrivate::FlvBits& Dst = NewFlash->mBits;

        // flv header
        Memory::Copy(Dst.AtDumpingAdded(3), "FLV", 3); // Signature
        Dst.AtAdding() = 0x01; // FLV Version
        Dst.AtAdding() = 0x01 | 0x04; // Video + Audio
        Memory::Copy(Dst.AtDumpingAdded(4), ToBE4((id_flash) NewFlash, 9), 4); // DataOffset
        Memory::Copy(Dst.AtDumpingAdded(4), ToBE4((id_flash) NewFlash, 0), 4); // PreviousTagSize0

        // ReadFocus가 flv header를 스킵함
        NewFlash->mReadFocus += 3 + 1 + 1 + 4 + 4;
        NewFlash->mReadTimeStamp = 0;

        // metaData
        FlvPrivate::FlvBits NewBits;
        NewBits.AtAdding() = 0x02; // 스트링데이터ID
        NewFlash->WriteECMA(NewBits, "onMetaData", 13);
        NewFlash->WriteNumber(NewBits, "duration", 0); // YouTube에는 없어도 되는 정보
        NewFlash->WriteNumber(NewBits, "width", width);
        NewFlash->WriteNumber(NewBits, "height", height);
        NewFlash->WriteNumber(NewBits, "videodatarate", 781.25);
        NewFlash->WriteNumber(NewBits, "framerate", 1000);
        NewFlash->WriteNumber(NewBits, "videocodecid", 7); // FLV_CODECID_H264: 7
        NewFlash->WriteNumber(NewBits, "audiodatarate", 128);
        NewFlash->WriteNumber(NewBits, "audiosamplerate", 44100);
        NewFlash->WriteNumber(NewBits, "audiosamplesize", 16);
        NewFlash->WriteBoolean(NewBits, "stereo", true);
        NewFlash->WriteNumber(NewBits, "audiocodecid", 10); // FLV_CODECID_AAC: 10
        NewFlash->WriteString(NewBits, "encoder", "Lavf53.24.2");
        NewFlash->WriteNumber(NewBits, "filesize", 0); // YouTube에는 없어도 되는 정보
        Memory::Copy(NewBits.AtDumpingAdded(3), ToBE3((id_flash) NewFlash, 9), 3); // scriptdata endcode: always 9
        WriteChunk((id_flash) NewFlash, 0x12, &NewBits[0], NewBits.Count()); // script

        // 0001 0... .... .... : AAC low complexity
        // .... .010 0... .... : 44100 hz
        // .... .... .001 0... : 2 channel
        // .... .... .... .0.. : 1024 sample
        // .... .... .... ..0. : depends on core coder
        // .... .... .... ...0 : extension flag
        // 1    2    1    0
        WriteChunk((id_flash) NewFlash, 0x08, (bytes) "\xaf\x00" "\x12\x10", 4); // audio
        return (id_flash) NewFlash;
    }

    id_flash Flv::Create(chars filename)
    {
        auto NewFlash = (FlvPrivate*) Buffer::Alloc<FlvPrivate>(BOSS_DBG 1);
        FlvPrivate::FlvBits& Dst = NewFlash->mBits;

        if(id_file_read FlvFile = Platform::File::OpenForRead(filename))
        {
            const sint32 FlvSize = Platform::File::Size(FlvFile);
            if(0 < FlvSize)
            {
                Platform::File::Read(FlvFile, Dst.AtDumpingAdded(FlvSize), FlvSize);
                Platform::File::Close(FlvFile);

                // ReadFocus가 flv header를 스킵함
                if(Dst[0] == 'F' && Dst[1] == 'L' && Dst[2] == 'V')
                {
                    NewFlash->mReadFocus += 3 + 1 + 1 + 4 + 4;
                    NewFlash->mReadTimeStamp = 0;
                    return (id_flash) NewFlash;
                }
            }
        }
        Buffer::Free((buffer) NewFlash);
        return nullptr;
    }

    void Flv::Remove(id_flash flash)
    {
        Buffer::Free((buffer) flash);
    }

    bytes Flv::ReadChunk(id_flash flash, uint08* type, sint32* length, sint32* timestamp)
    {
        if(!flash) return nullptr;
        const FlvPrivate::FlvBits& Src = ((FlvPrivate*) flash)->mBits;
        sint32 SrcFocus = ((FlvPrivate*) flash)->mReadFocus;
        if(Src.Count() <= SrcFocus)
            return nullptr;

        bytes Chunk = &Src[SrcFocus + 1 + 3 + 3 + 1 + 3];
        const sint32 ChunkLength = FromBE3(&Src[SrcFocus + 1]);
        const sint32 ChunkTimeStamp = FromBE3(&Src[SrcFocus + 1 + 3]) | ((Src[SrcFocus + 1 + 3 + 3] & 0xFF) << 24);
        ((FlvPrivate*) flash)->mReadFocus += 1 + 3 + 3 + 1 + 3 + ChunkLength + 4;
        ((FlvPrivate*) flash)->mReadTimeStamp = ChunkTimeStamp;

        if(type) *type = Src[SrcFocus];
        if(length) *length = ChunkLength;
        if(timestamp) *timestamp = ChunkTimeStamp;
        return Chunk;
    }

    void Flv::WriteChunk(id_flash flash, uint08 type, bytes chunk, sint32 length, sint32 timestamp)
    {
        if(!flash) return;
        FlvPrivate::FlvBits& Dst = ((FlvPrivate*) flash)->mBits;

        Dst.AtAdding() = type; // type
        Memory::Copy(Dst.AtDumpingAdded(3), ToBE3(flash, length), 3); // datasize
        Memory::Copy(Dst.AtDumpingAdded(3), ToBE3(flash, timestamp & 0x00FFFFFF), 3); // timestamp
        Dst.AtAdding() = (timestamp & 0xFF000000) >> 24; // timestamp extended
        Memory::Copy(Dst.AtDumpingAdded(3), ToBE3(flash, 0), 3); // streamid
        Memory::Copy(Dst.AtDumpingAdded(length), chunk, length);
        Memory::Copy(Dst.AtDumpingAdded(4), ToBE4(flash, length + 11), 4); // chunk size match
    }

    sint32 Flv::TimeStampForReadFocus(id_flash flash)
    {
        if(!flash) return 0;
        return ((FlvPrivate*) flash)->mReadTimeStamp;
    }

    void Flv::Empty(id_flash flash)
    {
        if(!flash) return;
        ((FlvPrivate*) flash)->mBits.SubtractionAll();
        ((FlvPrivate*) flash)->mReadFocus = 0;
        ((FlvPrivate*) flash)->mReadTimeStamp = 0;
    }

    bytes Flv::GetBits(id_flash_read flash, sint32* length)
    {
        if(!flash) return nullptr;
        FlvPrivate::FlvBits& Dst = ((FlvPrivate*) flash)->mBits;
        if(length) *length = Dst.Count();
        return (bytes) ((Share*)(id_share) Dst)->const_data();
    }

    String Flv::BuildLog(bytes bits, sint32 length)
    {
        String LogCollector;
        while(0 < length)
        {
            sint32 ChunkSize = 0;
            if(bits[0] == (uint08) 'F' && bits[1] == (uint08) 'L' && bits[2] == (uint08) 'V')
            {
                ChunkSize = 13;
                LogCollector += "[FLV]\r\n";
            }
            else
            {
                const sint32 DataSize = ((bits[1] & 0xFF) << 16) | ((bits[2] & 0xFF) << 8) | (bits[3] & 0xFF);
                ChunkSize = 11 + DataSize + 4;
                switch(bits[0])
                {
                case 0x08: LogCollector += "[Audio: "; break;
                case 0x09: LogCollector += "[Video: "; break;
                case 0x12: LogCollector += "[MetaData: "; break;
                default: LogCollector += "[Unknown: "; break;
                }
                const sint32 TimeStamp = ((bits[7] & 0xFF) << 24) | ((bits[4] & 0xFF) << 16) | ((bits[5] & 0xFF) << 8) | (bits[6] & 0xFF);
                LogCollector += String::Format("%dms, %dbyte]", TimeStamp, DataSize);

                // 청크데이터
                for(sint32 i = 0; i < DataSize; ++i)
                {
                    if((i % 16) == 0)
                        LogCollector += "\r\n\t";
                    LogCollector += String::Format("%02X ", bits[11 + i]);
                }
                LogCollector += "\r\n";
            }
            bits += ChunkSize;
            length -= ChunkSize;
        }
        BOSS_ASSERT("불완전한 청크가 존재합니다", length == 0);
        return LogCollector;
    }

    const void* Flv::ToBE2(id_flash flash, sint32 value)
    {
        auto& Result = ((FlvPrivate*) flash)->mTempBE2;
        Result  = (value >> 8) & 0x000000FF;
        Result |= (value << 8) & 0x0000FF00;
        return &Result;
    }

    const void* Flv::ToBE3(id_flash flash, sint32 value)
    {
        auto& Result = ((FlvPrivate*) flash)->mTempBE3;
        Result  = (value >> 16) & 0x000000FF;
        Result |= (value >>  0) & 0x0000FF00;
        Result |= (value << 16) & 0x00FF0000;
        return &Result;
    }

    const void* Flv::ToBE4(id_flash flash, sint32 value)
    {
        auto& Result = ((FlvPrivate*) flash)->mTempBE4;
        Result  = (value >> 24) & 0x000000FF;
        Result |= (value >>  8) & 0x0000FF00;
        Result |= (value <<  8) & 0x00FF0000;
        Result |= (value << 24) & 0xFF000000;
        return &Result;
    }

    const void* Flv::ToBE8_Double(id_flash flash, double value)
    {
        auto& Result = ((FlvPrivate*) flash)->mTempBE8_Double;
        const uint64 SrcValue = *((uint64*) &value);
        Result  = (SrcValue >> 56) & ox00000000000000FF;
        Result |= (SrcValue >> 40) & ox000000000000FF00;
        Result |= (SrcValue >> 24) & ox0000000000FF0000;
        Result |= (SrcValue >>  8) & ox00000000FF000000;
        Result |= (SrcValue <<  8) & ox000000FF00000000;
        Result |= (SrcValue << 24) & ox0000FF0000000000;
        Result |= (SrcValue << 40) & ox00FF000000000000;
        Result |= (SrcValue << 56) & oxFF00000000000000;
        return &Result;
    }

    sint32 Flv::FromBE2(const void* be2)
    {
        uint32 Result;
        bytes SrcValue = (bytes) be2;
        Result  = (SrcValue[0] & 0xFF) << 8;
        Result |= (SrcValue[1] & 0xFF) << 0;
        return *((sint32*) &Result);
    }

    sint32 Flv::FromBE3(const void* be3)
    {
        uint32 Result;
        bytes SrcValue = (bytes) be3;
        Result  = (SrcValue[0] & 0xFF) << 16;
        Result |= (SrcValue[1] & 0xFF) <<  8;
        Result |= (SrcValue[2] & 0xFF) <<  0;
        return *((sint32*) &Result);
    }

    sint32 Flv::FromBE4(const void* be4)
    {
        uint32 Result;
        bytes SrcValue = (bytes) be4;
        Result  = (SrcValue[0] & 0xFF) << 24;
        Result |= (SrcValue[1] & 0xFF) << 16;
        Result |= (SrcValue[2] & 0xFF) <<  8;
        Result |= (SrcValue[3] & 0xFF) <<  0;
        return *((sint32*) &Result);
    }

    double Flv::FromBE8_Double(const void* be8_double)
    {
        uint64 Result;
        bytes SrcValue = (bytes) be8_double;
        Result  = (SrcValue[0] & ox00000000000000FF) << 56;
        Result |= (SrcValue[1] & ox00000000000000FF) << 48;
        Result |= (SrcValue[2] & ox00000000000000FF) << 40;
        Result |= (SrcValue[3] & ox00000000000000FF) << 32;
        Result |= (SrcValue[4] & ox00000000000000FF) << 24;
        Result |= (SrcValue[5] & ox00000000000000FF) << 16;
        Result |= (SrcValue[6] & ox00000000000000FF) <<  8;
        Result |= (SrcValue[7] & ox00000000000000FF) <<  0;
        return *((double*) &Result);
    }
}
