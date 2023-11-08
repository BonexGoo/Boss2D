#include <boss.h>
#if BOSS_NEED_ADDON_WEBP

#include "boss_integration_libwebp-1.3.2.h"

extern "C"
{
    // sharpyuv_srcs
    namespace sharpyuv_c
    {
        using namespace sharpyuv_c;
        #include <addon/libwebp-1.3.2_for_boss/sharpyuv/sharpyuv.c>
    }
    //#include <addon/libwebp-1.3.2_for_boss/sharpyuv/sharpyuv_cpu.c> // src/dsp/cpu.c와 같음
    #include <addon/libwebp-1.3.2_for_boss/sharpyuv/sharpyuv_csp.c>
    #include <addon/libwebp-1.3.2_for_boss/sharpyuv/sharpyuv_dsp.c>
    #include <addon/libwebp-1.3.2_for_boss/sharpyuv/sharpyuv_gamma.c>
    #include <addon/libwebp-1.3.2_for_boss/sharpyuv/sharpyuv_neon.c>
    #include <addon/libwebp-1.3.2_for_boss/sharpyuv/sharpyuv_sse2.c>

    // dec_srcs
    #include <addon/libwebp-1.3.2_for_boss/src/dec/alpha_dec.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dec/buffer_dec.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dec/frame_dec.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dec/idec_dec.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dec/io_dec.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dec/quant_dec.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dec/tree_dec.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dec/vp8_dec.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dec/vp8l_dec.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dec/webp_dec.c>

    // demux_srcs
    #include <addon/libwebp-1.3.2_for_boss/src/demux/anim_decode.c>
    #include <addon/libwebp-1.3.2_for_boss/src/demux/demux.c>

    // dsp_dec_srcs
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/alpha_processing.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/alpha_processing_mips_dsp_r2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/alpha_processing_neon.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/alpha_processing_sse2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/alpha_processing_sse41.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/cpu.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/dec.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/dec_clip_tables.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/dec_mips32.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/dec_mips_dsp_r2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/dec_msa.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/dec_neon.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/dec_sse2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/dec_sse41.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/filters.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/filters_mips_dsp_r2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/filters_msa.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/filters_neon.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/filters_sse2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/lossless.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/lossless_mips_dsp_r2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/lossless_msa.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/lossless_neon.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/lossless_sse2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/lossless_sse41.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/rescaler.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/rescaler_mips32.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/rescaler_mips_dsp_r2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/rescaler_msa.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/rescaler_neon.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/rescaler_sse2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/upsampling.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/upsampling_mips_dsp_r2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/upsampling_msa.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/upsampling_neon.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/upsampling_sse2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/upsampling_sse41.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/yuv.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/yuv_mips32.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/yuv_mips_dsp_r2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/yuv_neon.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/yuv_sse2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/yuv_sse41.c>

    // dsp_enc_srcs
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/cost.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/cost_mips32.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/cost_mips_dsp_r2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/cost_neon.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/cost_sse2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/enc_mips32.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/enc_mips_dsp_r2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/enc_msa.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/enc_neon.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/enc_sse2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/enc_sse41.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/lossless_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/lossless_enc_mips32.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/lossless_enc_mips_dsp_r2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/lossless_enc_msa.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/lossless_enc_neon.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/lossless_enc_sse2.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/lossless_enc_sse41.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/ssim.c>
    #include <addon/libwebp-1.3.2_for_boss/src/dsp/ssim_sse2.c>

    // enc_srcs
    #include <addon/libwebp-1.3.2_for_boss/src/enc/alpha_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/analysis_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/backward_references_cost_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/backward_references_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/config_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/cost_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/filter_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/frame_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/histogram_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/iterator_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/near_lossless_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/picture_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/picture_csp_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/picture_psnr_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/picture_rescale_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/picture_tools_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/predictor_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/quant_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/syntax_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/token_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/tree_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/vp8l_enc.c>
    #include <addon/libwebp-1.3.2_for_boss/src/enc/webp_enc.c>

    // mux_srcs
    #include <addon/libwebp-1.3.2_for_boss/src/mux/anim_encode.c>
    #include <addon/libwebp-1.3.2_for_boss/src/mux/muxedit.c>
    #include <addon/libwebp-1.3.2_for_boss/src/mux/muxinternal.c>
    #include <addon/libwebp-1.3.2_for_boss/src/mux/muxread.c>

    // utils_dec_srcs
    #include <addon/libwebp-1.3.2_for_boss/src/utils/bit_reader_utils.c>
    #include <addon/libwebp-1.3.2_for_boss/src/utils/color_cache_utils.c>
    #include <addon/libwebp-1.3.2_for_boss/src/utils/filters_utils.c>
    #include <addon/libwebp-1.3.2_for_boss/src/utils/huffman_utils.c>
    #include <addon/libwebp-1.3.2_for_boss/src/utils/palette.c>
    #include <addon/libwebp-1.3.2_for_boss/src/utils/quant_levels_dec_utils.c>
    #include <addon/libwebp-1.3.2_for_boss/src/utils/random_utils.c>
    #include <addon/libwebp-1.3.2_for_boss/src/utils/rescaler_utils.c>
    #include <addon/libwebp-1.3.2_for_boss/src/utils/thread_utils.c>
    #include <addon/libwebp-1.3.2_for_boss/src/utils/utils.c>

    // utils_enc_srcs
    #include <addon/libwebp-1.3.2_for_boss/src/utils/bit_writer_utils.c>
    #include <addon/libwebp-1.3.2_for_boss/src/utils/huffman_encode_utils.c>
    #include <addon/libwebp-1.3.2_for_boss/src/utils/quant_levels_utils.c>
}

#endif
