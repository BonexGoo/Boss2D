#include <boss.hpp>
#include "boss_livechatservice.hpp"

#include <boss.hpp>
#include <format/boss_bmp.hpp>

namespace BOSS
{
    ////////////////////////////////////////////////////////////////////////////////
    // LiveChatCollector
    ////////////////////////////////////////////////////////////////////////////////
    class LiveChatCollector
    {
        BOSS_DECLARE_NONCOPYABLE_CLASS(LiveChatCollector)

    public:
        LiveChatCollector()
        {
            mMessageCb = nullptr;
            mMessageData = nullptr;
            mTasking = nullptr;
        }
        ~LiveChatCollector()
        {
            Tasking::Release(mTasking, true);
        }

    public: // 콜백인터페이스
        typedef void (*OnMessage)(payload data, chars text, chars author, chars photourl);

    private: // 전용객체
        class Manager
        {
            BOSS_DECLARE_NONCOPYABLE_CLASS(Manager)
        protected:
            Manager() {mCurl = AddOn::Curl::Create();}
            ~Manager() {AddOn::Curl::Release(mCurl);}
        public:
            virtual void DestroyMe() = 0;
        protected:
            id_curl mCurl;
        };
        class Payload
        {
            BOSS_DECLARE_NONCOPYABLE_CLASS(Payload)
        public:
            Payload(const LiveChatCollector* self = nullptr) : mSelf(self) {mManager = nullptr;}
            ~Payload() {mManager->DestroyMe();}
        public:
            const LiveChatCollector* mSelf;
            Manager* mManager;
        };

    public:
        bool Init(chars service, chars id, OnMessage cb, payload data)
        {
            BOSS_ASSERT("초기화는 한번만 할 수 있습니다", !mTasking);
            mService = service;
            mId = id;
            mMessageCb = cb;
            mMessageData = data;

            buffer NewPayload = Buffer::AllocNoConstructorOnce<Payload>(BOSS_DBG 1);
            BOSS_CONSTRUCTOR(NewPayload, 0, Payload, this);

            if(!mService.Compare("Google+"))
                mTasking = Tasking::Create(OnGoogleTask, NewPayload);
            else if(!mService.Compare("Facebook"))
                mTasking = Tasking::Create(OnFacebookTask, NewPayload);
            else
            {
                Buffer::Free(NewPayload);
                return false;
            }
            return true;
        }
        bool IsSame(chars service, chars id) const
        {
            return (!mId.Compare(id) && !mService.Compare(service));
        }

    protected:
        static sint32 OnGoogleTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
        {
            class GoogleManager : public Manager
            {
                BOSS_DECLARE_NONCOPYABLE_CLASS(GoogleManager)
            public:
                GoogleManager() {}
                ~GoogleManager() {}
            public:
                static Manager* Create() {return (Manager*) Buffer::Alloc<GoogleManager>(BOSS_DBG 1);}
                void DestroyMe() override {Buffer::Free((buffer) this);}

            public:
                void ParseVideoId(chars channelid)
                {
                    chars Result = AddOn::Curl::GetString(mCurl,
                        String::Format("https://" "www.youtube.com/embed/live_stream?channel=%s", channelid));
                    // 파싱과정
                    static const String FindKey = "\'VIDEO_ID\':";
                    sint32 FindIndex = String(Result).Find(0, FindKey);
                    if(FindIndex != -1)
                    {
                        FindIndex += FindKey.Length();
                        //'VIDEO_ID': "ABCDE"
                        while(Result[FindIndex++] != '\"');
                        const sint32 VideoIdPos = FindIndex;
                        while(Result[++FindIndex] != '\"');
                        const sint32 VideoIdLength = FindIndex - VideoIdPos;
                        mVideoId = String(&Result[VideoIdPos], VideoIdLength);
                    }
                }
                void ParseContent(OnMessage cb, payload data)
                {
                    chars Result = AddOn::Curl::GetString(mCurl,
                        String::Format("https://" "www.youtube.com/live_chat?v=%s", (chars) mVideoId));
                    // 파싱과정
                    static const String FindKey = "\"actions\":";
                    sint32 FindIndex = String(Result).Find(0, FindKey);
                    if(FindIndex != -1)
                    {
                        FindIndex += FindKey.Length();
                        const Context Actions(ST_Json, SO_OnlyReference, &Result[FindIndex]);
                        for(sint32 i = 0, iend = Actions.LengthOfIndexable(); i < iend; ++i)
                        {
                            const Context MessageRenderer = Actions[i]("addChatItemAction")("item")("liveChatTextMessageRenderer");
                            String Id = MessageRenderer("id").GetString();
                            if(!mTimestamps.Access(Id))
                            {
                                mTimestamps(Id) = MessageRenderer("timestampUsec").GetFloat();
                                cb(data,
                                    MessageRenderer("message")("runs")[0]("text").GetString(),
                                    MessageRenderer("authorName")("simpleText").GetString(),
                                    MessageRenderer("authorPhoto")("thumbnails")[0]("url").GetString());
                            }
                        }
                    }
                }

            public:
                String mVideoId;
                Map<float> mTimestamps;
            };

            Payload* CurPayload = (Payload*) self;
            // 매니저할당
            if(!CurPayload->mManager)
            {
                CurPayload->mManager = GoogleManager::Create();
                ((GoogleManager*) CurPayload->mManager)->ParseVideoId(CurPayload->mSelf->mId);
            }
            // 비디오ID로 채팅수집
            ((GoogleManager*) CurPayload->mManager)->ParseContent(
                CurPayload->mSelf->mMessageCb, CurPayload->mSelf->mMessageData);
            return 500;
        }
        static sint32 OnFacebookTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
        {
            return 500;
        }

    protected:
        String mService;
        String mId;
        OnMessage mMessageCb;
        payload mMessageData;
        id_tasking mTasking;
    };
    typedef Object<LiveChatCollector> CollectorObject;
    typedef Array<CollectorObject> CollectorObjects;

    ////////////////////////////////////////////////////////////////////////////////
    // LiveChatData
    ////////////////////////////////////////////////////////////////////////////////
    class LiveChatData
    {
        BOSS_DECLARE_NONCOPYABLE_CLASS(LiveChatData)

    public:
        LiveChatData()
        {
            mMessageMutex = Mutex::Open();
            mUserLastIndex = -1;
            mPictureClipper = nullptr;
        }
        ~LiveChatData()
        {
            mCollectors.Clear(); // 차후에 OnMessage가 호출되지 않도록 선행하여 제거
            Mutex::Close(mMessageMutex);
            while(LiveChatService::Message* NewMessage = mMessageQueue.Dequeue())
                NewMessage->DestroyMe();
            Bmp::Remove(mPictureClipper);
        }

    public:
        class UserInfo
        {
            BOSS_DECLARE_NONCOPYABLE_CLASS(UserInfo)
        public:
            UserInfo() {}
            ~UserInfo() {}
        public:
            String mName;
            Image mPicture;
        };

    public:
        static void OnMessage(payload data, chars text, chars author, chars photourl)
        {
            auto Self = (LiveChatData*) data;
            Mutex::Lock(Self->mMessageMutex);
            {
                sint32 CurUserId = -1;
                if(sint32* CurIndex = Self->mUserIndexMap.Access(photourl))
                    CurUserId = *CurIndex;
                else if(id_curl NewCurl = AddOn::Curl::Create())
                {
                    CurUserId = ++Self->mUserLastIndex;
                    Self->mUserIndexMap(photourl) = CurUserId;
                    auto& NewUserInfo = Self->mUserInfoMap[CurUserId];
                    NewUserInfo.mName = author;

                    sint32 JpgDataSize = 0;
                    bytes JpgDataPtr = AddOn::Curl::GetBytes(NewCurl, photourl, &JpgDataSize);
                    if(id_bitmap NewBitmap = AddOn::Jpg::ToBmp(JpgDataPtr, JpgDataSize))
                    {
                        NewUserInfo.mPicture.LoadBitmap(NewBitmap);
                        if(Self->mPictureClipper)
                            NewUserInfo.mPicture.ReplaceAlphaChannelBy(Self->mPictureClipper);
                        Bmp::Remove(NewBitmap);
                    }
                    AddOn::Curl::Release(NewCurl);
                }

                auto NewMessage = LiveChatService::Message::Create();
                NewMessage->mText = text;
                NewMessage->mUserId = CurUserId;
                Self->mMessageQueue.Enqueue(NewMessage);
            }
            Mutex::Unlock(Self->mMessageMutex);
        }

    public:
        id_mutex mMessageMutex;
        CollectorObjects mCollectors;
        Queue<LiveChatService::Message*> mMessageQueue;
        sint32 mUserLastIndex;
        Map<sint32> mUserIndexMap;
        Map<UserInfo> mUserInfoMap;
        id_bitmap mPictureClipper;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // LiveChatService
    ////////////////////////////////////////////////////////////////////////////////
    #define mDATA (mShare->At<LiveChatData>(0))

    LiveChatService::LiveChatService() : mShare(Share::Create(Buffer::Alloc<LiveChatData>(BOSS_DBG 1)))
    {
    }

    LiveChatService::LiveChatService(const LiveChatService& rhs) : mShare(nullptr)
    {
        operator=(rhs);
    }

    LiveChatService::~LiveChatService()
    {
        Share::Remove(mShare);
    }

    LiveChatService& LiveChatService::operator=(const LiveChatService& rhs)
    {
        Share::Remove(mShare);
        if(rhs.mShare) mShare = rhs.mShare->Clone();
        return *this;
    }

    const Strings LiveChatService::GetValidServices()
    {
        static Strings Services = []()
        {
            Strings Collector;
            Collector.AtAdding() = "Google+";
            Collector.AtAdding() = "Facebook";
            return Collector;
        }();
        return Services;
    }

    bool LiveChatService::AddService(chars service, chars id, id_bitmap clipper)
    {
        bool Result = false;
        Mutex::Lock(mDATA.mMessageMutex);
        {
            mDATA.mCollectors.AtAdding() = CollectorObject(ObjectAllocType::Now);
            auto& NewCollector = mDATA.mCollectors.At(-1).Value();
            Result = NewCollector.Init(service, id, LiveChatData::OnMessage, &mDATA);
            Bmp::Remove(mDATA.mPictureClipper);
            mDATA.mPictureClipper = clipper;
        }
        Mutex::Unlock(mDATA.mMessageMutex);
        return Result;
    }

    bool LiveChatService::SubService(chars service, chars id)
    {
        bool Result = false;
        Mutex::Lock(mDATA.mMessageMutex);
        {
            for(sint32 i = 0, iend = mDATA.mCollectors.Count(); i < iend; ++i)
            {
                if(mDATA.mCollectors[i].ConstValue().IsSame(service, id))
                {
                    for(sint32 j = i + 1; j < iend; ++j)
                        mDATA.mCollectors.At(j - 1) = mDATA.mCollectors[j];
                    mDATA.mCollectors.SubtractionOne();
                    Result = true;
                    break;
                }
            }
        }
        Mutex::Unlock(mDATA.mMessageMutex);
        return Result;
    }

    LiveChatService::Message* LiveChatService::TryNextMessage()
    {
        return mDATA.mMessageQueue.Dequeue();
    }

    const String& LiveChatService::GetName(const Message* message)
    {
        BOSS_ASSERT("message가 nullptr입니다", message);
        if(message && message->mUserId != -1)
        if(auto CurUserInfo = mDATA.mUserInfoMap.Access(message->mUserId))
            return CurUserInfo->mName;
        static const String Null;
        return Null;
    }

    const Image& LiveChatService::GetPicture(const Message* message)
    {
        BOSS_ASSERT("message가 nullptr입니다", message);
        if(message && message->mUserId != -1)
        if(auto CurUserInfo = mDATA.mUserInfoMap.Access(message->mUserId))
            return CurUserInfo->mPicture;
        static const Image Null;
        return Null;
    }
}
