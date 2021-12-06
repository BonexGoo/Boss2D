#pragma once
#include <platform/boss_platform.hpp>
#include <functional>

namespace BOSS
{
    /// @brief 리치에디터 쿠미 (CUMI, Can Unified Modeling Interface)
    //
    // ● 인코딩 : 항상 UTF8
    // ● 주의사항 : 일반워드나 []내에서의 [, ], <, >, $, %의 사용은
    //    &[, &], &<, &>, &$, &%로 대체, &는 &&로 대체
    // ● 아래 ▽는 커서가 위치가능한 자리(빈칸삽입은 항상 가능)
    // ● 커서와 구간선택 : ▽는 실제로는 $-커서, %-선택시작지점
    //
    // ■ 메인토큰 : <cumi.옵션_값[사용자토큰들...]>
    //    <cumi[h<str[e]>l<str[l]>o]>
    //    <cumi[▽h▽<str[▽e▽]>▽l▽<str[▽l▽]>▽o▽]>
    //    <cumi.height_150%.bgcolor_FF00FF[he<str.color_000000[ll]>o]>
    //
    // ■ 사용자토큰 : <토큰타입명.옵션1_값1.옵션2_값2[텍<토큰1>스<토큰2>트]>
    //    <type>
    //    ▽<type>▽
    //    <type[text]>
    //    ▽<type[▽t▽e▽x▽t▽]>▽
    //    <type[te<type2[]> x <type3>t]>
    //    ▽<type[▽t▽e▽<type2[▽]>▽ ▽x▽ ▽<type3>▽t▽]>▽
    //    <type.opt1_v1.opt2_v2[text1][ <type2[ ]>]>
    //    ▽<type.opt1_v1.opt2_v2[▽t▽e▽x▽t▽1▽][▽ ▽<type2[▽ ▽]>▽]>▽

    class CumiNode;

    class CumiEditor
    {
    public:
        CumiEditor();
        ~CumiEditor();

    public:
        CumiEditor(CumiEditor&& rhs);
        CumiEditor(const CumiEditor& rhs);
        CumiEditor& operator=(CumiEditor&& rhs);
        CumiEditor& operator=(const CumiEditor& rhs);

    public:
        enum class RenderMode {SizeOnly, ReadOnly, Writeable};
        typedef std::function<size64(const CumiNode& self, void* graphics, sint32 id, RenderMode mode)> RenderCB;
        typedef std::function<bool(CumiNode& self, sint32 code, char key)> KeyCB;
        typedef std::function<String(const String& child)> TextCB;
        typedef std::function<void()> UpdateCB;

    public:
        /// @brief 토큰처리기를 추가
        /// @param name : 토큰처리기의 명칭
        /// @param body : 바디 랜더러
        /// @param blank : 공백 랜더러
        /// @param cursor : 커서 랜더러
        /// @param key : 키입력 처리함수
        /// @param text : 텍스트 수집함수
        static void AddToken(chars name,
            RenderCB body, RenderCB blank, RenderCB cursor, KeyCB key, TextCB text);

    public:
        /// @brief Cumi스크립트를 파싱하여 로드
        /// @param text : Cumi스트립트
        /// @param length : text의 길이(-1이면 자동설정)
        /// @return 성공여부
        bool LoadScript(chars text, sint32 length = -1);

        /// @brief Cumi스크립트를 파싱하여 붙임
        /// @param text : Cumi스트립트
        /// @param length : text의 길이(-1이면 자동설정)
        /// @return 성공여부
        bool AddScript(chars text, sint32 length = -1);

        /// @brief Cumi스크립트로 세이브
        /// @return Cumi스크립트
        String SaveScript() const;

        /// @brief 커서위치부터 뒤쪽까지 Cumi스크립트로 잘라내기
        /// @return Cumi스크립트
        String CutCursorScript();

        /// @brief 평문으로 세이브
        /// @return 텍스트
        String SaveText() const;

        /// @brief 에디터 랜더링
        /// @param view : 뷰핸들
        /// @param graphics : 랜더링에 필요한 그래픽스인스턴스
        /// @param id : 에디터의 유니크ID
        /// @param readonly : 읽기전용모드
        void Render(h_view view, void* graphics, sint32 id, bool readonly) const;

        /// @brief 랜더링될 가로길이
        /// @param graphics : 랜더링에 필요한 그래픽스인스턴스
        sint32 GetRenderWidth(void* graphics) const;

        /// @brief 업데이트를 전달받을 함수를 추가
        /// @param updater : 업데이트 처리함수
        void SetUpdater(UpdateCB updater);

        /// @brief 커서를 추가
        /// @param graphics : 위치계산에 필요한 그래픽스인스턴스
        /// @param pos : 커서의 위치
        void SetCursor(void* graphics, sint32 pos);

        /// @brief 커서를 앞쪽에 추가
        void SetCursorToFront();

        /// @brief 커서를 뒷쪽에 추가
        void SetCursorToRear();

    private:
        CumiNode* mTopToken;
    };
    typedef Array<CumiEditor> CumiEditors;

    class CumiNode
    {
    public:
        CumiNode() {}
        virtual ~CumiNode() {}

    public:
        virtual size64 Render(void* graphics, sint32 id, CumiEditor::RenderMode mode) const = 0;
        virtual void RenderChildGroup(sint32 group, void* graphics, sint32 id, bool readonly) const {}
        virtual size64 GetChildGroupSize(sint32 group, void* graphics, sint32 id) const {return {0, 0};}
        virtual void Update() const {}

    public:
        virtual bool IsTypeWord(char* utf8 = nullptr, uint32* utf32 = nullptr) const {return false;}
        virtual bool IsTypeBlank() const {return false;}
        virtual bool IsTypeCursor() const {return false;}
        virtual bool IsTypeToken(String* type = nullptr) const {return false;}
        virtual void ChangeType(chars type) {}
        virtual String* GetOption(chars key) const {return nullptr;}

    public:
        virtual CumiNode* GetTopNode() const {return nullptr;}
        virtual bool CanCursor() const {return false;}
        virtual bool HasCursor() const {return false;}
        virtual Point GetCursorPos() const {return Point();}
        virtual bool IsCursorFront() const {return false;}
        virtual bool IsCursorRear() const {return false;}
        virtual void SendTextNotify(chars topic, chars text) const {}
        virtual void SendCursorNotify(chars topic) const {}
        virtual void NeedCapture() {}

    public:
        virtual void CursorToLeft() {}
        virtual void CursorToRight() {}
        virtual void CursorToDelete() {}
        virtual void CursorToBackSpace() {}
        virtual void ClearCursor(const CumiNode* except) {}
        virtual const CumiNode* AddCursor(sint32 group, sint32 index) {return nullptr;}
        virtual const CumiNode* AddCursorForInside(void* graphics, sint32 posx) {return nullptr;}
        virtual const CumiNode* AddCursorForOutside(sint32 posx) {return nullptr;}
        virtual const CumiNode* AddCursorByOther(const CumiNode* other, bool front) {return nullptr;}
        virtual const CumiNode* AddCursorToFront() {return nullptr;}
        virtual const CumiNode* AddCursorToRear() {return nullptr;}
        virtual void InsertWord(char word) {}
        virtual void InsertToken(chars script, bool focusing) {}
        virtual const CumiNode* GetNodeByCursorPos(sint32 seek) const {return nullptr;}
        virtual const CumiNode* GetNodeBySelf(sint32 seek) const {return nullptr;}

    public:
        mutable float mTempValue {0};
    };
}
