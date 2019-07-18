#include <boss.h>
#if BOSS_NEED_ADDON_AAC

#include "boss_integration_fdk-aac-0.1.4.h"

#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/aacenc.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/aacenc_hcr.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/aacenc_lib.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/aacenc_pns.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/aacEnc_ram.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/aacEnc_rom.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/aacenc_tns.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/adj_thr.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/bandwidth.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/band_nrg.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/bitenc.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/bit_cnt.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/block_switch.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/channel_map.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/chaosmeasure.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/dyn_bits.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/grp_data.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/intensity.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/line_pe.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/metadata_compressor.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/metadata_main.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/ms_stereo.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/noisedet.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/pnsparam.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/pre_echo_control.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/psy_configuration.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/psy_main.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/qc_main.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/quantize.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/sf_estim.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/spreading.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/tonality.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libAACenc/src/transform.cpp>

#include <addon/fdk-aac-0.1.4_for_boss/libFDK/src/autocorr2nd.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libFDK/src/dct.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libFDK/src/FDK_bitbuffer.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libFDK/src/FDK_core.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libFDK/src/FDK_crc.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libFDK/src/FDK_hybrid.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libFDK/src/FDK_tools_rom.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libFDK/src/FDK_trigFcts.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libFDK/src/fft.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libFDK/src/fft_rad2.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libFDK/src/fixpoint_math.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libFDK/src/mdct.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libFDK/src/qmf.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libFDK/src/scale.cpp>

#include <addon/fdk-aac-0.1.4_for_boss/libSYS/src/cmdl_parser.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSYS/src/conv_string.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSYS/src/genericStds.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSYS/src/wav_file.cpp>

#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/bit_sbr.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/code_env.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/env_bit.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/env_est.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/fram_gen.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/invf_est.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/mh_det.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/nf_est.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/ps_bitenc.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/ps_encode.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/ps_main.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/resampler.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/sbrenc_freq_sca.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/sbr_encoder.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/sbr_misc.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/sbr_ram.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/sbr_rom.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/ton_corr.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libSBRenc/src/tran_det.cpp>

#include <addon/fdk-aac-0.1.4_for_boss/libMpegTPEnc/src/tpenc_adif.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libMpegTPEnc/src/tpenc_adts.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libMpegTPEnc/src/tpenc_asc.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libMpegTPEnc/src/tpenc_latm.cpp>
#include <addon/fdk-aac-0.1.4_for_boss/libMpegTPEnc/src/tpenc_lib.cpp>

#endif
