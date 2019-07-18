#pragma once
#include <addon/boss_fakewin.h>

#if BOSS_WINDOWS
    #define __x86__ // 32/64에서 모두 본 매크로가 필요
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// _BOSS_BATCH_COMMAND_ {"type" : "include-alias", "prm" : "BOSS_FDKAAC_", "restore-comment" : " //original-code:"}
#define BOSS_FDKAAC_U_audio_h                         "addon/fdk-aac-0.1.4_for_boss/libSYS/include/audio.h"
#define BOSS_FDKAAC_U_cmdl_parser_h                   "addon/fdk-aac-0.1.4_for_boss/libSYS/include/cmdl_parser.h"
#define BOSS_FDKAAC_U_conv_string_h                   "addon/fdk-aac-0.1.4_for_boss/libSYS/include/conv_string.h"
#define BOSS_FDKAAC_U_FDK_audio_h                     "addon/fdk-aac-0.1.4_for_boss/libSYS/include/FDK_audio.h"
#define BOSS_FDKAAC_U_genericStds_h                   "addon/fdk-aac-0.1.4_for_boss/libSYS/include/genericStds.h"
#define BOSS_FDKAAC_U_machine_type_h                  "addon/fdk-aac-0.1.4_for_boss/libSYS/include/machine_type.h"
#define BOSS_FDKAAC_U_wav_file_h                      "addon/fdk-aac-0.1.4_for_boss/libSYS/include/wav_file.h"
#define BOSS_FDKAAC_U_abs_h                           "addon/fdk-aac-0.1.4_for_boss/libFDK/include/abs.h"
#define BOSS_FDKAAC_U_autocorr2nd_h                   "addon/fdk-aac-0.1.4_for_boss/libFDK/include/autocorr2nd.h"
#define BOSS_FDKAAC_U_clz_h                           "addon/fdk-aac-0.1.4_for_boss/libFDK/include/clz.h"
#define BOSS_FDKAAC_U_common_fix_h                    "addon/fdk-aac-0.1.4_for_boss/libFDK/include/common_fix.h"
#define BOSS_FDKAAC_U_cplx_mul_h                      "addon/fdk-aac-0.1.4_for_boss/libFDK/include/cplx_mul.h"
#define BOSS_FDKAAC_U_dct_h                           "addon/fdk-aac-0.1.4_for_boss/libFDK/include/dct.h"
#define BOSS_FDKAAC_U_FDK_archdef_h                   "addon/fdk-aac-0.1.4_for_boss/libFDK/include/FDK_archdef.h"
#define BOSS_FDKAAC_U_FDK_bitbuffer_h                 "addon/fdk-aac-0.1.4_for_boss/libFDK/include/FDK_bitbuffer.h"
#define BOSS_FDKAAC_U_FDK_bitstream_h                 "addon/fdk-aac-0.1.4_for_boss/libFDK/include/FDK_bitstream.h"
#define BOSS_FDKAAC_U_FDK_core_h                      "addon/fdk-aac-0.1.4_for_boss/libFDK/include/FDK_core.h"
#define BOSS_FDKAAC_U_FDK_crc_h                       "addon/fdk-aac-0.1.4_for_boss/libFDK/include/FDK_crc.h"
#define BOSS_FDKAAC_U_FDK_hybrid_h                    "addon/fdk-aac-0.1.4_for_boss/libFDK/include/FDK_hybrid.h"
#define BOSS_FDKAAC_U_FDK_tools_rom_h                 "addon/fdk-aac-0.1.4_for_boss/libFDK/include/FDK_tools_rom.h"
#define BOSS_FDKAAC_U_FDK_trigFcts_h                  "addon/fdk-aac-0.1.4_for_boss/libFDK/include/FDK_trigFcts.h"
#define BOSS_FDKAAC_U_fft_h                           "addon/fdk-aac-0.1.4_for_boss/libFDK/include/fft.h"
#define BOSS_FDKAAC_U_fft_rad2_h                      "addon/fdk-aac-0.1.4_for_boss/libFDK/include/fft_rad2.h"
#define BOSS_FDKAAC_U_fixmadd_h                       "addon/fdk-aac-0.1.4_for_boss/libFDK/include/fixmadd.h"
#define BOSS_FDKAAC_U_fixminmax_h                     "addon/fdk-aac-0.1.4_for_boss/libFDK/include/fixminmax.h"
#define BOSS_FDKAAC_U_fixmul_h                        "addon/fdk-aac-0.1.4_for_boss/libFDK/include/fixmul.h"
#define BOSS_FDKAAC_U_fixpoint_math_h                 "addon/fdk-aac-0.1.4_for_boss/libFDK/include/fixpoint_math.h"
#define BOSS_FDKAAC_U_mdct_h                          "addon/fdk-aac-0.1.4_for_boss/libFDK/include/mdct.h"
#define BOSS_FDKAAC_U_qmf_h                           "addon/fdk-aac-0.1.4_for_boss/libFDK/include/qmf.h"
#define BOSS_FDKAAC_U_scale_h                         "addon/fdk-aac-0.1.4_for_boss/libFDK/include/scale.h"
#define BOSS_FDKAAC_U_scramble_h                      "addon/fdk-aac-0.1.4_for_boss/libFDK/include/scramble.h"
#define BOSS_FDKAAC_U_mpegFileWrite_h                 "addon/fdk-aac-0.1.4_for_boss/libMpegTPEnc/include/mpegFileWrite.h"
#define BOSS_FDKAAC_U_tp_data_h                       "addon/fdk-aac-0.1.4_for_boss/libMpegTPEnc/include/tp_data.h"
#define BOSS_FDKAAC_U_tpenc_lib_h                     "addon/fdk-aac-0.1.4_for_boss/libMpegTPEnc/include/tpenc_lib.h"
#define BOSS_FDKAAC_U_sbr_encoder_h                   "addon/fdk-aac-0.1.4_for_boss/libSBRenc/include/sbr_encoder.h"
#define BOSS_FDKAAC_U_aacenc_lib_h                    "addon/fdk-aac-0.1.4_for_boss/libAACenc/include/aacenc_lib.h"
