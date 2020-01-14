#pragma once
#define BOSS_NOT_INCLUDE_FOR_INTELLIGENCE
#include <boss.hpp>

namespace BOSS
{
    BOSS_DECLARE_ID(id_asset);
    BOSS_DECLARE_ID(id_assetpath);

    /// @brief 어셋함수지원
    class Asset
    {
    public:
        /// @brief 어셋존재여부 얻기
        /// @param filename : 파일명
        /// @param assetpath : 우선탐색용 패스
        /// @return 어셋없음(roottype_null) 또는 루트타입
        /// @see GetPathForExist
        static roottype Exist(chars filename, id_assetpath_read assetpath = nullptr);

        /// @brief Exist결과 얻기
        /// @param type : 루트타입
        /// @param assetpath : 우선탐색용 패스
        /// @return 해당 패스명
        /// @see Exist
        static chars GetPathForExist(roottype type, id_assetpath_read assetpath = nullptr);

        /// @brief 어셋을 버퍼로 반환(읽기전용)
        /// @param filename : 파일명
        /// @param assetpath : 우선탐색용 패스
        /// @return 버퍼
        static buffer ToBuffer(chars filename, id_assetpath_read assetpath = nullptr);

        /// @brief 어셋열기(읽기전용)
        /// @param filename : 파일명
        /// @param assetpath : 우선탐색용 패스
        /// @return 어셋ID
        /// @see Close
        static id_asset_read OpenForRead(chars filename, id_assetpath_read assetpath = nullptr);

        /// @brief 어셋열기(쓰기전용)
        /// @param filename : 파일명
        /// @param autocreatedir : 폴더가 없을 경우 자동으로 생성
        /// @return 어셋ID
        /// @see Close
        static id_asset OpenForWrite(chars filename, bool autocreatedir = false);

        /// @brief 어셋닫기
        /// @param asset : 어셋ID
        /// @see OpenForRead, OpenForWrite
        static void Close(id_asset_read asset);

        /// @brief 어셋사이즈 얻기
        /// @param asset : 어셋ID
        /// @return 어셋사이즈
        static const sint32 Size(id_asset_read asset);

        /// @brief 어셋읽기
        /// @param asset : 어셋ID
        /// @param data : 저장할 데이터
        /// @param size : 데이터의 길이(byte)
        /// @see OpenForRead
        static void Read(id_asset_read asset, uint08* data, const sint32 size);

        /// @brief 어셋쓰기
        /// @param asset : 어셋ID
        /// @param data : 불러올 데이터
        /// @param size : 데이터의 길이(byte)
        /// @see OpenForWrite
        static void Write(id_asset asset, bytes data, const sint32 size);

        /// @brief 어셋구간통과
        /// @param asset : 어셋ID
        /// @param size : 통과할 길이(byte)
        /// @see OpenForRead, OpenForWrite
        static void Skip(id_asset_read asset, const sint32 size);

        /// @brief 캐시의 유효성검사(수정시간 비교방식)
        /// @param filename : 원본파일
        /// @param cachename : 캐시파일
        /// @param assetpath : 우선탐색용 패스
        /// @return 유효여부
        static bool ValidCache(chars filename, chars cachename, id_assetpath_read assetpath = nullptr);

        /// @brief 어셋패스 생성
        /// @param pathname : 패스명
        /// @return 어셋패스ID
        /// @see ReleasePath
        static id_assetpath CreatePath(chars pathname = nullptr);

        /// @brief 기존 어셋패스에 패스추가
        /// @param assetpath : 어셋패스ID
        /// @param pathname : 패스명
        static void AddByPath(id_assetpath assetpath, chars pathname);

        /// @brief 어셋패스 해제
        /// @param assetpath : 어셋패스ID
        /// @see CreatePath
        static void ReleasePath(id_assetpath_read assetpath);
    };
}
