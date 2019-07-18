
#ifndef OPENCV_FLANN_DUMMY_H_
#define OPENCV_FLANN_DUMMY_H_

namespace cvflann
{

#if (defined WIN32 || defined _WIN32 || defined WINCE) && defined CVAPI_EXPORTS
BOSS_FAKEWIN_DECLSPEC_DLLEXPORT
#endif
void dummyfunc();

}


#endif  /* OPENCV_FLANN_DUMMY_H_ */
