﻿#pragma once
#include <boss_mutex.hpp>

namespace BOSS
{
    typedef void (*QueueFreeCB)(void*);

    /// @brief 큐지원
    template<typename TYPE, bool FORTHREAD = true, QueueFreeCB FREECB = nullptr>
    class Queue
    {
    public:
        /// @brief 큐에 1개 데이터생성
        /// @return 생성된 데이터
        TYPE* Create()
        {
            TYPE* NewData = nullptr;
            if(FORTHREAD) Mutex::Lock(DataMutex);
            {
                NewData = Head.Create();
                DataCount++;
            }
            if(FORTHREAD) Mutex::Unlock(DataMutex);
            return NewData;
        }

        /// @brief 큐에 1개 데이터적재
        /// @param data : 데이터
        void Enqueue(TYPE data)
        {
            if(FORTHREAD) Mutex::Lock(DataMutex);
            {
                Head.Enqueue(data);
                DataCount++;
            }
            if(FORTHREAD) Mutex::Unlock(DataMutex);
        }

        /// @brief 큐에서 1개 데이터배출
        /// @param nullvalue : 데이터가 없을 경우의 값
        /// @return 해당 데이터
        TYPE Dequeue(TYPE nullvalue = TYPE())
        {
            TYPE Data = nullvalue;
            if(FORTHREAD) Mutex::Lock(DataMutex);
            if(0 < DataCount)
            {
                Data = Head.Dequeue();
                DataCount--;
            }
            if(FORTHREAD) Mutex::Unlock(DataMutex);
            return Data;
        }

        /// @brief 큐에서 배출될 데이터를 미리 확인
        /// @return 해당 데이터
        TYPE* TryLockPreview()
        {
            if(FORTHREAD) Mutex::Lock(DataMutex);
            if(0 < DataCount)
            {
                if(Head.Next)
                    return &Head.Next->Data;
            }
            if(FORTHREAD) Mutex::Unlock(DataMutex);
            return nullptr;
        }

        /// @brief 배출확인 성공시 Unlock처리
        void UnlockPreview()
        {
            if(FORTHREAD) Mutex::Unlock(DataMutex);
        }

        /// @brief 현재 큐에 적재된 데이터수량
        /// @return 데이터수량
        sint32 Count() const
        {
            sint32 Result = 0;
            if(FORTHREAD) Mutex::Lock(DataMutex);
            {
                Result = DataCount;
            }
            if(FORTHREAD) Mutex::Unlock(DataMutex);
            return Result;
        }

        /// @brief 생성자
        Queue() : Head(TYPE()), DataCount(0), DataMutex(nullptr)
        {
            if(FORTHREAD) DataMutex = Mutex::Open();
        }

        /// @brief 복사생성자(불허)
        Queue(const Queue&)
        {BOSS_ASSERT("Queue는 복사될 수 없습니다", false);}

        /// @brief 이동생성자
        /// @param rhs : 이동할 인스턴스
        Queue(Queue&& rhs) : Head(TYPE())
        {
            operator=(ToReference(rhs));
        }

        /// @brief 소멸자
        ~Queue()
        {
            if(FREECB)
            {
                if(FORTHREAD) Mutex::Lock(DataMutex);
                while(0 < DataCount)
                {
                    TYPE OldData = Head.Dequeue();
                    void* OldPtr = *((void**) &OldData);
                    FREECB(OldPtr);
                    DataCount--;
                }
                if(FORTHREAD) Mutex::Unlock(DataMutex);
            }
            else if(IsTypePointer<TYPE>())
                BOSS_ASSERT("큐에 해제되지 않은 데이터가 존재합니다", DataCount == 0);

            if(FORTHREAD)
                Mutex::Close(DataMutex);
        }

        /// @brief 복사(불허)
        Queue& operator=(const Queue&)
        {BOSS_ASSERT("Queue는 복사될 수 없습니다", false); return *this;}

        /// @brief 이동
        /// @param rhs : 이동할 인스턴스
        /// @return 자기 객체
        Queue& operator=(Queue&& rhs)
        {
            Head.Prev = rhs.Head.Prev; rhs.Head.Prev = &rhs.Head;
            Head.Prev->Next = &Head;
            Head.Next = rhs.Head.Next; rhs.Head.Next = &rhs.Head;
            Head.Next->Prev = &Head;
            DataCount = rhs.DataCount; rhs.DataCount = 0;
            return *this;
        }

    private:
        class QueueElement
        {
        public:
            TYPE Data;
            QueueElement* Prev;
            QueueElement* Next;

        public:
            QueueElement() : Prev(this), Next(this) {}

            QueueElement(TYPE data) : Data(data), Prev(this), Next(this) {}

            ~QueueElement()
            {
                if(Prev) Prev->Next = nullptr;
                QueueElement* CurrNode = nullptr;
                QueueElement* NextNode = Next;
                while((CurrNode = NextNode) != nullptr)
                {
                    NextNode = CurrNode->Next;
                    CurrNode->Prev = nullptr;
                    CurrNode->Next = nullptr;
                    delete CurrNode;
                }
            }

        public:
            TYPE* Create()
            {
                QueueElement* NewNode = new QueueElement();
                (NewNode->Prev = Prev)->Next = NewNode;
                (NewNode->Next = this)->Prev = NewNode;
                return &NewNode->Data;
            }

            void Enqueue(TYPE data)
            {
                QueueElement* NewNode = new QueueElement(data);
                (NewNode->Prev = Prev)->Next = NewNode;
                (NewNode->Next = this)->Prev = NewNode;
            }

            TYPE Dequeue()
            {
                QueueElement* DeleteNode = Next;
                DeleteNode->Next->Prev = DeleteNode->Prev;
                DeleteNode->Prev->Next = DeleteNode->Next;
                DeleteNode->Prev = nullptr;
                DeleteNode->Next = nullptr;
                TYPE Result = DeleteNode->Data;
                delete DeleteNode;
                return Result;
            }
        };

    private:
        QueueElement Head;
        sint32 DataCount;
        mutable id_mutex DataMutex;
    };
}
