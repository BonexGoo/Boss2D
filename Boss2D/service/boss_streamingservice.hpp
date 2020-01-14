#pragma once
#include <boss.hpp>

namespace BOSS
{
    /// @brief 서비스-Streaming
    class StreamingService
    {
    BOSS_DECLARE_NONCOPYABLE_CLASS(StreamingService)

    public:
        class Frame
        {
        public:
            uint08s mBitmap;
            Array<uint08s> mPcms;
        public:
            Frame() {}
            Frame(const Frame& rhs) {operator=(rhs);}
            ~Frame() {}
            Frame& operator=(const Frame& rhs) {mBitmap = rhs.mBitmap; mPcms = rhs.mPcms; return *this;}
            operator void*() const {return nullptr;}
            bool operator!() const {return (mBitmap.Count() == 0);}
        };

    public:
        class Codec
        {
        public:
            struct EncodingReport
            {
                sint32 mWaitingFrameCount; // 인코딩 대기중인 프레임원본 수량
                sint32 mUnsentFlvBitsCount; // 전송 대기중인 FlvBits 수량
                sint32 mSendedChunkCount; // 보내진 청크의 수량
                sint32 mDroppedChunkCount; // 버려진 청크의 수량
                sint32 mSleepTimeMsec; // 전송원활 상황에 따른 슬립조절
                sint32 mAacEncodingTimeMsec; // Aac인코딩시간
                sint32 mH264EncodingTimeMsec; // H264인코딩시간
                sint32 mReportTimeMsec; // 전체작업시간
            };

        public:
            Codec();
            ~Codec();

        public:
            void Bind(const StreamingService* service);
            void Unbind(const StreamingService* service);
            void AddFrame(id_bitmap_read bitmap, uint64 bitmaptimems, Queue<uint08s>* pcms);
            EncodingReport* GetReportOnce() const;

        private:
            static sint32 OnCodecTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common);

        private:
            id_tasking mTasking;
            bool mNeedCreate;
            Queue<Frame>* mFrameQueueRef;
        };

    public:
        StreamingService();
        ~StreamingService();
        StreamingService(StreamingService&& rhs);
        StreamingService& operator=(StreamingService&& rhs);

    public:
        void InitForBroadcast(chars servicename, chars streamkey);
        void InitForRecord();
        sint32 GetBroadcastState(uint64& beginTimeMsec, sint64& sendedBytes) const;

    public:
        inline chars service_name() const {return mServiceName;}

    private:
        static sint32 OnBroadcastTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common);
        static sint32 OnRecordTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common);

    private:
        String mServiceName;
        id_tasking mTasking;
    };
}
