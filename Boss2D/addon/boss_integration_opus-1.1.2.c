#include <boss.h>
#if BOSS_NEED_ADDON_WEBRTC

#include "boss_integration_opus-1.1.2.h"

#include <addon/opus-1.1.2_for_boss/src/analysis.c>
#include <addon/opus-1.1.2_for_boss/src/mlp.c>
#include <addon/opus-1.1.2_for_boss/src/mlp_data.c>
#include <addon/opus-1.1.2_for_boss/src/opus.c>
#include <addon/opus-1.1.2_for_boss/src/opus_compare.c>
#include <addon/opus-1.1.2_for_boss/src/opus_decoder.c>
#include <addon/opus-1.1.2_for_boss/src/opus_encoder.c>
#include <addon/opus-1.1.2_for_boss/src/opus_multistream.c>
#include <addon/opus-1.1.2_for_boss/src/opus_multistream_decoder.c>
#include <addon/opus-1.1.2_for_boss/src/opus_multistream_encoder.c>
#include <addon/opus-1.1.2_for_boss/src/repacketizer.c>

#if FIXED_POINT
    #include <addon/opus-1.1.2_for_boss/silk/fixed/apply_sine_window_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/autocorr_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/burg_modified_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/corrMatrix_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/encode_frame_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/find_LPC_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/find_LTP_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/find_pitch_lags_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/find_pred_coefs_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/k2a_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/k2a_Q16_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/LTP_analysis_filter_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/LTP_scale_ctrl_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/noise_shape_analysis_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/pitch_analysis_core_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/prefilter_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/process_gains_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/regularize_correlations_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/residual_energy16_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/residual_energy_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/schur64_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/schur_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/solve_LS_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/vector_ops_FIX.c>
    #include <addon/opus-1.1.2_for_boss/silk/fixed/warped_autocorrelation_FIX.c>
#else
    #include <addon/opus-1.1.2_for_boss/silk/float/apply_sine_window_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/autocorrelation_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/burg_modified_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/bwexpander_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/corrMatrix_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/encode_frame_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/energy_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/find_LPC_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/find_LTP_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/find_pitch_lags_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/find_pred_coefs_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/inner_product_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/k2a_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/levinsondurbin_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/LPC_analysis_filter_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/LPC_inv_pred_gain_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/LTP_analysis_filter_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/LTP_scale_ctrl_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/noise_shape_analysis_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/pitch_analysis_core_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/prefilter_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/process_gains_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/regularize_correlations_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/residual_energy_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/scale_copy_vector_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/scale_vector_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/schur_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/solve_LS_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/sort_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/warped_autocorrelation_FLP.c>
    #include <addon/opus-1.1.2_for_boss/silk/float/wrappers_FLP.c>
#endif

#endif
