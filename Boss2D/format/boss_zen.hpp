#pragma once
#include <boss.hpp>

namespace BOSS
{
    //! \brief 포맷-ZEN
    class Zen
    {
    public:
        /*!
        \brief 자신의 유효여부
        \return 유효여부
        */
        inline bool IsValid() const
        {return (this != &NullChild());}

        /*!
        \brief 자신의 문자열값 얻기
        \param defaultvalue : 디폴트값
        \return 문자열값
        */
        inline chars GetString(chars defaultvalue = nullptr) const
        {return IsValid()? (chars) &JumperToPtr(m_string)[m_jumpbase] : defaultvalue;}

        /*!
        \brief 자신의 정수값 얻기
        \param defaultvalue : 디폴트값
        \return 정수값
        */
        inline sint32 GetInt(const sint32 defaultvalue = 0) const
        {return IsValid()? m_int : defaultvalue;}

        /*!
        \brief 자신의 소수값 얻기
        \param defaultvalue : 디폴트값
        \return 소수값
        */
        inline float GetFloat(const float defaultvalue = 0) const
        {return IsValid()? m_float : defaultvalue;}

        /*!
        \brief 자식의 수량얻기(배열식)
        \return 수량값
        */
        inline sint32 LengthOfIndexable() const
        {return m_iLength;}

        /*!
        \brief 자식의 수량얻기(키워드식)
        \return 수량값
        */
        inline sint32 LengthOfNamable() const
        {return m_nLength;}

        /*!
        \brief 자식으로 순번접근(배열식)
        \param index : 자식의 순번
        \return 해당 자식의 인스턴스
        */
        const Zen& operator[](sint32 index) const;

        /*!
        \brief 자식으로 순번접근(키워드식)
        \param index : 자식의 순번
        \param getname : 자식의 이름얻기
        \return 해당 자식의 인스턴스
        */
        const Zen& operator()(sint32 index, chararray* getname = nullptr) const;

        /*!
        \brief 자식으로 명칭접근(키워드식)
        \param name : 자식의 이름
        \return 해당 자식의 인스턴스
        */
        const Zen& operator()(chars name) const;

    public:
        /*!
        \brief 생성자
        */
        Zen();

        /*!
        \brief 복사생성자
        \param rhs : 복사할 인스턴스
        */
        Zen(const Zen& rhs);

        /*!
        \brief 생성자
        \param source : Zen소스
        \param offset : 시작위치
        */
        Zen(const sint32s& source, sint32 offset);

        /*!
        \brief 소멸자
        */
        ~Zen();

        /*!
        \brief 복사
        \param rhs : 복사할 인스턴스
        \return 자기 객체
        */
        Zen& operator=(const Zen& rhs);

    public:
        /*!
        \brief Zen소스 읽기
        \param source : Zen소스
        \param success : 성공여부
        \return Zen객체
        */
        static Zen Load(const sint32s& source, bool* getSuccess = nullptr);

        /*!
        \brief Zen소스 제작
        \param data : 대상 콘텍스트
        \return Zen소스
        */
        static sint32s Build(const Context& data);

    private:
        static const String& BuildHeader();
        static jumper BuildCore(const Context& data, sint32s& col, Map<sint32>& map);
        static inline const Zen& NullChild()
        {return *BOSS_STORAGE_SYS(Zen);}

    private:
        sint32s m_source;
        sblock m_jumpbase;
        jumper m_string;
        sint32 m_int;
        float m_float;
        sint32 m_iLength;
        jumper m_iIndexer;
        sint32 m_nLength;
        jumper m_nIndexer;
        jumper m_nFinder;
    };
}
