#pragma once
#include <service/boss_zay.hpp>

namespace BOSS
{
    //! \brief 웹게시물형 리소스파서
    class ContactPool;
    class ParaSource
    {
    public:
        enum Type {IIS, NaverCafe};

    public:
        ParaSource(Type type);
        ParaSource(chars domain);
        ~ParaSource();

    public:
        inline chars GetLastError() const {return mLastError;}

    public:
        void SetContact(chars domain, uint16 port, sint32 timeout = 5000);
        bool GetFile(uint08s& file, chars path, chars args = nullptr);
        bool GetJson(Context& json, chars path, chars args = nullptr);
        bool GetJsons(Contexts& jsons, chars path, chars args = nullptr);
        bool GetLastSpecialJson(Context& json);

    private:
        typedef bool (*JsonLoaderCB)(payload data, chars content, sint32 length);
        bool LoadJsons(JsonLoaderCB cb, payload data, chars path, chars args);
        bool HTTPQuery(chars path, chars args);
        bool SetLastError(chars message);

    private:
        Type mType;
        chararray mResponseData;
        ContactPool* mLastContact;
        String mLastError;
    };

    class ParaAsset
    {
    public:
        ParaAsset();
        virtual ~ParaAsset();

    public:
        void Init(chars script);

    protected:
        virtual void InitForCache(chars assetname) = 0;

    protected:
        id_tasking mTasking;
    };

    class ParaJson : public ParaAsset
    {
    public:
        ParaJson();
        ~ParaJson() override;

    public:
        virtual Context* GetContext();

    private:
        void InitForCache(chars assetname) override;

    protected:
        Context* mContext;
    };

    class ParaView : public ParaAsset
    {
    public:
        ParaView();
        ~ParaView() override;

    public:
        virtual ZayPanel::SubRenderCB GetRenderer();

    private:
        void InitForCache(chars assetname) override;

    protected:
        Image mImage;
        uint64 mLoadingMsec;
    };
}
