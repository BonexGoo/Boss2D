#include <boss.hpp>
#include "boss_zen.hpp"

namespace BOSS
{
    const Zen& Zen::operator[](sint32 index) const
    {
        if((0 <= index) & (index < m_iLength))
        {
            BOSS_ASSERT("데이터에 심각한 오류가 있습니다", m_iIndexer != jumper_null);
            const jumper* CurIndexer = (const jumper*) &JumperToPtr(m_iIndexer)[m_jumpbase];

            BOSS_ASSERT("데이터에 심각한 오류가 있습니다", CurIndexer[index] != jumper_null);
            return *((Zen*) JumperToPtr(CurIndexer[index]));
        }
        return NullChild();
    }

    struct NamableIndexer
    {
        jumper Finder;
        sint32 NameLength;
    };

    struct NamableFinder
    {
        jumper Parent;
        jumper ZenData;
        char Word;
        char WordExtend;
        uint08 ChildFirst;
        uint08 ChildLast;
        jumper16 Childs[256];
    };

    const Zen& Zen::operator()(sint32 index, chararray* getname) const
    {
        if((0 <= index) & (index < m_nLength))
        {
            BOSS_ASSERT("데이터에 심각한 오류가 있습니다", m_nIndexer != jumper_null);
            const NamableIndexer& CurIndexer = ((const NamableIndexer*) &JumperToPtr(m_nIndexer)[m_jumpbase])[index];

            BOSS_ASSERT("데이터에 심각한 오류가 있습니다", CurIndexer.Finder != jumper_null);
            const NamableFinder* CurFinder = (const NamableFinder*) JumperToPtr(CurIndexer.Finder);

            // 경로수집
            if(getname)
            {
                chararray Collector;
                char* CurWord = &Collector.AtWherever(CurIndexer.NameLength);
                *CurWord = '\0';

                const NamableFinder* FinderLoop = CurFinder;
                while(FinderLoop->Parent != jumper_null)
                {
                    *(--CurWord) = FinderLoop->Word;
                    FinderLoop = (const NamableFinder*) JumperToPtr(FinderLoop->Parent);
                }

                BOSS_ASSERT("데이터에 심각한 오류가 있습니다", CurWord == &Collector.At(0));
                *getname = Collector;
            }

            BOSS_ASSERT("데이터에 심각한 오류가 있습니다", CurFinder->ZenData != jumper_null);
            return *((Zen*) JumperToPtr(CurFinder->ZenData));
        }
        return NullChild();
    }

    const Zen& Zen::operator()(chars name) const
    {
        if(!name | (m_nFinder == jumper_null))
            return NullChild();

        const NamableFinder* CurFinder = (const NamableFinder*) &JumperToPtr(m_nFinder)[m_jumpbase];
        while(*name)
        {
            const sint32 CurChild = *name & 0xFF;
            const jumper16& CurChildFinder = CurFinder->Childs[CurChild - CurFinder->ChildFirst];
            if((CurChild < CurFinder->ChildFirst) | (CurFinder->ChildLast < CurChild) | (CurChildFinder == jumper16_null))
                return NullChild();

            CurFinder = (const NamableFinder*) Jumper16ToPtr(CurChildFinder);
            BOSS_ASSERT("데이터에 심각한 오류가 있습니다", CurFinder->Word == *name);
            name++;
        }

        if(CurFinder->ZenData == jumper_null)
            return NullChild();
        return *((Zen*) JumperToPtr(CurFinder->ZenData));
    }

    Zen::Zen() : m_source(sint32s::Null())
    {
        m_jumpbase = 0;
        m_string = jumper_null;
        m_int = 0;
        m_float = 0.0f;
        m_iLength = 0;
        m_iIndexer = jumper_null;
        m_nLength = 0;
        m_nIndexer = jumper_null;
        m_nFinder = jumper_null;
    }

    Zen::Zen(const Zen& rhs) : m_source(sint32s::Null())
    {
        operator=(rhs);
    }

    Zen::Zen(const sint32s& source, sint32 offset) : m_source(source)
    {
        const sint32* Data = &m_source[offset + (sizeof(sint32s) + sizeof(sblock)) / sizeof(sint32)];
        m_jumpbase = JumperPos(Data[0]) - JumperPos(m_string);
        Memory::Copy(&m_string, &Data[0], sizeof(Zen) - sizeof(sint32s) - sizeof(sblock));
    }

    Zen::~Zen()
    {
    }

    Zen& Zen::operator=(const Zen& rhs)
    {
        // rhs가 중간노드인 경우 rhs.m_source의 share값이 nullptr임
        if((id_share) rhs.m_source) m_source = rhs.m_source;
        m_jumpbase = rhs.m_jumpbase + (JumperPos(rhs.m_string) - JumperPos(m_string));
        Memory::Copy(&m_string, &rhs.m_string, sizeof(Zen) - sizeof(sint32s) - sizeof(sblock));
        return *this;
    }

    Zen Zen::Load(const sint32s& source, bool* getSuccess)
    {
        const String& Header = BuildHeader();
        const sint32 HeaderSize = Header.Length();
        const sint32 HeaderCount = (HeaderSize + sizeof(sint32)) / sizeof(sint32);

        const bool Success = !String::Compare((chars) Header, (chars) &source[0]);
        if(getSuccess) *getSuccess = Success;

        return (Success)? Zen(source, HeaderCount) : NullChild();
    }

    sint32s Zen::Build(const Context& data)
    {
        const String& Header = BuildHeader();
        const sint32 HeaderSize = Header.Length();
        const sint32 HeaderCount = (HeaderSize + sizeof(sint32)) / sizeof(sint32);

        sint32s Collector;
        sint32* HeaderDump = Collector.AtDumping(0, HeaderCount);
        Memory::Set(HeaderDump, 0x00, sizeof(sint32) * HeaderCount);
        Memory::Copy(HeaderDump, (chars) Header, HeaderSize);

        Map<sint32> StringPosMap;
        BuildCore(data, Collector, StringPosMap);
        return Collector;
    }

    const String& Zen::BuildHeader()
    {
        #if BOSS_X64
            static const String _ = String::Format("zen64{%s %s}", __TIME__, __DATE__);
        #else
            static const String _ = String::Format("zen32{%s %s}", __TIME__, __DATE__);
        #endif
        return _;
    }

    jumper Zen::BuildCore(const Context& data, sint32s& col, Map<sint32>& map)
    {
        // Zen영역 우선확보
        const sint32 ZenPos = col.Count();
        Memory::Set(col.AtDumping(ZenPos, sizeof(Zen) / sizeof(sint32)), 0x00, sizeof(Zen));

        // 수집될 포지션들
        sint32 StringPos = -1;
        sint32 IIndexerPos = -1;
        sint32 NIndexerPos = -1;
        sint32 NFinderPos = -1;

        // Indexable구성
        if(const sint32 ChildSize = data.LengthOfIndexable())
        {
            Array<jumper, datatype_pod_canmemcpy> NewIndexer;

            // Indexer수집
            for(sint32 i = 0; i < ChildSize; ++i)
                NewIndexer.AtAdding() = BuildCore(data[i], col, map);

            // Indexer의 Jumper업데이트
            IIndexerPos = col.Count();
            for(sint32 i = 0; i < ChildSize; ++i)
                NewIndexer.At(i) = (jumper) (NewIndexer[i] - (IIndexerPos + i));

            // Indexer쓰기
            const sint32 NewIndexerCount = NewIndexer.Count();
            sint32* IndexerDump = col.AtDumping(IIndexerPos, NewIndexerCount);
            Memory::Copy(IndexerDump, &NewIndexer[0], sizeof(sint32) * NewIndexerCount);
        }

        // Namable구성
        if(const sint32 ChildSize = data.LengthOfNamable())
        {
            Array<NamableIndexer, datatype_pod_canmemcpy> NewIndexerPool;
            Array<NamableFinder, datatype_pod_canmemcpy> NewFinderPool;
            Memory::Set(&NewFinderPool.AtAdding(), 0x00, sizeof(NamableFinder));

            // Indexer와 Finder의 수집
            for(sint32 i = 0; i < ChildSize; ++i)
            {
                chararray GetName;
                const jumper NewZenData = BuildCore(data(i, &GetName), col, map);

                // Finder업데이트
                sint32 FinderFocus = 0;
                chars NameFocus = &GetName[0];
                while(const uint08 CurWord = (*NameFocus & 0xFF))
                {
                    NamableFinder& CurFinder = NewFinderPool.At(FinderFocus);

                    // 범위구성
                    if(CurFinder.ChildFirst == 0)
                    {
                        CurFinder.ChildFirst = CurWord;
                        CurFinder.ChildLast = CurWord;
                    }
                    else if(CurWord < CurFinder.ChildFirst) CurFinder.ChildFirst = CurWord;
                    else if(CurFinder.ChildLast < CurWord) CurFinder.ChildLast = CurWord;

                    // 자식구성
                    if(CurFinder.Childs[CurWord] == jumper16_null)
                    {
                        CurFinder.Childs[CurWord] = (jumper16) NewFinderPool.Count();
                        NamableFinder& NewFinder = NewFinderPool.AtAdding();
                        Memory::Set(&NewFinder, 0x00, sizeof(NamableFinder));
                        NewFinder.Parent = (jumper) FinderFocus;
                        NewFinder.Word = *NameFocus;
                    }

                    FinderFocus = (sint32) NewFinderPool[FinderFocus].Childs[CurWord];
                    NameFocus++;
                }
                NewFinderPool.At(FinderFocus).ZenData = NewZenData;

                // Indexer생성
                NamableIndexer& NewIndexer = NewIndexerPool.AtAdding();
                NewIndexer.Finder = (jumper) FinderFocus;
                NewIndexer.NameLength = GetName.Count() - 1;
            }

            // Finder의 포지션들 예측
            NFinderPos = col.Count();
            Array<sint32, datatype_pod_canmemcpy> NFinderPosPool;
            *NFinderPosPool.AtDumping(0, NewFinderPool.Count() + 1) = NFinderPos;
            for(sint32 i = 0, iend = NewFinderPool.Count(); i < iend; ++i)
            {
                NamableFinder& CurFinder = NewFinderPool.At(i);
                sint32 CurFinderCount = (sizeof(NamableFinder) - sizeof(jumper16) * 256) / sizeof(sint32);
                if(CurFinder.ChildFirst != 0)
                {
                    const sint32 CurChildCount = 1 + CurFinder.ChildLast - CurFinder.ChildFirst;
                    const sint32 PackingRule = sizeof(sint32) / sizeof(jumper16);
                    CurFinderCount += (CurChildCount + PackingRule - 1) / PackingRule;
                }
                NFinderPosPool.At(i + 1) = NFinderPosPool[i] + CurFinderCount;
            }

            // Finder처리
            for(sint32 i = 0, iend = NewFinderPool.Count(); i < iend; ++i)
            {
                NamableFinder& CurFinder = NewFinderPool.At(i);
                const sint32* CurFinderPtr = (const sint32*) &CurFinder;

                // Finder의 Jumper업데이트
                if(0 < i)
                {
                    const sint32 PosByTopZen = NFinderPosPool[i] + (JumperPos(CurFinder.Parent) - CurFinderPtr);
                    CurFinder.Parent = (jumper) (NFinderPosPool[CurFinder.Parent] - PosByTopZen);
                }
                if(CurFinder.ZenData != jumper_null)
                {
                    const sint32 PosByTopZen = NFinderPosPool[i] + (JumperPos(CurFinder.ZenData) - CurFinderPtr);
                    CurFinder.ZenData = (jumper) (CurFinder.ZenData - PosByTopZen);
                }
                if(CurFinder.ChildFirst != 0)
                for(sint32 src = CurFinder.ChildFirst, dst = 0; src <= CurFinder.ChildLast; ++src, ++dst)
                {
                    if(CurFinder.Childs[src] != jumper16_null)
                    {
                        const sint32 PackingRule = sizeof(sint32) / sizeof(jumper16);
                        const sint32 Pos16ByTopZen = NFinderPosPool[i] * PackingRule + (Jumper16Pos(CurFinder.Childs[dst]) - (const sint16*) CurFinderPtr);
                        const sint32 Jumper16Value = NFinderPosPool[CurFinder.Childs[src]] * PackingRule - Pos16ByTopZen;
                        BOSS_ASSERT("Jumper16의 범위를 벗어납니다",
                            (Jumper16Value & 0xFFFF0000) == 0x00000000 || (Jumper16Value & 0xFFFF0000) == 0xFFFF0000);
                        CurFinder.Childs[dst] = (jumper16) Jumper16Value;
                    }
                    else CurFinder.Childs[dst] = jumper16_null;
                }

                // Finder쓰기
                BOSS_ASSERT("인코딩에 실패하였습니다", NFinderPosPool[i] == col.Count());
                const sint32 CurFinderCount = NFinderPosPool[i + 1] - NFinderPosPool[i];
                sint32* FinderDump = col.AtDumping(NFinderPosPool[i], CurFinderCount);
                Memory::Copy(FinderDump, CurFinderPtr, sizeof(sint32) * CurFinderCount);
            }

            // Indexer의 Jumper업데이트
            NIndexerPos = col.Count();
            for(sint32 i = 0, iend = NewIndexerPool.Count(); i < iend; ++i)
            {
                const sint32 PosByTopZen = NIndexerPos + sizeof(NamableIndexer) / sizeof(sint32) * i;
                NewIndexerPool.At(i).Finder = (jumper) (NFinderPosPool[NewIndexerPool[i].Finder] - PosByTopZen);
            }

            // Indexer쓰기
            const sint32 NewIndexerCount = sizeof(NamableIndexer) / sizeof(sint32) * NewIndexerPool.Count();
            sint32* IndexerDump = col.AtDumping(NIndexerPos, NewIndexerCount);
            Memory::Copy(IndexerDump, &NewIndexerPool[0], sizeof(sint32) * NewIndexerCount);
        }

        // String구성
        chars CurString = data.GetString();
        if(sint32* CurMap = map.Access(CurString))
            StringPos = *CurMap;
        else
        {
            StringPos = col.Count();
            map(CurString) = StringPos;

            // String쓰기
            const sint32 CurStringSize = (sint32) boss_strlen(CurString);
            const sint32 CurStringCount = (CurStringSize + sizeof(sint32)) / sizeof(sint32);
            sint32* StringDump = col.AtDumping(StringPos, CurStringCount);
            Memory::Set(StringDump, 0x00, sizeof(sint32) * CurStringCount);
            Memory::Copy(StringDump, CurString, CurStringSize);
        }

        // Zen구성
        Zen* NewZen = (Zen*) &col.At(ZenPos);
        if(StringPos != -1)
        {
            const sint32 PosByTopZen = JumperPos(NewZen->m_string) - &col[0];
            NewZen->m_string = (jumper) (StringPos - PosByTopZen);
        }
        NewZen->m_int = data.GetInt(0);
        NewZen->m_float = data.GetFloat(0);
        NewZen->m_iLength = data.LengthOfIndexable();
        if(IIndexerPos != -1)
        {
            const sint32 PosByTopZen = JumperPos(NewZen->m_iIndexer) - &col[0];
            NewZen->m_iIndexer = (jumper) (IIndexerPos - PosByTopZen);
        }
        NewZen->m_nLength = data.LengthOfNamable();
        if(NIndexerPos != -1)
        {
            const sint32 PosByTopZen = JumperPos(NewZen->m_nIndexer) - &col[0];
            NewZen->m_nIndexer = (jumper) (NIndexerPos - PosByTopZen);
        }
        if(NFinderPos != -1)
        {
            const sint32 PosByTopZen = JumperPos(NewZen->m_nFinder) - &col[0];
            NewZen->m_nFinder = (jumper) (NFinderPos - PosByTopZen);
        }
        return (jumper) ZenPos;
    }
}
