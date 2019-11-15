#pragma once
#include <service/boss_zay.hpp>
#include <service/boss_zayson.hpp>

class ZEDocument : public ZaySonDocument
{
    BOSS_DECLARE_NONCOPYABLE_INITIALIZED_CLASS(ZEDocument, ZaySonDocument("d"))
public:
    ZEDocument();
    ~ZEDocument() override;
    ZEDocument(ZEDocument&& rhs);
    ZEDocument& operator=(ZEDocument&& rhs);

public:
    void PostProcess(const String key, const String value) override;
    void ReserverFlush();

private:
    static void RemoveCB(chars itemname, payload data);
    class DownloadReserver
    {
        BOSS_DECLARE_STANDARD_CLASS(DownloadReserver)
    public:
        DownloadReserver();
        ~DownloadReserver();
    public:
        static sint32 OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common);
    public:
        bool mHtml;
        String mPath;
        String mFileName;
        String mContent;
    };
    typedef Array<DownloadReserver> DownloadReservers;

private:
    DownloadReservers mReservers;
    id_tasking mReserverTask;
};

class ZEWidgetDOM
{
public:
    static void Add(chars variable, chars formula);
    static void AddJson(const Context& json);
    static void AddFlush();
    static void ReserverFlush();
    static void Update(chars variable, chars formula);
    static void BindPipe(id_pipe pipe);
    static void UnbindPipe(id_pipe pipe);

private:
    void SendFlush();
    static void SendToPipe(id_pipe pipe, const Solver* solver);

private:
    ZEWidgetDOM() {}
    ~ZEWidgetDOM() {}

private:
    static ZEWidgetDOM& ST()
    {return *BOSS_STORAGE(ZEWidgetDOM);}

private:
    class Pipe
    {
    public:
        Pipe() {mRefPipe = nullptr; mMsec = 0;}
        ~Pipe() {}
    public:
        id_pipe mRefPipe;
        uint64 mMsec;
    };

private:
    ZEDocument mDocument;
    Map<Pipe> mPipeMap;
};

class ZEWidget
{
    BOSS_DECLARE_NONCOPYABLE_CLASS(ZEWidget)
public:
    typedef const Image* (*ResourceCB)(chars name);

public:
    ZaySon& Init(chars viewname, chars assetname = nullptr, ResourceCB cb = nullptr);
    void Reload(chars assetname = nullptr);
    void Render(ZayPanel& panel);
    bool TickOnce();

public:
    ZEWidget();
    ~ZEWidget();
    ZEWidget(ZEWidget&& rhs);
    ZEWidget& operator=(ZEWidget&& rhs);

public:
    static void BuildComponents(chars viewname, ZaySonInterface& interface, ResourceCB* pcb);
    static void BuildGlues(chars viewname, ZaySonInterface& interface, ResourceCB* pcb);

private:
    ZaySon mZaySon;
    String mZaySonViewName;
    String mZaySonAssetName;
    String mZaySonFileName;
    uint64 mZaySonFileSize;
    uint64 mZaySonModifyTime;
    ResourceCB mResourceCB;
    sint32 mProcedureID;
    uint64 mLastProcedureTime;
    id_pipe mPipe;
    uint64 mPipeModifyTime;
    Strings mPipeReservers;
};
typedef Array<ZEWidget> ZEWidgets;
