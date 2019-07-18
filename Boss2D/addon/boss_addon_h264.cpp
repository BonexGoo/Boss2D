#include <boss.h>

#if !defined(BOSS_NEED_ADDON_H264) || (BOSS_NEED_ADDON_H264 != 0 && BOSS_NEED_ADDON_H264 != 1)
    #error BOSS_NEED_ADDON_H264 macro is invalid use
#endif
bool __LINK_ADDON_H264__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_H264

#include "boss_addon_h264.hpp"

#include <boss.hpp>
#include <format/boss_bmp.hpp>
#include <format/boss_flv.hpp>
#include <platform/boss_platform.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(H264, CreateEncoder, id_h264, sint32, sint32, bool)
    BOSS_DECLARE_ADDON_FUNCTION(H264, CreateDecoder, id_h264, void)
    BOSS_DECLARE_ADDON_FUNCTION(H264, Release, void, id_h264)
    BOSS_DECLARE_ADDON_FUNCTION(H264, EncodeOnce, void, id_h264, const uint32*, id_flash, uint64)
    BOSS_DECLARE_ADDON_FUNCTION(H264, DecodeBitmapOnce, id_bitmap, id_h264, id_flash, uint64, uint64*)
    BOSS_DECLARE_ADDON_FUNCTION(H264, DecodeTextureOnce, id_texture, id_h264, id_flash, uint64, uint64*)
    BOSS_DECLARE_ADDON_FUNCTION(H264, DecodeSeek, void, id_h264, id_flash, uint64)

    static autorun Bind_AddOn_H264()
    {
        Core_AddOn_H264_CreateEncoder() = Customized_AddOn_H264_CreateEncoder;
        Core_AddOn_H264_CreateDecoder() = Customized_AddOn_H264_CreateDecoder;
        Core_AddOn_H264_Release() = Customized_AddOn_H264_Release;
        Core_AddOn_H264_EncodeOnce() = Customized_AddOn_H264_EncodeOnce;
        Core_AddOn_H264_DecodeBitmapOnce() = Customized_AddOn_H264_DecodeBitmapOnce;
        Core_AddOn_H264_DecodeTextureOnce() = Customized_AddOn_H264_DecodeTextureOnce;
        Core_AddOn_H264_DecodeSeek() = Customized_AddOn_H264_DecodeSeek;
        return true;
    }
    static autorun _ = Bind_AddOn_H264();
}

// 구현부
namespace BOSS
{
    id_h264 Customized_AddOn_H264_CreateEncoder(sint32 width, sint32 height, bool fastmode)
    {
        auto NewEncoder = new H264EncoderPrivate(width, height, fastmode);
        return (id_h264) NewEncoder;
    }

    id_h264 Customized_AddOn_H264_CreateDecoder(void)
    {
        auto NewDecoder = new H264DecoderPrivate();
        return (id_h264) NewDecoder;
    }

    void Customized_AddOn_H264_Release(id_h264 h264)
    {
        delete (H264Private*) h264;
    }

    void Customized_AddOn_H264_EncodeOnce(id_h264 h264, const uint32* rgba, id_flash flash, uint64 timems)
    {
        auto CurEncoder = H264EncoderPrivate::Test(h264);
        if(CurEncoder)
            CurEncoder->Encode(rgba, flash, timems);
    }

    id_bitmap Customized_AddOn_H264_DecodeBitmapOnce(id_h264 h264, id_flash flash, uint64 settimems, uint64* gettimems)
    {
        auto CurDecoder = H264DecoderPrivate::Test(h264);
        if(CurDecoder)
            return CurDecoder->DecodeBitmap(flash, settimems, gettimems);
        return nullptr;
    }

    id_texture Customized_AddOn_H264_DecodeTextureOnce(id_h264 h264, id_flash flash, uint64 settimems, uint64* gettimems)
    {
        auto CurDecoder = H264DecoderPrivate::Test(h264);
        if(CurDecoder)
            return CurDecoder->DecodeTexture(flash, settimems, gettimems);
        return nullptr;
    }

    void Customized_AddOn_H264_DecodeSeek(id_h264 h264, id_flash flash, uint64 timems)
    {
        auto CurDecoder = H264DecoderPrivate::Test(h264);
        if(CurDecoder)
            CurDecoder->Seek(flash, timems);
    }
}

H264Private::H264Private()
{
}

H264Private::~H264Private()
{
}

const void* H264Private::GetBE2(sint32 value)
{
    mTempBE2  = (value >>  8) & 0x000000FF;
    mTempBE2 |= (value <<  8) & 0x0000FF00;
    return &mTempBE2;
}

const void* H264Private::GetBE3(sint32 value)
{
    mTempBE3  = (value >> 16) & 0x000000FF;
    mTempBE3 |= (value >>  0) & 0x0000FF00;
    mTempBE3 |= (value << 16) & 0x00FF0000;
    return &mTempBE3;
}

const void* H264Private::GetBE4(sint32 value)
{
    mTempBE4  = (value >> 24) & 0x000000FF;
    mTempBE4 |= (value >>  8) & 0x0000FF00;
    mTempBE4 |= (value <<  8) & 0x00FF0000;
    mTempBE4 |= (value << 24) & 0xFF000000;
    return &mTempBE4;
}

const void* H264Private::GetBE8_Double(double value)
{
    const uint64 SrcValue = *((uint64*) &value);
    mTempBE8_Double  = (SrcValue >> 56) & ox00000000000000FF;
    mTempBE8_Double |= (SrcValue >> 40) & ox000000000000FF00;
    mTempBE8_Double |= (SrcValue >> 24) & ox0000000000FF0000;
    mTempBE8_Double |= (SrcValue >>  8) & ox00000000FF000000;
    mTempBE8_Double |= (SrcValue <<  8) & ox000000FF00000000;
    mTempBE8_Double |= (SrcValue << 24) & ox0000FF0000000000;
    mTempBE8_Double |= (SrcValue << 40) & ox00FF000000000000;
    mTempBE8_Double |= (SrcValue << 56) & oxFF00000000000000;
    return &mTempBE8_Double;
}

void H264Private::WriteTag(uint08s& dst, uint08 type, sint32 timestamp, const uint08s chunk)
{
    dst.AtAdding() = type; // type
    Memory::Copy(dst.AtDumpingAdded(3), GetBE3(chunk.Count()), 3); // datasize
    Memory::Copy(dst.AtDumpingAdded(3), GetBE3(timestamp & 0x00FFFFFF), 3); // timestamp
    dst.AtAdding() = (timestamp & 0xFF000000) >> 24; // timestamp extended
    Memory::Copy(dst.AtDumpingAdded(3), GetBE3(0), 3); // streamid
    Memory::Copy(dst.AtDumpingAdded(chunk.Count()), &chunk[0], chunk.Count());
    Memory::Copy(dst.AtDumpingAdded(4), GetBE4(chunk.Count() + 11), 4); // chunk size match
}

void H264Private::WriteScriptDataImpl(uint08s& dst, chars name)
{
    const sint32 Length = boss_strlen(name);
    Memory::Copy(dst.AtDumpingAdded(2), GetBE2(Length), 2); // stringsize
    Memory::Copy(dst.AtDumpingAdded(Length), name, Length); // string
}

void H264Private::WriteScriptDataECMA(uint08s& dst, chars name, sint32 value)
{
    WriteScriptDataImpl(dst, name);
    dst.AtAdding() = 0x08; // type: ecma
    Memory::Copy(dst.AtDumpingAdded(4), GetBE4(value), 4); // value
}

void H264Private::WriteScriptDataNumber(uint08s& dst, chars name, double value)
{
    WriteScriptDataImpl(dst, name);
    dst.AtAdding() = 0x00; // type: number
    Memory::Copy(dst.AtDumpingAdded(8), GetBE8_Double(value), 8); // value
}

void H264Private::WriteScriptDataBoolean(uint08s& dst, chars name, bool value)
{
    WriteScriptDataImpl(dst, name);
    dst.AtAdding() = 0x01; // type: boolean
    dst.AtAdding() = value; // value
}

void H264Private::WriteScriptDataString(uint08s& dst, chars name, chars value)
{
    WriteScriptDataImpl(dst, name);
    dst.AtAdding() = 0x02; // type: string
    WriteScriptDataImpl(dst, value);
}

static void TraceCallback(void* ctx, int level, const char* string)
{
    if(level == LEVEL_1_0)
        BOSS_ASSERT(string, false);
    else BOSS_TRACE(string);
}

H264EncoderPrivate::H264EncoderPrivate(sint32 width, sint32 height, bool fastmode)
{
    BOSS_ASSERT("width값은 항상 짝수여야 합니다", (width & 1) == 0);
    BOSS_ASSERT("height값은 항상 짝수여야 합니다", (height & 1) == 0);
	width -= (width & 1);
	height -= (height & 1);

    mEncoder = nullptr;
    int rv = WelsCreateSVCEncoder(&mEncoder);
    BOSS_ASSERT("WelsCreateSVCEncoder가 실패하였습니다", rv == cmResultSuccess && mEncoder != nullptr);

    unsigned int uiTraceLevel = WELS_LOG_DETAIL;
    mEncoder->SetOption(ENCODER_OPTION_TRACE_LEVEL, &uiTraceLevel);
    WelsTraceCallback cbTraceCallback = TraceCallback;
    mEncoder->SetOption(ENCODER_OPTION_TRACE_CALLBACK, &cbTraceCallback);

    SEncParamExt param;
    mEncoder->GetDefaultParams(&param);
    param.iUsageType                 = SCREEN_CONTENT_REAL_TIME;
    param.iPicWidth                  = width;                // width of picture in samples
    param.iPicHeight                 = height;               // height of picture in samples
    param.fMaxFrameRate              = 60.0f;                // input frame rate
    param.iTargetBitrate             = 4 * width * height;   // target bitrate desired
    param.iMaxBitrate                = UNSPECIFIED_BIT_RATE;
    param.iRCMode                    = (fastmode)? RC_BITRATE_MODE : RC_QUALITY_MODE;
    param.iTemporalLayerNum          = 3;                    // layer number at temporal level
    param.iSpatialLayerNum           = 1;                    // layer number at spatial level
    param.bEnableDenoise             = 0;                    // denoise control
    param.bEnableBackgroundDetection = 1;                    // background detection control
    param.bEnableAdaptiveQuant       = 1;                    // adaptive quantization control
    param.bEnableFrameSkip           = 1;                    // frame skipping
    param.bEnableLongTermReference   = 0;                    // long term reference control
    param.iLtrMarkPeriod             = 30;
    param.uiIntraPeriod              = 320;                  // period of Intra frame
    param.eSpsPpsIdStrategy          = CONSTANT_ID;
    param.bPrefixNalAddingCtrl       = 0;
    param.iComplexityMode            = (fastmode)? LOW_COMPLEXITY : HIGH_COMPLEXITY;
    param.bSimulcastAVC              = false;
    param.iEntropyCodingModeFlag     = 1;                    // 0:CAVLC  1:CABAC.
    for (int i = 0; i < param.iSpatialLayerNum; i++)
    {
        param.sSpatialLayers[i].uiProfileIdc               = PRO_MAIN;
        param.sSpatialLayers[i].iVideoWidth                = width;
        param.sSpatialLayers[i].iVideoHeight               = height;
        param.sSpatialLayers[i].fFrameRate                 = param.fMaxFrameRate;
        param.sSpatialLayers[i].iSpatialBitrate            = param.iTargetBitrate;
        param.sSpatialLayers[i].iMaxSpatialBitrate         = param.iMaxBitrate;
        param.sSpatialLayers[i].sSliceArgument.uiSliceMode = SM_FIXEDSLCNUM_SLICE;
        param.sSpatialLayers[i].sSliceArgument.uiSliceNum  = 1;
    }
    rv = mEncoder->InitializeExt(&param);
    BOSS_ASSERT("InitWithParam이 실패하였습니다", rv == cmResultSuccess);

    memset(&mPic, 0, sizeof(mPic));
    mPic.iPicWidth = width;
    mPic.iPicHeight = height;
    mPic.iColorFormat = videoFormatI420;
    mPic.iStride[0] = mPic.iPicWidth;
    mPic.iStride[1] = mPic.iStride[2] = mPic.iPicWidth / 2;
    mPic.pData[0] = mFrame.AtDumping(0, width * height * 3 / 2); // I420: 1(Y) + 1/4(U) + 1/4(V)
    mPic.pData[1] = mPic.pData[0] + width * height;
    mPic.pData[2] = mPic.pData[1] + width * height / 4;

    memset(&mInfo, 0, sizeof(mInfo));
}

H264EncoderPrivate::~H264EncoderPrivate()
{
    if(mEncoder)
    {
        mEncoder->Uninitialize();
        WelsDestroySVCEncoder(mEncoder);
    }
}

#define BSTIME_RATE (10)

void H264EncoderPrivate::Encode(const uint32* rgba, id_flash flash, uint64 timems)
{
    uint08* yplane = mPic.pData[0];
    uint08* uplane = mPic.pData[1];
    uint08* vplane = mPic.pData[2];
    for(sint32 i = 0, iend = mPic.iPicWidth * mPic.iPicHeight; i < iend; ++i)
    {
        Bmp::bitmappixel& color = *((Bmp::bitmappixel*) rgba++);
        *(yplane++) = ((66 * color.r + 129 * color.g + 25 * color.b) >> 8) + 16;
        if(!((i / mPic.iPicWidth) & 1) && !(i & 1))
        {
            *(uplane++) = ((-38 * color.r + -74 * color.g + 112 * color.b) >> 8) + 128;
            *(vplane++) = ((112 * color.r + -94 * color.g + -18 * color.b) >> 8) + 128;
        }
    }

    int rv = mEncoder->EncodeFrame(&mPic, &mInfo);
    BOSS_ASSERT("EncodeFrame이 실패하였습니다", rv == cmResultSuccess);

    if(rv == cmResultSuccess && mInfo.eFrameType != videoFrameTypeSkip)
    {
        const sint32 CompositionTime = timems / BSTIME_RATE;
        for(sint32 i = 0; i < mInfo.iLayerNum; ++i)
        {
            const SLayerBSInfo& CurLayer = mInfo.sLayerInfo[i];
            sint32 BufSize = 0;
            for(sint32 j = 0; j < CurLayer.iNalCount; ++j)
                BufSize += CurLayer.pNalLengthInByte[j];

            // Video태그헤더(5)
            mTempChunk.SubtractionAll();
            const bool IsInterframe = (CurLayer.eFrameType == videoFrameTypeI);
            const bool IsNALU = (CurLayer.uiLayerType == VIDEO_CODING_LAYER);
            mTempChunk.AtAdding() = (IsInterframe)? 0x27 : 0x17; // 1_:keyframe, 2_:interframe, _7:AVC
            mTempChunk.AtAdding() = (IsNALU)? 0x01 : 0x00; // AVC NALU, AVC sequence header
            Memory::Copy(mTempChunk.AtDumpingAdded(3), GetBE3(CompositionTime & 0x00FFFFFF), 3); // CompositionTime

            // 태그데이터
            if(IsNALU)
            {
                Memory::Copy(mTempChunk.AtDumpingAdded(4), GetBE4(BufSize), 4); // NALU Size
                Memory::Copy(mTempChunk.AtDumpingAdded(BufSize), CurLayer.pBsBuf, BufSize); // NALU
                Flv::WriteChunk(flash, 0x09, &mTempChunk[0], mTempChunk.Count(), CompositionTime * BSTIME_RATE); // video
            }
            else
            {
                BOSS_ASSERT("AVC sequence header는 iNalCount가 2여야만 합니다", CurLayer.iNalCount == 2);
                const sint32 SPSBegin = 4;
                const sint32 SPSEnd = CurLayer.pNalLengthInByte[0];
                const sint32 PPSBegin = SPSEnd + 4;
                const sint32 PPSEnd = SPSEnd + CurLayer.pNalLengthInByte[1];
                mTempChunk.AtAdding() = 0x01; // Configuration Version
                mTempChunk.AtAdding() = CurLayer.pBsBuf[5]; // AVCProfile-Indication
                mTempChunk.AtAdding() = CurLayer.pBsBuf[6]; // AVCProfile-Compatibility
                mTempChunk.AtAdding() = CurLayer.pBsBuf[7]; // AVCLevel-Indication
                mTempChunk.AtAdding() = 0xFF; // lengthSizeMinusOne
                // SPS
                mTempChunk.AtAdding() = 0x01; // NumberOfSPS
                Memory::Copy(mTempChunk.AtDumpingAdded(2), GetBE2(SPSEnd - SPSBegin), 2); // SPS-Length
                Memory::Copy(mTempChunk.AtDumpingAdded(SPSEnd - SPSBegin), &CurLayer.pBsBuf[SPSBegin], SPSEnd - SPSBegin); // SPS-NALU
                // PPS
                mTempChunk.AtAdding() = 0x01; // NumberOfPPS
                Memory::Copy(mTempChunk.AtDumpingAdded(2), GetBE2(PPSEnd - PPSBegin), 2); // PPS-Length
                Memory::Copy(mTempChunk.AtDumpingAdded(PPSEnd - PPSBegin), &CurLayer.pBsBuf[PPSBegin], PPSEnd - PPSBegin); // PPS-NALU
                Flv::WriteChunk(flash, 0x09, &mTempChunk[0], mTempChunk.Count(), CompositionTime * BSTIME_RATE); // video
            }
        }
    }
}

H264DecoderPrivate::H264DecoderPrivate()
{
    mDecoder = nullptr;
    int rv = WelsCreateDecoder(&mDecoder);
    BOSS_ASSERT("WelsCreateSVCDecoder가 실패하였습니다", rv == cmResultSuccess && mDecoder != nullptr);

    unsigned int uiTraceLevel = WELS_LOG_DETAIL;
    mDecoder->SetOption(DECODER_OPTION_TRACE_LEVEL, &uiTraceLevel);
    WelsTraceCallback cbTraceCallback = TraceCallback;
    mDecoder->SetOption(DECODER_OPTION_TRACE_CALLBACK, &cbTraceCallback);

    SDecodingParam param;
    memset (&param, 0, sizeof(SDecodingParam));
    param.uiTargetDqLayer = UCHAR_MAX;
    param.eEcActiveIdc = ERROR_CON_SLICE_COPY;
    param.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_DEFAULT;
    rv = mDecoder->Initialize(&param);
    BOSS_ASSERT("Initialize가 실패하였습니다", rv == cmResultSuccess);
}

H264DecoderPrivate::~H264DecoderPrivate()
{
    if(mDecoder)
    {
        mDecoder->Uninitialize();
        WelsDestroyDecoder(mDecoder);
    }
}

id_bitmap H264DecoderPrivate::DecodeBitmap(id_flash flash, uint64 settimems, uint64* gettimems)
{
    id_bitmap Result = nullptr;
    void* Payload[2] = {this, &Result};
    const uint64 ResultMsec = DecodeCore(flash, settimems,
        [](payload data, const Frame& frame)->void
        {
            auto& Self = *((H264DecoderPrivate*) ((void**) data)[0]);
            auto& Result = *((id_bitmap*) ((void**) data)[1]);
            const sint32 Width = frame.mY.mWidth;
            const sint32 Height = frame.mY.mHeight;

            Result = Bmp::Create(4, Width, Height);
            auto Bits = (Bmp::bitmappixel*) Bmp::GetBits(Result);
            for(sint32 y = 0; y < Height; ++y)
            {
                auto DstBits = &Bits[Width * (Height - 1 - y)];
                auto SrcY = &frame.mY.mData[frame.mY.mStride * y];
                auto SrcU = &frame.mU.mData[frame.mU.mStride * (y >> 1)];
                auto SrcV = &frame.mV.mData[frame.mV.mStride * (y >> 1)];
                for(sint32 x = 0; x < Width; ++x)
                {
                    const sint32 CurY = *SrcY & 0xFF;
                    const sint32 CurU = (*SrcU & 0xFF) - 128;
                    const sint32 CurV = (*SrcV & 0xFF) - 128;
                    const sint32 BaseColor = (sint32)(1.164f * (CurY - 16));
                    const sint32 RValue = (sint32)(1.596f * CurV);
                    const sint32 GValue = (sint32)(0.391f * CurU + 0.813f * CurV);
                    const sint32 BValue = (sint32)(2.018f * CurU);
                    DstBits->a = 0xFF;
                    DstBits->r = Math::Clamp(BaseColor + RValue, 0, 255);
                    DstBits->g = Math::Clamp(BaseColor - GValue, 0, 255);
                    DstBits->b = Math::Clamp(BaseColor + BValue, 0, 255);
                    DstBits++;
                    SrcY++;
                    SrcU += (x & 1);
                    SrcV += (x & 1);
                }
            }
        }, Payload);

    if(gettimems) *gettimems = ResultMsec;
    return Result;
}

id_texture H264DecoderPrivate::DecodeTexture(id_flash flash, uint64 settimems, uint64* gettimems)
{
    id_texture Result = nullptr;
    void* Payload[2] = {this, &Result};
    const uint64 ResultMsec = DecodeCore(flash, settimems,
        [](payload data, const Frame& frame)->void
        {
            auto& Self = *((H264DecoderPrivate*) ((void**) data)[0]);
            auto& Result = *((id_texture*) ((void**) data)[1]);
            const sint32 Width = frame.mY.mWidth;
            const sint32 Height = frame.mY.mHeight;
            const sint32 WidthHalf = Width / 2;
            const sint32 HeightHalf = Height / 2;

            uint08* DstY = Self.mTempBits.AtDumping(0, Width * Height * 3 / 2);
            uint08* DstUV = DstY + (Width * Height);
            bytes SrcY = frame.mY.mData + (frame.mY.mStride * (Height - 1));
            bytes SrcU = frame.mU.mData + (frame.mU.mStride * (HeightHalf - 1));
            bytes SrcV = frame.mV.mData + (frame.mV.mStride * (HeightHalf - 1));
            for(sint32 y = 0; y < Height; ++y)
            {
                Memory::Copy(DstY, SrcY, Width);
                DstY += Width;
                SrcY -= frame.mY.mStride;
            }
            for(sint32 y = 0; y < HeightHalf; ++y)
            {
                Memory::CopyStencil(DstUV + 1, SrcU, WidthHalf);
                Memory::CopyStencil(DstUV + 0, SrcV, WidthHalf);
                DstUV += Width;
                SrcU -= frame.mU.mStride;
                SrcV -= frame.mV.mStride;
            }
            Result = Platform::Graphics::CreateTexture(true, false, Width, Height, &Self.mTempBits[0]);
        }, Payload);

    if(gettimems) *gettimems = ResultMsec;
    return Result;
}

uint64 H264DecoderPrivate::DecodeCore(id_flash flash, uint64 settimems, OnDecodeFrame cb, payload data)
{
    if(settimems != 0 && settimems < Flv::TimeStampForReadFocus(flash))
        return settimems;

    uint08 Type = 0;
    bytes Chunk = nullptr;
    sint32 ChunkSize = 0, ChunkMsec = 0;
    while(Type != 0x09)
    {
        Chunk = Flv::ReadChunk(flash, &Type, &ChunkSize, &ChunkMsec);
        if(!Chunk) break;
		if(ChunkMsec < settimems)
		{
            const bool IsKeyFrame = !!(Chunk[0] & 0x10);
            const bool IsNALU = !!(Chunk[1] & 0x01);
            sint32 BsSize = 0;
            CollectorType* Collector = nullptr;
            bytes BsBuf = GetBsBuf(IsNALU, Chunk, ChunkSize, BsSize, Collector);
            DecodeFrame(BsBuf, BsSize, settimems, ChunkMsec);
            delete Collector;
            Type = 0;
        }
    }

    bytes BsBuf = nullptr;
    sint32 BsSize = 0;
    CollectorType* Collector = nullptr;
    if(Chunk)
    {
        const bool IsKeyFrame = !!(Chunk[0] & 0x10);
        const bool IsNALU = !!(Chunk[1] & 0x01);
        BsBuf = GetBsBuf(IsNALU, Chunk, ChunkSize, BsSize, Collector);
    }

    const uint64 ResultMsec = DecodeFrame(BsBuf, BsSize, settimems, ChunkMsec, cb, data);
    delete Collector;
    return ResultMsec;
}

void H264DecoderPrivate::Seek(id_flash flash, uint64 timems)
{
    sint32 CurChunkMsec = 0;
    while(CurChunkMsec < timems)
    {
        uint08 Type = 0;
        bytes Chunk = nullptr;
        sint32 ChunkSize = 0;
        while(Type != 0x09)
        {
            Chunk = Flv::ReadChunk(flash, &Type, &ChunkSize, &CurChunkMsec);
            if(!Chunk) return;
        }

        const bool IsKeyFrame = !!(Chunk[0] & 0x10);
        const bool IsNALU = !!(Chunk[1] & 0x01);
        sint32 BsSize = 0;
        CollectorType* Collector = nullptr;
        bytes BsBuf = GetBsBuf(IsNALU, Chunk, ChunkSize, BsSize, Collector);

        if(IsKeyFrame || !IsNALU)
            CurChunkMsec = (sint32) DecodeFrame(BsBuf, BsSize, 0, CurChunkMsec);

        delete Collector;
    }
}

uint64 H264DecoderPrivate::DecodeFrame(bytes src, sint32 sliceSize, sint32 setmsec, sint32 chunkmsec, OnDecodeFrame cb, payload data)
{
    uint08* bufdata[3] = {};
    SBufferInfo bufInfo;
    memset(bufdata, 0, sizeof(bufdata));
    memset(&bufInfo, 0, sizeof(SBufferInfo));
    bufInfo.uiInBsTimeStamp = ((setmsec == 0)? chunkmsec : setmsec) / BSTIME_RATE;
    if(setmsec == 0) // 원하는 시각이 없을 경우, 원본과 동일하게 디코딩
    {
        DECODING_STATE rv = mDecoder->DecodeFrame2(src, sliceSize, bufdata, &bufInfo);
        BOSS_ASSERT("DecodeFrame2가 실패하였습니다", rv == dsErrorFree);
    }
    else
    {
        DECODING_STATE rv = mDecoder->DecodeFrameNoDelay(src, sliceSize, bufdata, &bufInfo);
        BOSS_ASSERT("DecodeFrameNoDelay가 실패하였습니다", rv == dsErrorFree);
    }

    if(cb && bufInfo.iBufferStatus == 1)
    {
        const Frame frame =
        {
            {
                // y plane
                bufdata[0],
                bufInfo.UsrData.sSystemBuffer.iWidth,
                bufInfo.UsrData.sSystemBuffer.iHeight,
                bufInfo.UsrData.sSystemBuffer.iStride[0]
            },
            {
                // u plane
                bufdata[1],
                bufInfo.UsrData.sSystemBuffer.iWidth / 2,
                bufInfo.UsrData.sSystemBuffer.iHeight / 2,
                bufInfo.UsrData.sSystemBuffer.iStride[1]
            },
            {
                // v plane
                bufdata[2],
                bufInfo.UsrData.sSystemBuffer.iWidth / 2,
                bufInfo.UsrData.sSystemBuffer.iHeight / 2,
                bufInfo.UsrData.sSystemBuffer.iStride[1]
            }
        };
        cb(data, frame);
    }
    return bufInfo.uiOutYuvTimeStamp * BSTIME_RATE;
}

bytes H264DecoderPrivate::GetBsBuf(bool nalu, bytes chunk, sint32 chunksize, sint32& bssize, CollectorType*& collector)
{
    sint32 BsSize = 0;
    bytes BsBuf = nullptr;
    bytes ChunkFocus = &chunk[5];
    bytes ChunkEnd = &chunk[chunksize];
    collector = nullptr;
    if(nalu)
    {
        BsSize  = (*(ChunkFocus++) & 0xFF) << 24;
        BsSize |= (*(ChunkFocus++) & 0xFF) << 16;
        BsSize |= (*(ChunkFocus++) & 0xFF) <<  8;
        BsSize |= (*(ChunkFocus++) & 0xFF) <<  0;
        BsBuf = ChunkFocus;
    }
    else
    {
        while((*(ChunkFocus++) & 0xFC) != 0xFC) // 1111:1100
            if(ChunkEnd < ChunkFocus)
                return nullptr;
        collector = new CollectorType();
        for(sint32 i = 0; i < 2; ++i) // SPS와 PPS
        {
            sint32 Count = *(ChunkFocus++);
            for(sint32 j = 0; j < Count; ++j)
            {
                uint32 PsSize = 0;
                PsSize  = (*(ChunkFocus++) & 0xFF) << 8;
                PsSize |= (*(ChunkFocus++) & 0xFF) << 0;
                Memory::Copy(collector->AtDumpingAdded(4), GetBE4(1), 4);
                Memory::Copy(collector->AtDumpingAdded(PsSize), ChunkFocus, PsSize);
                if(ChunkEnd < (ChunkFocus += PsSize))
                {
                    delete collector;
                    return nullptr;
                }
            }
        }
        BsSize = collector->Count();
        BsBuf = &(*collector)[0];
    }
    bssize = BsSize;
    return BsBuf;
}

#endif
