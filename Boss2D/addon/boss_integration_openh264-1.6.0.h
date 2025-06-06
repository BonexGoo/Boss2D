﻿#pragma once
#include <addon/boss_fakewin.h>

#undef min

////////////////////////////////////////////////////////////////////////////////////////////////////
// _BOSS_BATCH_COMMAND_ {"type" : "include-alias", "prm" : "BOSS_OPENH264_", "restore-comment" : " //original-code:"}
#define BOSS_OPENH264_V_measure_time_h                <addon/openh264-1.6.0_for_boss/codec/common/inc/measure_time.h>

#define BOSS_OPENH264_U_codec_api_h                   "addon/openh264-1.6.0_for_boss/codec/api/svc/codec_api.h"
#define BOSS_OPENH264_U_codec_app_def_h               "addon/openh264-1.6.0_for_boss/codec/api/svc/codec_app_def.h"
#define BOSS_OPENH264_U_codec_def_h                   "addon/openh264-1.6.0_for_boss/codec/api/svc/codec_def.h"
#define BOSS_OPENH264_U_codec_ver_h                   "addon/openh264-1.6.0_for_boss/codec/api/svc/codec_ver.h"
#define BOSS_OPENH264_U_copy_mb_h                     "addon/openh264-1.6.0_for_boss/codec/common/inc/copy_mb.h"
#define BOSS_OPENH264_U_cpu_h                         "addon/openh264-1.6.0_for_boss/codec/common/inc/cpu.h"
#define BOSS_OPENH264_U_cpu_core_h                    "addon/openh264-1.6.0_for_boss/codec/common/inc/cpu_core.h"
#define BOSS_OPENH264_U_crt_util_safe_x_h             "addon/openh264-1.6.0_for_boss/codec/common/inc/crt_util_safe_x.h"
#define BOSS_OPENH264_U_deblocking_common_h           "addon/openh264-1.6.0_for_boss/codec/common/inc/deblocking_common.h"
#define BOSS_OPENH264_U_expand_pic_h                  "addon/openh264-1.6.0_for_boss/codec/common/inc/expand_pic.h"
#define BOSS_OPENH264_U_golomb_common_h               "addon/openh264-1.6.0_for_boss/codec/common/inc/golomb_common.h"
#define BOSS_OPENH264_U_intra_pred_common_h           "addon/openh264-1.6.0_for_boss/codec/common/inc/intra_pred_common.h"
#define BOSS_OPENH264_U_ls_defines_h                  "addon/openh264-1.6.0_for_boss/codec/common/inc/ls_defines.h"
#define BOSS_OPENH264_U_macros_h                      "addon/openh264-1.6.0_for_boss/codec/common/inc/macros.h"
#define BOSS_OPENH264_U_mc_h                          "addon/openh264-1.6.0_for_boss/codec/common/inc/mc.h"
#define BOSS_OPENH264_U_measure_time_h                "addon/openh264-1.6.0_for_boss/codec/common/inc/measure_time.h"
#define BOSS_OPENH264_U_memory_align_h                "addon/openh264-1.6.0_for_boss/codec/common/inc/memory_align.h"
#define BOSS_OPENH264_U_sad_common_h                  "addon/openh264-1.6.0_for_boss/codec/common/inc/sad_common.h"
#define BOSS_OPENH264_U_typedefs_h                    "addon/openh264-1.6.0_for_boss/codec/common/inc/typedefs.h"
#define BOSS_OPENH264_U_utils_h                       "addon/openh264-1.6.0_for_boss/codec/common/inc/utils.h"
#define BOSS_OPENH264_U_version_h                     "addon/openh264-1.6.0_for_boss/codec/common/inc/version.h"
#define BOSS_OPENH264_U_WelsCircleQueue_h             "addon/openh264-1.6.0_for_boss/codec/common/inc/WelsCircleQueue.h"
#define BOSS_OPENH264_U_welsCodecTrace_h              "addon/openh264-1.6.0_for_boss/codec/common/inc/welsCodecTrace.h"
#define BOSS_OPENH264_U_WelsList_h                    "addon/openh264-1.6.0_for_boss/codec/common/inc/WelsList.h"
#define BOSS_OPENH264_U_WelsLock_h                    "addon/openh264-1.6.0_for_boss/codec/common/inc/WelsLock.h"
#define BOSS_OPENH264_U_WelsTask_h                    "addon/openh264-1.6.0_for_boss/codec/common/inc/WelsTask.h"
#define BOSS_OPENH264_U_WelsTaskThread_h              "addon/openh264-1.6.0_for_boss/codec/common/inc/WelsTaskThread.h"
#define BOSS_OPENH264_U_WelsThread_h                  "addon/openh264-1.6.0_for_boss/codec/common/inc/WelsThread.h"
#define BOSS_OPENH264_U_WelsThreadLib_h               "addon/openh264-1.6.0_for_boss/codec/common/inc/WelsThreadLib.h"
#define BOSS_OPENH264_U_WelsThreadPool_h              "addon/openh264-1.6.0_for_boss/codec/common/inc/WelsThreadPool.h"
#define BOSS_OPENH264_U_wels_common_defs_h            "addon/openh264-1.6.0_for_boss/codec/common/inc/wels_common_defs.h"
#define BOSS_OPENH264_U_wels_const_common_h           "addon/openh264-1.6.0_for_boss/codec/common/inc/wels_const_common.h"
#define BOSS_OPENH264_U_IWelsVP_h                     "addon/openh264-1.6.0_for_boss/codec/processing/interface/IWelsVP.h"
#define BOSS_OPENH264_U_common_h                      "addon/openh264-1.6.0_for_boss/codec/processing/src/common/common.h"
#define BOSS_OPENH264_U_memory_h                      "addon/openh264-1.6.0_for_boss/codec/processing/src/common/memory.h"
#define BOSS_OPENH264_U_resource_h                    "addon/openh264-1.6.0_for_boss/codec/processing/src/common/resource.h"
#define BOSS_OPENH264_U_typedef_h                     "addon/openh264-1.6.0_for_boss/codec/processing/src/common/typedef.h"
#define BOSS_OPENH264_U_util_h                        "addon/openh264-1.6.0_for_boss/codec/processing/src/common/util.h"
#define BOSS_OPENH264_U_WelsFrameWork_h               "addon/openh264-1.6.0_for_boss/codec/processing/src/common/WelsFrameWork.h"

#if defined(_INCLUDE_FOR_ENCODER_)
    #define BOSS_OPENH264_U_as264_common_h                "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/as264_common.h"
    #define BOSS_OPENH264_U_au_set_h                      "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/au_set.h"
    #define BOSS_OPENH264_U_deblocking_h                  "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/deblocking.h"
    #define BOSS_OPENH264_U_decode_mb_aux_h               "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/decode_mb_aux.h"
    #define BOSS_OPENH264_U_dq_map_h                      "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/dq_map.h"
    #define BOSS_OPENH264_U_encoder_h                     "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/encoder.h"
    #define BOSS_OPENH264_U_encoder_context_h             "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/encoder_context.h"
    #define BOSS_OPENH264_U_encode_mb_aux_h               "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/encode_mb_aux.h"
    #define BOSS_OPENH264_U_extern_h                      "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/extern.h"
    #define BOSS_OPENH264_U_get_intra_predictor_h         "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/get_intra_predictor.h"
    #define BOSS_OPENH264_U_mb_cache_h                    "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/mb_cache.h"
    #define BOSS_OPENH264_U_md_h                          "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/md.h"
    #define BOSS_OPENH264_U_mt_defs_h                     "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/mt_defs.h"
    #define BOSS_OPENH264_U_mv_pred_h                     "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/mv_pred.h"
    #define BOSS_OPENH264_U_nal_encap_h                   "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/nal_encap.h"
    #define BOSS_OPENH264_U_parameter_sets_h              "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/parameter_sets.h"
    #define BOSS_OPENH264_U_param_svc_h                   "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/param_svc.h"
    #define BOSS_OPENH264_U_paraset_strategy_h            "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/paraset_strategy.h"
    #define BOSS_OPENH264_U_picture_h                     "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/picture.h"
    #define BOSS_OPENH264_U_picture_handle_h              "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/picture_handle.h"
    #define BOSS_OPENH264_U_rc_h                          "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/rc.h"
    #define BOSS_OPENH264_U_ref_list_mgr_svc_h            "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/ref_list_mgr_svc.h"
    #define BOSS_OPENH264_U_sample_h                      "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/sample.h"
    #define BOSS_OPENH264_U_set_mb_syn_cabac_h            "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/set_mb_syn_cabac.h"
    #define BOSS_OPENH264_U_set_mb_syn_cavlc_h            "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/set_mb_syn_cavlc.h"
    #define BOSS_OPENH264_U_slice_h                       "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/slice.h"
    #define BOSS_OPENH264_U_slice_multi_threading_h       "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/slice_multi_threading.h"
    #define BOSS_OPENH264_U_stat_h                        "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/stat.h"
    #define BOSS_OPENH264_U_svc_base_layer_md_h           "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/svc_base_layer_md.h"
    #define BOSS_OPENH264_U_svc_encode_mb_h               "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/svc_encode_mb.h"
    #define BOSS_OPENH264_U_svc_encode_slice_h            "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/svc_encode_slice.h"
    #define BOSS_OPENH264_U_svc_enc_frame_h               "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/svc_enc_frame.h"
    #define BOSS_OPENH264_U_svc_enc_golomb_h              "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/svc_enc_golomb.h"
    #define BOSS_OPENH264_U_svc_enc_macroblock_h          "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/svc_enc_macroblock.h"
    #define BOSS_OPENH264_U_svc_enc_slice_segment_h       "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/svc_enc_slice_segment.h"
    #define BOSS_OPENH264_U_svc_mode_decision_h           "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/svc_mode_decision.h"
    #define BOSS_OPENH264_U_svc_motion_estimate_h         "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/svc_motion_estimate.h"
    #define BOSS_OPENH264_U_svc_set_mb_syn_h              "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/svc_set_mb_syn.h"
    #define BOSS_OPENH264_U_svc_set_mb_syn_cavlc_h        "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/svc_set_mb_syn_cavlc.h"
    #define BOSS_OPENH264_U_vlc_encoder_h                 "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/vlc_encoder.h"
    #define BOSS_OPENH264_U_wels_common_basis_h           "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/wels_common_basis.h"
    #define BOSS_OPENH264_U_wels_const_h                  "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/wels_const.h"
    #define BOSS_OPENH264_U_wels_func_ptr_def_h           "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/wels_func_ptr_def.h"
    #define BOSS_OPENH264_U_wels_preprocess_h             "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/wels_preprocess.h"
    #define BOSS_OPENH264_U_wels_task_base_h              "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/wels_task_base.h"
    #define BOSS_OPENH264_U_wels_task_encoder_h           "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/wels_task_encoder.h"
    #define BOSS_OPENH264_U_wels_task_management_h        "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/wels_task_management.h"
    #define BOSS_OPENH264_U_wels_transpose_matrix_h       "addon/openh264-1.6.0_for_boss/codec/encoder/core/inc/wels_transpose_matrix.h"
    #define BOSS_OPENH264_U_welsEncoderExt_h              "addon/openh264-1.6.0_for_boss/codec/encoder/plus/inc/welsEncoderExt.h"
#elif defined(_INCLUDE_FOR_DECODER_)
    #define BOSS_OPENH264_U_au_parser_h                   "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/au_parser.h"
    #define BOSS_OPENH264_U_bit_stream_h                  "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/bit_stream.h"
    #define BOSS_OPENH264_U_cabac_decoder_h               "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/cabac_decoder.h"
    #define BOSS_OPENH264_U_deblocking_h                  "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/deblocking.h"
    #define BOSS_OPENH264_U_decoder_h                     "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/decoder.h"
    #define BOSS_OPENH264_U_decoder_context_h             "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/decoder_context.h"
    #define BOSS_OPENH264_U_decoder_core_h                "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/decoder_core.h"
    #define BOSS_OPENH264_U_decode_mb_aux_h               "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/decode_mb_aux.h"
    #define BOSS_OPENH264_U_decode_slice_h                "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/decode_slice.h"
    #define BOSS_OPENH264_U_dec_frame_h                   "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/dec_frame.h"
    #define BOSS_OPENH264_U_dec_golomb_h                  "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/dec_golomb.h"
    #define BOSS_OPENH264_U_error_code_h                  "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/error_code.h"
    #define BOSS_OPENH264_U_error_concealment_h           "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/error_concealment.h"
    #define BOSS_OPENH264_U_fmo_h                         "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/fmo.h"
    #define BOSS_OPENH264_U_get_intra_predictor_h         "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/get_intra_predictor.h"
    #define BOSS_OPENH264_U_manage_dec_ref_h              "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/manage_dec_ref.h"
    #define BOSS_OPENH264_U_mb_cache_h                    "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/mb_cache.h"
    #define BOSS_OPENH264_U_memmgr_nal_unit_h             "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/memmgr_nal_unit.h"
    #define BOSS_OPENH264_U_mv_pred_h                     "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/mv_pred.h"
    #define BOSS_OPENH264_U_nalu_h                        "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/nalu.h"
    #define BOSS_OPENH264_U_nal_prefix_h                  "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/nal_prefix.h"
    #define BOSS_OPENH264_U_parameter_sets_h              "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/parameter_sets.h"
    #define BOSS_OPENH264_U_parse_mb_syn_cabac_h          "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/parse_mb_syn_cabac.h"
    #define BOSS_OPENH264_U_parse_mb_syn_cavlc_h          "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/parse_mb_syn_cavlc.h"
    #define BOSS_OPENH264_U_picture_h                     "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/picture.h"
    #define BOSS_OPENH264_U_pic_queue_h                   "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/pic_queue.h"
    #define BOSS_OPENH264_U_rec_mb_h                      "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/rec_mb.h"
    #define BOSS_OPENH264_U_slice_h                       "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/slice.h"
    #define BOSS_OPENH264_U_vlc_decoder_h                 "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/vlc_decoder.h"
    #define BOSS_OPENH264_U_wels_common_basis_h           "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/wels_common_basis.h"
    #define BOSS_OPENH264_U_wels_const_h                  "addon/openh264-1.6.0_for_boss/codec/decoder/core/inc/wels_const.h"
    #define BOSS_OPENH264_U_welsDecoderExt_h              "addon/openh264-1.6.0_for_boss/codec/decoder/plus/inc/welsDecoderExt.h"
#endif
