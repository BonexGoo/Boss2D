#include <boss.h>

#if !defined(BOSS_NEED_ADDON_GIT) || (BOSS_NEED_ADDON_GIT != 0 && BOSS_NEED_ADDON_GIT != 1)
    #error BOSS_NEED_ADDON_GIT macro is invalid use
#endif
bool __LINK_ADDON_GIT__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_GIT

#include <iostream>
#include <functional>
#include <cctype>

// 도구준비
#include <addon/boss_fakewin.h>
#include <addon/boss_integration_libgit2-0.22.2.h>

#include <boss.hpp>
#include "boss_addon_git.hpp"

extern "C"
{
    #undef BOSS_LIBGIT2_U_common_h
    #define BOSS_LIBGIT2_U_common_h "common.h"
    #include <addon/libgit2-0.22.2_for_boss/include/git2.h>

    typedef struct {
        git_error* last_error;
        git_error error_t;
        char oid_fmt[GIT_OID_HEXSZ+1];
    } git_global_st;
    git_global_st* git__global_state(void);

    static inline void Git_ErrorCheck(sint32 code)
    {
        String ErrorMessage = "unknown";
        if(git__global_state()->last_error) ErrorMessage = git__global_state()->last_error->message;
        else if(git__global_state()->error_t.message) ErrorMessage = git__global_state()->error_t.message;
        BOSS_ASSERT(String::Format("GIT(%s/%d)", (chars) ErrorMessage, code), 0 <= code);
    }

    static sint32 Git_CredAcquire(git_cred** out, chars url, chars username, uint32 types, payload data)
    {
        return git_cred_userpass_plaintext_new(out, username, ((GitClass*) data)->GetPassword());
    }

    static sint32 Git_Progress_DownloadPack(chars string, sint32 length, payload data)
    {
        ((GitClass*) data)->Progress(BOSS::PT_DownloadPack, String::Format("{\"info\"=\"%.*s\"}", length - 1, string));
        return 0;
    }

    static sint32 Git_Progress_DownloadTip(chars refname, const git_oid* oldid, const git_oid* newid, payload data)
    {
        char NewOID[GIT_OID_HEXSZ + 1];
        git_oid_fmt(NewOID, newid);
        NewOID[GIT_OID_HEXSZ] = '\0';

        if(git_oid_iszero(oldid))
            ((GitClass*) data)->Progress(BOSS::PT_DownloadTip,
                String::Format("{\"type\"=\"new\",\"refname\"=\"%s\",\"newoid\"=\"%.20s\"}", refname, NewOID));
        else
        {
            char OldOID[GIT_OID_HEXSZ + 1];
            git_oid_fmt(OldOID, oldid);
            OldOID[GIT_OID_HEXSZ] = '\0';

            ((GitClass*) data)->Progress(BOSS::PT_DownloadTip,
                String::Format("{\"type\"=\"updated\",\"refname\"=\"%s\",\"oldoid\"=\"%.20s\",\"newoid\"=\"%.20s\"}",
                refname, OldOID, NewOID));
        }
        return 0;
    }

    static void Git_Progress_InflateFile(chars filepath, size_t index, size_t length, payload data)
    {
        ((GitClass*) data)->Progress(BOSS::PT_InflateFile,
            String::Format("{\"filepath\"=\"%s\",\"index\"=\"%d\",\"length\"=\"%d\"}",
            filepath, index, length));
    }
}

#include <platform/boss_platform.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(Git, Create, id_git, chars, chars, chars, chars)
    BOSS_DECLARE_ADDON_FUNCTION(Git, Release, void, id_git)
    BOSS_DECLARE_ADDON_FUNCTION(Git, Update, void, id_git, AddOn::Git::ProgressCB, payload)

    static autorun Bind_AddOn_Git()
    {
        Core_AddOn_Git_Create() = Customized_AddOn_Git_Create;
        Core_AddOn_Git_Release() = Customized_AddOn_Git_Release;
        Core_AddOn_Git_Update() = Customized_AddOn_Git_Update;
        return true;
    }
    static autorun _ = Bind_AddOn_Git();
}

// 구현부
namespace BOSS
{
    sint32 GitClass::s_instance_count = 0;

    GitClass::GitClass()
    {
        m_progress_cb = nullptr;
        m_progress_data = nullptr;
        if(s_instance_count++ == 0)
            git_libgit2_init();
    }

    GitClass::GitClass(const GitClass& rhs)
    {
        operator=(rhs);
    }

    GitClass::~GitClass()
    {
        if(--s_instance_count == 0)
            git_libgit2_shutdown();
    }

    GitClass& GitClass::operator=(const GitClass& rhs)
    {
        m_rootpath = rhs.m_rootpath;
        m_sshname = rhs.m_sshname;
        m_id = rhs.m_id;
        m_password = rhs.m_password;
        m_progress_cb = rhs.m_progress_cb;
        m_progress_data = rhs.m_progress_data;
        return *this;
    }

    id_git Customized_AddOn_Git_Create(chars rootpath, chars sshname, chars id, chars password)
    {
        GitClass* NewGit = (GitClass*) Buffer::Alloc<GitClass>(BOSS_DBG 1);
        NewGit->Init(rootpath, sshname, id, password);
        return (id_git) NewGit;
    }

    void Customized_AddOn_Git_Release(id_git git)
    {
        Buffer::Free((buffer) git);
    }

    void Customized_AddOn_Git_Update(id_git git, AddOn::Git::ProgressCB cb, payload data)
    {
        GitClass* CurGit = (GitClass*) git;
        CurGit->SetProgress(cb, data);

        git_repository* CurRepository = nullptr;
        git_repository_open(&CurRepository, CurGit->GetRootPath());
        String SshUrl = "ssh://";
        SshUrl += CurGit->GetID();
        SshUrl += '@';
        SshUrl += CurGit->GetSshName();

        if(!CurRepository)
        {
            // 복사받아 새로 구성(마스터구성 + 리모트다운로드 + 체크아웃)
            git_clone_options NewCloneOptions = GIT_CLONE_OPTIONS_INIT;
            NewCloneOptions.remote_callbacks.credentials = Git_CredAcquire;
            NewCloneOptions.remote_callbacks.update_tips = &Git_Progress_DownloadTip;
            NewCloneOptions.remote_callbacks.sideband_progress = &Git_Progress_DownloadPack;
            NewCloneOptions.remote_callbacks.payload = git;
            NewCloneOptions.checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE;
            NewCloneOptions.checkout_opts.progress_cb = Git_Progress_InflateFile;
            NewCloneOptions.checkout_opts.progress_payload = git;
            Git_ErrorCheck(git_clone(&CurRepository, SshUrl, CurGit->GetRootPath(), &NewCloneOptions));
        }
        else
        {
            // 업데이트가 필요하면 팩을 얻어오고 업버전
            git_remote* NewRemote = nullptr;
            Git_ErrorCheck(git_remote_create_anonymous(&NewRemote, CurRepository, SshUrl, nullptr));
            git_remote_callbacks NewRemoteCallbacks = GIT_REMOTE_CALLBACKS_INIT;
            NewRemoteCallbacks.credentials = Git_CredAcquire;
            NewRemoteCallbacks.update_tips = &Git_Progress_DownloadTip;
            NewRemoteCallbacks.sideband_progress = &Git_Progress_DownloadPack;
            NewRemoteCallbacks.payload = git;
            Git_ErrorCheck(git_remote_set_callbacks(NewRemote, &NewRemoteCallbacks));
            Git_ErrorCheck(git_remote_connect(NewRemote, GIT_DIRECTION_FETCH));
            Git_ErrorCheck(git_remote_add_fetch(NewRemote, "master:master"));
            Git_ErrorCheck(git_remote_download(NewRemote, nullptr));
            Git_ErrorCheck(git_remote_update_tips(NewRemote, nullptr, nullptr));
            git_remote_disconnect(NewRemote);
            git_remote_free(NewRemote);

            // 지정된 버전으로 복원
            git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
            checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE;
            checkout_opts.progress_cb = Git_Progress_InflateFile;
            checkout_opts.progress_payload = git;
            Git_ErrorCheck(git_checkout_head(CurRepository, &checkout_opts));
        }
        git_repository_free(CurRepository);
    }
}

#endif
