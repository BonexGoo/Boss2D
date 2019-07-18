// author BOSS

#ifndef OPENALPR_TIMING_H
#define OPENALPR_TIMING_H

#include <iostream>
#include <ctime>
#include BOSS_FAKEWIN_V_stdint_h //original-code:<stdint.h>



#if BOSS_WINDOWS & !BOSS_WINDOWS_MINGW // modified by BOSS, original-code: #ifdef WINDOWS
    // Import windows only stuff
	#include BOSS_FAKEWIN_V_windows_h //original-code:<windows.h>
	#if _MSC_VER < 1900
        struct timespec
        {
                time_t tv_sec;  // Seconds - >= 0
                long   tv_nsec; // Nanoseconds - [0, 999999999]
        };
	#else

	#endif
#else
    #include <sys/time.h>
#endif

// Support for OS X
#if defined(__APPLE__) && defined(__MACH__)
#include <mach/clock.h>
#include <mach/mach.h>
#endif

namespace alpr
{

  void getTimeMonotonic(timespec* time);
  int64_t getTimeMonotonicMs();
  
  double diffclock(timespec time1,timespec time2);

  int64_t getEpochTimeMs();

}

#endif
