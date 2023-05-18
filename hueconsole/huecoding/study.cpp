#include <huecoding.h>

HUE_DECLARE_APP("스터디23", main_study23)
void main_study23()
{
    int sum;
    int books = 3;
    int penalty = 1500000000000000000;
    int extra_penalty = 100;
    sum = books * ((45 - 3) * penalty + (45 - 30) * extra_penalty);

    print("책 3권을 45일 후에 반납 시 과태료눈 %i원입니다.\n", sum);
}

HUE_DECLARE_APP("스터디01", main_study01)
void main_study01()
{
    print("팔푼이 \n");
}
