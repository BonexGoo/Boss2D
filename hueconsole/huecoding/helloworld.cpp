#include <huecoding.h>

void OnClickOK()
{
    gotoxy(3, 3);
    setcolor("#ff00ff");
    setbgcolor("#ffffff");
    print("클릭했다");
}

void OnClickRepaint()
{
    repaint();
}

HUE_DECLARE_APP("헬로월드", helloworld)
void helloworld()
{
    setbgcolor("#ffffff");

    gotoxy(3, 3);
    setcolor("#ff0000");
    print("헬로월드!!!");

    gotoxy(3, 5);
    setcolor("#ff00ff");
    setbgcolor("#80e0e0");
    clickbox(4, 1, OnClickOK);
    print("클릭");

    gotoxy(12, 5);
    setcolor("#ff00ff");
    clickbox(9, 1, OnClickRepaint);
    print(" repaint ");
}

int a = 10;
int b = 30;

void OnScanResult(const char* text, int enter)
{
    if(enter)
    {
        int YourResult = boss_atoi(text);

        gotoxy(1, 5);
        print("당신의 답은 %d\n ---> %s", YourResult,
            (a * b == YourResult)? "정답입니다!!!" : "오답입니다ㅜㅜ");
    }
}

HUE_DECLARE_APP("수학공부", mathholic)
void mathholic()
{
    clrscr(80, 30, "#ffffee");

    gotoxy(8, 1);
    print("[수학홀릭]");

    gotoxy(1, 3);
    print("%d x %d = ", a, b);
    scan(5, OnScanResult);
}
