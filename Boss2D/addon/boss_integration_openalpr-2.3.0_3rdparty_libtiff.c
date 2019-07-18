#include <boss.h>
#if BOSS_NEED_ADDON_ALPR | BOSS_NEED_ADDON_TESSERACT

#include "boss_integration_openalpr-2.3.0_3rdparty_libtiff.h"

// define for tesseract
#define int8 int8_tesseract
#define uint8 uint8_tesseract
#define int16 int16_tesseract
#define uint16 uint16_tesseract
#define int32 int32_tesseract
#define uint32 uint32_tesseract

#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_aux.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_close.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_codec.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_color.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_compress.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_dir.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_dirinfo.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_dirread.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_dirwrite.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_dumpmode.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_error.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_extension.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_fax3.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_fax3sm.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_flush.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_getimage.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_jbig.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_jpeg.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_luv.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_lzw.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_next.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_ojpeg.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_open.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_packbits.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_pixarlog.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_predict.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_print.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_read.c>
#define summarize summarize_tif_strip_BOSS
#define multiply multiply_tif_strip_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_strip.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_swab.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_thunder.c>
#define summarize summarize_tif_tile_BOSS
#define multiply multiply_tif_tile_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_tile.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_version.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_warning.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_write.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_zip.c>

#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_msdos.c>
//#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/libtiff/libtiff/src/tif_unix.c>

#endif
