#pragma once
#include <service/boss_zay.hpp>
#include <service/boss_zayson.hpp>

namespace BOSS
{
    class ZayWidgetDocumentP;

    /// @brief 제이에디터연동 위젯
    class ZayWidget
    {
        BOSS_DECLARE_NONCOPYABLE_CLASS(ZayWidget)

    public:
        ZayWidget();
        ~ZayWidget();
        ZayWidget(ZayWidget&& rhs);
        ZayWidget& operator=(ZayWidget&& rhs);

    public:
        typedef const Image* (*ResourceCB)(chars name);
        ZaySon& Init(chars viewname, chars assetname = nullptr, ResourceCB cb = nullptr);
        void Reload(chars assetname = nullptr);
        void Render(ZayPanel& panel);
        bool TickOnce();

    public:
        static void BuildComponents(chars viewname, ZaySonInterface& interface, ResourceCB* pcb);
        static void BuildGlues(chars viewname, ZaySonInterface& interface, ResourceCB* pcb);

    private:
        ZaySon mZaySon;
        String mZaySonViewName;
        String mZaySonAssetName;
        uint64 mZaySonFileSize;
        uint64 mZaySonModifyTime;
        ResourceCB mResourceCB;
        sint32 mProcedureID;
        uint64 mLastProcedureTime;
        id_pipe mPipe;
        uint64 mPipeModifyTime;
        Strings mPipeReservers;
    };
    typedef Array<ZayWidget> ZayWidgets;

    /// @brief 제이에디터연동 문서객체모델
    class ZayWidgetDOM
    {
    private:
        static ZayWidgetDOM& ST()
        {return *BOSS_STORAGE(ZayWidgetDOM);}

    private:
        ZayWidgetDOM();
        ~ZayWidgetDOM();

    public:
        static void Add(chars variable, chars formula);
        static void AddJson(const Context& json);
        static void AddFlush();
        static void ReserverFlush();
        static void Update(chars variable, chars formula);
        static void UpdateJson(const Context& json);
        static void RemoveAll(chars keyword);
        static SolverValue GetValue(chars variable);
        static void BindPipe(id_pipe pipe);
        static void UnbindPipe(id_pipe pipe);

    private:
        void UpdateFlush();
        static void UpdateToPipe(id_pipe pipe, const Solver* solver);
        void RemoveFlush(chars variable);
        static void RemoveToPipe(id_pipe pipe, chars variable);

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
        ZayWidgetDocumentP* const mDocument;
        Map<Pipe> mPipeMap;
    };
}
