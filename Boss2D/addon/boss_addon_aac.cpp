#include <boss.h>

#if !defined(BOSS_NEED_ADDON_AAC) || (BOSS_NEED_ADDON_AAC != 0 && BOSS_NEED_ADDON_AAC != 1)
    #error BOSS_NEED_ADDON_AAC macro is invalid use
#endif
bool __LINK_ADDON_AAC__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_AAC

#include "boss_addon_aac.hpp"

#include <boss.hpp>
#include <format/boss_flv.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(Aac, Create, id_acc, sint32, sint32, sint32)
    BOSS_DECLARE_ADDON_FUNCTION(Aac, Release, void, id_acc)
    BOSS_DECLARE_ADDON_FUNCTION(Aac, EncodeTo, void, id_acc, bytes, sint32, id_flash, uint64)
    BOSS_DECLARE_ADDON_FUNCTION(Aac, SilenceTo, void, id_acc, id_flash, uint64)

    static autorun Bind_AddOn_Aac()
    {
        Core_AddOn_Aac_Create() = Customized_AddOn_Aac_Create;
        Core_AddOn_Aac_Release() = Customized_AddOn_Aac_Release;
        Core_AddOn_Aac_EncodeTo() = Customized_AddOn_Aac_EncodeTo;
        Core_AddOn_Aac_SilenceTo() = Customized_AddOn_Aac_SilenceTo;
        return true;
    }
    static autorun _ = Bind_AddOn_Aac();
}

// 구현부
namespace BOSS
{
    id_acc Customized_AddOn_Aac_Create(sint32 bitrate, sint32 channel, sint32 samplerate)
    {
        BaseEncoderAac* NewEncoder = new BaseEncoderAac(bitrate, channel, samplerate);
        if(NewEncoder->IsValid()) return (id_acc) NewEncoder;
        delete NewEncoder;
        return nullptr;
    }

    void Customized_AddOn_Aac_Release(id_acc acc)
    {
        BaseEncoderAac* OldEncoder = (BaseEncoderAac*) acc;
        delete OldEncoder;
    }

    void Customized_AddOn_Aac_EncodeTo(id_acc acc, bytes pcm, sint32 length, id_flash flash, uint64 timems)
    {
        BaseEncoderAac* CurEncoder = (BaseEncoderAac*) acc;
        if(CurEncoder)
            return CurEncoder->EncodeTo(pcm, length, flash, timems);
    }

    void Customized_AddOn_Aac_SilenceTo(id_acc acc, id_flash flash, uint64 timems)
    {
        BaseEncoderAac* CurEncoder = (BaseEncoderAac*) acc;
        if(CurEncoder)
            return CurEncoder->SilenceTo(flash, timems);
    }
}

BaseEncoderAac::BaseEncoderAac(sint32 bitrate, sint32 channel, sint32 samplerate)
{
    mEncoder = nullptr;
    Memory::Set(&mInfo, 0x00, sizeof(AACENC_InfoStruct));
    mChunkSize = 0;

    CHANNEL_MODE mode;
    switch(channel)
    {
    case 1: mode = MODE_1; break;
    case 2: mode = MODE_2; break;
    case 3: mode = MODE_1_2; break;
    case 4: mode = MODE_1_2_1; break;
    case 5: mode = MODE_1_2_2; break;
    case 6: mode = MODE_1_2_2_1; break;
    default: return;
    }

    const sint32 aot = 2;
    const sint32 afterburner = 1;
    const sint32 eld_sbr = 0;
    const sint32 vbr = 0;
    mEncoder = new HANDLE_AACENCODER();

    if(aacEncOpen(mEncoder, 0, channel) != AACENC_OK) goto InitEncoderError_Step_Delete;
    if(aacEncoder_SetParam(*mEncoder, AACENC_AOT, aot) != AACENC_OK) goto InitEncoderError_Step_Close;
    if(aot == 39 && eld_sbr && aacEncoder_SetParam(*mEncoder, AACENC_SBR_MODE, 1) != AACENC_OK) goto InitEncoderError_Step_Close;
    if(aacEncoder_SetParam(*mEncoder, AACENC_SAMPLERATE, samplerate) != AACENC_OK) goto InitEncoderError_Step_Close;
    if(aacEncoder_SetParam(*mEncoder, AACENC_CHANNELMODE, mode) != AACENC_OK) goto InitEncoderError_Step_Close;
    if(aacEncoder_SetParam(*mEncoder, AACENC_CHANNELORDER, 1) != AACENC_OK) goto InitEncoderError_Step_Close;
    if(vbr) {if(aacEncoder_SetParam(*mEncoder, AACENC_BITRATEMODE, vbr) != AACENC_OK) goto InitEncoderError_Step_Close;}
    else if(aacEncoder_SetParam(*mEncoder, AACENC_BITRATE, bitrate) != AACENC_OK) goto InitEncoderError_Step_Close;
    if(aacEncoder_SetParam(*mEncoder, AACENC_TRANSMUX, 2) != AACENC_OK) goto InitEncoderError_Step_Close;
    if(aacEncoder_SetParam(*mEncoder, AACENC_AFTERBURNER, afterburner) != AACENC_OK) goto InitEncoderError_Step_Close;
    if(aacEncEncode(*mEncoder, NULL, NULL, NULL, NULL) != AACENC_OK) goto InitEncoderError_Step_Close;
    if(aacEncInfo(*mEncoder, &mInfo) != AACENC_OK) goto InitEncoderError_Step_Close;
    mChunkSize = channel * 2 * mInfo.frameLength;
    return;

InitEncoderError_Step_Close:
    aacEncClose(mEncoder);
InitEncoderError_Step_Delete:
    delete mEncoder;
    mEncoder = nullptr;
}

BaseEncoderAac::~BaseEncoderAac()
{
    if(mEncoder)
        aacEncClose(mEncoder);
}

void BaseEncoderAac::EncodeTo(bytes pcm, sint32 length, id_flash flash, uint64 timems)
{
    static uint08s Contents, Chunk;
    Contents.SubtractionAll();

    while(true)
    {
        AACENC_BufDesc in_buf = {0}, out_buf = {0};
        AACENC_InArgs in_args = {0};
        AACENC_OutArgs out_args = {0};
        uint08 outbuf[20480];

        void* in_ptr = (void*) pcm;
        void* out_ptr = outbuf;
        sint32 in_identifier = IN_AUDIO_DATA;
        sint32 out_identifier = OUT_BITSTREAM_DATA;
        sint32 in_size = Math::Min(length, mChunkSize);
        sint32 out_size = sizeof(outbuf);
        sint32 in_elem_size = 2;
        sint32 out_elem_size = 1;
        if(in_size == 0)
            in_args.numInSamples = -1;
        else
        {
            in_args.numInSamples = in_size / 2;
            in_buf.numBufs = 1;
            in_buf.bufs = &in_ptr;
            in_buf.bufferIdentifiers = &in_identifier;
            in_buf.bufSizes = &in_size;
            in_buf.bufElSizes = &in_elem_size;
        }
        out_buf.numBufs = 1;
        out_buf.bufs = &out_ptr;
        out_buf.bufferIdentifiers = &out_identifier;
        out_buf.bufSizes = &out_size;
        out_buf.bufElSizes = &out_elem_size;

        AACENC_ERROR err = aacEncEncode(*mEncoder, &in_buf, &out_buf, &in_args, &out_args);
        if(err != AACENC_OK)
        {
            BOSS_ASSERT(String::Format("AAC인코딩에 실패하였습니다(errcode=%d)", (sint32) err),
                err == AACENC_ENCODE_EOF);
            break;
        }

        pcm += in_size;
        length -= in_size;
        if(out_args.numOutBytes == 0)
            continue;
        Memory::Copy(Contents.AtDumpingAdded(out_args.numOutBytes), outbuf, out_args.numOutBytes);
    }

    if(sint32 CurAacSize = Contents.Count())
    {
        bytes CurAacPtr = Contents.AtDumping(0, CurAacSize);
        const sint32 AacHeadSize = 7;
        while(AacHeadSize < CurAacSize)
        {
            Chunk.SubtractionAll();
            Memory::Copy(Chunk.AtDumpingAdded(2), "\xaf\x01", 2);
            const sint32 CurChunkSize = (((CurAacPtr[4] & 0xFF) << 4) | ((CurAacPtr[5] & 0xF0) >> 4)) / 2;
            const sint32 CurContentSize = CurChunkSize - AacHeadSize;
            Memory::Copy(Chunk.AtDumpingAdded(CurContentSize), &CurAacPtr[AacHeadSize], CurContentSize);
            Flv::AddChunk(flash, 0x08, &Chunk[0], Chunk.Count(), timems); // audio
            CurAacPtr += CurChunkSize;
            CurAacSize -= CurChunkSize;
        }
    }
}

void BaseEncoderAac::SilenceTo(id_flash flash, uint64 timems)
{
    static const uint08 Silence[7056] = {0,};
    EncodeTo(Silence, 7056, flash, timems);
}

#endif
