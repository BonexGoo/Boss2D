#pragma once
#include <boss_queue.hpp>

#define BOSS_COMMON(TASKING, BUF) \
    for(buffer _ = (buffer) 1, BUF = BOSS::Tasking::LockCommon(TASKING); _; _ = BOSS::Tasking::UnlockCommon(TASKING, BUF))
#define BOSS_COMMON_IF(TASKING, BUF) \
    for(buffer BUF = BOSS::Tasking::LockCommon(TASKING, true); BUF; BUF = BOSS::Tasking::UnlockCommon(TASKING, BUF))
#define BOSS_COMMON_TASK(COMMON, BUF) \
    for(buffer _ = (buffer) 1, BUF = BOSS::Tasking::LockCommonForTask(COMMON); _; _ = BOSS::Tasking::UnlockCommonForTask(COMMON, BUF))
#define BOSS_COMMON_TASK_IF(COMMON, BUF) \
    for(buffer BUF = BOSS::Tasking::LockCommonForTask(COMMON, true); BUF; BUF = BOSS::Tasking::UnlockCommonForTask(COMMON, BUF))

namespace BOSS
{
    BOSS_DECLARE_ID(id_tasking);
    BOSS_DECLARE_ID(id_common);

    /// @brief 태스킹함수지원
    class Tasking
    {
    public:
        typedef sint32 (*TaskCB)(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common);

    public:
        /// @brief 태스킹 생성
        /// @param cb : 태스크를 진행할 콜백(-1을 리턴하면 태스킹종료)
        /// @param self : 콜백에서 사용될 인스턴스
        /// @return 생성된 태스킹ID
        /// @see Release
        static id_tasking Create(TaskCB cb, buffer self = nullptr);

        /// @brief 태스킹 해제
        /// @param tasking : 태스킹ID
        /// @param doWait : 해당 스레드의 소멸을 기다릴지 여부
        /// @see Create
        static void Release(id_tasking tasking, bool doWait = false);

        /// @brief 태스킹의 해제여부(태스크전용)
        /// @param common : 커먼ID
        /// @return 해제여부
        static bool IsReleased(id_common common);

        /// @brief 태스킹 정지
        /// @param tasking : 태스킹ID
        /// @see Resume
        static void Pause(id_tasking tasking);

        /// @brief 태스킹 재개
        /// @param tasking : 태스킹ID
        /// @see Pause
        static void Resume(id_tasking tasking);

        /// @brief 해당 태스킹의 생존여부
        /// @param tasking : 태스킹ID
        /// @return 생존여부
        static bool IsAlive(id_tasking tasking);

        /// @brief 모든 태스킹에서의 생존수량
        /// @return 생존수량
        static sint32 GetAliveCount();

        /// @brief 해당 태스킹에게 질의송신
        /// @param tasking : 태스킹ID
        /// @param query : 송신할 버퍼
        /// @see GetAnswer
        static void SendQuery(id_tasking tasking, buffer query);

        /// @brief 해당 태스킹으로부터 송신한 처리되지 않은 버퍼수량
        /// @param tasking : 태스킹ID
        /// @return 처리되지 않은 버퍼수량
        /// @see SendQuery
        static sint32 GetQueryCount(id_tasking tasking);

        /// @brief 해당 태스킹으로부터 응답수신
        /// @param tasking : 태스킹ID
        /// @return 수신된 버퍼(없으면 nullptr)
        /// @see SendQuery, GetAnswerCount, KeepAnswer
        static buffer GetAnswer(id_tasking tasking);

        /// @brief 해당 태스킹으로부터 수신된 버퍼수량
        /// @param tasking : 태스킹ID
        /// @return 버퍼수량
        /// @see GetAnswer
        static sint32 GetAnswerCount(id_tasking tasking);

        /// @brief 수신한 응답을 처리하지 않고 다시 보관
        /// @param tasking : 태스킹ID
        /// @param answer : 수신된 버퍼
        /// @see GetAnswer
        static void KeepAnswer(id_tasking tasking, buffer answer);

        /// @brief 해당 태스킹으로부터 공통버퍼 얻기
        /// @param tasking : 태스킹ID
        /// @param autounlock : 공통버퍼가 nullptr일 경우 자동 Unlock처리
        /// @return 공통버퍼
        /// @see UnlockCommon
        static buffer LockCommon(id_tasking tasking, bool autounlock = false);

        /// @brief 해당 태스킹에 공통버퍼 반환
        /// @param tasking : 태스킹ID
        /// @param buf : 공통버퍼(수정가능/재할당가능)
        /// @return 스택형 루프처리(무조건 nullptr)
        /// @see LockCommon
        static nullbuffer UnlockCommon(id_tasking tasking, buffer buf);

        /// @brief 해당 커먼으로부터 공통버퍼 얻기(태스크전용)
        /// @param common : 커먼ID
        /// @param autounlock : 공통버퍼가 nullptr일 경우 자동 Unlock처리
        /// @return 공통버퍼
        /// @see UnlockCommonForTask
        static buffer LockCommonForTask(id_common common, bool autounlock = false);

        /// @brief 해당 커먼에 공통버퍼 반환(태스크전용)
        /// @param common : 커먼ID
        /// @param buf : 공통버퍼(수정가능/재할당가능)
        /// @return 스택형 루프처리(무조건 nullptr)
        /// @see LockCommonForTask
        static nullbuffer UnlockCommonForTask(id_common common, buffer buf);
    };
}
