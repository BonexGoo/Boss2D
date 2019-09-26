#pragma once
#include <boss.hpp>

namespace BOSS
{
    /// @brief 매니저-비동기파일
    class AsyncFileManager
    {
    public:
        AsyncFileManager();
        virtual ~AsyncFileManager();
        AsyncFileManager& operator=(const AsyncFileManager&)
        {
            BOSS_ASSERT("해당 객체는 복사할 수 없습니다", false);
            return *this;
        }

    public:
        bool Exist(chars path) const;
        void SaveChunks(chars path, buffers chunks);
        buffers LoadChunks(chars path, sint32 offset, sint32 length) const;
        static void ClearChunks(buffers& chunks);

    protected:
        virtual void OnWriteFile(id_file file, const buffers chunks) const;
        virtual buffers OnReadFile(id_file_read file, sint32 offset, sint32 length) const;

    private:
        id_tasking m_tasking;
        static sint32 OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common);
    };
}
