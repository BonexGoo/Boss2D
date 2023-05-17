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

HUE_DECLARE_APP("수학공부", mathholic)
void mathholic()
{
    clrscr(40, 40, "#ffffee");

    gotoxy(8, 1);
    print("[수학홀릭]");

    gotoxy(1, 3);
    clickbox(4, 1, OnClickOK);
    print("클릭");
}
