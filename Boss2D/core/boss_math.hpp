#pragma once
#define BOSS_NOT_INCLUDE_FOR_INTELLIGENCE
#include <boss.hpp>

namespace BOSS
{
    /// @brief 수학함수지원
    class Math
    {
    public:
        /// @brief 절대값 구하기
        /// @param v : 인수
        /// @return 절대값
        inline static sint32 Abs(const sint32 v)
        {return (v < 0)? -v : v;}

        /// @brief 절대값 구하기
        /// @param v : 인수
        /// @return 절대값
        inline static float AbsF(const float v)
        {return (v < 0)? -v : v;}

        /// @brief 최소값 구하기
        /// @param a : 비교A
        /// @param b : 비교B
        /// @return 최소값
        /// @see Max
        inline static sint32 Min(const sint32 a, const sint32 b)
        {return (a < b)? a : b;}

        /// @brief 최소값 구하기(float)
        /// @param a : 비교A
        /// @param b : 비교B
        /// @return 최소값
        /// @see MaxF
        static float MinF(const float a, const float b);

        /// @brief 최대값 구하기
        /// @param a : 비교A
        /// @param b : 비교B
        /// @return 최대값
        /// @see Min
        inline static sint32 Max(const sint32 a, const sint32 b)
        {return (a > b)? a : b;}

        /// @brief 최대값 구하기(float)
        /// @param a : 비교A
        /// @param b : 비교B
        /// @return 최대값
        /// @see MinF
        static float MaxF(const float a, const float b);

        /// @brief 보정값 구하기
        /// @param v : 인수
        /// @param low : 하한값
        /// @param high : 상한값
        /// @return 보정값
        inline static sint32 Clamp(const sint32 v, const sint32 low, const sint32 high)
        {return Max(low, Min(v, high));}

        /// @brief 보정값 구하기(float)
        /// @param v : 인수
        /// @param low : 하한값
        /// @param high : 상한값
        /// @return 보정값
        inline static float ClampF(const float v, const float low, const float high)
        {return MaxF(low, MinF(v, high));}

        /// @brief 순환값 구하기
        /// @param v : 인수
        /// @param low : 순환시작
        /// @param len : 순환구간
        /// @return 순환값
        static sint32 Cycle(const sint32 v, const sint32 low, const sint32 len);

        /// @brief 순환값 구하기(float)
        /// @param v : 인수
        /// @param low : 순환시작
        /// @param len : 순환구간
        /// @return 순환값
        static float CycleF(const float v, const float low, const float len);

        /// @brief 올림값 구하기
        /// @param v : 인수
        /// @return 올림값
        static float Ceil(const float v);

        /// @brief 내림값 구하기
        /// @param v : 인수
        /// @return 내림값
        static float Floor(const float v);

        /// @brief 반올림값 구하기
        /// @param v : 인수
        /// @return 반올림값
        static float Round(const float v);

        /// @brief 나머지 구하기
        /// @param a : 피제수
        /// @param b : 제수
        /// @return 나머지
        static float Mod(const float a, const float b);

        /// @brief 제곱 구하기
        /// @param v : 인수
        /// @return 제곱
        inline static float Pow(const float v)
        {return v * v;}

        /// @brief 제곱근 구하기
        /// @param v : 인수
        /// @return 제곱근
        static float Sqrt(const float v);

        /// @brief 코사인 구하기
        /// @param rad : 라디안
        /// @return 코사인값
        static float Cos(const float rad);

        /// @brief 사인 구하기
        /// @param rad : 라디안
        /// @return 사인값
        static float Sin(const float rad);

        /// @brief 탄젠트 구하기
        /// @param rad : 라디안
        /// @return 탄젠트값
        static float Tan(const float rad);

        /// @brief 아크탄젠트 구하기
        /// @param x : 변화량X
        /// @param y : 변화량Y
        /// @return 아크탄젠트
        static float Atan(const float x, const float y);

        /// @brief 각도를 라디안으로 변환하기
        /// @param deg : 각도
        /// @return 라디안
        inline static float ToRadian(const float deg)
        {return deg * PI() / 180.0f;}

        /// @brief 라디안을 각도로 변환하기
        /// @param rad : 라디안
        /// @return 각도
        inline static float ToDegree(const float rad)
        {return rad * 180.0f / PI();}

        /// @brief 거리 구하기
        /// @param x1 : 좌표X1
        /// @param y1 : 좌표Y1
        /// @param x2 : 좌표X2
        /// @param y2 : 좌표Y2
        /// @return 거리
        static float Distance(const float x1, const float y1, const float x2, const float y2);

        /// @brief 랜덤값 구하기
        /// @return 랜덤값(0.0f에서 1.0f포함까지)
        static float Random();

        /// @brief PI값 얻기
        /// @return PI값
        inline static float PI()
        {return 3.1415926535897932385F;}

        /// @brief float의 최대값 얻기
        /// @return float의 최대값
        inline static float FloatMax()
        {return 3.40282346638528859812e+38F;}

        /// @brief float의 최소값 얻기
        /// @return float의 최소값
        inline static float FloatMin()
        {return 1.17549435082228750797e-38F;}

        /// @brief double의 최대값 얻기
        /// @return double의 최대값
        inline static double DoubleMax()
        {return (double) 1.79769313486231570815e+308L;}

        /// @brief double의 최소값 얻기
        /// @return double의 최소값
        inline static double DoubleMin()
        {return (double) 2.22507385850720138309e-308L;}
    };
}
