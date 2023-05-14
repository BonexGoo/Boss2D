#include <huecoding.h>

void OnClickOK()
{
}

HUE_DECLARE_APP("헬로월드", helloworld)
void helloworld()
{
    gotoxy(3, 3);
    print("헬로월드!");

    gotoxy(3, 5);
    clickbox(2, 1, OnClickOK);
    print("클릭");
}

HUE_DECLARE_APP("헬로월드2", helloworld2)
void helloworld2()
{
    gotoxy(5, 5);
    print("헬로월드!");

    gotoxy(1, 1);
    clickbox(2, 1, OnClickOK);
    print("클릭");
}
