#pragma once
#include <element/boss_image.hpp>

namespace BOSS
{
    //! \brief 서비스-LiveChat
    class LiveChatService
    {
    public:
        class Message
        {
            BOSS_DECLARE_NONCOPYABLE_CLASS(Message)
        public:
            Message() {mUserId = -1;}
            ~Message() {}
        public:
            inline static Message* Create() {return (Message*) Buffer::Alloc<Message>(BOSS_DBG 1);}
            inline void DestroyMe() {Buffer::Free((buffer) this);}
        public:
            sint32 mUserId;
            String mText;
        };

    public:
        LiveChatService();
        LiveChatService(const LiveChatService& rhs);
        virtual ~LiveChatService();
        LiveChatService& operator=(const LiveChatService& rhs);

    // 정적함수
    public:
        static const Strings GetValidServices();

    // 서비스함수
    public:
        bool AddService(chars service, chars id, id_bitmap clipper = nullptr);
        bool SubService(chars service, chars id);
        Message* TryNextMessage();
        const String& GetName(const Message* message);
        const Image& GetPicture(const Message* message);

    // 데이터(공유모델)
    protected:
        const Share* mShare;
    };
    typedef Array<LiveChatService::Message*, datatype_pod_canmemcpy_zeroset> LiveChatMessagePtrs;
}
