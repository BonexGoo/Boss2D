#include <boss.h>
#if BOSS_NEED_ADDON_H264

#include "boss_integration_openh264-1.6.0.h"

#include <addon/openh264-1.6.0_for_boss/codec/common/src/common_tables.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/common/src/copy_mb.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/common/src/cpu.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/common/src/crt_util_safe_x.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/common/src/deblocking_common.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/common/src/expand_pic.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/common/src/intra_pred_common.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/common/src/mc.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/common/src/memory_align.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/common/src/sad_common.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/common/src/utils.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/common/src/welsCodecTrace.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/common/src/WelsTaskThread.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/common/src/WelsThread.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/common/src/WelsThreadLib.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/common/src/WelsThreadPool.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/processing/src/adaptivequantization/AdaptiveQuantization.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/processing/src/backgrounddetection/BackgroundDetection.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/processing/src/common/WelsFrameWork.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/processing/src/common/WelsFrameWorkEx.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/processing/src/common/memory.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/processing/src/complexityanalysis/ComplexityAnalysis.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/processing/src/denoise/denoise.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/processing/src/denoise/denoise_filter.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/processing/src/downsample/downsample.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/processing/src/downsample/downsamplefuncs.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/processing/src/imagerotate/imagerotate.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/processing/src/imagerotate/imagerotatefuncs.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/processing/src/scenechangedetection/SceneChangeDetection.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/processing/src/scrolldetection/ScrollDetection.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/processing/src/scrolldetection/ScrollDetectionFuncs.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/processing/src/vaacalc/vaacalcfuncs.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/processing/src/vaacalc/vaacalculation.cpp>

#endif
