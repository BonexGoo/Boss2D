#include <boss.hpp>
#include "boss_streamingservice.hpp"

#include <boss.hpp>
#include <format/boss_bmp.hpp>
#include <format/boss_flv.hpp>
#include <platform/boss_platform.hpp>

class CodecSender
{
public:
    CodecSender()
    {
        mFlash = nullptr;
        mFirstTimeMs = 0;
        mImageCodec = nullptr;
        mSoundCodec = nullptr;
    }
    ~CodecSender()
    {
        Reset();
    }

public:
    void Reset()
    {
        if(mFlash)
        {
            Flv::Remove(mFlash);
            mFlash = nullptr;
        }
        mFirstTimeMs = 0;
        if(mImageCodec)
        {
            AddOn::H264::Release(mImageCodec);
            mImageCodec = nullptr;
        }
        if(mSoundCodec)
        {
            AddOn::Aac::Release(mSoundCodec);
            mSoundCodec = nullptr;
        }
        while(0 < mFrameQueue.Count())
            mFrameQueue.Dequeue();
    }

public:
    id_flash mFlash;
    uint64 mFirstTimeMs;
    id_h264 mImageCodec;
    id_acc mSoundCodec;
    Queue<StreamingService::Frame> mFrameQueue;
    Array<Queue<uint08s>*, datatype_pod_canmemcpy> mFlvBitsQueueRefs;
};

class CodecReceiver
{
public:
    CodecReceiver()
    {
        mKilled = false;
        mBeginTimeMsec = 0;
    }
    virtual ~CodecReceiver()
    {
        ResetFlvBitsQueue();
    }

protected:
    void ResetFlvBitsQueue()
    {
        while(0 < mFlvBitsQueue.Count())
            mFlvBitsQueue.Dequeue();
    }

public:
    bool mKilled;
    Queue<uint08s> mFlvBitsQueue;
    uint64 mBeginTimeMsec;
};

class CodecBroadcastReceiver : public CodecReceiver
{
public:
    CodecBroadcastReceiver()
    {
        mSendedBytes = 0;
        mEpisodeNumber = 0;
    }
    ~CodecBroadcastReceiver() override
    {
        mSavedFrame.Clear();
    }

public:
    void Reset()
    {
        ResetFlvBitsQueue();
        mSavedFrame.Clear();
    }

public:
    String mServiceName;
    String mStreamKey;
    uint08s mSavedFrame;
    sint64 mSendedBytes;
    sint32 mEpisodeNumber;
};

class CodecRecordReceiver : public CodecReceiver
{
public:
    CodecRecordReceiver()
    {
        mSavedBytes = 0;
    }
    ~CodecRecordReceiver() override
    {
    }

public:

public:
    sint64 mSavedBytes;
};

class CurlSender
{
public:
    CurlSender()
    {
        mStream = AddOn::Curl::Create();
    }
    ~CurlSender()
    {
        AddOn::Curl::Release(mStream);
    }

public:
    static boss_size_t OnRead(void* ptr, boss_size_t size, boss_size_t nitems, payload data)
    {
        uint64 BeginTime = Platform::Utility::CurrentTimeMsec();
        while(Platform::Utility::CurrentTimeMsec() - BeginTime < 3000)
        {
            bool IsKilled = false;
            boss_size_t CopiedSize = 0;
            BOSS_COMMON_TASK((id_common) data, common_buffer)
            if(auto CodecR = (CodecBroadcastReceiver*) common_buffer)
            {
                uint08s CurFrame = CodecR->mSavedFrame;
                CodecR->mSavedFrame.Clear();

                const boss_size_t MaxSize = size * nitems;
                while(CopiedSize < MaxSize && !IsKilled)
                {
                    if(0 < CurFrame.Count())
                    {
                        if(CopiedSize + CurFrame.Count() <= MaxSize)
                        {
                            Memory::Copy(((uint08*) ptr) + CopiedSize, &CurFrame[0], CurFrame.Count());
                            CopiedSize += CurFrame.Count();
                            CodecR->mSendedBytes += CurFrame.Count();
                        }
                        else
                        {
                            CodecR->mSavedFrame = CurFrame;
                            CurFrame.Clear();
                            break;
                        }
                    }
                    if(0 < CodecR->mFlvBitsQueue.Count())
                        CurFrame = CodecR->mFlvBitsQueue.Dequeue();
                    else break;
                    IsKilled = CodecR->mKilled;
                }
            }
            // 복사된 내용이 하나라도 있으면 송신
            if(0 < CopiedSize || IsKilled) return CopiedSize;
            // 복사된 내용이 하나도 없으면 스트리밍이 종료되기 때문에 3초까지 50ms단위로 기다림
            Platform::Utility::Sleep(50, false, false);
        }
        return 0;
    }

public:
    id_curl mStream;
};

class VideoSaver
{
public:
    VideoSaver()
    {
        mFlvFile = nullptr;
        mLogFile = nullptr;
    }
    ~VideoSaver()
    {
        Asset::Close(mFlvFile);
        Asset::Close(mLogFile);
    }

public:
    void Append(CodecRecordReceiver* receiver)
    {
        if(!mFlvFile) mFlvFile = Asset::OpenForWrite("videoes/temp.flv", true);
        if(!mLogFile) mLogFile = Asset::OpenForWrite("videoes/temp.log", true);

        while(0 < receiver->mFlvBitsQueue.Count())
        {
            uint08s CurFlvBits = receiver->mFlvBitsQueue.Dequeue();
            if(0 < CurFlvBits.Count())
            {
                // Flv
                Asset::Write(mFlvFile, &CurFlvBits[0], CurFlvBits.Count());
                receiver->mSavedBytes += CurFlvBits.Count();
                // Log
                String NewLog = Flv::BuildLog(&CurFlvBits[0], CurFlvBits.Count());
                Asset::Write(mLogFile, (bytes)(chars) NewLog, NewLog.Length());
            }
        }
    }

public:
    id_asset mFlvFile;
    id_asset mLogFile;
};

namespace BOSS
{
    ////////////////////////////////////////////////////////////////////////////////
    // Codec
    ////////////////////////////////////////////////////////////////////////////////
    StreamingService::Codec::Codec()
    {
        mTasking = Tasking::Create(OnCodecTask);
        mNeedCreate = true;
        mFrameQueueRef = nullptr;
        BOSS_COMMON(mTasking, common_buffer)
        {
            Buffer::Free(common_buffer);
            common_buffer = Buffer::Alloc<CodecSender>(BOSS_DBG 1);
            if(auto CodecS = (CodecSender*) common_buffer)
                mFrameQueueRef = &CodecS->mFrameQueue;
        }
    }

    StreamingService::Codec::~Codec()
    {
        Tasking::Release(mTasking, true);
    }

    void StreamingService::Codec::Bind(const StreamingService* service)
    {
        Queue<uint08s>* CurFlvBitsQueue = nullptr;
        BOSS_COMMON(service->mTasking, receiver_common_buffer)
            if(auto CodecR = (CodecReceiver*) receiver_common_buffer)
                CurFlvBitsQueue = &CodecR->mFlvBitsQueue;

        BOSS_COMMON(mTasking, common_buffer)
        if(auto CodecS = (CodecSender*) common_buffer)
        {
            CodecS->Reset();
            CodecS->mFlvBitsQueueRefs.AtAdding() = CurFlvBitsQueue;
            mNeedCreate = true;
        }
    }

    void StreamingService::Codec::Unbind(const StreamingService* service)
    {
        Queue<uint08s>* CurFlvBitsQueue = nullptr;
        BOSS_COMMON(service->mTasking, receiver_common_buffer)
            if(auto CodecR = (CodecReceiver*) receiver_common_buffer)
                CurFlvBitsQueue = &CodecR->mFlvBitsQueue;

        BOSS_COMMON(mTasking, common_buffer)
        if(auto CodecS = (CodecSender*) common_buffer)
        {
            for(sint32 i = 0, iend = CodecS->mFlvBitsQueueRefs.Count(); i < iend; ++i)
            {
                if(CodecS->mFlvBitsQueueRefs[i] == CurFlvBitsQueue)
                {
                    for(sint32 j = i; j < iend - 1; ++j)
                        CodecS->mFlvBitsQueueRefs.At(j) = CodecS->mFlvBitsQueueRefs[j + 1];
                    CodecS->mFlvBitsQueueRefs.SubtractionOne();
                    break;
                }
            }
        }
    }

    #define DS_BITRATE 128000
    #define DS_CHANNEL 2
    #define DS_SAMPLERATE 44100
    void StreamingService::Codec::AddFrame(id_bitmap_read bitmap, uint64 bitmaptimems, Queue<uint08s>* pcms)
    {
        if(mNeedCreate)
        {
            BOSS_COMMON(mTasking, common_buffer)
            if(auto CodecS = (CodecSender*) common_buffer)
            {
                mNeedCreate = false;
                // 플래시비디오 초기화
                if(!CodecS->mFlash)
                {
                    CodecS->mFlash = Flv::Create(Bmp::GetWidth(bitmap), Bmp::GetHeight(bitmap));
                    CodecS->mFirstTimeMs = bitmaptimems;
                }
                // 이미지코덱 초기화
                if(!CodecS->mImageCodec)
                    CodecS->mImageCodec = AddOn::H264::CreateEncoder(Bmp::GetWidth(bitmap), Bmp::GetHeight(bitmap), true);//false);
                // 사운드코덱 초기화
                if(!CodecS->mSoundCodec)
                    CodecS->mSoundCodec = AddOn::Aac::Create(DS_BITRATE, DS_CHANNEL, DS_SAMPLERATE);
            }
        }

        Frame NewFrame;
        {
            // 비트맵을 현재시간과 함께 큐저장
            uint08s NewBitmap;
            const sint32 NewBitmapSize = Bmp::GetFileSizeWithoutBM(bitmap);
            Memory::Copy(NewBitmap.AtDumping(0, sizeof(uint64) + NewBitmapSize), &bitmaptimems, sizeof(uint64));
            Memory::Copy(NewBitmap.AtDumping(sizeof(uint64), NewBitmapSize), bitmap, NewBitmapSize);
            NewFrame.mBitmap = NewBitmap;

            // 한 프레임동안 발생한 모든 사운드를 큐저장
            if(pcms)
            while(0 < pcms->Count())
                NewFrame.mPcms.AtAdding() = pcms->Dequeue();
        }
        mFrameQueueRef->Enqueue(NewFrame);
    }

    StreamingService::Codec::EncodingReport* StreamingService::Codec::GetReportOnce() const
    {
        return (EncodingReport*) Tasking::GetAnswer(mTasking);
    }

    sint32 StreamingService::Codec::OnCodecTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
    {
        static sint32 SleepTimeMsec = 20;
        BOSS_COMMON_TASK(common, common_buffer)
        if(auto CodecS = (CodecSender*) common_buffer)
        if(0 < CodecS->mFrameQueue.Count())
        {
            auto NewReport = (EncodingReport*) Buffer::Alloc<EncodingReport, datatype_pod_canmemcpy_zeroset>(BOSS_DBG 1);
            const uint64 ReportBegin = Platform::Utility::CurrentTimeMsec();

            NewReport->mWaitingFrameCount = CodecS->mFrameQueue.Count();
            // 프레임의 보관상한선
            while(128 < CodecS->mFrameQueue.Count())
                CodecS->mFrameQueue.Dequeue();

            if(0 < CodecS->mFlvBitsQueueRefs.Count()) // Flv수신처가 있다면
            {
                // 모든 채널들에 송신하지 못하고 쌓인 데이터량 수집
                sint32 SumFlvBitsQueueCount = 0;
                for(sint32 i = 0, iend = CodecS->mFlvBitsQueueRefs.Count(); i < iend; ++i)
                    SumFlvBitsQueueCount += CodecS->mFlvBitsQueueRefs[i]->Count();
                NewReport->mUnsentFlvBitsCount = SumFlvBitsQueueCount;

                // 그에 따른 슬립조절
                const sint32 AvgFlvBitsQueueCount = SumFlvBitsQueueCount / CodecS->mFlvBitsQueueRefs.Count();
                if(AvgFlvBitsQueueCount < 10) SleepTimeMsec = Math::Max(20, SleepTimeMsec - 1);
                else SleepTimeMsec = Math::Min(SleepTimeMsec + 1, 1000);
                NewReport->mSleepTimeMsec = SleepTimeMsec;

                const sint64 MinCheck = (sint64) (Platform::Utility::CurrentTimeMsec() - 100);
                sint32 FrameCount = CodecS->mFrameQueue.Count();
                while(FrameCount--)
                {
                    Frame CurFrameQueue = CodecS->mFrameQueue.Dequeue();
                    const auto& CurBitmapWithTime = CurFrameQueue.mBitmap;
                    uint64 CurFrameTimeMsec = *((uint64*) &CurBitmapWithTime[0]);
                    const uint64 FlashTimeMsec = CurFrameTimeMsec - CodecS->mFirstTimeMs;
                    const sint32 SpanValue = Math::Min((sint32) (MinCheck - (sint64) CurFrameTimeMsec), 1000);
                    const sint32 RandValue = ((sint32) Platform::Utility::Random()) % 1000;
                    // 100ms이전의 청크는 확율적으로 버림
                    if(RandValue < SpanValue)
                        NewReport->mDroppedChunkCount++;
                    else
                    {
                        NewReport->mSendedChunkCount++;
                        // H264인코딩
                        const uint64 H264EncodingBegin = Platform::Utility::CurrentTimeMsec();
                        if(CodecS->mImageCodec)
                        {
                            bytes CurBitmap = &CurBitmapWithTime[sizeof(uint64)];
                            const uint32* CurBitmapBits = (const uint32*) Bmp::GetBits((id_bitmap_read) CurBitmap);
                            AddOn::H264::EncodeOnce(CodecS->mImageCodec, CurBitmapBits, CodecS->mFlash, FlashTimeMsec);
                        }
                        NewReport->mH264EncodingTimeMsec += (sint32) (Platform::Utility::CurrentTimeMsec() - H264EncodingBegin);

                        // AAC인코딩
                        const uint64 AacEncodingBegin = Platform::Utility::CurrentTimeMsec();
                        if(CodecS->mSoundCodec)
                        {
                            const auto& CurPcmsWithTime = CurFrameQueue.mPcms;
                            if(CurPcmsWithTime.Count() == 0) // 사운드데이터가 없을 경우
                                AddOn::Aac::SilenceTo(CodecS->mSoundCodec, CodecS->mFlash, FlashTimeMsec);
                            else for(sint32 i = 0, iend = CurPcmsWithTime.Count(); i < iend; ++i)
                            {
                                bytes CurPcm = &CurPcmsWithTime[i][sizeof(uint64)];
                                const sint32 CurPcmLength = CurPcmsWithTime[i].Count() - sizeof(uint64);
                                AddOn::Aac::EncodeTo(CodecS->mSoundCodec, CurPcm, CurPcmLength, CodecS->mFlash, FlashTimeMsec);
                            }
                        }
                        NewReport->mAacEncodingTimeMsec += (sint32) (Platform::Utility::CurrentTimeMsec() - AacEncodingBegin);

                        // 각 스트림에 배포
                        sint32 VideoLength = 0;
                        bytes Video = Flv::GetBits(CodecS->mFlash, &VideoLength);
                        if(0 < VideoLength)
                        {
                            uint08s ClonedBits;
                            Memory::Copy(ClonedBits.AtDumpingAdded(VideoLength), Video, VideoLength);
                            for(sint32 i = 0, iend = CodecS->mFlvBitsQueueRefs.Count(); i < iend; ++i)
                                CodecS->mFlvBitsQueueRefs[i]->Enqueue(ClonedBits);
                        }
                        Flv::Empty(CodecS->mFlash);
                    }
                }
            }

            NewReport->mReportTimeMsec = (sint32) (Platform::Utility::CurrentTimeMsec() - ReportBegin);
            answer.Enqueue((buffer) NewReport);
            // answer의 보관상한선
            while(128 < answer.Count())
                Buffer::Free(answer.Dequeue());
        }
        return SleepTimeMsec;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // StreamingService
    ////////////////////////////////////////////////////////////////////////////////
    StreamingService::StreamingService()
    {
        mTasking = nullptr;
    }

    StreamingService::~StreamingService()
    {
        if(mTasking)
        BOSS_COMMON(mTasking, common_buffer)
        if(auto CodecR = (CodecReceiver*) common_buffer)
            CodecR->mKilled = true;
        Tasking::Release(mTasking, true);
    }

    StreamingService::StreamingService(StreamingService&& rhs)
    {
        operator=(rhs);
    }

    StreamingService& StreamingService::operator=(StreamingService&& rhs)
    {
        mServiceName = ToReference(rhs.mServiceName);
        mTasking = rhs.mTasking;
        rhs.mTasking = nullptr;
        return *this;
    }

    void StreamingService::InitForBroadcast(chars servicename, chars streamkey)
    {
        BOSS_ASSERT("본 객체는 여러번 초기화될 수 없습니다", !mTasking);
        mServiceName = servicename;
        mTasking = Tasking::Create(OnBroadcastTask);
        BOSS_COMMON(mTasking, common_buffer)
        {
            common_buffer = Buffer::Alloc<CodecBroadcastReceiver>(BOSS_DBG 1);
            auto NewCodec = (CodecBroadcastReceiver*) common_buffer;
            NewCodec->mBeginTimeMsec = Platform::Utility::CurrentTimeMsec();
            NewCodec->mServiceName = servicename;
            NewCodec->mStreamKey = streamkey;
        }
    }

    void StreamingService::InitForRecord()
    {
        BOSS_ASSERT("본 객체는 여러번 초기화될 수 없습니다", !mTasking);
        mTasking = Tasking::Create(OnRecordTask);
        BOSS_COMMON(mTasking, common_buffer)
        {
            common_buffer = Buffer::Alloc<CodecRecordReceiver>(BOSS_DBG 1);
            auto NewCodec = (CodecRecordReceiver*) common_buffer;
            NewCodec->mBeginTimeMsec = Platform::Utility::CurrentTimeMsec();
        }
    }

    sint32 StreamingService::GetBroadcastState(uint64& beginTimeMsec, sint64& sendedBytes) const
    {
        sint32 Result = 0;
        if(mTasking)
        BOSS_COMMON(mTasking, common_buffer)
        if(Buffer::TypeCheck<CodecBroadcastReceiver, datatype_class_nomemcpy>(common_buffer))
        if(auto CodecR = (CodecBroadcastReceiver*) common_buffer)
        {
            beginTimeMsec = CodecR->mBeginTimeMsec;
            sendedBytes = CodecR->mSendedBytes;
            Result = CodecR->mEpisodeNumber;
        }
        return Result;
    }

    sint32 StreamingService::OnBroadcastTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
    {
        if(!self) self = Buffer::Alloc<CurlSender>(BOSS_DBG 1);
        auto CurSender = (CurlSender*) self;

        bool ValidService = false;
        String ServiceName, StreamKey;
        BOSS_COMMON_TASK(common, common_buffer)
        if(auto CodecR = (CodecBroadcastReceiver*) common_buffer)
        {
            ValidService = (!CodecR->mKilled && 0 < CodecR->mFlvBitsQueue.Count());
            if(ValidService)
            {
                ServiceName = CodecR->mServiceName;
                StreamKey = CodecR->mStreamKey;
                CodecR->mBeginTimeMsec = Platform::Utility::CurrentTimeMsec();
                CodecR->mSendedBytes = 0;
                CodecR->mEpisodeNumber++;
            }
        }

        if(ValidService)
        {
            String Url = "";
            if(!String::Compare(ServiceName, "YouTube"))
                Url = String::Format("rtmp://a.rtmp.youtube.com/live2/%s", (chars) StreamKey);
            else if(!String::Compare(ServiceName, "Facebook"))
                Url = String::Format("rtmp://rtmp-api.facebook.com:80/rtmp/%s", (chars) StreamKey);
            else BOSS_ASSERT("준비되지 않은 서비스입니다", false);

            // 여기서 무한루프
            AddOn::Curl::SendStream(CurSender->mStream, Url, CurlSender::OnRead, common);

            // SendStream루프를 빠져나왔다는 것은 접속불량 상황
            BOSS_COMMON_TASK(common, common_buffer)
            if(auto CodecR = (CodecBroadcastReceiver*) common_buffer)
                CodecR->Reset();
        }
        return 500;
    }

    sint32 StreamingService::OnRecordTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
    {
        if(!self) self = Buffer::Alloc<VideoSaver>(BOSS_DBG 1);
        auto CurSaver = (VideoSaver*) self;

        BOSS_COMMON_TASK(common, common_buffer)
        if(auto CodecR = (CodecRecordReceiver*) common_buffer)
        if(!CodecR->mKilled && 0 < CodecR->mFlvBitsQueue.Count())
            CurSaver->Append(CodecR);
        return 500;
    }
}
