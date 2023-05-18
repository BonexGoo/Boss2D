#include <huecoding.h>

void OnDan(const char* text, int enter)
{
    if(enter)
    {
        int dan = boss_atoi(text);

        print("\n [%i단]\n", dan);
        for(int i = 1; i <= 9; ++i)
            print(" %i x %i = %2i\n", dan, i, dan * i);

        print("\n ");
        clickbox(8, 1, repaint);
        print("다시하기\n");
    }
}

HUE_DECLARE_APP("구구단마스터", googoodan)
void googoodan()
{
    clrscr(60, 20, "#ffffff");
    print("\n 구구단의 단(2~9)을 입력하세요 : ");

    setcolor("#ff0000");
    scan(4, OnDan);
    setcolor("#000000");
}
