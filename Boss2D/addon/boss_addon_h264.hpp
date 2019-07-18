#pragma once
#include <boss.h>

#include "boss_integration_openh264-1.6.0.h"
#include <addon/openh264-1.6.0_for_boss/codec/api/svc/codec_api.h>

#include <boss.hpp>

class H264Private
{
public:
    H264Private();
    virtual ~H264Private();

private:
    virtual sint32 typecode() const = 0;

protected:
    const void* GetBE2(sint32 value);
    const void* GetBE3(sint32 value);
    const void* GetBE4(sint32 value);
    const void* GetBE8_Double(double value);
    void WriteTag(uint08s& dst, uint08 type, sint32 timestamp, const uint08s chunk);
    void WriteScriptDataImpl(uint08s& dst, chars name);
    void WriteScriptDataECMA(uint08s& dst, chars name, sint32 value);
    void WriteScriptDataNumber(uint08s& dst, chars name, double value);
    void WriteScriptDataBoolean(uint08s& dst, chars name, bool value);
    void WriteScriptDataString(uint08s& dst, chars name, chars value);

private:
    uint32 mTempBE2;
    uint32 mTempBE3;
    uint32 mTempBE4;
    uint64 mTempBE8_Double;
};

class H264EncoderPrivate : public H264Private
{
public:
    typedef void (*EncodeFrameCB)(const SFrameBSInfo& frameInfo);

public:
    H264EncoderPrivate(sint32 width, sint32 height, bool fastmode);
    ~H264EncoderPrivate() override;

private:
    sint32 typecode() const override {return 1;}
public:
    static H264EncoderPrivate* Test(id_h264 h264)
    {
        auto Encoder = (H264EncoderPrivate*) h264;
        return (Encoder && Encoder->typecode() == 1)? Encoder : nullptr;
    }

public:
    void Encode(const uint32* rgba, id_flash flash, uint64 timems);

private:
    ISVCEncoder* mEncoder;
    uint08s mFrame;
    SSourcePicture mPic;
    SFrameBSInfo mInfo;
    uint08s mTempChunk;
};

class H264DecoderPrivate : public H264Private
{
public:
    struct Plane
    {
        bytes mData;
        sint32 mWidth;
        sint32 mHeight;
        sint32 mStride;
    };
    struct Frame
    {
        Plane mY;
        Plane mU;
        Plane mV;
    };
    typedef void (*OnDecodeFrame)(payload data, const Frame& frame);

public:
    H264DecoderPrivate();
    ~H264DecoderPrivate() override;

private:
    sint32 typecode() const override {return 2;}
public:
    static H264DecoderPrivate* Test(id_h264 h264)
    {
        auto Decoder = (H264DecoderPrivate*) h264;
        return (Decoder && Decoder->typecode() == 2)? Decoder : nullptr;
    }

public:
    id_bitmap DecodeBitmap(id_flash flash, uint64 settimems, uint64* gettimems);
    id_texture DecodeTexture(id_flash flash, uint64 settimems, uint64* gettimems);
    uint64 DecodeCore(id_flash flash, uint64 settimems, OnDecodeFrame cb, payload data);
    void Seek(id_flash flash, uint64 timems);

private:
    uint64 DecodeFrame(bytes src, sint32 sliceSize, sint32 setmsec, sint32 chunkmsec, OnDecodeFrame cb = nullptr, payload data = nullptr);

    typedef Array<uint08, datatype_pod_canmemcpy, 256> CollectorType;
    bytes GetBsBuf(bool nalu, bytes chunk, sint32 chunksize, sint32& bssize, CollectorType*& collector);

private:
    ISVCDecoder* mDecoder;
    uint08s mTempBits;
};
