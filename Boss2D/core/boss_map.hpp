#pragma once
#include <boss_array.hpp>

namespace BOSS
{
    class MapPath
    {
    public:
        const MapPath* parent;
        const sint32 value;
    public:
        MapPath(const MapPath* parent, const sint32 value)
            : parent(parent), value(value) {}
        ~MapPath() {}
        chararray GetPath() const
        {
            chararray collector;
            const MapPath* path = this;
            while(path && path->parent)
            {
                char& onechar = collector.AtAdding();
                onechar = path->value;
                path = path->parent;
                onechar |= path->value << 4;
                path = path->parent;
            }
            chararray result;
            for(sint32 i = collector.Count() - 1; 0 <= i; --i)
                result.AtAdding() = collector[i];
            result.AtAdding() = '\0';
            return result;
        }
    };

    //! \brief 맵지원
    template<typename TYPE, sint32 ENDMARK = 0>
    class Map
    {
    public:
        typedef void (*AccessCB)(const MapPath*, TYPE*, payload);

    public:
        /*!
        \brief 정수식 접근(생성가능)
        \param key : 검색키
        \return 해당 객체
        */
        TYPE& operator[](uint08 key) {return CreativeByIndexable(key);}
        TYPE& operator[](uint16 key) {return CreativeByIndexable(key);}
        TYPE& operator[](uint32 key) {return CreativeByIndexable(key);}
        TYPE& operator[](uint64 key) {return CreativeByIndexable(key);}
        TYPE& operator[](sint08 key) {BOSS_ASSERT("key가 음수값입니다", 0 <= key); return CreativeByIndexable((uint08) key);}
        TYPE& operator[](sint16 key) {BOSS_ASSERT("key가 음수값입니다", 0 <= key); return CreativeByIndexable((uint16) key);}
        TYPE& operator[](sint32 key) {BOSS_ASSERT("key가 음수값입니다", 0 <= key); return CreativeByIndexable((uint32) key);}
        TYPE& operator[](sint64 key) {BOSS_ASSERT("key가 음수값입니다", 0 <= key); return CreativeByIndexable((uint64) key);}

        /*!
        \brief 정수식 마지막 추가후 접근
        \return 해당 객체
        */
        TYPE& AtAdding() {return CreativeByIndexable((uint32) Info->Count);}

        /*!
        \brief 문자열(UTF8/CP949)식 접근(생성가능)
        \param key : 검색키
        \param length : 검색키의 길이(-1이면 자동설정)
        \return 해당 객체
        */
        TYPE& operator()(chars key, sint32 length = -1)
        {
            Map* CurNode = this;
            if(length == -1)
            {
                do
                {
                    CurNode = CurNode->ValidChild((*key & 0xF0) >> 4);
                    CurNode = CurNode->ValidChild(*key & 0x0F);
                    if(*key) key++;
                }
                while(*key && *key != ENDMARK);
            }
            else if(length == 0)
            {
                CurNode = CurNode->ValidChild(0);
                CurNode = CurNode->ValidChild(0);
            }
            else do
            {
                CurNode = CurNode->ValidChild((*key & 0xF0) >> 4);
                CurNode = CurNode->ValidChild(*key & 0x0F);
                key++;
            }
            while(--length);
            Info->LastTip = key;

            if(!CurNode->Data)
            {
                Info->Count++;
                CurNode->Data = new TYPE;
                if(IsTypePointer<TYPE>())
                    *((void**) CurNode->Data) = nullptr;
            }
            return *CurNode->Data;
        }

        /*!
        \brief 문자열(UTF16)식 접근(생성가능)
        \param key : 검색키
        \param length : 검색키의 길이(-1이면 자동설정)
        \return 해당 객체
        */
        TYPE& operator()(wchars key, sint32 length = -1)
        {
            Map* CurNode = this;
            if(length == -1)
            {
                do
                {
                    CurNode = CurNode->ValidChild((*key & 0xF000) >> 12);
                    CurNode = CurNode->ValidChild((*key & 0x0F00) >> 8);
                    CurNode = CurNode->ValidChild((*key & 0x00F0) >> 4);
                    CurNode = CurNode->ValidChild(*key & 0x000F);
                    if(*key) key++;
                }
                while(*key && *key != ENDMARK);
            }
            else if(length == 0)
            {
                CurNode = CurNode->ValidChild(0);
                CurNode = CurNode->ValidChild(0);
                CurNode = CurNode->ValidChild(0);
                CurNode = CurNode->ValidChild(0);
            }
            else do
            {
                CurNode = CurNode->ValidChild((*key & 0xF000) >> 12);
                CurNode = CurNode->ValidChild((*key & 0x0F00) >> 8);
                CurNode = CurNode->ValidChild((*key & 0x00F0) >> 4);
                CurNode = CurNode->ValidChild(*key & 0x000F);
                key++;
            }
            while(--length);
            Info->LastTip = key;

            if(!CurNode->Data)
            {
                Info->Count++;
                CurNode->Data = new TYPE;
                if(IsTypePointer<TYPE>())
                    *((void**) CurNode->Data) = nullptr;
            }
            return *CurNode->Data;
        }

        /*!
        \brief 정수식 접근(생성불가)
        \param key : 검색키
        \return 해당 객체의 포인터
        */
        TYPE* Access(uint08 key) const {return AccessByIndexable(key);}
        TYPE* Access(uint16 key) const {return AccessByIndexable(key);}
        TYPE* Access(uint32 key) const {return AccessByIndexable(key);}
        TYPE* Access(uint64 key) const {return AccessByIndexable(key);}
        TYPE* Access(sint08 key) const {BOSS_ASSERT("key가 음수값입니다", 0 <= key); return AccessByIndexable((uint08) key);}
        TYPE* Access(sint16 key) const {BOSS_ASSERT("key가 음수값입니다", 0 <= key); return AccessByIndexable((uint16) key);}
        TYPE* Access(sint32 key) const {BOSS_ASSERT("key가 음수값입니다", 0 <= key); return AccessByIndexable((uint32) key);}
        TYPE* Access(sint64 key) const {BOSS_ASSERT("key가 음수값입니다", 0 <= key); return AccessByIndexable((uint64) key);}

        /*!
        \brief 문자열(UTF8/CP949)식 접근(생성불가)
        \param key : 검색키
        \param length : 검색키의 길이(-1이면 자동설정)
        \return 해당 객체의 포인터
        */
        TYPE* Access(chars key, sint32 length = -1) const
        {
            const Map* CurNode = this;
            if(length == -1)
            {
                do
                {
                    if(!CurNode->Table || !(CurNode = CurNode->Table->Child[(*key & 0xF0) >> 4])) return nullptr;
                    if(!CurNode->Table || !(CurNode = CurNode->Table->Child[*key & 0x0F])) return nullptr;
                    if(*key) key++;
                }
                while(*key && *key != ENDMARK);
            }
            else if(length == 0)
            {
                if(!CurNode->Table || !(CurNode = CurNode->Table->Child[0])) return nullptr;
                if(!CurNode->Table || !(CurNode = CurNode->Table->Child[0])) return nullptr;
            }
            else do
            {
                if(!CurNode->Table || !(CurNode = CurNode->Table->Child[(*key & 0xF0) >> 4])) return nullptr;
                if(!CurNode->Table || !(CurNode = CurNode->Table->Child[*key & 0x0F])) return nullptr;
                key++;
            }
            while(--length);
            Info->LastTip = key;
            return CurNode->Data;
        }

        /*!
        \brief 문자열(UTF16)식 접근(생성불가)
        \param key : 검색키
        \param length : 검색키의 길이(-1이면 자동설정)
        \return 해당 객체의 포인터
        */
        TYPE* Access(wchars key, sint32 length = -1) const
        {
            const Map* CurNode = this;
            if(length == -1)
            {
                do
                {
                    if(!CurNode->Table || !(CurNode = CurNode->Table->Child[(*key & 0xF000) >> 12])) return nullptr;
                    if(!CurNode->Table || !(CurNode = CurNode->Table->Child[(*key & 0x0F00) >> 8])) return nullptr;
                    if(!CurNode->Table || !(CurNode = CurNode->Table->Child[(*key & 0x00F0) >> 4])) return nullptr;
                    if(!CurNode->Table || !(CurNode = CurNode->Table->Child[*key & 0x000F])) return nullptr;
                    if(*key) key++;
                }
                while(*key && *key != ENDMARK);
            }
            else if(length == 0)
            {
                if(!CurNode->Table || !(CurNode = CurNode->Table->Child[0])) return nullptr;
                if(!CurNode->Table || !(CurNode = CurNode->Table->Child[0])) return nullptr;
                if(!CurNode->Table || !(CurNode = CurNode->Table->Child[0])) return nullptr;
                if(!CurNode->Table || !(CurNode = CurNode->Table->Child[0])) return nullptr;
            }
            else do
            {
                if(!CurNode->Table || !(CurNode = CurNode->Table->Child[(*key & 0xF000) >> 12])) return nullptr;
                if(!CurNode->Table || !(CurNode = CurNode->Table->Child[(*key & 0x0F00) >> 8])) return nullptr;
                if(!CurNode->Table || !(CurNode = CurNode->Table->Child[(*key & 0x00F0) >> 4])) return nullptr;
                if(!CurNode->Table || !(CurNode = CurNode->Table->Child[*key & 0x000F])) return nullptr;
                key++;
            }
            while(--length);
            Info->LastTip = key;
            return CurNode->Data;
        }

        /*!
        \brief 순번식 접근(생성불가)
        \param order : 순번
        \param getpath : 경로얻기
        \return 해당 객체의 포인터
        */
        TYPE* AccessByOrder(sint32 order, chararray* getpath = nullptr) const
        {
            return AccessByOrderCore(MapPath(nullptr, 0), order, getpath);
        }

        /*!
        \brief 콜백식 접근(생성불가)
        \param cb : 콜백함수
        \param param : 전달인수
        */
        void AccessByCallback(AccessCB cb, payload param) const
        {
            if(Table)
            for(sint32 i = 0; i < 16; ++i)
            {
                if(!Table->Child[i]) continue;
                Table->Child[i]->AccessByCallbackCore(MapPath(nullptr, i), cb, param);
            }
        }

        /*!
        \brief 정수식 데이터제거
        \param key : 검색키
        \return 성공여부
        */
        bool Remove(uint08 key) {return RemoveByIndexable(key);}
        bool Remove(uint16 key) {return RemoveByIndexable(key);}
        bool Remove(uint32 key) {return RemoveByIndexable(key);}
        bool Remove(uint64 key) {return RemoveByIndexable(key);}
        bool Remove(sint08 key) {BOSS_ASSERT("key가 음수값입니다", 0 <= key); return RemoveByIndexable((uint08) key);}
        bool Remove(sint16 key) {BOSS_ASSERT("key가 음수값입니다", 0 <= key); return RemoveByIndexable((uint16) key);}
        bool Remove(sint32 key) {BOSS_ASSERT("key가 음수값입니다", 0 <= key); return RemoveByIndexable((uint32) key);}
        bool Remove(sint64 key) {BOSS_ASSERT("key가 음수값입니다", 0 <= key); return RemoveByIndexable((uint64) key);}

        /*!
        \brief 문자열(UTF8/CP949)식 데이터제거
        \param key : 검색키
        \return 성공여부
        */
        bool Remove(chars key)
        {
            if(RemoveCoreByNameable(key))
            {
                Info->Count--;
                return true;
            }
            return false;
        }

        /*!
        \brief 문자열(UTF16)식 데이터제거
        \param key : 검색키
        \return 성공여부
        */
        bool Remove(wchars key)
        {
            if(RemoveCoreByNameable(key))
            {
                Info->Count--;
                return true;
            }
            return false;
        }

        /*!
        \brief 전체초기화
        */
        void Reset()
        {
            if(Info)
            {
                Info->Count = 0;
                Info->LastTip = nullptr;
            }
            if(Table)
            {
                for(sint32 i = 0; i < 16; ++i)
                {
                    if(!Table->Child[i]) continue;
                    delete Table->Child[i]->Data;
                    Table->Child[i]->Data = nullptr;
                    delete Table->Child[i];
                    Table->Child[i] = nullptr;
                }
                delete Table;
                Table = nullptr;
            }
        }

        /*!
        \brief 데이터수량 얻기
        \return 데이터수량
        */
        const sint32 Count() const {return Info->Count;}

        /*!
        \brief 팁정보 얻기
        \return operator[]/operator()와 Access등에 의해 발생된 팁정보
        */
        const void* GetLastTip() const {return Info->LastTip;}

        /*!
        \brief 생성자
        */
        Map()
        {
            Table = nullptr;
            Info = new VarInfo();
        }

        /*!
        \brief 복사생성자
        \param rhs : 복사할 인스턴스
        */
        Map(const Map& rhs)
        {
            Table = nullptr;
            Info = new VarInfo();
            operator=(rhs);
        }

        /*!
        \brief 이동생성자
        \param rhs : 이동할 인스턴스
        */
        Map(Map&& rhs)
        {
            Table = nullptr;
            Info = nullptr;
            operator=(ToReference(rhs));
        }

        /*!
        \brief 소멸자
        */
        ~Map()
        {
            delete Info;
            Info = nullptr;
            Reset();
        }

        /*!
        \brief 복사
        \param rhs : 복사할 인스턴스
        \return 자기 객체
        */
        Map& operator=(const Map& rhs)
        {
            CopyCore(&rhs, true);
            return *this;
        }

        /*!
        \brief 이동
        \param rhs : 이동할 인스턴스
        \return 자기 객체
        */
        Map& operator=(Map&& rhs)
        {
            delete Info;
            Info = nullptr;
            Reset();
            Table = rhs.Table; rhs.Table = nullptr;
            Info = rhs.Info; rhs.Info = new VarInfo();
            return *this;
        }

    protected:
        /*!
        \brief 생성자(내부호출용)
        */
        Map(sint32)
        {
            Table = nullptr;
            Data = nullptr;
        }

    private:
        class VarTable
        {
            private: sint32 Count;
            public: Map* Child[16];
            public: VarTable()
            {
                Count = 0;
                Memory::Set(Child, 0x00, sizeof(Map*) * 16);
            }
            public: VarTable(const VarTable& rhs)
            {
                Count = 0;
                Memory::Set(Child, 0x00, sizeof(Map*) * 16);
                operator=(rhs);
            }
            public: VarTable& operator=(const VarTable& rhs)
            {
                Count = rhs.Count;
                for(sint32 i = 0; i < 16; ++i)
                {
                    if(rhs.Child[i])
                    {
                        if(!Child[i]) Child[i] = new Map(0);
                        Child[i]->CopyCore(rhs.Child[i], false);
                    }
                    else if(Child[i])
                    {
                        delete Child[i];
                        Child[i] = nullptr;
                    }
                }
                return *this;
            }
            public: Map* Valid(const sint32 index)
            {
                if(!Child[index])
                {
                    Count++;
                    Child[index] = new Map(0);
                }
                return Child[index];
            }
            public: sint32 Invalide(const sint32 index)
            {
                BOSS_ASSERT("이미 삭제된 Child를 삭제하려 합니다", Child[index]);
                delete Child[index];
                Child[index] = nullptr;
                return --Count;
            }
        };

        class VarInfo
        {
            public: sint32 Count;
            public: mutable const void* LastTip;
            public: VarInfo() : Count(0), LastTip(nullptr) {}
            public: ~VarInfo() {}
        };

        VarTable* Table;
        union
        {
            VarInfo* Info;
            TYPE* Data;
        };

        static inline uint64 ShiftedKey(uint64 key, sint32& step)
        {
            uint32 flag;
            flag = !(key & oxFFFFFFFF00000000); key <<= flag << 5; step += flag << 3;
            flag = !(key & oxFFFF000000000000); key <<= flag << 4; step += flag << 2;
            flag = !(key & oxFF00000000000000); key <<= flag << 3; step += flag << 1;
            flag = !(key & oxF000000000000000); key <<= flag << 2; step += flag << 0;
            return key;
        }

        static inline uint32 ShiftedKey(uint32 key, sint32& step)
        {
            uint32 flag;
            flag = !(key & 0xFFFF0000); key <<= flag << 4; step += flag << 2;
            flag = !(key & 0xFF000000); key <<= flag << 3; step += flag << 1;
            flag = !(key & 0xF0000000); key <<= flag << 2; step += flag << 0;
            return key;
        }

        static inline uint16 ShiftedKey(uint16 key, sint32& step)
        {
            uint32 flag;
            flag = !(key & 0xFF00); key <<= flag << 3; step += flag << 1;
            flag = !(key & 0xF000); key <<= flag << 2; step += flag << 0;
            return key;
        }

        static inline uint08 ShiftedKey(uint08 key, sint32& step)
        {
            uint32 flag;
            flag = !(key & 0xF0); key <<= flag << 2; step += flag << 0;
            return key;
        }

        inline Map* ValidChild(const sint32 index)
        {
            if(!Table) Table = new VarTable();
            return Table->Valid(index);
        }

        inline void InvalideChild(const sint32 index)
        {
            if(Table->Invalide(index) == 0)
            {
                delete Table;
                Table = nullptr;
            }
        }

        void CopyCore(const Map* rhs, const bool root)
        {
            if(root)
            {
                Info->Count = rhs->Info->Count;
                Info->LastTip = rhs->Info->LastTip;
            }
            else
            {
                if(rhs->Data)
                {
                    if(!Data) Data = new TYPE;
                    *Data = *rhs->Data;
                }
                else if(Data)
                {
                    delete Data;
                    Data = nullptr;
                }
            }

            if(rhs->Table)
            {
                if(!Table) Table = new VarTable();
                *Table = *rhs->Table;
            }
            else if(Table)
            {
                delete Table;
                Table = nullptr;
            }
        }

        template<typename KEYTYPE>
        TYPE& CreativeByIndexable(KEYTYPE key)
        {
            const sint32 ShiftTest = sizeof(KEYTYPE) * 8 - 4;

            sint32 step = 0;
            key = ShiftedKey(key, step);
            Map* CurNode = this;
            do
            {
                CurNode = CurNode->ValidChild(key >> ShiftTest);
                key <<= 4;
            }
            while(++step < sizeof(KEYTYPE) * 2);

            if(!CurNode->Data)
            {
                Info->Count++;
                CurNode->Data = new TYPE;
                if(IsTypePointer<TYPE>())
                    *((void**) CurNode->Data) = nullptr;
            }
            return *CurNode->Data;
        }

        template<typename KEYTYPE>
        TYPE* AccessByIndexable(KEYTYPE key) const
        {
            const sint32 ShiftTest = sizeof(KEYTYPE) * 8 - 4;

            sint32 step = 0;
            key = ShiftedKey(key, step);
            const Map* CurNode = this;
            do
            {
                if(!CurNode->Table || !(CurNode = CurNode->Table->Child[key >> ShiftTest])) return nullptr;
                key <<= 4;
            }
            while(++step < sizeof(KEYTYPE) * 2);
            return CurNode->Data;
        }

        TYPE* AccessByOrderCore(const MapPath& path, sint32& order, chararray* name) const
        {
            if(Data && order-- == -1)
            {
                if(name) *name = path.GetPath();
                return Data;
            }
            if(Table)
            for(sint32 i = 0; i < 16; ++i)
            {
                if(!Table->Child[i]) continue;
                TYPE* Result = Table->Child[i]->AccessByOrderCore(MapPath(&path, i), order, name);
                if(Result) return Result;
            }
            return nullptr;
        }

        void AccessByCallbackCore(const MapPath& path, AccessCB cb, payload param) const
        {
            if(Data) cb(&path, Data, param);
            if(Table)
            for(sint32 i = 0; i < 16; ++i)
            {
                if(!Table->Child[i]) continue;
                Table->Child[i]->AccessByCallbackCore(MapPath(&path, i), cb, param);
            }
        }

        template<typename KEYTYPE>
        bool RemoveByIndexable(KEYTYPE key)
        {
            sint32 step = 0;
            key = ShiftedKey(key, step);
            if(RemoveCoreByIndexable(key, step))
            {
                Info->Count--;
                return true;
            }
            return false;
        }

        template<typename KEYTYPE>
        bool RemoveCoreByIndexable(KEYTYPE key, sint32 step)
        {
            const sint32 ShiftTest = sizeof(KEYTYPE) * 8 - 4;

            bool Result = false;
            Map* CurChild = nullptr;
            if(Table && (CurChild = Table->Child[key >> ShiftTest]))
            {
                if(++step < sizeof(KEYTYPE) * 2)
                    Result = CurChild->RemoveCoreByIndexable(key << 4, step);
                else if(CurChild->Data)
                {
                    Result = true;
                    delete CurChild->Data;
                    CurChild->Data = nullptr;
                }
                if(!CurChild->Table && !CurChild->Data) InvalideChild(key >> ShiftTest);
            }
            return Result;
        }

        bool RemoveCoreByNameable(chars key)
        {
            bool Result = false;
            Map* CurChild1 = nullptr;
            Map* CurChild2 = nullptr;
            if(Table && (CurChild1 = Table->Child[(*key & 0xF0) >> 4]) && (CurChild2 = CurChild1->Table->Child[*key & 0x0F]))
            {
                if(*key && *(key + 1)) Result = CurChild2->RemoveCoreByNameable(key + 1);
                else if(CurChild2->Data)
                {
                    Result = true;
                    delete CurChild2->Data;
                    CurChild2->Data = nullptr;
                }
                if(!CurChild2->Table && !CurChild2->Data) CurChild1->InvalideChild(*key & 0x0F);
                if(!CurChild1->Table && !CurChild1->Data) InvalideChild((*key & 0xF0) >> 4);
            }
            return Result;
        }

        bool RemoveCoreByNameable(wchars key)
        {
            bool Result = false;
            Map* CurChild1 = nullptr;
            Map* CurChild2 = nullptr;
            Map* CurChild3 = nullptr;
            Map* CurChild4 = nullptr;
            if(Table && (CurChild1 = Table->Child[(*key & 0xF000) >> 12]) && (CurChild2 = CurChild1->Table->Child[(*key & 0x0F00) >> 8])
                && (CurChild3 = CurChild2->Table->Child[(*key & 0x00F0) >> 4]) && (CurChild4 = CurChild3->Table->Child[*key & 0x000F]))
            {
                if(*key && *(key + 1)) Result = CurChild4->RemoveCoreByNameable(key + 1);
                else if(CurChild4->Data)
                {
                    Result = true;
                    delete CurChild4->Data;
                    CurChild4->Data = nullptr;
                }
                if(!CurChild4->Table && !CurChild4->Data) CurChild3->InvalideChild(*key & 0x000F);
                if(!CurChild3->Table && !CurChild3->Data) CurChild2->InvalideChild((*key & 0x00F0) >> 4);
                if(!CurChild2->Table && !CurChild2->Data) CurChild1->InvalideChild((*key & 0x0F00) >> 8);
                if(!CurChild1->Table && !CurChild1->Data) InvalideChild((*key & 0xF000) >> 12);
            }
            return Result;
        }
    };
}
