#include <boss.h>
#if BOSS_NEED_ADDON_H264

#define _INCLUDE_FOR_ENCODER_
#include "boss_integration_openh264-1.6.0.h"

#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/au_set.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/deblocking.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/decode_mb_aux.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/encoder.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/encoder_data_tables.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/encoder_ext.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/encode_mb_aux.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/get_intra_predictor.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/md.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/mv_pred.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/nal_encap.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/paraset_strategy.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/picture_handle.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/ratectl.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/ref_list_mgr_svc.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/sample.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/set_mb_syn_cabac.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/set_mb_syn_cavlc.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/slice_multi_threading.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/svc_base_layer_md.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/svc_encode_mb.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/svc_encode_slice.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/svc_enc_slice_segment.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/svc_mode_decision.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/svc_motion_estimate.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/svc_set_mb_syn_cabac.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/svc_set_mb_syn_cavlc.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/wels_preprocess.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/wels_task_base.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/wels_task_encoder.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/core/src/wels_task_management.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/encoder/plus/src/welsEncoderExt.cpp>

#endif
