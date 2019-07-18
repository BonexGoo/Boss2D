#ifndef OPENALPR_PLATFORM_H
#define OPENALPR_PLATFORM_H

#include <string.h>
#include <sstream>

#ifdef WINDOWS
	#include BOSS_FAKEWIN_V_windows_h //original-code:<windows.h>
#else
	#include <unistd.h>
#endif

namespace alpr
{

  void sleep_ms(int sleepMs);

  std::string getExeDir();

}

#endif //OPENALPR_PLATFORM_H
