#pragma once
#include <boss_map.hpp>

namespace BOSS
{
    //! \brief 트리지원
    template<typename TYPE>
    class Tree
    {
    public:
        typedef void (*DeleteCB)(TYPE*); 

    public:
        /*!
        \brief 생성자
        */
        Tree()
        {
            mValue = nullptr;
            mDestroyer = nullptr;
        }

        /*!
        \brief 복사생성자(불허)
        */
        Tree(const Tree&)
        {BOSS_ASSERT("Tree는 복사될 수 없습니다", false);}

        /*!
        \brief 이동생성자
        \param rhs : 이동할 인스턴스
        */
        Tree(Tree&& rhs)
        {
            operator=(ToReference(rhs));
        }

        /*!
        \brief 소멸자
        */
        ~Tree()
        {
            RemoveValue();
        }

        /*!
        \brief 복사(불허)
        */
        Tree& operator=(const Tree&)
        {BOSS_ASSERT("Tree는 복사될 수 없습니다", false); return *this;}

        /*!
        \brief 이동
        \param rhs : 이동할 인스턴스
        \return 자기 객체
        */
        Tree& operator=(Tree&& rhs)
        {
            mChild = ToReference(rhs.mChild);
            mValue = rhs.mValue; rhs.mValue = nullptr;
            mDestroyer = rhs.mDestroyer; rhs.mDestroyer = nullptr;
            return *this;
        }

    public:
        /*!
        \brief 키워드식 쓰기접근(없으면 생성)
        \param key : 자식트리의 이름
        \param length : 이름의 길이(-1이면 자동설정)
        \return 해당 자식트리
        */
        Tree& operator()(chars key, sint32 length = -1)
        {
            return mChild(key, length);
        }

        /*!
        \brief 키워드식 읽기접근(없으면 NullChild를 리턴)
        \param key : 자식트리의 이름
        \param length : 이름의 길이(-1이면 자동설정)
        \return 해당 자식트리
        */
        const Tree& operator()(chars key, sint32 length = -1) const
        {
            const Tree* Result = mChild.Access(key, length);
            return (Result)? *Result : NullChild();
        }

        /*!
        \brief 인덱스식 쓰기접근(없으면 생성)
        \param index : 자식트리의 순번
        \return 해당 자식트리
        */
        Tree& operator[](sint64 index)
        {
            return mChild[index];
        }

        /*!
        \brief 인덱스식 읽기접근(없으면 NullChild를 리턴)
        \param index : 자식트리의 순번
        \return 해당 자식트리
        */
        const Tree& operator[](sint64 index) const
        {
            const Tree* Result = mChild.Access(index);
            return (Result)? *Result : NullChild();
        }

        /*!
        \brief 키워드식 자식제거
        \param key : 자식트리의 이름
        */
        void Remove(chars key)
        {
            mChild.Remove(key);
        }

        /*!
        \brief 인덱스식 자식제거
        \param index : 자식트리의 순번
        */
        void Remove(sint64 index)
        {
            mChild.Remove(index);
        }

        /*!
        \brief 모든 자식제거
        */
        void RemoveAll()
        {
            mChild.Reset();
        }

        /*!
        \brief 배열수량 구하기
        \return 배열수량
        */
        sint32 Count() const
        {
            return mChild.Count();
        }

        /*!
        \brief Value생성
        \param value : 생성된 인스턴스
        \param cb : 인스턴스의 파괴자
        \return 생성된 인스턴스
        */
        TYPE* CreateValue(TYPE* value = new TYPE, DeleteCB cb = [](TYPE* ptr){delete ptr;})
        {
            RemoveValue();
            mValue = value;
            mDestroyer = cb;
            return mValue;
        }

        /*!
        \brief Value제거
        */
        void RemoveValue()
        {
            if(mDestroyer)
                mDestroyer(mValue);
            mValue = nullptr;
            mDestroyer = nullptr;
        }

        /*!
        \brief Value얻기
        \return Value의 인스턴스
        */
        TYPE* Value()
        {
            return mValue;
        }

        /*!
        \brief Value얻기(읽기전용)
        \return Value의 인스턴스
        */
        TYPE const* Value() const
        {
            return mValue;
        }

        /*!
        \brief Value를 즉시 접근
        \return Value의 인스턴스
        */
        TYPE* operator->()
        {
            BOSS_ASSERT("mValue가 nullptr입니다", mValue);
            return mValue;
        }

        /*!
        \brief Value를 즉시 접근(읽기전용)
        \return Value의 인스턴스
        */
        TYPE const* operator->() const
        {
            BOSS_ASSERT("mValue가 nullptr입니다", mValue);
            return mValue;
        }

    private:
        const Tree& NullChild() const
        {
            return *BOSS_STORAGE_SYS(Tree);
        }

    private:
        Map<Tree> mChild;
        TYPE* mValue;
        DeleteCB mDestroyer;
    };
}
