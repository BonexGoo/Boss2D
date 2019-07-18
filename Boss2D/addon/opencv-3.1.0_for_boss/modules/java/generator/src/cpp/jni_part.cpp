#include "common.h"

#include BOSS_OPENCV_U_opencv2__opencv_modules_hpp //original-code:"opencv2/opencv_modules.hpp"

#ifdef HAVE_OPENCV_FEATURES2D
#  include BOSS_OPENCV_U_opencv2__features2d_hpp //original-code:"opencv2/features2d.hpp"
#endif

#ifdef HAVE_OPENCV_VIDEO
#  include BOSS_OPENCV_U_opencv2__video_hpp //original-code:"opencv2/video.hpp"
#endif

#ifdef HAVE_OPENCV_CONTRIB
#  include "opencv2/contrib.hpp"
#endif

extern "C" {

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* )
{
    JNIEnv* env;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK)
        return -1;

    /* get class with (*env)->FindClass */
    /* register methods with (*env)->RegisterNatives */

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL
JNI_OnUnload(JavaVM*, void*)
{
  //do nothing
}

} // extern "C"
