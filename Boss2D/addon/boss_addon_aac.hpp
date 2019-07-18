#pragma once
#include <boss.h>

#include "boss_integration_fdk-aac-0.1.4.h"
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/include/aacenc_lib.h>

#include <boss.hpp>

class BaseEncoderAac
{
public:
    BaseEncoderAac(sint32 bitrate, sint32 channel, sint32 samplerate);
    ~BaseEncoderAac();
    void EncodeTo(bytes pcm, sint32 length, id_flash flash, uint64 timems);
    void SilenceTo(id_flash flash, uint64 timems);

public:
    inline bool IsValid() const
    {return (mEncoder != nullptr);}

private:
    HANDLE_AACENCODER* mEncoder;
    AACENC_InfoStruct mInfo;
    sint32 mChunkSize;
};
