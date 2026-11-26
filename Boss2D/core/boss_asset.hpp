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
        /// @brief 웹어셋의 기준 URL설정 및 파일인덱스 초기화(BOSS_NEED_WEBFILES_ASSET이 필요)
        /// @param url : 기준 디렉토리 URL(빈 값이면 WASM은 현재 페이지, 앱목록 미설정 PC는 기존 로컬어셋)
        /// @details 앱목록을 설정한 PC에서 빈 URL은 사이트해제이며 main/만 로컬원본으로 읽음. 사이트전환은 TrySetWebFilesUrl()을 사용
        /// @details WASM은 동기식 XHR을 사용하며, 쓰기캐시와 열린 어셋은 유지됨
        static void SetWebFilesUrl(chars url = nullptr);

        /// @brief ToText()를 제공하는 값객체로 웹어셋 URL설정
        template<typename TYPE>
        static auto SetWebFilesUrl(const TYPE& url) -> decltype(url.ToText(), void())
        {
            SetWebFilesUrl(url.ToText());
        }

        /// @brief 임베디드어셋을 위한 리빌드(BOSS_GENERATE_ASSET 및 BOSS_NEED_EMBEDDED_ASSET이 필요)
        /// @return 리빌드 실시여부
        static bool RebuildForEmbedded();

        /// @brief BOSS_GENERATE_ASSET에서 기준폴더 전체를 비교하여 지정한 파일인덱스 갱신
        /// @param filename : 기준폴더 내 인덱스파일명(하위경로 제외)
        /// @param version : 인덱스의 버전명칭(Boss2D-AssetsIndex-1은 기존 files/format 규격)
        /// @param rootpath : 검색과 인덱스 저장의 기준폴더(nullptr이면 기존 assets폴더)
        /// @return 인덱스를 다시 생성하여 앱을 종료해야 하는지 여부
        static bool RebuildForAssetsIndex(chars filename = "assetsindex.json",
            chars version = "Boss2D-AssetsIndex-1", chars rootpath = nullptr);

        /// @brief 최초 어셋접근 전에 웹어셋의 파일인덱스와 앱목록 규격 설정
        /// @param filename : 기준 URL에 상대적인 인덱스파일명(nullptr이면 assetsindex.json)
        /// @param version : 인덱스의 버전명칭(nullptr이면 Boss2D-AssetsIndex-1)
        /// @param deckname : 앱목록파일명(nullptr이면 기존 웹어셋 방식, 지정하면 PC의 main/ 외 앱리소스는 웹/캐시 전용)
        static void SetWebFilesIndex(chars filename, chars version, chars deckname = nullptr);

        /// @brief PC에서 웹어셋 URL의 인덱스와 앱목록을 검증하고 성공한 경우에만 사이트 전환
        /// @param url : 인덱스와 앱목록이 있는 HTTP/HTTPS 기준 디렉토리 URL
        /// @return 성공시 앱목록 JSON, 실패시 nullptr(반환문자열은 다음 동일함수 호출까지 유효, 해제금지)
        /// @details SetWebFilesIndex()의 deckname과 BOSS_NEED_WEBFILES_ASSET이 필요하며 실패시 기존 사이트를 유지함
        static chars TrySetWebFilesUrl(chars url);

        /// @brief 현재 웹어셋의 기준 URL 얻기
        /// @return 기준 URL(미설정시 빈 문자열, 다음 동일함수 호출까지 유효, 해제금지)
        static chars GetWebFilesUrl();

        /// @brief PC에서 사이트별 파생리소스를 저장할 assets-rem 기준 캐시경로 얻기
        /// @return URL해시/ 경로(사이트 미설정 또는 WASM이면 빈 문자열, 다음 동일함수 호출까지 유효, 해제금지)
        static chars GetWebCachePrefix();

        /// @brief 네이티브 파일 API에서 사용할 어셋의 실제 파일경로 얻기(웹어셋은 필요시 캐시에 다운로드)
        /// @param filename : assets 기준 상대파일명
        /// @return 실제 파일경로(파일을 얻지 못하면 빈 문자열, 다음 동일함수 호출까지 유효, 해제금지)
        static chars GetFilePath(chars filename);

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

        /// @brief 어셋삭제(쓰기전용)
        /// @param filename : 파일명
        /// @param autoremovedir : 파일삭제후 상위폴더가 비어버린 경우 자동으로 폴더 삭제
        /// @return 성공여부
        /// @see OpenForWrite
        static bool RemoveForWrite(chars filename, bool autoremovedir = false);

        /// @brief 어셋개명(쓰기전용)
        /// @param old_filename : 예전 파일명
        /// @param new_filename : 새 파일명
        /// @return 성공여부
        /// @see OpenForWrite
        static bool RenameForWrite(chars old_filename, chars new_filename);

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
        /// @details BOSS_STATIC_ASSET에서는 assetsindex.json의 목록을 메모리에서 열거함
        /// @param assetpath : 어셋패스ID
        /// @param cb : 콜백함수
        /// @param data : 콜백함수에 전달할 데이터
        /// @param originalonly : 원본리소스만 대상으로 할지의 여부
        /// @return 검색된 수량
        static sint32 Search(id_assetpath_read assetpath,
            SearchCB cb = nullptr, payload data = nullptr, bool originalonly = false);
    };
}
