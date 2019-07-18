#pragma once
#include <boss.hpp>

namespace BOSS
{
    class GitClass
    {
    public:
        GitClass();
        GitClass(const GitClass& rhs);
        ~GitClass();
        GitClass& operator=(const GitClass& rhs);

    public:
        inline void Init(chars rootpath, chars sshname, chars id, chars password)
        {
            m_rootpath = rootpath;
            m_sshname = sshname;
            m_id = id;
            m_password = password;
        }
        inline void SetProgress(AddOn::Git::ProgressCB cb, payload data)
        {
            m_progress_cb = cb;
            m_progress_data = data;
        }
        inline chars GetRootPath() {return m_rootpath;}
        inline chars GetSshName() {return m_sshname;}
        inline chars GetID() {return m_id;}
        inline chars GetPassword() {return m_password;}
        inline void Progress(GitProgressType type, chars comment)
        {m_progress_cb(m_progress_data, type, comment);}

    private:
        static sint32 s_instance_count;
        String m_rootpath;
        String m_sshname;
        String m_id;
        String m_password;
        AddOn::Git::ProgressCB m_progress_cb;
        payload m_progress_data;
    };
}
