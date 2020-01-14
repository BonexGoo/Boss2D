#include <boss.h>
#if BOSS_NEED_ADDON_H264

#define _INCLUDE_FOR_DECODER_
#include "boss_integration_openh264-1.6.0.h"

#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/au_parser.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/bit_stream.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/cabac_decoder.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/deblocking.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/decode_mb_aux.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/decode_slice.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/decoder.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/decoder_core.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/decoder_data_tables.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/error_concealment.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/fmo.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/get_intra_predictor.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/manage_dec_ref.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/memmgr_nal_unit.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/mv_pred.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/parse_mb_syn_cabac.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/parse_mb_syn_cavlc.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/pic_queue.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/core/src/rec_mb.cpp>
#include <addon/openh264-1.6.0_for_boss/codec/decoder/plus/src/welsDecoderExt.cpp>

#endif
