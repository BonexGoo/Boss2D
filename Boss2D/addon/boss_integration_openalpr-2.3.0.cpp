#include <boss.h>
#if BOSS_NEED_ADDON_ALPR
#include <iostream>
#include <functional>
#include <cctype>

#if BOSS_WINDOWS
    #pragma comment(lib, "gdi32.lib")
#endif

#include "boss_integration_openalpr-2.3.0.h"

#pragma warning(disable : 4305)
#define OPENALPR_MAJOR_VERSION "2"
#define OPENALPR_MINOR_VERSION "3"
#define OPENALPR_PATCH_VERSION "0"
#define NO_THREADS
#if BOSS_WINDOWS && !BOSS_NEED_FORCED_FAKEWIN
    #define INSTALL_PREFIX "../assets"
#else
    #define INSTALL_PREFIX "assets:"
#endif

#include <time.h>
#if _MSC_VER >= 1900
    #if BOSS_X64
        #define timespec timespec64_openalpr_BOSS
        typedef long long time64_t_openalpr_BOSS;
        struct timespec64_openalpr_BOSS
        {
            time64_t_openalpr_BOSS tv_sec;
            long tv_nsec;
        };
    #else
        #define timespec timespec32_openalpr_BOSS
        #ifdef __mips__
            typedef    long long time32_t_openalpr_BOSS;
        #else
            typedef    int time32_t_openalpr_BOSS;
        #endif
        struct timespec32_openalpr_BOSS
        {
            time32_t_openalpr_BOSS tv_sec;
            int tv_nsec;
        };
    #endif
#endif

#define sprintf_s sprintf_s_openalpr_BOSS
extern "C" int sprintf_s_openalpr_BOSS(char* str, int strlen, const char* format, ...)
{
    va_list Args;
    va_start(Args, format);
    int Result = boss_vsnprintf(str, strlen, format, Args);
    va_end(Args);
    return Result;
}

extern "C"
{
    void AlprDebug_AddRect(int x, int y, int w, int h);
    void AlprDebug_Flush(const char* subject, const char* comment);
}

#include <addon/openalpr-2.3.0_for_boss/src/openalpr/alpr.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/alpr_impl.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/config.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/config_helper.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/binarize_wolf.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/cjson.c>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/colorfilter.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/licenseplatecandidate.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/motiondetector.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/pipeline_data.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/prewarp.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/result_aggregator.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/transformation.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/utility.cpp>

#include <addon/openalpr-2.3.0_for_boss/src/openalpr/detection/detector.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/detection/detectorcpu.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/detection/detectorcuda.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/detection/detectorfactory.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/detection/detectormask.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/detection/detectormorph.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/detection/detectorocl.cpp>

#include <addon/openalpr-2.3.0_for_boss/src/openalpr/edges/edgefinder.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/edges/platecorners.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/edges/platelines.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/edges/scorekeeper.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/edges/textlinecollection.cpp>

#include <addon/openalpr-2.3.0_for_boss/src/openalpr/postprocess/postprocess.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/postprocess/regexrule.cpp>

extern "C"
{
    #include <addon/openalpr-2.3.0_for_boss/src/openalpr/simpleini/ConvertUTF.c>
}
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/simpleini/snippets.cpp>

#include <addon/openalpr-2.3.0_for_boss/src/openalpr/textdetection/characteranalysis.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/textdetection/linefinder.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/textdetection/platemask.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/textdetection/textcontours.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/textdetection/textline.cpp>

#include <addon/openalpr-2.3.0_for_boss/src/openalpr/ocr/ocr.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/ocr/ocrfactory.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/ocr/tesseract_ocr.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/ocr/segmentation/histogram.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/ocr/segmentation/histogramhorizontal.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/ocr/segmentation/charactersegmenter.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/ocr/segmentation/histogramvertical.cpp>

#include <addon/openalpr-2.3.0_for_boss/src/statedetection/featurematcher.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/statedetection/line_segment.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/statedetection/state_detector.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/statedetection/state_detector_impl.cpp>

#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/filesystem.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/platform.cpp>
#define clock_gettime clock_gettime_timing_BOSS
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/timing.cpp>
#undef clock_gettime
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/tinythread.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/utf8.cpp>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/version.cpp>

#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/bitstate.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/compile.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/dfa.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/filtered_re2.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/mimics_pcre.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/nfa.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/onepass.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/parse.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/perl_groups.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/prefilter.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/prefilter_tree.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/prog.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/re2.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/regexp.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/set.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/simplify.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/stringpiece.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/tostring.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/unicode_casefold.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/unicode_groups.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/util/hash.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/util/rune.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/util/stringprintf.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/util/strutil.cc>
#include <addon/openalpr-2.3.0_for_boss/src/openalpr/support/re2/util/valgrind.cc>

#endif
