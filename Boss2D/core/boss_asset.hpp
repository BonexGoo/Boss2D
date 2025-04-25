#pragma once
#define BOSS_NOT_INCLUDE_FOR_INTELLIGENCE
#include <boss.hpp>

namespace BOSS
{
    BOSS_DECLARE_ID(id_asset);
    BOSS_DECLARE_ID(id_assetpath);

    /// @brief 어셋지원
    class Asset
    {
    public:
        /// @brief 임베디드어셋을 위한 리빌드(BOSS_NEED_EMBEDDED_ASSET이 필요)
        /// @return 리빌드 실시여부
        static bool RebuildForEmbedded();

        /// @brief 어셋을 버퍼로 반환(읽기전용)
        /// @param filename : 파일명
        /// @param assetpath : 어셋패스로 여러 폴더를 탐색
        /// @return 버퍼
        static buffer ToBuffer(chars filename, id_assetpath_read assetpath = nullptr);

        /// @brief 어셋존재여부 얻기
        /// @param filename : 파일명
        /// @param assetpath : 어셋패스로 여러 폴더를 탐색
        /// @param size : 파일사이즈 얻기
        /// @param ctime : 파일생성시간 얻기
        /// @param atime : 파일접근시간 얻기
        /// @param mtime : 파일수정시간 얻기
        /// @return 존재여부
        static bool Exist(chars filename, id_assetpath_read assetpath = nullptr,
            uint64* size = nullptr, uint64* ctime = nullptr, uint64* atime = nullptr, uint64* mtime = nullptr);

        /// @brief 어셋열기(읽기전용)
        /// @param filename : 파일명
        /// @param assetpath : 어셋패스로 여러 폴더를 탐색
        /// @param originalonly : 원본리소스만 대상으로 할지의 여부
        /// @return 어셋ID
        /// @see Close
        static id_asset_read OpenForRead(chars filename, id_assetpath_read assetpath = nullptr, bool originalonly = false);

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
        /// @return 어셋의 사이즈(byte)
        static sint32 Size(id_asset_read asset);

        /// @brief 어셋읽기
        /// @param asset : 어셋ID
        /// @param data : 저장할 데이터
        /// @param size : 데이터의 길이(byte)
        /// @return 실제로 읽어진 길이(byte)
        /// @see OpenForRead
        static sint32 Read(id_asset_read asset, uint08* data, const sint32 size);

        /// @brief 어셋쓰기
        /// @param asset : 어셋ID
        /// @param data : 불러올 데이터
        /// @param size : 데이터의 길이(byte)
        /// @return 실제로 쓰여진 길이(byte)
        /// @see OpenForWrite
        static sint32 Write(id_asset asset, bytes data, const sint32 size);

        /// @brief 어셋구간통과
        /// @param asset : 어셋ID
        /// @param size : 통과할 길이(byte)
        /// @return 어셋의 포커스(byte)
        /// @see OpenForRead
        static sint32 Skip(id_asset_read asset, const sint32 size);
    };

    /// @brief 어셋패스지원
    class AssetPath
    {
    public:
        typedef void (*SearchCB)(chars filepath, chars filename, payload data);

        /// @brief 어셋패스 생성
        /// @return 어셋패스ID
        /// @see Release
        static id_assetpath Create();

        /// @brief 기존 어셋패스에 루트추가
        /// @param assetpath : 어셋패스ID
        /// @param pathname : 패스명
        static void AddRoot(id_assetpath assetpath);

        /// @brief 기존 어셋패스에 패스추가
        /// @param assetpath : 어셋패스ID
        /// @param pathname : 패스명
        static void AddPath(id_assetpath assetpath, chars pathname);

        /// @brief 어셋패스 해제
        /// @param assetpath : 어셋패스ID
        /// @see Create
        static void Release(id_assetpath assetpath);

        /// @brief 기존 어셋패스에서 파일 검색
        /// @param assetpath : 어셋패스ID
        /// @param cb : 콜백함수
        /// @param data : 콜백함수에 전달할 데이터
        /// @param originalonly : 원본리소스만 대상으로 할지의 여부
        /// @return 검색된 수량
        static sint32 Search(id_assetpath_read assetpath,
            SearchCB cb = nullptr, payload data = nullptr, bool originalonly = false);
    };
}
