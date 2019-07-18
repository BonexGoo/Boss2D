#include <boss.h>
#if BOSS_NEED_ADDON_GIT

#include "boss_integration_libgit2-0.22.2.h"

#if BOSS_WINDOWS
    #pragma comment(lib, "advapi32.lib")
#endif

#if BOSS_WINDOWS
    #define DOT_GIT ".git"
    #define DOT_GIT_MODULE_A ( \
        tolower(path->ptr[len - 4]) != '.' || \
        tolower(path->ptr[len - 3]) != 'g' || \
        tolower(path->ptr[len - 2]) != 'i' || \
        tolower(path->ptr[len - 1]) != 't')
    #define DOT_GIT_MODULE_B ( \
        next_hfs_char(&path, &len) != '.' || \
        next_hfs_char(&path, &len) != 'g' || \
        next_hfs_char(&path, &len) != 'i' || \
        next_hfs_char(&path, &len) != 't' || \
        next_hfs_char(&path, &len) != 0)
    #define DOT_GIT_MODULE_C ( \
        component[0] == '.' && \
        (component[1] == 'g' || component[1] == 'G') && \
        (component[2] == 'i' || component[2] == 'I') && \
        (component[3] == 't' || component[3] == 'T'))
#else
    #define DOT_GIT "_git"
    #define DOT_GIT_MODULE_A ( \
        tolower(path->ptr[len - 4]) != '_' || \
        tolower(path->ptr[len - 3]) != 'g' || \
        tolower(path->ptr[len - 2]) != 'i' || \
        tolower(path->ptr[len - 1]) != 't')
    #define DOT_GIT_MODULE_B ( \
        next_hfs_char(&path, &len) != '_' || \
        next_hfs_char(&path, &len) != 'g' || \
        next_hfs_char(&path, &len) != 'i' || \
        next_hfs_char(&path, &len) != 't' || \
        next_hfs_char(&path, &len) != 0)
    #define DOT_GIT_MODULE_C ( \
        component[0] == '_' && \
        (component[1] == 'g' || component[1] == 'G') && \
        (component[2] == 'i' || component[2] == 'I') && \
        (component[3] == 't' || component[3] == 'T'))
#endif
#define DOT_GIT_LEN (sizeof(DOT_GIT) - 1)

#include BOSS_LIBGIT2_U_git2__attr_h
#include BOSS_LIBGIT2_U_git2__blob_h
#include BOSS_LIBGIT2_U_git2__index_h
#include BOSS_LIBGIT2_U_git2__sys__filter_h
#include BOSS_LIBGIT2_U_common_h
#include <addon/libgit2-0.22.2_for_boss/src/errors.c>
#include <addon/libgit2-0.22.2_for_boss/src/win32/error.c>

#define giterr_set(...) boss_giterr_set(BOSS_DBG __VA_ARGS__)
void boss_giterr_set(BOSS_DBG_PRM int error_class, const char *string, ...)
{
    char Err[1024];
    va_list Args;
    va_start(Args, string);
    boss_vsnprintf(Err, 1024, string, Args);
    va_end(Args);

    BOSS_ASSERT_PRM(Err, false);
}

#define printf(...) boss_printf(BOSS_DBG __VA_ARGS__)
void boss_printf(BOSS_DBG_PRM const char *string, ...)
{
    char Err[1024];
    va_list Args;
    va_start(Args, string);
    boss_vsnprintf(Err, 1024, string, Args);
    va_end(Args);

    BOSS_TRACE("boss_printf(%s) - %s, %d, %s", Err, BOSS_DBG_FILE, BOSS_DBG_LINE, BOSS_DBG_FUNC);
}

#if !BOSS_WINDOWS
    void qsort_s(
        void *els, size_t nel, size_t elsize, git__sort_r_cmp cmp, void *payload)
    {
        BOSS_ASSERT("qsort_s가 준비되지 않았습니다", false);
    }
#endif

#define NO_DUMMY_DECL
#include <addon/libgit2-0.22.2_for_boss/src/strmap.h>
GIT__USE_STRMAP;

#undef GIT__USE_STRMAP
#define GIT__USE_STRMAP
#include <addon/libgit2-0.22.2_for_boss/src/oidmap.h>
GIT__USE_OIDMAP;

#undef GIT__USE_OIDMAP
#define GIT__USE_OIDMAP
#include <addon/libgit2-0.22.2_for_boss/src/annotated_commit.c>
#include <addon/libgit2-0.22.2_for_boss/src/attr.c>
#include <addon/libgit2-0.22.2_for_boss/src/attr_file.c>
#include <addon/libgit2-0.22.2_for_boss/src/attrcache.c>
#include <addon/libgit2-0.22.2_for_boss/src/blame.c>
#include <addon/libgit2-0.22.2_for_boss/src/blame_git.c>
#include <addon/libgit2-0.22.2_for_boss/src/blob.c>
#include <addon/libgit2-0.22.2_for_boss/src/branch.c>
#include <addon/libgit2-0.22.2_for_boss/src/buf_text.c>
#include <addon/libgit2-0.22.2_for_boss/src/buffer.c>
#include <addon/libgit2-0.22.2_for_boss/src/cache.c>
#include <addon/libgit2-0.22.2_for_boss/src/checkout.c>
#include <addon/libgit2-0.22.2_for_boss/src/cherrypick.c>
#include <addon/libgit2-0.22.2_for_boss/src/clone.c>
#include <addon/libgit2-0.22.2_for_boss/src/commit.c>
#include <addon/libgit2-0.22.2_for_boss/src/commit_list.c>
#include <addon/libgit2-0.22.2_for_boss/src/config.c>
#include <addon/libgit2-0.22.2_for_boss/src/config_cache.c>
#include <addon/libgit2-0.22.2_for_boss/src/config_file.c>
#include <addon/libgit2-0.22.2_for_boss/src/crlf.c>
#include <addon/libgit2-0.22.2_for_boss/src/date.c>
#include <addon/libgit2-0.22.2_for_boss/src/delta.c>
#include <addon/libgit2-0.22.2_for_boss/src/delta-apply.c>
#include <addon/libgit2-0.22.2_for_boss/src/describe.c>
#include <addon/libgit2-0.22.2_for_boss/src/diff.c>
#include <addon/libgit2-0.22.2_for_boss/src/diff_driver.c>
#include <addon/libgit2-0.22.2_for_boss/src/diff_file.c>
#include <addon/libgit2-0.22.2_for_boss/src/diff_patch.c>
#include <addon/libgit2-0.22.2_for_boss/src/diff_print.c>
#include <addon/libgit2-0.22.2_for_boss/src/diff_stats.c>
#include <addon/libgit2-0.22.2_for_boss/src/diff_tform.c>
#include <addon/libgit2-0.22.2_for_boss/src/diff_xdiff.c>
#include <addon/libgit2-0.22.2_for_boss/src/fetch.c>
#include <addon/libgit2-0.22.2_for_boss/src/fetchhead.c>
#include <addon/libgit2-0.22.2_for_boss/src/filebuf.c>
#include <addon/libgit2-0.22.2_for_boss/src/fileops.c>
#include <addon/libgit2-0.22.2_for_boss/src/filter.c>
#include <addon/libgit2-0.22.2_for_boss/src/fnmatch.c>
#include <addon/libgit2-0.22.2_for_boss/src/global.c>
#include <addon/libgit2-0.22.2_for_boss/src/graph.c>
#include <addon/libgit2-0.22.2_for_boss/src/hash.c>
#include <addon/libgit2-0.22.2_for_boss/src/hashsig.c>
#include <addon/libgit2-0.22.2_for_boss/src/ident.c>
#include <addon/libgit2-0.22.2_for_boss/src/ignore.c>
#include <addon/libgit2-0.22.2_for_boss/src/index.c>
#include <addon/libgit2-0.22.2_for_boss/src/indexer.c>
#include <addon/libgit2-0.22.2_for_boss/src/iterator.c>
#include <addon/libgit2-0.22.2_for_boss/src/merge.c>
#include <addon/libgit2-0.22.2_for_boss/src/merge_file.c>
#include <addon/libgit2-0.22.2_for_boss/src/message.c>
#include <addon/libgit2-0.22.2_for_boss/src/mwindow.c>
#include <addon/libgit2-0.22.2_for_boss/src/netops.c>
#include <addon/libgit2-0.22.2_for_boss/src/notes.c>
#include <addon/libgit2-0.22.2_for_boss/src/object.c>
#include <addon/libgit2-0.22.2_for_boss/src/object_api.c>
#include <addon/libgit2-0.22.2_for_boss/src/odb.c>
#include <addon/libgit2-0.22.2_for_boss/src/odb_loose.c>
#include <addon/libgit2-0.22.2_for_boss/src/odb_mempack.c>
#include <addon/libgit2-0.22.2_for_boss/src/odb_pack.c>
#include <addon/libgit2-0.22.2_for_boss/src/oid.c>
#include <addon/libgit2-0.22.2_for_boss/src/oidarray.c>
#include <addon/libgit2-0.22.2_for_boss/src/openssl_stream.c>
#include <addon/libgit2-0.22.2_for_boss/src/pack.c>
#include <addon/libgit2-0.22.2_for_boss/src/pack-objects.c>
#include <addon/libgit2-0.22.2_for_boss/src/path.c>
#include <addon/libgit2-0.22.2_for_boss/src/pathspec.c>
#include <addon/libgit2-0.22.2_for_boss/src/pool.c>
#include <addon/libgit2-0.22.2_for_boss/src/posix.c>
#include <addon/libgit2-0.22.2_for_boss/src/pqueue.c>
#include <addon/libgit2-0.22.2_for_boss/src/push.c>
#include <addon/libgit2-0.22.2_for_boss/src/rebase.c>
#include <addon/libgit2-0.22.2_for_boss/src/refdb.c>
#include <addon/libgit2-0.22.2_for_boss/src/refdb_fs.c>
#include <addon/libgit2-0.22.2_for_boss/src/reflog.c>
#include <addon/libgit2-0.22.2_for_boss/src/refs.c>
#include <addon/libgit2-0.22.2_for_boss/src/refspec.c>
#include <addon/libgit2-0.22.2_for_boss/src/remote.c>
#include <addon/libgit2-0.22.2_for_boss/src/repository.c>
#include <addon/libgit2-0.22.2_for_boss/src/reset.c>
#include <addon/libgit2-0.22.2_for_boss/src/revert.c>
#include <addon/libgit2-0.22.2_for_boss/src/revparse.c>
#include <addon/libgit2-0.22.2_for_boss/src/revwalk.c>
#include <addon/libgit2-0.22.2_for_boss/src/settings.c>
#include <addon/libgit2-0.22.2_for_boss/src/sha1_lookup.c>
#include <addon/libgit2-0.22.2_for_boss/src/signature.c>
#include <addon/libgit2-0.22.2_for_boss/src/socket_stream.c>
#include <addon/libgit2-0.22.2_for_boss/src/sortedcache.c>
#include <addon/libgit2-0.22.2_for_boss/src/stash.c>
#include <addon/libgit2-0.22.2_for_boss/src/status.c>
#include <addon/libgit2-0.22.2_for_boss/src/strmap.c>
#include <addon/libgit2-0.22.2_for_boss/src/submodule.c>
#include <addon/libgit2-0.22.2_for_boss/src/sysdir.c>
#include <addon/libgit2-0.22.2_for_boss/src/tag.c>
#include <addon/libgit2-0.22.2_for_boss/src/thread-utils.c>
#include <addon/libgit2-0.22.2_for_boss/src/trace.c>
#include <addon/libgit2-0.22.2_for_boss/src/transaction.c>
#include <addon/libgit2-0.22.2_for_boss/src/transport.c>
#include <addon/libgit2-0.22.2_for_boss/src/tree.c>
#include <addon/libgit2-0.22.2_for_boss/src/tree-cache.c>
#include <addon/libgit2-0.22.2_for_boss/src/tsort.c>
#include <addon/libgit2-0.22.2_for_boss/src/util.c>
#include <addon/libgit2-0.22.2_for_boss/src/vector.c>
#include <addon/libgit2-0.22.2_for_boss/src/zstream.c>

#define BOSS_LIBGIT2_U_buffer_h                       "addon/libgit2-0.22.2_for_boss/src/buffer.h"
#define BOSS_LIBGIT2_U_netops_h                       "addon/libgit2-0.22.2_for_boss/src/netops.h"
#define BOSS_LIBGIT2_U_path_h                         "addon/libgit2-0.22.2_for_boss/src/path.h"
#define BOSS_LIBGIT2_U_stream_h                       "addon/libgit2-0.22.2_for_boss/src/stream.h"
#define BOSS_LIBGIT2_U_socket_stream_h                "addon/libgit2-0.22.2_for_boss/src/socket_stream.h"
#define BOSS_LIBGIT2_U_openssl_stream_h               "addon/libgit2-0.22.2_for_boss/src/openssl_stream.h"
#define BOSS_LIBGIT2_U_global_h                       "addon/libgit2-0.22.2_for_boss/src/global.h"
#define BOSS_LIBGIT2_U_refs_h                         "addon/libgit2-0.22.2_for_boss/src/refs.h"
#define BOSS_LIBGIT2_U_posix_h                        "addon/libgit2-0.22.2_for_boss/src/posix.h"
#define BOSS_LIBGIT2_U_repository_h                   "addon/libgit2-0.22.2_for_boss/src/repository.h"
#define BOSS_LIBGIT2_U_odb_h                          "addon/libgit2-0.22.2_for_boss/src/odb.h"
#define BOSS_LIBGIT2_U_push_h                         "addon/libgit2-0.22.2_for_boss/src/push.h"
#define BOSS_LIBGIT2_U_remote_h                       "addon/libgit2-0.22.2_for_boss/src/remote.h"
#define BOSS_LIBGIT2_U_refspec_h                      "addon/libgit2-0.22.2_for_boss/src/refspec.h"
#define BOSS_LIBGIT2_U_vector_h                       "addon/libgit2-0.22.2_for_boss/src/vector.h"
#define BOSS_LIBGIT2_U_util_h                         "addon/libgit2-0.22.2_for_boss/src/util.h"
#define BOSS_LIBGIT2_U_map_h                          "addon/libgit2-0.22.2_for_boss/src/map.h"
#define BOSS_LIBGIT2_U_pack_M_objects_h               "addon/libgit2-0.22.2_for_boss/src/pack-objects.h"

#include <addon/libgit2-0.22.2_for_boss/src/hash/hash_generic.c>
#include <addon/libgit2-0.22.2_for_boss/src/transports/auth.c>
#include <addon/libgit2-0.22.2_for_boss/src/transports/auth_negotiate.c>
#include <addon/libgit2-0.22.2_for_boss/src/transports/cred.c>
#include <addon/libgit2-0.22.2_for_boss/src/transports/cred_helpers.c>
#include <addon/libgit2-0.22.2_for_boss/src/transports/git.c>
#include <addon/libgit2-0.22.2_for_boss/src/transports/http.c>
#include <addon/libgit2-0.22.2_for_boss/src/transports/local.c>
#include <addon/libgit2-0.22.2_for_boss/src/transports/smart.c>
#include <addon/libgit2-0.22.2_for_boss/src/transports/smart_pkt.c>
#include <addon/libgit2-0.22.2_for_boss/src/transports/smart_protocol.c>
#include <addon/libgit2-0.22.2_for_boss/src/transports/ssh.c>
#include <addon/libgit2-0.22.2_for_boss/src/win32/dir.c>
#include <addon/libgit2-0.22.2_for_boss/src/win32/findfile.c>
#include <addon/libgit2-0.22.2_for_boss/src/win32/map.c>
#include <addon/libgit2-0.22.2_for_boss/src/win32/path_w32.c>
#include <addon/libgit2-0.22.2_for_boss/src/win32/posix_w32.c>
#if BOSS_WINDOWS
    #include <addon/libgit2-0.22.2_for_boss/src/win32/pthread.c>
#endif
#include <addon/libgit2-0.22.2_for_boss/src/win32/utf-conv.c>
#include <addon/libgit2-0.22.2_for_boss/src/win32/w32_util.c>
#include <addon/libgit2-0.22.2_for_boss/src/xdiff/xdiffi.c>
#include <addon/libgit2-0.22.2_for_boss/src/xdiff/xemit.c>
#include <addon/libgit2-0.22.2_for_boss/src/xdiff/xhistogram.c>
#include <addon/libgit2-0.22.2_for_boss/src/xdiff/xmerge.c>
#include <addon/libgit2-0.22.2_for_boss/src/xdiff/xpatience.c>
#include <addon/libgit2-0.22.2_for_boss/src/xdiff/xprepare.c>
#include <addon/libgit2-0.22.2_for_boss/src/xdiff/xutils.c>

#include <addon/libgit2-0.22.2_for_boss/deps/http-parser/http_parser.c>
#include <addon/libgit2-0.22.2_for_boss/deps/regex/regex.c>
#include <addon/libgit2-0.22.2_for_boss/deps/zlib/adler32.c>
#include <addon/libgit2-0.22.2_for_boss/deps/zlib/crc32.c>
#include <addon/libgit2-0.22.2_for_boss/deps/zlib/deflate.c>
#include <addon/libgit2-0.22.2_for_boss/deps/zlib/infback.c>
#include <addon/libgit2-0.22.2_for_boss/deps/zlib/inffast.c>
#include <addon/libgit2-0.22.2_for_boss/deps/zlib/inflate.c>
#include <addon/libgit2-0.22.2_for_boss/deps/zlib/inftrees.c>
#include <addon/libgit2-0.22.2_for_boss/deps/zlib/trees.c>
#include <addon/libgit2-0.22.2_for_boss/deps/zlib/zutil.c>

#endif
