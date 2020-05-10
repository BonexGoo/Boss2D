#pragma once
#include <boss.hpp>
#include <element/boss_color.hpp>
#include <element/boss_point.hpp>
#include <element/boss_vector.hpp>

#if BOSS_NEED_SILENT_NIGHT
    #define BOSS_SILENT_NIGHT_IS_ENABLED
#elif defined(BOSS_PLATFORM_QT5) && !defined(QT_HAVE_GRAPHICS)
    #define BOSS_SILENT_NIGHT_IS_ENABLED
#endif

namespace BOSS
{

enum WindowEvent {WE_Tick, WE_Max};
enum UIRole {UIR_Menu = 0x1, UIR_Tool = 0x2, UIR_Both = 0x3};
static UIRole operator|(UIRole a, UIRole b) {return (UIRole) (int(a) | int(b));}
enum UIDirection {UID_Left = 0x1, UID_Top = 0x2, UID_Right = 0x4, UID_Bottom = 0x8, UID_Wherever = 0xF};
static UIDirection operator|(UIDirection a, UIDirection b) {return (UIDirection) (int(a) | int(b));}
enum UIAlign {
    UIA_LeftTop,    UIA_CenterTop,    UIA_RightTop,
    UIA_LeftMiddle, UIA_CenterMiddle, UIA_RightMiddle,
    UIA_LeftBottom, UIA_CenterBottom, UIA_RightBottom};
enum UIFontAlign {
    UIFA_LeftTop,    UIFA_CenterTop,    UIFA_RightTop,    UIFA_JustifyTop,
    UIFA_LeftMiddle, UIFA_CenterMiddle, UIFA_RightMiddle, UIFA_JustifyMiddle,
    UIFA_LeftAscent, UIFA_CenterAscent, UIFA_RightAscent, UIFA_JustifyAscent,
    UIFA_LeftBottom, UIFA_CenterBottom, UIFA_RightBottom, UIFA_JustifyBottom};
enum UIFontElide {UIFE_None, UIFE_Left, UIFE_Center, UIFE_Right};
enum UIStretchForm {UISF_Strong, UISF_Inner, UISF_Outer, UISF_Width, UISF_Height};
enum UIEditType {UIET_String, UIET_Int, UIET_Float};
enum UITestOrder {UITO_ScissorOn, UITO_ScissorOff};
enum UIStack {UIS_PushPop, UIS_Push, UIS_Current, UIS_Pop};
enum MaskRole {MR_SrcOver, MR_DstOver, MR_Clear, MR_Src, MR_Dst,
    MR_SrcIn, MR_DstIn, MR_SrcOut, MR_DstOut,
    MR_SrcAtop, MR_DstAtop, MR_Xor, MR_Default = MR_SrcOver};
enum CursorRole {CR_Arrow, CR_UpArrow, CR_Cross, CR_Wait, CR_IBeam, CR_Blank,
    CR_SizeVer, CR_SizeHor, CR_SizeBDiag, CR_SizeFDiag, CR_SizeAll,
    CR_PointingHand, CR_OpenHand, CR_ClosedHand, CR_Forbidden, CR_Busy, CR_WhatsThis};
enum DialogShellType {DST_FileOpen, DST_FileSave, DST_Dir};
enum DialogButtonType {DBT_YesNo, DBT_Ok, DBT_OKCancel, DBT_OkCancelIgnore};
enum ConnectStatus {CS_Connecting, CS_Connected, CS_Disconnected};
enum PurchaseType {PT_Consumable, PT_Unlockable};
class ViewClass;
class ViewManager;
typedef void (*ProcedureCB)(payload data);
typedef bool (*PassCB)(void* view, payload data);
typedef void (*ThreadCB)(void* data);
typedef uint32 (*ThreadExCB)(void* data);
typedef void (*PurchaseCB)(id_purchase purchase, bool success, chars comment);
typedef sint32 (*SerialDecodeCB)(bytes data, sint32 length, uint08s& outdata, sint32* outtype);
typedef void (*SerialEncodeCB)(bytes data, sint32 length, uint08s& outdata, sint32 type);

/// @brief 크로스플랫폼지원
class Platform
{
public:
    /// @brief 플랫폼초기화(GL뷰)
    static void InitForGL(bool frameless = false, bool topmost = false);

    /// @brief 플랫폼초기화(MDI뷰)
    static void InitForMDI(bool frameless = false, bool topmost = false);

    /// @brief 뷰생성기 설정
    /// @param creator : 뷰생성기
    static void SetViewCreator(View::CreatorCB creator);

    /// @brief 윈도우이름 설정
    /// @param name : 윈도우이름
    static void SetWindowName(chars name);

    /// @brief 윈도우뷰 설정
    /// @param viewclass : 로드할 뷰클래스(BOSS_DECLARE_VIEW로 선언, nullptr일 경우 _defaultview_)
    /// @return 뷰핸들
    static h_view SetWindowView(chars viewclass = nullptr);

    /// @brief 윈도우위치 설정
    /// @param x : 가로위치(px)
    /// @param y : 세로위치(px)
    static void SetWindowPos(sint32 x, sint32 y);

    /// @brief 윈도우사이즈 설정
    /// @param width : 가로길이(px)
    /// @param height : 세로길이(px)
    static void SetWindowSize(sint32 width, sint32 height);

    /// @brief 윈도우영역 설정
    /// @param x : 가로위치(px)
    /// @param y : 세로위치(px)
    /// @param width : 가로길이(px)
    /// @param height : 세로길이(px)
    static void SetWindowRect(sint32 x, sint32 y, sint32 width, sint32 height);

    /// @brief 윈도우영역 얻기
    /// @param normally : 전체화면화된 경우 노멀값을 사용할지의 여부
    /// @return 윈도우영역(px)
    static rect128 GetWindowRect(bool normally = false);

    /// @brief 윈도우보여짐 설정
    /// @param visible : 보여짐여부
    static void SetWindowVisible(bool visible);

    /// @brief 윈도우 강조하기
    static void SetWindowFlash();

    /// @brief 윈도우마스크 설정
    /// @param image : 마스킹할 이미지(nullptr이면 마스킹옵션이 제거됨)
    /// @return 성공여부(윈도우가 frameless스타일이어야 성공)
    static bool SetWindowMask(id_image_read image = nullptr);

    /// @brief 윈도우투명도 설정
    /// @param value : 0~1사이의 불투명도값
    static void SetWindowOpacity(float value);

    /// @brief 윈도우프로시저 추가
    /// @param event : 호출시점
    /// @param cb : 콜백함수
    /// @param data : 콜백함수에 전달할 데이터
    /// @return 생성된 윈도우프로시저의 ID
    static sint32 AddWindowProcedure(WindowEvent event, ProcedureCB cb, payload data = nullptr);

    /// @brief 윈도우프로시저 제거
    /// @param id : 제거할 윈도우프로시저의 ID
    static void SubWindowProcedure(sint32 id);

    /// @brief 상태창 설정
    /// @param text : 스트링
    /// @param stack : 스택처리
    static void SetStatusText(chars text, UIStack stack);

    /// @brief 아이콘 생성
    /// @param filepath : 파일경로(png)
    /// @return 아이콘핸들
    static h_icon CreateIcon(chars filepath);

    /// @brief 액션 생성
    /// @param name : 액션이름
    /// @param tip : 툴팁
    /// @param view : 이벤트를 전달할 뷰핸들
    /// @param icon : 지정할 아이콘핸들
    /// @return 액션핸들
    static h_action CreateAction(chars name, chars tip, h_view view, h_icon icon = h_icon::null());

    /// @brief 사이즈정책 생성
    /// @param minwidth : 최소 가로길이(px)
    /// @param minheight : 최소 가로길이(px)
    /// @param maxwidth : 최대 가로길이(px)
    /// @param maxheight : 최대 가로길이(px)
    /// @return 사이즈정책핸들
    static h_policy CreatePolicy(sint32 minwidth, sint32 minheight, sint32 maxwidth = 0xFFFF, sint32 maxheight = 0xFFFF);

    /// @brief 뷰 생성
    /// @param name : 뷰이름
    /// @param width : 가로길이(px)
    /// @param height : 세로길이(px)
    /// @param policy : 사이즈정책핸들
    /// @param viewclass : 로드할 뷰클래스(BOSS_DECLARE_VIEW로 선언, nullptr일 경우 _defaultview_)
    /// @return 뷰핸들
    static h_view CreateView(chars name, sint32 width, sint32 height, h_policy policy, chars viewclass = nullptr);

    /// @brief 해당 뷰에 변경할 뷰클래스를 지정
    /// @param view : 뷰핸들
    /// @param viewclass : 변경할 뷰클래스(BOSS_DECLARE_VIEW로 선언, nullptr일 경우 _defaultview_)
    /// @return 뷰클래스객체
    static void* SetNextViewClass(h_view view, chars viewclass = nullptr);

    /// @brief 해당 뷰에 변경할 뷰오브젝트를 지정
    /// @param view : 뷰핸들
    /// @param viewmanager : 변경할 뷰매니저
    /// @return 변경 성공 여부
    static bool SetNextViewManager(h_view view, View* viewmanager);

    /// @brief 도킹바 생성 및 추가
    /// @param view : 뷰핸들
    /// @param direction : 도킹할 방향
    /// @param directionbase : 도킹이 가능한 모든 방향
    /// @return 도킹바핸들
    static h_dock CreateDock(h_view view, UIDirection direction, UIDirection directionbase = UID_Wherever);

    /// @brief 윈도우에 액션 추가
    /// @param group : 그룹이름
    /// @param action : 액션핸들
    /// @param role : 추가할 곳
    static void AddAction(chars group, h_action action, UIRole role = UIR_Both);

    /// @brief 윈도우에 구분선 추가
    /// @param group : 그룹이름
    /// @param role : 추가할 곳
    static void AddSeparator(chars group, UIRole role = UIR_Both);

    /// @brief 윈도우에 도킹바용 토글 추가
    /// @param group : 그룹이름
    /// @param dock : 도킹바핸들
    /// @param role : 추가할 곳
    static void AddToggle(chars group, h_dock dock, UIRole role = UIR_Both);

    /// @brief 차일드식 윈도우 생성 및 추가
    /// @param view : 뷰핸들
    /// @param icon : 지정할 아이콘핸들
    /// @return 윈도우핸들
    static h_window OpenChildWindow(h_view view, h_icon icon = h_icon::null());

    /// @brief 팝업식 윈도우 생성 및 추가
    /// @param view : 뷰핸들
    /// @param icon : 지정할 아이콘핸들
    /// @return 윈도우핸들
    static h_window OpenPopupWindow(h_view view, h_icon icon = h_icon::null());

    /// @brief 트레이식 윈도우 생성 및 추가
    /// @param view : 뷰핸들
    /// @param icon : 지정할 아이콘핸들
    /// @return 윈도우핸들
    static h_window OpenTrayWindow(h_view view, h_icon icon);

    /// @brief 윈도우 제거
    /// @param window : 윈도우핸들
    static void CloseWindow(h_window window);

    /// @brief 뷰에 알림사항 전달
    /// @param view : 뷰핸들
    /// @param topic : 주제
    /// @param in : 전달할 공유객체
    /// @param needout : 전달받을 공유객체 필요여부
    /// @return 전달받을 공유객체
    static id_cloned_share SendNotify(h_view view, chars topic, id_share in, bool needout = false);

    /// @brief 다수의 뷰에 알림사항 방송
    /// @param topic : 주제
    /// @param in : 전달할 공유객체
    /// @param type : 알림타입
    /// @param viewclass : 전달받을 뷰클래스(BOSS_DECLARE_VIEW로 선언, nullptr일 경우 전체 뷰클래스)
    static void BroadcastNotify(chars topic, id_share in, NotifyType type = NT_Normal, chars viewclass = nullptr);

    /// @brief 특정 콜백함수를 모든 뷰에 통과시킴
    /// @param cb : 콜백함수
    /// @param data : 콜백함수에 전달할 데이터
    static void PassAllViews(PassCB cb, payload data);

    /// @brief 모든 뷰의 화면을 갱신
    static void UpdateAllViews();

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief 팝업개체지원
    class Popup
    {
    public:
        /// @brief 텍스트 다이얄로그
        /// @param text : 입력(초기값 필요) 및 결과
        /// @param title : 창제목
        /// @param topic : 입력주제
        /// @param ispassword : 패스워드여부
        /// @return 유효성(확인버튼이 아닌 종료는 무효)
        static bool TextDialog(String& text, chars title, chars topic, bool ispassword = false);

        /// @brief 컬러 다이얄로그
        /// @param r : 적색-입력(초기값 필요) 및 결과
        /// @param g : 녹색-입력(초기값 필요) 및 결과
        /// @param b : 청색-입력(초기값 필요) 및 결과
        /// @param a : 알파-입력(초기값 필요) 및 결과
        /// @param title : 창제목
        /// @return 유효성(확인버튼이 아닌 종료는 무효)
        static bool ColorDialog(uint08& r, uint08& g, uint08& b, uint08& a, chars title);

        /// @brief 파일(디렉토리) 다이얄로그
        /// @param type : 종류(파일오픈, 파일저장, 디렉토리)
        /// @param path : 입력(초기값 필요) 및 결과
        /// @param shortpath : 아이템명
        /// @param title : 창제목
        /// @return 유효성(확인버튼이 아닌 종료는 무효)
        static bool FileDialog(DialogShellType type, String& path, String* shortpath, chars title);

        /// @brief 메시지 다이얄로그
        /// @param title : 창제목
        /// @param text : 내용
        /// @param type : 버튼타입
        /// @return 버튼결과(0-긍정, 1-부정, 2-무시)
        static sint32 MessageDialog(chars title, chars text, DialogButtonType type = DBT_Ok);

        /// @brief 웹브라우저 다이얄로그
        /// @param url : 접속할 URL
        static void WebBrowserDialog(chars url);

        /// @brief 응용프로그램 다이얄로그
        /// @param exepath : 실행파일의 경로
        /// @param args : 전달할 스트링
        /// @param admin : 관리자권한
        static void ProgramDialog(chars exepath, chars args = nullptr, bool admin = false);

        /// @brief 텍스트편집 트래커 열기
        /// @param text : 입력(초기값 필요) 및 결과
        /// @param type : 입력조건
        /// @param l : 좌측위치(px)
        /// @param t : 상단위치(px)
        /// @param r : 우측길이(px)
        /// @param b : 하단길이(px)
        /// @param enter : 종료시 엔터가 눌러졌는지 여부
        /// @return 유효성(Esc로 종료되었다면 무효)
        static bool OpenEditTracker(String& text, UIEditType type, sint32 l, sint32 t, sint32 r, sint32 b, bool* enter = nullptr);

        /// @brief 텍스트선택 트래커 열기
        /// @param textes : 표시될 스트링들
        /// @param l : 좌측위치(px)
        /// @param t : 상단위치(px)
        /// @param r : 우측길이(px)
        /// @param b : 하단길이(px)
        /// @return 선택된 인덱스(Esc로 종료되었다면 -1)
        static sint32 OpenListTracker(Strings textes, sint32 l, sint32 t, sint32 r, sint32 b);

        /// @brief 모든 트래커 닫기
        static void CloseAllTracker();

        /// @brief 열린 트래커 존재여부
        /// @return 존재여부
        static bool HasOpenedTracker();

        /// @brief 툴팁 보여주기
        /// @param text : 표시될 스트링
        static void ShowToolTip(String text);

        /// @brief 툴팁 감추기
        static void HideToolTip();

        /// @brief 스플래시 보여주기
        /// @param filepath : 파일경로(png)
        static void ShowSplash(chars filepath);

        /// @brief 스플래시 감추기
        static void HideSplash();
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief 유틸리티지원
    class Utility
    {
    public:
        /// @brief 난수 구하기
        /// @return 난수
        static uint32 Random();

        /// @brief 슬립처리
        /// @param ms : 밀리초
        /// @param process_input : 입력처리
        /// @param process_socket : 소켓처리
        static void Sleep(sint32 ms, bool process_input, bool process_socket);

        /// @brief 프로그램의 최소화
        static void SetMinimize();

        /// @brief 프로그램의 전체화면화
        static void SetFullScreen();

        /// @brief 프로그램의 전체화면화 여부
        static bool IsFullScreen();

        /// @brief 프로그램의 기본윈도우화
        static void SetNormalWindow();

        /// @brief 프로그램의 종료
        static void ExitProgram();

        /// @brief 프로그램의 경로
        /// @param dironly : 실행파일명을 제외한 폴더경로만 받을지의 여부
        /// @return 파일 또는 폴더경로
        static String GetProgramPath(bool dironly = false);

        /// @brief 프로그램에 들어온 인수를 확인
        /// @param i : 인수인덱스
        /// @param getcount : 수량정보
        /// @return i번째에 해당하는 인수스트링(없으면 "")
        static chars GetArgument(sint32 i, sint32* getcount = nullptr);

        /// @brief URL스키마의 내용을 체크
        /// @param schema : URL스키마(예: http, https, mailto)
        /// @param comparepath : 비교할 패스(comparepath의 길이까지만 비교)
        /// @return 성공여부
        static bool TestUrlSchema(chars schema, chars comparepath);

        /// @brief 프로그램을 OS에 URL스키마로 등록
        /// @param schema : URL스키마(예: http, https, mailto)
        /// @param exepath : 레지스트리에 기록할 실행파일의 경로(생략시 레지스트리에 URL스키마의 존재유무만 확인)
        /// @param forcewrite : 무조건 기록
        /// @return 성공여부
        static bool BindUrlSchema(chars schema, chars exepath, bool forcewrite = false);

        /// @brief 프로그램에 해당 URL스키마와 함께 들어온 인수를 확인
        /// @param schema : URL스키마(예: http, https, mailto)
        /// @return 프로그램 URL스키마로 동작된 경우 인수반환(없으면 nullptr)
        static chars GetArgumentForUrlSchema(chars schema);

        /// @brief 스크린영역 얻기
        /// @param rect : 스크린영역(px)
        /// @param screenid : 스크린ID(-1일 경우 모든 스크린)
        /// @param available_only : 시작표시줄등을 제외한 유효영역만으로 제한
        /// @return 스크린수량
        static sint32 GetScreenRect(rect128& rect, sint32 screenid = -1, bool available_only = true);

        /// @brief 물리적인 모니터 연결여부
        /// @return 연결여부
        static bool IsScreenConnected();

        /// @brief 스크린샷 이미지 얻기
        /// @param rect : 스크린영역(px)
        /// @return 스크린샷 이미지(nullptr은 실패)
        static id_image_read GetScreenshotImage(const rect128& rect);

        /// @brief 이미지로부터 비트맵 얻기
        /// @param image : 이미지
        /// @param ori : 90도기준 회전상태와 상하반전여부(디폴트는 상하반전)
        /// @return 비트맵(nullptr은 실패)
        static id_bitmap ImageToBitmap(id_image_read image, orientationtype ori = orientationtype_fliped0);

        /// @brief 커서모양 바꾸기
        /// @param role : 커서모양
        static void SetCursor(CursorRole role);

        /// @brief 커서위치 얻기
        /// @param pos : 윈도우좌표계 커서위치(px)
        static void GetCursorPos(point64& pos);

        /// @brief 윈도우내 커서위치 얻기
        /// @param pos : 클라이언트좌표계 커서위치(px)
        /// @return 윈도우의 안쪽인지의 여부
        static bool GetCursorPosInWindow(point64& pos);

        /// @brief OS의 사용자배율 얻기
        /// @return 사용자배율(1.0, 1.25, 1.5, 1.75, 2.0...)
        static float GetPixelRatio();

        /// @brief OS명칭 얻기
        /// @return OS명칭
        static chars GetOSName();

        /// @brief 디바이스ID 얻기
        /// @return 디바이스ID
        static chars GetDeviceID();

        /// @brief 콜백함수(ThreadCB)를 스레드방식으로 실행
        /// @param cb : 콜백함수
        /// @param data : 콜백함수에 전달할 데이터
        static void Threading(ThreadCB cb, payload data);

        /// @brief 콜백함수(ThreadExCB)를 스레드방식으로 실행
        /// @param cb : 콜백함수
        /// @param data : 콜백함수에 전달할 데이터
        /// @return 스레드핸들
        static void* ThreadingEx(ThreadExCB cb, payload data);

        /// @brief 소속된 스레드ID 구하기
        /// @return 스레드ID
        static uint64 CurrentThreadID();

        /// @brief 현재시간 구하기
        /// @return 현재시간(ms)
        static uint64 CurrentTimeMsec();

        /// @brief OS의 가용메모리현황
        /// @param totalbytes : 전체 메모리양(바이트단위)
        /// @return 사용 가능한 메모리양(바이트단위)
        static sint64 CurrentAvailableMemory(sint64* totalbytes = nullptr);

        /// @brief 디스크의 가용공간현황
        /// @param drivecode : 드라이브코드(1~26)
        /// @param totalbytes : 전체 디스크양(바이트단위)
        /// @return 사용 가능한 디스크양(바이트단위)
        static sint64 CurrentAvailableDisk(sint32 drivecode, sint64* totalbytes = nullptr);

        /// @brief 마지막 핫키를 반환(비워짐)
        /// @return 키값(저장된 키값이 없으면 -1)
        static sint32 LastHotKey();
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief 클록지원
    class Clock
    {
    public:
        /// @brief 클럭의 기준시각변경
        /// @param timestring : 변경할 시간("yyyy-MM-dd HH:mm:ss")
        static void SetBaseTime(chars timestring);

        /// @brief 임의시간으로 클럭생성
        /// @param year : 연도(2014, 2015, 2016, ...)
        /// @param month : 1년미만의 개월수(1 ~ 12)
        /// @param day : 1개월미만의 일수(1 ~ 31)
        /// @param hour : 1일미만의 시간(0 ~ 23)
        /// @param min : 1시간미만의 분(0 ~ 59)
        /// @param sec : 1분미만의 초(0 ~ 59)
        /// @param nsec : 1초미만의 나노초(0 ~ 999,999,999)
        /// @return 생성된 클럭
        static id_clock Create(sint32 year, sint32 month, sint32 day, sint32 hour, sint32 min, sint32 sec, sint64 nsec);

        /// @brief 윈도우타임으로 클럭생성
        /// @param msec : 밀리초기준 윈도우타임
        /// @return 생성된 클럭
        static id_clock CreateAsWindowTime(uint64 msec);

        /// @brief 현재시간으로 클럭생성
        /// @return 생성된 클럭
        static id_clock CreateAsCurrent();

        /// @brief 해당 클럭을 복사
        /// @param clock : 복사할 클럭
        /// @return 생성된 클럭
        static id_clock CreateAsClone(id_clock_read clock);

        /// @brief 클럭해제
        /// @param clock : 해제할 클럭
        static void Release(id_clock clock);

        /// @brief 클럭간 거리구하기
        /// @param from : 기준 클럭
        /// @param to : 목표 클럭
        /// @return 거리시간(ns)
        static sint64 GetPeriodNsec(id_clock_read from, id_clock_read to);

        /// @brief 클럭에 시간더하기
        /// @param dest : 대상 클럭
        /// @param nsec : 더하는 시간(ns)
        static void AddNsec(id_clock dest, sint64 nsec);

        /// @brief 클럭의 시간구하기
        /// @param clock : 대상 클럭
        /// @return 현재시간(ms)
        static uint64 GetMsec(id_clock clock);

        /// @brief UTC표준시에서 로컬시간까지의 거리
        /// @return 로컬시간까지의 거리(ms)
        static sint64 GetLocalMsecFromUTC();

        /// @brief 클럭의 세부정보
        /// @param clock : 대상 클럭
        /// @param nsec : 1초미만의 나노초(0 ~ 999,999,999)
        /// @param sec : 1분미만의 초(0 ~ 59)
        /// @param min : 1시간미만의 분(0 ~ 59)
        /// @param hour : 1일미만의 시간(0 ~ 23)
        /// @param day : 1개월미만의 일수(1 ~ 31)
        /// @param month : 1년미만의 개월수(1 ~ 12)
        /// @param year : 연도(2014, 2015, 2016, ...)
        static void GetDetail(id_clock clock, sint64* nsec, sint32* sec = nullptr, sint32* min = nullptr, sint32* hour = nullptr, sint32* day = nullptr, sint32* month = nullptr, sint32* year = nullptr);
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief 옵션지원
    class Option
    {
    public:
        /// @brief 옵션플래그 설정하기
        /// @param name : 플래그명
        /// @param flag : 플래그값
        static void SetFlag(chars name, bool flag);

        /// @brief 옵션플래그 얻어오기
        /// @param name : 플래그명
        /// @return 플래그값(디폴트값은 false)
        static bool GetFlag(chars name);

        /// @brief 옵션플래그 리스팅
        /// @return 존재하는 모든 플래그명들
        static Strings GetFlagNames();

        /// @brief 옵션텍스트 설정하기
        /// @param name : 텍스트명
        /// @param text : 텍스트값
        static void SetText(chars name, chars text);

        /// @brief 옵션텍스트 얻어오기
        /// @param name : 텍스트명
        /// @return 텍스트값(디폴트값은 "")
        static chars GetText(chars name);

        /// @brief 옵션텍스트 리스팅
        /// @return 존재하는 모든 텍스트명들
        static Strings GetTextNames();

        /// @brief 옵션페이로드 설정하기
        /// @param name : 페이로드명
        /// @param data : 데이터주소
        static void SetPayload(chars name, payload data);

        /// @brief 옵션페이로드 얻어오기
        /// @param name : 페이로드명
        /// @return 데이터주소(디폴트값은 nullptr)
        static payload GetPayload(chars name);

        /// @brief 옵션페이로드 리스팅
        /// @return 존재하는 모든 페이로드명들
        static Strings GetPayloadNames();
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief 그래픽스지원
    class Graphics
    {
    public:
        /// @brief 절단면지정
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        /// @param w : 가로길이(px)
        /// @param h : 세로길이(px)
        static void SetScissor(float x, float y, float w, float h);

        /// @brief 컬러지정
        /// @param r : 적색값
        /// @param g : 녹색값
        /// @param b : 청색값
        /// @param a : 알파값
        static void SetColor(uint08 r, uint08 g, uint08 b, uint08 a);

        /// @brief 마스크지정
        /// @param role : 마스크방식
        static void SetMask(MaskRole role);

        /// @brief 폰트지정
        /// @param name : 폰트명
        /// @param size : 사이즈값
        static void SetFont(chars name, float size);

        /// @brief 프리타입식 폰트지정(BOSS_NEED_ADDON_FREETYPE가 1일때 가능)
        /// @param nickname : 프리타입 Create시 작성한 nickname
        /// @param height : 세로길이(px)
        /// @see AddOn::FreeType::Create
        static void SetFontForFreeType(chars nickname, sint32 height);

        /// @brief 줌지정
        /// @param zoom : 줌값
        static void SetZoom(float zoom);

        /// @brief 사각영역 지우기
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        /// @param w : 가로길이(px)
        /// @param h : 세로길이(px)
        static void EraseRect(float x, float y, float w, float h);

        /// @brief 사각형 출력
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        /// @param w : 가로길이(px)
        /// @param h : 세로길이(px)
        static void FillRect(float x, float y, float w, float h);

        /// @brief 타원 출력
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        /// @param w : 가로길이(px)
        /// @param h : 세로길이(px)
        static void FillCircle(float x, float y, float w, float h);

        /// @brief 폴리곤 출력
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        /// @param p : 꼭지점들(px)
        static void FillPolygon(float x, float y, Points p);

        /// @brief 사각라인 출력(FillRect출력영역 바깥으로 두께만큼)
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        /// @param w : 가로길이(px)
        /// @param h : 세로길이(px)
        /// @param thick : 두께(px)
        static void DrawRect(float x, float y, float w, float h, float thick);

        /// @brief 라인 출력
        /// @param begin : 시작점(px)
        /// @param end : 끝점(px)
        /// @param thick : 두께(px)
        static void DrawLine(const Point& begin, const Point& end, float thick);

        /// @brief 타원라인 출력
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        /// @param w : 가로길이(px)
        /// @param h : 세로길이(px)
        /// @param thick : 두께(px)
        static void DrawCircle(float x, float y, float w, float h, float thick);

        /// @brief 베지어 출력
        /// @param begin : 시작점(px)
        /// @param end : 끝점(px)
        /// @param thick : 두께(px)
        static void DrawBezier(const Vector& begin, const Vector& end, float thick);

        /// @brief 폴리라인 출력
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        /// @param p : 꼭지점들(px)
        /// @param thick : 두께(px)
        static void DrawPolyLine(float x, float y, Points p, float thick);

        /// @brief 폴리베지어 출력
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        /// @param p : 꼭지점들(px)
        /// @param thick : 두께(px)
        /// @param showfirst : 첫선 보임여부
        /// @param showlast : 끝선 보임여부
        static void DrawPolyBezier(float x, float y, Points p, float thick, bool showfirst, bool showlast);

        /// @brief 링베지어 출력
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        /// @param p : 꼭지점들(px)
        /// @param thick : 두께(px)
        /// @param curve : 커브율
        static void DrawRingBezier(float x, float y, Points p, float thick, float curve = 0.2);

        /// @brief 지정한 FBO핸들로 텍스처 출력
        /// @param texture : 출력할 텍스처
        /// @param tx : 텍스처의 좌측위치(px)
        /// @param ty : 텍스처의 상단위치(px)
        /// @param tw : 텍스처의 가로길이(px)
        /// @param th : 텍스처의 세로길이(px)
        /// @param ori : 출력할 방식(90도기준 회전상태와 상하반전여부)
        /// @param antialiasing : 계단현상 보간처리 적용여부
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        /// @param w : 가로길이(px)
        /// @param h : 세로길이(px)
        /// @param color : 컬러링값
        /// @param fbo : 출력될 프레임버퍼의 FBO핸들(화면출력시 0)
        static void DrawTextureToFBO(id_texture_read texture, float tx, float ty, float tw, float th, orientationtype ori, bool antialiasing, float x, float y, float w, float h, Color color = Color::White, uint32 fbo = 0);

        /// @brief 이미지 생성
        /// @param bitmap : 비트맵
        /// @param mirrored : 상하반전여부
        /// @return 생성된 이미지
        static id_image CreateImage(id_bitmap_read bitmap, bool mirrored);

        /// @brief 이미지 가로길이 얻기
        /// @param image : 이미지
        /// @return 이미지 가로길이
        static sint32 GetImageWidth(id_image_read image);

        /// @brief 이미지 세로길이 얻기
        /// @param image : 이미지
        /// @return 이미지 세로길이
        static sint32 GetImageHeight(id_image_read image);

        /// @brief 이미지 삭제
        /// @param image : 이미지
        static void RemoveImage(id_image image);

        /// @brief 이미지루틴 생성
        /// @param bitmap : 비트맵
        /// @param coloring : 컬러링 색상
        /// @param resizing_width : 리사이징용 가로길이(px)
        /// @param resizing_height : 리사이징용 세로길이(px)
        /// @return 이미지루틴
        static id_image_routine CreateImageRoutine(id_bitmap_read bitmap, sint32 resizing_width = -1, sint32 resizing_height = -1, const Color coloring = Color::ColoringDefault);

        /// @brief 이미지루틴 빌드의 한계시간을 업데이트
        /// @param msec : 현재시간부터 앞으로 기다릴 밀리초시간
        /// @see BuildImageRoutineOnce
        static void UpdateImageRoutineTimeout(uint64 msec);

        /// @brief 이미지루틴에 빌드를 가하여 이미지 생성
        /// @param routine : 비트맵
        /// @param use_timeout : 타임아웃의 사용여부
        /// @return 생성된 이미지(실패시 nullptr)
        /// @see UpdateImageRoutineTimeout
        static id_image_read BuildImageRoutineOnce(id_image_routine routine, bool use_timeout);

        /// @brief 이미지루틴 삭제
        /// @param routine : 이미지루틴
        static void RemoveImageRoutine(id_image_routine routine);

        /// @brief 이미지 출력
        /// @param image : 출력할 이미지
        /// @param ix : 이미지의 좌측위치(px)
        /// @param iy : 이미지의 상단위치(px)
        /// @param iw : 이미지의 가로길이(px)
        /// @param ih : 이미지의 세로길이(px)
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        /// @param w : 가로길이(px)
        /// @param h : 세로길이(px)
        static void DrawImage(id_image_read image, float ix, float iy, float iw, float ih, float x, float y, float w, float h);


        /// @brief 지정한 FBO핸들로 폴리곤영역식 이미지 출력
        /// @param image : 출력할 이미지
        /// @param ip : 이미지의 꼭지점들(uv값, x는 0~1, y는 0~1)
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        /// @param p : 꼭지점들(px)
        /// @param color : 컬러링값
        /// @param fbo : 출력될 프레임버퍼의 FBO핸들(화면출력시 0)
        static void DrawPolyImageToFBO(id_image_read image, const Point (&ip)[3], float x, float y, const Point (&p)[3], Color color = Color::White, uint32 fbo = 0);

        /// @brief 문자열 출력
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        /// @param w : 가로길이(px)
        /// @param h : 세로길이(px)
        /// @param string : 문자열
        /// @param count : 문자열의 길이(char단위)
        /// @param align : 정렬
        /// @param elide : 생략기호(...) 방향
        /// @return 실제로 생략이 되었는지의 여부
        static bool DrawString(float x, float y, float w, float h, chars string, sint32 count = -1, UIFontAlign align = UIFA_CenterMiddle, UIFontElide elide = UIFE_None);

        /// @brief 문자열 출력(UTF16)
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        /// @param w : 가로길이(px)
        /// @param h : 세로길이(px)
        /// @param string : 문자열
        /// @param count : 문자열의 길이(wchar단위)
        /// @param align : 정렬
        /// @param elide : 생략기호(...) 방향
        /// @return 실제로 생략이 되었는지의 여부
        static bool DrawStringW(float x, float y, float w, float h, wchars string, sint32 count = -1, UIFontAlign align = UIFA_CenterMiddle, UIFontElide elide = UIFE_None);

        /// @brief 특정 가로길이 공간에 잘려진 문자열 수량얻기
        /// @param byword : 단어단위 적용여부
        /// @param clipping_width : 자를 가로길이(px)
        /// @param string : 문자열
        /// @param count : 문자열의 길이(char단위)
        /// @return 잘린 문자열의 수량
        static sint32 GetLengthOfString(bool byword, sint32 clipping_width, chars string, sint32 count = -1);

        /// @brief 특정 가로길이 공간에 잘려진 문자열 수량얻기(UTF16)
        /// @param byword : 단어단위 적용여부
        /// @param clipping_width : 자를 가로길이(px)
        /// @param string : 문자열
        /// @param count : 문자열의 길이(wchar단위)
        /// @return 잘린 문자열의 수량
        static sint32 GetLengthOfStringW(bool byword, sint32 clipping_width, wchars string, sint32 count = -1);

        /// @brief 폰트의 가로길이 얻기
        /// @param string : 문자열
        /// @param count : 문자열의 길이(char단위)
        /// @return 가로길이(px)
        static sint32 GetStringWidth(chars string, sint32 count = -1);

        /// @brief 폰트의 가로길이 얻기(UTF16)
        /// @param string : 문자열
        /// @param count : 문자열의 길이(wchar단위)
        /// @return 가로길이(px)
        static sint32 GetStringWidthW(wchars string, sint32 count = -1);

        /// @brief 프리타입전용 폰트의 가로길이 얻기(그래픽스상황 관계없음)
        /// @param nickname : 프리타입 Create시 작성한 nickname
        /// @param height : 세로길이(px)
        /// @param string : 문자열
        /// @param count : 문자열의 길이(char단위)
        /// @return 가로길이(px)
        static sint32 GetFreeTypeStringWidth(chars nickname, sint32 height, chars string, sint32 count = -1);

        /// @brief 폰트의 세로길이 얻기
        /// @return 세로길이(px)
        static sint32 GetStringHeight();

        /// @brief 폰트의 Ascent위치 얻기
        /// @return Ascent위치(px)
        static sint32 GetStringAscent();

        /// @brief GL드로잉 시작
        static bool BeginGL();

        /// @brief GL드로잉 끝
        static void EndGL();

        /// @brief 텍스처 생성
        /// @param nv21 : NV21타입인지의 여부
        /// @param bitmapcache : 비트맵생성용 캐시를 보관해둘지의 여부
        /// @param width : 가로길이(px)
        /// @param height : 세로길이(px)
        /// @param bits : 초기화할 비트맵 Bits데이터
        /// @return 생성된 텍스쳐
        static id_texture CreateTexture(bool nv21, bool bitmapcache, sint32 width, sint32 height, const void* bits = nullptr);

        /// @brief 텍스처 복제
        /// @param texture : 텍스처
        /// @return 복제된 텍스쳐
        static id_texture CloneTexture(id_texture texture);

        /// @brief 텍스처의 NV21타입 여부
        /// @param texture : 텍스처
        /// @return NV21타입인지의 여부
        static bool IsTextureNV21(id_texture_read texture);

        /// @brief 텍스처의 핸들 얻기(OpenGL의 텍스처ID)
        /// @param texture : 텍스처
        /// @param i : 텍스처순번
        /// @return 텍스처 핸들
        static uint32 GetTextureID(id_texture_read texture, sint32 i = 0);

        /// @brief 텍스처 가로길이 얻기
        /// @param texture : 텍스처
        /// @return 텍스처 가로길이
        static sint32 GetTextureWidth(id_texture_read texture);

        /// @brief 텍스처 세로길이 얻기
        /// @param texture : 텍스처
        /// @return 텍스처 세로길이
        static sint32 GetTextureHeight(id_texture_read texture);

        /// @brief 텍스처 삭제
        /// @param texture : 삭제할 텍스처
        static void RemoveTexture(id_texture texture);

        /// @brief 텍스처에서 비트맵 생성하기(GL방식)
        /// @param texture : 텍스처
        /// @return 텍스처 비트맵(nullptr은 실패)
        static id_bitmap CreateBitmapFromTextureGL(id_texture_read texture);

        /// @brief 텍스처에서 비트맵 생성하기(Copy방식)
        /// @param texture : 텍스처
        /// @return 텍스처 비트맵(nullptr은 실패)
        static id_bitmap CreateBitmapFromTextureFast(id_texture texture);

        /// @brief 서피스(FBO) 생성
        /// @param width : 가로길이(px)
        /// @param height : 세로길이(px)
        /// @return 생성된 서피스
        static id_surface CreateSurface(sint32 width, sint32 height);

        /// @brief 서피스의 FBO핸들 얻기(OpenGL의 FBO핸들)
        /// @param surface : 서피스
        /// @return 서피스 FBO핸들
        static uint32 GetSurfaceFBO(id_surface_read surface);

        /// @brief 서피스 가로길이 얻기
        /// @param surface : 서피스
        /// @return 서피스 가로길이
        static sint32 GetSurfaceWidth(id_surface_read surface);

        /// @brief 서피스 세로길이 얻기
        /// @param surface : 서피스
        /// @return 서피스 세로길이
        static sint32 GetSurfaceHeight(id_surface_read surface);

        /// @brief 서피스 삭제
        /// @param surface : 삭제할 서피스
        static void RemoveSurface(id_surface surface);

        /// @brief 현재 그래픽스에 서피스를 연동
        /// @param surface : 해당 서피스
        static void BindSurface(id_surface surface);

        /// @brief 현재 그래픽스에서 서피스의 연동을 해제
        /// @param surface : 해당 서피스
        static void UnbindSurface(id_surface surface);

        /// @brief 현재 그래픽스에 서피스 출력
        /// @param surface : 출력할 서피스
        /// @param sx : 서피스의 좌측위치(px)
        /// @param sy : 서피스의 상단위치(px)
        /// @param sw : 서피스의 가로길이(px)
        /// @param sh : 서피스의 세로길이(px)
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        /// @param w : 가로길이(px)
        /// @param h : 세로길이(px)
        static void DrawSurface(id_surface_read surface, float sx, float sy, float sw, float sh, float x, float y, float w, float h);

        /// @brief 지정한 FBO핸들로 서피스 출력(서피스에서 서피스로 가능)
        /// @param surface : 출력할 서피스
        /// @param sx : 서피스의 좌측위치(px)
        /// @param sy : 서피스의 상단위치(px)
        /// @param sw : 서피스의 가로길이(px)
        /// @param sh : 서피스의 세로길이(px)
        /// @param ori : 출력할 방식(90도기준 회전상태와 상하반전여부)
        /// @param antialiasing : 계단현상 보간처리 적용여부
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        /// @param w : 가로길이(px)
        /// @param h : 세로길이(px)
        /// @param color : 컬러링값
        /// @param fbo : 출력될 프레임버퍼의 FBO핸들(화면출력시 0)
        /// @see GetSurfaceFBO
        static void DrawSurfaceToFBO(id_surface_read surface, float sx, float sy, float sw, float sh, orientationtype ori, bool antialiasing, float x, float y, float w, float h, Color color = Color::White, uint32 fbo = 0);

        /// @brief 서피스에서 이미지 얻기
        /// @param surface : 해당 서피스
        /// @return 서피스 이미지(nullptr은 실패)
        static id_image_read GetImageFromSurface(id_surface_read surface);

        /// @brief 서피스에서 비트맵 얻기
        /// @param surface : 해당 서피스
        /// @param ori : 90도기준 회전상태와 상하반전여부(디폴트는 상하반전)
        /// @return 서피스 비트맵(nullptr은 실패)
        static id_bitmap_read GetBitmapFromSurface(id_surface_read surface, orientationtype ori = orientationtype_fliped0);
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief 파일지원
    class File
    {
    public:
        typedef void (*SearchCB)(chars itemname, payload data);

        /// @brief 파일존재여부 얻기
        /// @param filename : 파일명
        /// @return 파일존재여부
        static bool Exist(chars filename);

        /// @brief 폴더존재여부 얻기
        /// @param dirname : 폴더명
        /// @return 폴더존재여부
        static bool ExistForDir(chars dirname);

        /// @brief 파일열기(읽기전용)
        /// @param filename : 파일명
        /// @return 파일ID
        /// @see Close
        static id_file_read OpenForRead(chars filename);

        /// @brief 파일열기(쓰기전용)
        /// @param filename : 파일명
        /// @param autocreatedir : 중간폴더가 없을 경우 자동으로 생성
        /// @return 파일ID
        /// @see Close
        static id_file OpenForWrite(chars filename, bool autocreatedir = false);

        /// @brief 파일닫기
        /// @param file : 파일ID
        /// @see OpenForRead, OpenForWrite
        static void Close(id_file_read file);

        /// @brief 파일사이즈 얻기
        /// @param file : 파일ID
        /// @return 파일사이즈
        static const sint32 Size(id_file_read file);

        /// @brief 파일읽기
        /// @param file : 파일ID
        /// @param data : 저장할 데이터
        /// @param size : 데이터의 길이(byte)
        /// @return 읽은 파일사이즈
        /// @see OpenForRead
        static const sint32 Read(id_file_read file, uint08* data, const sint32 size);

        /// @brief 파일에서 한줄읽기
        /// @param file : 파일ID
        /// @param text : 저장할 텍스트
        /// @param size : 텍스트의 길이(byte, 널문자포함)
        /// @return 읽은 파일사이즈
        /// @see OpenForRead
        static const sint32 ReadLine(id_file_read file, char* text, const sint32 size);

        /// @brief 파일쓰기
        /// @param file : 파일ID
        /// @param data : 불러올 데이터
        /// @param size : 데이터의 길이(byte)
        /// @return 쓴 파일사이즈
        /// @see OpenForWrite
        static const sint32 Write(id_file file, bytes data, const sint32 size);

        /// @brief 파일위치지정(SEEK_SET)
        /// @param file : 파일ID
        /// @param focus : 위치할 곳(byte)
        /// @see OpenForRead, OpenForWrite, Focus
        static void Seek(id_file_read file, const sint32 focus);

        /// @brief 파일포커스
        /// @param file : 파일ID
        /// @see Seek
        static const sint32 Focus(id_file_read file);

        /// @brief 대상폴더내 파일/폴더 검색
        /// @param dirname : 대상폴더명
        /// @param cb : 콜백함수
        /// @param data : 콜백함수에 전달할 데이터
        /// @param needfullpath : 콜백함수에 풀패스를 전달할지의 여부
        /// @return 검색된 수량
        static sint32 Search(chars dirname, SearchCB cb, payload data, bool needfullpath);

        /// @brief 아이템의 속성값 조사
        /// @param itemname : 아이템명(파일 또는 폴더)
        /// @param size : 파일사이즈 얻기
        /// @param ctime : 파일생성시간 얻기
        /// @param atime : 파일접근시간 얻기
        /// @param mtime : 파일수정시간 얻기
        /// @return 속성값
        static sint32 GetAttributes(wchars itemname, uint64* size = nullptr, uint64* ctime = nullptr, uint64* atime = nullptr, uint64* mtime = nullptr);

        /// @brief 아이템의 풀패스명 조사
        /// @param itemname : 아이템명(파일 또는 폴더)
        /// @return 풀패스명
        static WString GetFullPath(wchars itemname);

        /// @brief 아이템의 폴더명 조사
        /// @param itemname : 아이템명(파일 또는 폴더)
        /// @param badslash : 잘못된 슬래시기호
        /// @param goodslash : 정상적인 슬래시기호
        /// @return 폴더명
        static WString GetDirName(wchars itemname, wchar_t badslash = L'\\', wchar_t goodslash = L'/');

        /// @brief 아이템의 단축명 조사
        /// @param itemname : 아이템명(파일 또는 폴더)
        /// @return 단축명
        static WString GetShortName(wchars itemname);

        /// @brief 작업드라이브 코드명얻기
        /// @return 코드명(1~26)
        static sint32 GetDriveCode();

        /// @brief 아이템 존재확인
        /// @param itemname : 확인할 아이템명(파일 또는 폴더)
        /// @return 성공여부
        static bool CanAccess(wchars itemname);

        /// @brief 아이템 쓰기확인
        /// @param itemname : 확인할 아이템명(파일 또는 폴더)
        /// @return 성공여부
        static bool CanWritable(wchars itemname);

        /// @brief 아이템 삭제
        /// @param itemname : 삭제할 아이템명(파일 또는 폴더)
        /// @param autoremovedir : 중간폴더가 빈폴더가 되어 필요없을 경우 자동으로 삭제
        /// @return 성공여부
        static bool Remove(wchars itemname, bool autoremovedir = false);

        /// @brief 아이템 이름변경
        /// @param existing_itemname : 변경될 아이템명(파일 또는 폴더)
        /// @param new_itemname : 변경할 아이템명(파일 또는 폴더)
        /// @return 성공여부
        static bool Rename(wchars existing_itemname, wchars new_itemname);

        /// @brief 신규아이템 이름생성
        /// @param format : 아이템명 포맷(부여될 자리는 X로 표식)
        /// @param length : format의 길이
        /// @return 성공여부
        static bool Tempname(char* format, sint32 length);

        /// @brief 폴더 생성
        /// @param dirname : 생성할 폴더명
        /// @param autocreatedir : 중간폴더가 없을 경우 자동으로 생성
        /// @return 성공여부
        static bool CreateDir(wchars dirname, bool autocreatedir = false);

        /// @brief 폴더 삭제
        /// @param dirname : 삭제할 폴더명
        /// @param autoremovedir : 중간폴더가 빈폴더가 되어 필요없을 경우 자동으로 삭제
        /// @return 성공여부
        static bool RemoveDir(wchars dirname, bool autoremovedir = false);

        /// @brief FD타입 파일열기
        /// @param filename : 파일명
        /// @param writable : 쓰기가 가능
        /// @param append : 파일끝에 추가
        /// @param exclusive : 기존 파일이 없을때만 가능
        /// @param truncate : 파일사이즈를 0으로 초기화
        /// @return 파일FD
        /// @see FDClose
        static sint32 FDOpen(wchars filename, bool writable, bool append, bool exclusive, bool truncate);

        /// @brief FD타입 파일닫기
        /// @param fd : 파일FD
        /// @return 성공여부
        /// @see FDOpen
        static bool FDClose(sint32 fd);

        /// @brief boss_file으로 FD타입 파일열기
        /// @param file : boss_file객체
        /// @return 파일FD
        /// @see FDClose, FDToFile
        static sint32 FDFromFile(boss_file file);

        /// @brief FD타입으로 boss_file열기
        /// @param fd : 파일FD
        /// @return boss_file객체
        /// @see FDFromFile
        static boss_file FDToFile(sint32 fd);

        /// @brief FD타입 파일읽기
        /// @param fd : 파일FD
        /// @param data : 읽어올 데이터
        /// @param size : 데이터의 길이
        /// @return 실제로 읽어온 길이
        static sint64 FDRead(sint32 fd, void* data, sint64 size);

        /// @brief FD타입 파일쓰기
        /// @param fd : 파일FD
        /// @param data : 쓸 데이터
        /// @param size : 데이터의 길이
        /// @return 실제로 쓴 길이
        static sint64 FDWrite(sint32 fd, const void* data, sint64 size);

        /// @brief FD타입 포커싱
        /// @param fd : 파일FD
        /// @param offset : origin으로부터의 상대위치
        /// @param origin : 0-파일시작, 1-마지막포커스, 2-파일끝
        /// @return 마지막포커스
        static sint64 FDSeek(sint32 fd, sint64 offset, sint32 origin);

        /// @brief FD타입 리사이징
        /// @param fd : 파일FD
        /// @param size : 변경할 파일사이즈
        /// @return 성공여부
        static bool FDResize(sint32 fd, sint64 size);

        /// @brief FD타입 메모리매핑 설정
        /// @param file : boss_file객체
        /// @param offset : 매핑할 시작위치
        /// @param size : 매핑할 사이즈
        /// @param readonly : 읽기전용
        /// @return 매핑된 메모리주소
        /// @see FDUnmap
        static void* FDMap(boss_file file, sint64 offset, sint64 size, bool readonly);

        /// @brief FD타입 메모리매핑 해제
        /// @param map : 매핑된 메모리주소
        /// @return 성공여부
        /// @see FDMap
        static bool FDUnmap(const void* map);

        /// @brief FD타입 속성조사
        /// @param fd : 파일FD
        /// @param size : 파일사이즈
        /// @param ctime : 생성시간
        /// @param atime : 접근시간
        /// @param mtime : 수정시간
        /// @return 파일속성값
        static uint32 FDAttributes(sint32 fd, uint64* size = nullptr, uint64* ctime = nullptr, uint64* atime = nullptr, uint64* mtime = nullptr);

        /// @brief 원본(읽기전용) 어셋폴더 경로재정의
        /// @param dirname : 재정의할 폴더명
        static void ResetAssetsRoot(chars dirname);

        /// @brief 사본(읽기/쓰기) 어셋폴더 경로재정의
        /// @param dirname : 재정의할 폴더명
        static void ResetAssetsRemRoot(chars dirname);

        /// @brief 원본(읽기전용) 어셋폴더 경로얻기
        /// @return 경로
        static const String RootForAssets();

        /// @brief 사본(읽기/쓰기) 어셋폴더 경로얻기
        /// @return 경로
        static const String RootForAssetsRem();

        /// @brief 데이터전용(읽기/쓰기) 어셋폴더 경로얻기
        /// @return 경로
        static const String RootForData();

        /// @brief 바탕화면 경로얻기
        /// @return 경로
        static const String RootForDesktop();

        /// @brief 스타트업 경로얻기
        /// @return 경로
        static const String RootForStartup();
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief 사운드지원
    class Sound
    {
    public:
        /// @brief 파일방식 사운드열기
        /// @param filename : 파일명
        /// @param loop : 루프방식여부
        /// @param fade_msec : 페이드시간(밀리초)
        /// @return 사운드ID
        /// @see Close
        static id_sound OpenForFile(chars filename, bool loop = false, sint32 fade_msec = 0);

        /// @brief 스트림방식 사운드열기
        /// @param channel : 채널수(1, 2)
        /// @param sample_rate : 초당 샘플링양(8000, 44100)
        /// @param sample_size : 비트수(8, 16)
        /// @return 사운드ID
        /// @see Close
        static id_sound OpenForStream(sint32 channel, sint32 sample_rate, sint32 sample_size);

        /// @brief 사운드닫기
        /// @param sound : 사운드ID
        /// @see Open
        static void Close(id_sound sound);

        /// @brief 전체볼륨지정
        /// @param volume : 볼륨크기(0.0f~1.0f)
        /// @param fade_msec : 페이드시간(밀리초)
        static void SetVolume(float volume, sint32 fade_msec = 0);

        /// @brief 사운드출력
        /// @param sound : 사운드ID
        /// @param volume_rate : 전체볼륨에 곱해질 배수값(0.0f~1.0f, 그 이상도 가능)
        static void Play(id_sound sound, float volume_rate = 1.0f);

        /// @brief 사운드중단
        /// @param sound : 사운드ID
        static void Stop(id_sound sound);

        /// @brief 사운드가 현재 플레이중인지 조사
        /// @param sound : 사운드ID
        /// @return 플레이여부
        static bool NowPlaying(id_sound sound);

        /// @brief 스트림방식을 위한 사운드입력
        /// @param sound : 사운드ID
        /// @param raw : 저수준 사운드데이터
        /// @param size : raw데이터의 길이
        /// @param timeout : 타임아웃
        /// @return 입력 실패시 -1, 성공시 현재 볼륨값(0~256, 그 이상도 가능)
        static sint32 AddStreamForPlay(id_sound sound, bytes raw, sint32 size, sint32 timeout = 3000);

        /// @brief 현재 플레이되고 있는 사운드중단
        static void StopAll();

        /// @brief 현재 플레이되고 있는 사운드일시정지
        static void PauseAll();

        /// @brief 현재 일시정지되고 있는 사운드재개
        static void ResumeAll();
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief 소켓지원
    class Socket
    {
    public:
        /// @brief 소켓열기(TCP)
        /// @return 소켓ID
        /// @see Close
        static id_socket OpenForTcp();

        /// @brief 소켓열기(UDP)
        /// @return 소켓ID
        /// @see Close
        static id_socket OpenForUdp();

        /// @brief 소켓닫기
        /// @param socket : 소켓ID
        /// @param timeout : 타임아웃
        /// @return 성공여부
        /// @see OpenForTcp, OpenForUdp
        static bool Close(id_socket socket, sint32 timeout = 3000);

        /// @brief 서버와 연결
        /// @param socket : 소켓ID
        /// @param domain : 도메인명(IP가능)
        /// @param port : 포트번호
        /// @param timeout : 타임아웃
        /// @return 성공여부
        /// @see Disconnect
        static bool Connect(id_socket socket, chars domain, uint16 port, sint32 timeout = 3000);

        /// @brief 서버와 연결해제
        /// @param socket : 소켓ID
        /// @param timeout : 타임아웃
        /// @return 성공여부
        /// @see Connect
        static bool Disconnect(id_socket socket, sint32 timeout = 3000);

        /// @brief UDP서비스개시
        /// @param socket : 소켓ID
        /// @param port : 포트번호
        /// @param timeout : 타임아웃
        /// @return 성공여부
        /// @see OpenForUdp, Close
        static bool BindForUdp(id_socket socket, uint16 port, sint32 timeout = 3000);

        /// @brief 대기중인 읽기용 버퍼조사
        /// @param socket : 소켓ID
        /// @return 읽을 수 있는 길이
        /// @see Recv
        static sint32 RecvAvailable(id_socket socket);

        /// @brief 소켓읽기
        /// @param socket : 소켓ID
        /// @param data : 읽어올 데이터
        /// @param size : 데이터의 길이
        /// @param timeout : 타임아웃
        /// @param ip_udp : UDP소켓일 경우, 접속자의 IP
        /// @param ip_port : UDP소켓일 경우, 접속자의 포트번호
        /// @return 실제로 읽어온 길이
        /// @see RecvAvailable
        static sint32 Recv(id_socket socket, uint08* data, sint32 size, sint32 timeout = 3000, ip4address* ip_udp = nullptr, uint16* port_udp = nullptr);

        /// @brief 소켓쓰기
        /// @param socket : 소켓ID
        /// @param data : 쓸 데이터
        /// @param size : 데이터의 길이
        /// @param timeout : 타임아웃
        /// @return 실제로 쓴 길이
        static sint32 Send(id_socket socket, bytes data, sint32 size, sint32 timeout = 3000);

        /// @brief gethostbyname 기능제공
        /// @param name : 호스트명
        /// @return hostent구조체
        static void* GetHostByName(chars name);

        /// @brief getservbyname 기능제공
        /// @param name : 서비스명
        /// @param proto : 프로토콜명
        /// @return servent구조체
        static void* GetServByName(chars name, chars proto);

        /// @brief 자신의 IP주소 얻기
        /// @param ip6 : IP6주소
        /// @return IP4주소
        static ip4address GetLocalAddress(ip6address* ip6 = nullptr);
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief 서버지원
    class Server
    {
    public:
        /// @brief 서버ID 할당
        /// @param sizefield : 사이즈필드 사용여부
        /// @return 서버ID(nullptr은 실패)
        /// @see Release, Listen
        static id_server Create(bool sizefield);

        /// @brief 서버ID 반환
        /// @param server : 서버ID
        /// @see Create
        static void Release(id_server server);

        /// @brief 서비스개시
        /// @param server : 서버ID
        /// @param port : 포트번호
        /// @return 성공여부
        /// @see Create, Release
        static bool Listen(id_server server, uint16 port);

        /// @brief 수신된 패킷확인 및 포커싱
        /// @param server : 서버ID
        /// @return 수신여부
        /// @see GetPacketType, GetPacketPeerID, GetPacketBuffer
        static bool TryNextPacket(id_server server);

        /// @brief 포커싱된 패킷종류 반환
        /// @param server : 서버ID
        /// @return 패킷종류
        /// @see TryNextPacket
        static packettype GetPacketType(id_server server);

        /// @brief 포커싱된 패킷의 송신자ID 반환
        /// @param server : 서버ID
        /// @return 송신자ID
        /// @see TryNextPacket
        static sint32 GetPacketPeerID(id_server server);

        /// @brief 포커싱된 패킷버퍼 반환
        /// @param server : 서버ID
        /// @param getsize : 버퍼의 길이를 요청
        /// @return 패킷버퍼
        /// @see TryNextPacket
        static bytes GetPacketBuffer(id_server server, sint32* getsize = nullptr);

        /// @brief 특정 송신자에게 데이터전달
        /// @param server : 서버ID
        /// @param peerid : 송신자ID
        /// @param buffer : 데이터버퍼
        /// @param buffersize : 버퍼의 길이
        /// @return 성공여부
        static bool SendToPeer(id_server server, sint32 peerid, const void* buffer, sint32 buffersize);

        /// @brief 특정 송신자를 접속해제
        /// @param server : 서버ID
        /// @param peerid : 송신자ID
        /// @return 성공여부
        static bool KickPeer(id_server server, sint32 peerid);

        /// @brief 특정 송신자의 정보를 반환
        /// @param server : 서버ID
        /// @param peerid : 송신자ID
        /// @param ip4 : IP4주소를 요청
        /// @param ip6 : IP6주소를 요청
        /// @param port : 포트번호를 요청
        /// @return 성공여부
        static bool GetPeerInfo(id_server server, sint32 peerid, ip4address* ip4 = nullptr, ip6address* ip6 = nullptr, uint16* port = nullptr);
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief 프로세스간 1:1통신지원
    class Pipe
    {
    public:
        /// @brief 파이프 열기
        /// @param name : 파이프명칭
        /// @return 파이프ID(nullptr은 실패)
        /// @see Close
        static id_pipe Open(chars name);

        /// @brief 파이프 닫기
        /// @param pipe : 파이프ID
        /// @see Open
        static void Close(id_pipe pipe);

        /// @brief 서버인지의 여부
        /// @param pipe : 파이프ID
        /// @see 서버이면 true
        static bool IsServer(id_pipe pipe);

        /// @brief 연결상태 조사
        /// @param pipe : 파이프ID
        /// @return 연결상태
        /// @see Open, Close
        static ConnectStatus Status(id_pipe pipe);

        /// @brief 수신된 사이즈 조사
        /// @param pipe : 파이프ID
        /// @return 수신된 사이즈(0~N)
        /// @see Recv
        static sint32 RecvAvailable(id_pipe pipe);

        /// @brief 데이터수신
        /// @param pipe : 파이프ID
        /// @param data : 수신할 데이터버퍼
        /// @param size : 데이터버퍼의 사이즈
        /// @return 수신한 데이터의 사이즈(0~N)
        /// @see RecvAvailable
        static sint32 Recv(id_pipe pipe, uint08* data, sint32 size);

        /// @brief Json수신
        /// @param pipe : 파이프ID
        /// @return Json객체(없으면 nullptr)
        /// @see SendJson
        static const Context* RecvJson(id_pipe pipe);

        /// @brief 데이터송신
        /// @param pipe : 파이프ID
        /// @param data : 송신할 데이터
        /// @param size : 데이터의 사이즈
        /// @return 송신성공여부
        /// @see Recv
        static bool Send(id_pipe pipe, bytes data, sint32 size);

        /// @brief Json송신
        /// @param pipe : 파이프ID
        /// @param json : 송신할 Json객체
        /// @return 송신성공여부
        /// @see RecvJson
        static bool SendJson(id_pipe pipe, const String& json);
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief 웹지원
    class Web
    {
    public:
        typedef void (*EventCB)(payload data, chars type, chars param);

        /// @brief 웹핸들 할당
        /// @param url : 로드할 웹주소
        /// @param width : 웹페이지 가로길이(px)
        /// @param height : 웹페이지 세로길이(px)
        /// @param clearcookies : 쿠키제거여부
        /// @param cb : 콜백함수
        /// @param data : 콜백함수에 전달할 데이터
        /// @return 웹핸들
        /// @see Release
        static h_web Create(chars url, sint32 width, sint32 height, bool clearcookies, EventCB cb = nullptr, payload data = nullptr);

        /// @brief 웹핸들 반환
        /// @param web : 웹핸들
        /// @see Create
        static void Release(h_web web);

        /// @brief 웹페이지 리로드
        /// @param web : 해당 웹핸들
        /// @param url : 로드할 웹주소
        static void Reload(h_web web, chars url);

        /// @brief 현재 로딩상황
        /// @param web : 해당 웹핸들
        /// @param rate : 로딩진척도(0.0 ~ 1.0)
        /// @return 로딩중인지의 여부
        static bool NowLoading(h_web web, float* rate = nullptr);

        /// @brief 웹페이지 리사이징
        /// @param web : 해당 웹핸들
        /// @param width : 웹페이지 가로길이(px)
        /// @param height : 웹페이지 세로길이(px)
        /// @return 리사이징 실시여부
        static bool Resize(h_web web, sint32 width, sint32 height);

        /// @brief 웹페이지에 터치이벤트 전달
        /// @param web : 해당 웹핸들
        /// @param type : 터치타입
        /// @param x : 좌측위치(px)
        /// @param y : 상단위치(px)
        static void SendTouchEvent(h_web web, TouchType type, sint32 x, sint32 y);

        /// @brief 웹페이지에 키이벤트 전달
        /// @param web : 해당 웹핸들
        /// @param code : 키코드
        /// @param text : 키문자열
        /// @param pressed : 눌려짐 여부
        static void SendKeyEvent(h_web web, sint32 code, chars text, bool pressed);

        /// @brief 웹페이지의 자바스크립트함수 호출
        /// @param web : 해당 웹핸들
        /// @param script : 자바스크립트 소스코드(예시: "func(1, 2);")
        /// @param matchid : 호출결과를 매칭하기 위한 ID(Create의 EventCB를 통해 결과받음)
        /// @see Create
        static void CallJSFunction(h_web web, chars script, sint32 matchid = 0);

        /// @brief 웹페이지 텍스처 얻기
        /// @param web : 해당 웹핸들
        /// @return 웹페이지 텍스처(nullptr은 실패)
        static id_texture_read GetPageTexture(h_web web);

        /// @brief 웹페이지 이미지 얻기
        /// @param web : 해당 웹핸들
        /// @return 웹페이지 이미지(nullptr은 실패)
        static id_image_read GetPageImage(h_web web);

        /// @brief 웹페이지 비트맵 얻기
        /// @param web : 해당 웹핸들
        /// @param ori : 90도기준 회전상태와 상하반전여부(디폴트는 상하반전)
        /// @return 웹페이지 비트맵(nullptr은 실패)
        static id_bitmap_read GetPageBitmap(h_web web, orientationtype ori = orientationtype_fliped0);

        /// @brief 네이티브형 웹핸들 할당
        /// @param url : 로드할 웹주소
        /// @param clearcookies : 쿠키제거여부
        /// @param cb : 콜백함수
        /// @param data : 콜백함수에 전달할 데이터
        /// @return 네이티브형 웹핸들
        /// @see ReleaseNative
        static h_web_native CreateNative(chars url, bool clearcookies, EventCB cb = nullptr, payload data = nullptr);

        /// @brief 네이티브형 웹핸들 반환
        /// @param web_native : 네이티브형 웹핸들
        /// @see CreateNative
        static void ReleaseNative(h_web_native web_native);

        /// @brief 네이티브형 웹페이지 리로드
        /// @param web_native : 해당 네이티브형 웹핸들
        /// @param url : 로드할 웹주소
        static void ReloadNative(h_web_native web_native, chars url);
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief 인앱상품지원
    class Purchase
    {
    public:
        /// @brief 인앱상품 연결
        /// @param name : 앱스토어에 등록한 인앱상품명
        /// @param type : 인앱상품의 종류
        /// @return 구매ID(nullptr은 실패)
        /// @see Close
        static id_purchase Open(chars name, PurchaseType type);

        /// @brief 인앱상품 연결해제
        /// @param purchase : 구매ID
        /// @see Open
        static void Close(id_purchase purchase);

        /// @brief 이전에 구매한 적이 있는지 여부
        /// @param purchase : 구매ID
        /// @return 이전 구매여부
        static bool IsPurchased(id_purchase purchase);

        /// @brief 구매하기
        /// @param purchase : 구매ID
        /// @param cb : 구매결과를 전달받을 콜백함수
        /// @return 상품을 찾지 못한 경우 false
        static bool Purchasing(id_purchase purchase, PurchaseCB cb = nullptr);
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief 블루투스(BLE)지원
    class Bluetooth
    {
    public:
        /// @brief 블루투스이름 리스팅
        /// @param service_uuid : 블루투스기기의 서비스Uuid("{XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}")
        /// @param timeout : 검색제한시간(ms)
        /// @param spec : json형태로 블루투스들의 스펙을 받음(선택사항)
        /// @return 검색된 모든 블루투스Uuid
        static Strings GetAllUuids(chars service_uuid, sint32 timeout, String* spec = nullptr);

        /// @brief 블루투스ID 할당
        /// @param uuid : 블루투스Uuid
        /// @return 블루투스ID(nullptr은 실패)
        /// @see Close
        static id_bluetooth Open(chars uuid);

        /// @brief 블루투스ID 반환
        /// @param bluetooth : 블루투스ID
        /// @see Open
        static void Close(id_bluetooth bluetooth);

        /// @brief 현재 접속상황
        /// @param bluetooth : 블루투스ID
        /// @return 접속여부
        static bool Connected(id_bluetooth bluetooth);

        /// @brief 현재 읽기스트림의 사이즈얻기
        /// @param bluetooth : 블루투스ID
        /// @return 대기중인 읽기스트림의 바이트길이(에러시 -1)
        static sint32 ReadAvailable(id_bluetooth bluetooth);

        /// @brief 읽기스트림에서 읽기
        /// @param bluetooth : 블루투스ID
        /// @param data : 데이터를 받을 공간
        /// @param size : data의 크기(바이트단위)
        /// @return 읽은 크기(바이트단위, 에러시 -1)
        static sint32 Read(id_bluetooth bluetooth, uint08* data, const sint32 size);

        /// @brief 쓰기스트림에 쓰기
        /// @param bluetooth : 블루투스ID
        /// @param data : 보낼 데이터
        /// @param size : data의 크기(바이트단위)
        /// @return 에러여부
        static void Write(id_bluetooth bluetooth, const uint08* data, const sint32 size);

        /// @brief 적재된 이벤트메시지를 소환
        /// @param bluetooth : 블루투스ID
        /// @return 이벤트메시지를 반환(없으면 nullptr)
        static chars EventFlush(id_bluetooth bluetooth);
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief 시리얼통신지원
    class Serial
    {
    public:
        #define BOSS_SERIAL_ERROR_DISCONNECTED (-0x10000) // 접속해제, 해제사유코드
        #define BOSS_SERIAL_ERROR_BROKEN_FORMAT (-0x20000) // 파싱포맷의 문법오류<포맷스트링내 위치> (휴먼에러)
        #define BOSS_SERIAL_ERROR_SHORT_STREAM (-0x30000) // 읽기시 확보된 스트림부족<처리못한 가변인수의 순번> (휴먼에러)

        /// @brief 시리얼포트이름 리스팅
        /// @param spec : json형태로 시리얼포트들의 스펙을 받음(선택사항)
        /// @return 존재하는 모든 시리얼포트이름
        static Strings GetAllNames(String* spec = nullptr);

        /// @brief 시리얼통신ID 할당
        /// @param name : 시리얼포트이름(없으면 첫번째 시리얼포트찾음)
        /// @param dec : 읽기시 
        /// @param enc :
        /// @return 시리얼통신ID(nullptr은 실패)
        /// @see Close
        static id_serial Open(chars name = "", SerialDecodeCB dec = nullptr, SerialEncodeCB enc = nullptr);

        /// @brief 시리얼통신ID 반환
        /// @param serial : 시리얼통신ID
        /// @see Open
        static void Close(id_serial serial);

        /// @brief 현재 접속상황
        /// @param serial : 시리얼통신ID
        /// @return 접속여부
        static bool Connected(id_serial serial);

        /// @brief 읽기스트림으로 청크단위 수신하기(등록된 디코더가 없다면 불특정길이)
        /// @param serial : 시리얼통신ID
        /// @param gettype : 디코더에 의해 부여된 패킷타입ID
        /// @return 수신여부
        static bool ReadReady(id_serial serial, sint32* gettype = nullptr);

        /// @brief 현재 읽기스트림의 사이즈얻기
        /// @param serial : 시리얼통신ID
        /// @return 대기중인 읽기스트림의 바이트길이(에러시, BOSS_SERIAL_ERROR계열)
        static sint32 ReadAvailable(id_serial serial);

        /// @brief 읽기스트림에서 파싱포맷에 의해 읽기
        /// @param serial : 시리얼통신ID
        /// @param format : 파싱포맷(ex: "[be] #4 / #2 #1 #4:s2 [le] #4:skip, #1=n #n:str", 빈칸/콤마/슬래시등은 주석)
        /// @param ... : 포맷으로부터 입력받을 가변인수들(ex: &a, &b, &c)
        /// @return 읽은후 남은 읽기스트림의 바이트길이(에러시, BOSS_SERIAL_ERROR계열)
        static sint32 Read(id_serial serial, chars format, ...);

        /// @brief 쓰기스트림에 파싱포맷에 의해 쓰기
        /// @param serial : 시리얼통신ID
        /// @param format : 파싱포맷(ex: "[be] #4=12 #2=0x00 #1 #4:s2 [le] #1:s4 #n:str", 빈칸/콤마/슬래시등은 주석)
        /// @param ... : 포맷으로 출력할 가변인수들(ex: a, b, c)
        /// @return 모여진 쓰기스트림의 바이트길이(에러시, BOSS_SERIAL_ERROR계열)
        static sint32 Write(id_serial serial, chars format, ...);

        /// @brief 쓰기스트림을 송신하기
        /// @param serial : 시리얼통신ID
        /// @param type : 인코더에 전달될 패킷타입ID
        static void WriteFlush(id_serial serial, sint32 type = 0);
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief 카메라지원
    class Camera
    {
    public:
        /// @brief 카메라이름 리스팅
        /// @param spec : json형태로 카메라들의 스펙을 받음(선택사항)
        /// @return 존재하는 모든 카메라이름
        static Strings GetAllNames(String* spec = nullptr);

        /// @brief 카메라ID 할당
        /// @param name : 카메라이름(없으면 첫번째 카메라를 찾음)
        /// @param width : 가로해상도(-1이면 최대사이즈를 찾음)
        /// @param height : 세로해상도(-1이면 최대사이즈를 찾음)
        /// @return 카메라ID(nullptr은 실패)
        /// @see Close
        static id_camera Open(chars name = "", sint32 width = -1, sint32 height = -1);

        /// @brief 카메라ID 반환
        /// @param camera : 카메라ID
        /// @see Open
        static void Close(id_camera camera);

        /// @brief 카메라ID 복제
        /// @param camera : 카메라ID
        /// @return 복제된 카메라ID(nullptr은 실패)
        /// @see Open
        static id_camera Clone(id_camera camera);

        /// @brief 촬영하기
        /// @param camera : 카메라ID
        /// @param preview : 프리뷰식 촬영여부(해상도가 낮지만 촬영속도는 빠름)
        /// @param needstop : 캡쳐직후 촬영을 중지할 여부
        static void Capture(id_camera camera, bool preview, bool needstop);

        /// @brief 마지막 촬영된 텍스처 복제
        /// @param camera : 카메라ID
        /// @param bitmapcache : 비트맵생성용 캐시를 보관해둘지의 여부
        /// @return 복제된 텍스처(nullptr은 실패)
        static id_texture CloneCapturedTexture(id_camera camera, bool bitmapcache);

        /// @brief 마지막 촬영된 이미지 얻기
        /// @param camera : 카메라ID
        /// @param maxwidth : 가로길이 최대값
        /// @param maxheight : 세로길이 최대값
        /// @param rotate : 회전각도
        /// @return 촬영된 이미지(nullptr은 실패)
        static id_image_read LastCapturedImage(id_camera camera, sint32 maxwidth = -1, sint32 maxheight = -1, sint32 rotate = 0);

        /// @brief 마지막 촬영된 비트맵 얻기
        /// @param camera : 카메라ID
        /// @param ori : 90도기준 회전상태와 상하반전여부(디폴트는 상하반전)
        /// @return 촬영된 비트맵(nullptr은 실패)
        static id_bitmap_read LastCapturedBitmap(id_camera camera, orientationtype ori = orientationtype_fliped0);

        /// @brief 마지막 촬영된 이미지크기 얻기
        /// @param camera : 카메라ID
        /// @return 촬영된 이미지크기
        static size64 LastCapturedSize(id_camera camera);

        /// @brief 마지막 촬영된 시간 얻기
        /// @param camera : 카메라ID
        /// @param avgmsec : 평균촬영간격
        /// @return 촬영된 시간
        static uint64 LastCapturedTimeMsec(id_camera camera, sint32* avgmsec = nullptr);

        /// @brief 픽쳐방식으로 얻어온 총 촬영횟수
        /// @param camera : 카메라ID
        /// @return 촬영횟수
        static sint32 TotalPictureShotCount(id_camera camera);

        /// @brief 프리뷰방식으로 얻어온 총 촬영횟수
        /// @param camera : 카메라ID
        /// @return 촬영횟수
        static sint32 TotalPreviewShotCount(id_camera camera);
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief 마이크지원
    class Microphone
    {
    public:
        /// @brief 마이크이름 리스팅
        /// @param spec : json형태로 마이크들의 스펙을 받음(선택사항)
        /// @return 존재하는 모든 마이크이름
        static Strings GetAllNames(String* spec = nullptr);

        /// @brief 마이크ID 할당
        /// @param name : 마이크이름(없으면 첫번째 마이크를 찾음)
        /// @param maxcount : 사운드큐의 최대크기(너무 크게 잡으면 비디오와의 싱크가 맞지 않음)
        /// @return 마이크ID(nullptr은 실패)
        /// @see Close
        static id_microphone Open(chars name = "", sint32 maxcount = 4);

        /// @brief 마이크ID 반환
        /// @param microphone : 마이크ID
        /// @see Open
        static void Close(id_microphone microphone);

        /// @brief BitRate정보 얻기
        /// @param microphone : 마이크ID
        /// @return BitRate정보
        static sint32 GetBitRate(id_microphone microphone);

        /// @brief Channel정보 얻기
        /// @param microphone : 마이크ID
        /// @return Channel정보
        static sint32 GetChannel(id_microphone microphone);

        /// @brief SampleRate정보 얻기
        /// @param microphone : 마이크ID
        /// @return SampleRate정보
        static sint32 GetSampleRate(id_microphone microphone);

        /// @brief 적재된 사운드확인 및 포커싱
        /// @param microphone : 마이크ID
        /// @return 포키싱여부
        static bool TryNextSound(id_microphone microphone);

        /// @brief 포커싱된 사운드데이터 반환
        /// @param microphone : 마이크ID
        /// @param length : 사운드데이터의 크기(바이트단위)
        /// @param timems : 사운드데이터 발생시각
        /// @return 사운드데이터(audio/pcm)
        /// @see TryNextSound
        static bytes GetSoundData(id_microphone microphone, sint32* length = nullptr, uint64* timems = nullptr);
    };
};

}
