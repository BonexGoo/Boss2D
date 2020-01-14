#include <boss.hpp>
#include "boss_asyncfilemanager.hpp"

#include <platform/boss_platform.hpp>

class SaveQuery
{
public:
    SaveQuery() {}
    ~SaveQuery()
    {
        AsyncFileManager::ClearChunks(m_chunks);
    }
    SaveQuery& operator=(const SaveQuery& rhs)
    {
        BOSS_ASSERT("해당 객체는 복사할 수 없습니다", false);
        return *this;
    }

public:
    String m_path;
    buffers m_chunks;
};

class SaveListing
{
public:
    SaveListing() : m_manager(nullptr) {}
    ~SaveListing() {}
    SaveListing& operator=(const SaveListing& rhs)
    {
        BOSS_ASSERT("해당 객체는 복사할 수 없습니다", false);
        return *this;
    }

public:
    const AsyncFileManager* m_manager;
    Map<SaveQuery*> m_queries;
};

namespace BOSS
{
    AsyncFileManager::AsyncFileManager()
    {
        m_tasking = Tasking::Create(OnTask);
        BOSS_COMMON(m_tasking, common_buffer)
        {
            Buffer::Free(common_buffer);
            common_buffer = Buffer::Alloc<SaveListing>(BOSS_DBG 1);
            ((SaveListing*) common_buffer)->m_manager = this;
        }
    }

    AsyncFileManager::~AsyncFileManager()
    {
        Tasking::Release(m_tasking, true);
    }

    bool AsyncFileManager::Exist(chars path) const
    {
        bool Result = false;
        BOSS_COMMON_IF(m_tasking, common_buffer)
        {
            SaveListing* List = (SaveListing*) common_buffer;
            if(SaveQuery** OldQuery = List->m_queries.Access(path))
            {
                Result = true;
                BOSS_TRACE("[Exist] 요청목록에 파일이 있음 : %s", path);
            }
        }

        if(!Result)
        {
            Result = Platform::File::Exist(path);
            BOSS_TRACE("[Exist] 실제폴더에 파일이 %s : %s", (Result)? "있음" : "없음", path);
        }
        return Result;
    }

    void AsyncFileManager::SaveChunks(chars path, buffers chunks)
    {
        SaveQuery** OldQuery = nullptr;
        BOSS_COMMON_IF(m_tasking, common_buffer)
        {
            SaveListing* List = (SaveListing*) common_buffer;
            if(OldQuery = List->m_queries.Access(path))
            {
                ClearChunks((*OldQuery)->m_chunks);
                (*OldQuery)->m_chunks = chunks;
                BOSS_TRACE("[SaveChunks] 요청목록을 재갱신 : %s", path);
            }
        }

        if(!OldQuery)
        {
            SaveQuery* NewQuery = (SaveQuery*) Buffer::Alloc<SaveQuery>(BOSS_DBG 1);
            NewQuery->m_path = path;
            NewQuery->m_chunks = chunks;

            BOSS_COMMON_IF(m_tasking, common_buffer)
            {
                SaveListing* List = (SaveListing*) common_buffer;
                List->m_queries(path) = NewQuery;
                Tasking::SendQuery(m_tasking, (buffer) NewQuery);
                BOSS_TRACE("[SaveChunks] 요청목록에 신규요청 : %s", path);
            }
        }
    }

    buffers AsyncFileManager::LoadChunks(chars path, sint32 offset, sint32 length) const
    {
        BOSS_ASSERT("length값은 0보다 커야 합니다", 0 < length);
        buffers Result;
        BOSS_COMMON_IF(m_tasking, common_buffer)
        {
            SaveListing* List = (SaveListing*) common_buffer;
            if(SaveQuery** OldQuery = List->m_queries.Access(path))
            {
                const buffers& OldChunks = (*OldQuery)->m_chunks;
                for(sint32 i = offset, iend = i + length; i < iend; ++i)
                    Result.AtAdding() = Buffer::Clone(BOSS_DBG OldChunks[i]);
                BOSS_TRACE("[LoadChunks] 요청목록에서 파일을 읽음 : %s", path);
            }
        }

        if(!Result.Count())
        {
            id_file_read OldFile = Platform::File::OpenForRead(path);
            if(OldFile)
            {
                Result = OnReadFile(OldFile, offset, length);
                Platform::File::Close(OldFile);
            }
            BOSS_TRACE("[LoadChunks] 실제폴더에서 파일을 %s : %s", (OldFile)? "읽음" : "찾지못함", path);
        }
        return Result;
    }

    void AsyncFileManager::ClearChunks(buffers& chunks)
    {
        for(sint32 i = 0, iend = chunks.Count(); i < iend; ++i)
            Buffer::Free(chunks.At(i));
        chunks.Clear();
    }

    void AsyncFileManager::OnWriteFile(id_file file, const buffers chunks) const
    {
        for(sint32 i = 0, iend = chunks.Count(); i < iend; ++i)
        {
            const buffer OldBuffer = chunks[i];
            const sint32 CopyCount = Buffer::CountOf(OldBuffer);
            Platform::File::Write(file, (bytes) &CopyCount, sizeof(CopyCount));
            Platform::File::Write(file, (bytes) OldBuffer, Buffer::SizeOf(OldBuffer) * CopyCount);
        }
    }

    buffers AsyncFileManager::OnReadFile(id_file_read file, sint32 offset, sint32 length) const
    {
        buffers Result;
        for(sint32 i = offset, iend = i + length; i < iend; ++i)
        {
            sint32 CopyCount = 0;
            Platform::File::Read(file, (uint08*) &CopyCount, sizeof(CopyCount));
            buffer& NewBuffer = Result.AtAdding();
            NewBuffer = Buffer::Realloc(BOSS_DBG NewBuffer, CopyCount);
            Platform::File::Read(file, (uint08*) NewBuffer, Buffer::SizeOf(NewBuffer) * CopyCount);
        }
        return Result;
    }

    sint32 AsyncFileManager::OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
    {
        if(SaveQuery* CurQuery = (SaveQuery*) query.Dequeue())
        {
            const AsyncFileManager* Manager = nullptr;
            BOSS_COMMON_TASK_IF(common, common_buffer)
                Manager = ((SaveListing*) common_buffer)->m_manager;

            if(Manager)
            {
                BOSS_TRACE("[OnTask] 해당 요청을 실제폴더에 저장후 요청목록에서 제거 : %s", (chars) CurQuery->m_path);
                id_file NewFile = Platform::File::OpenForWrite(CurQuery->m_path, true);
                Manager->OnWriteFile(NewFile, CurQuery->m_chunks);
                Platform::File::Close(NewFile);

                BOSS_COMMON_TASK_IF(common, common_buffer)
                    (((SaveListing*) common_buffer))->m_queries.Remove(CurQuery->m_path);
                Buffer::Free((buffer) CurQuery);
                return 0;
            }
            else BOSS_ASSERT("AsyncFileManager의 인스턴스가 없습니다", false);
        }
        return 100;
    }
}
