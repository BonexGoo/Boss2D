#include <boss.h>
#if BOSS_NEED_ADDON_ALPR | BOSS_NEED_ADDON_TESSERACT

#include "boss_integration_openalpr-2.3.0_3rdparty_liblept.h"

// define for tesseract
#define int8 int8_tesseract
#define uint8 uint8_tesseract
#define int16 int16_tesseract
#define uint16 uint16_tesseract
#define int32 int32_tesseract
#define uint32 uint32_tesseract

#pragma warning(disable : 4838)
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/include/environ.h>
#define HAVE_LIBJPEG  0
#define HAVE_LIBTIFF  1
#define HAVE_LIBPNG   0
#define HAVE_LIBZ     0
#define HAVE_LIBGIF   0
#define HAVE_LIBUNGIF 0
#define HAVE_LIBWEBP  0

#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/adaptmap.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/affine.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/affinecompose.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/arrayaccess.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/bardecode.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/baseline.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/bbuffer.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/bilateral.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/bilinear.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/binarize.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/binexpand.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/binreduce.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/blend.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/bmf.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/bmpio.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/bmpiostub.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/boxbasic.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/boxfunc1.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/boxfunc2.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/boxfunc3.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/boxfunc4.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/bytearray.c>
#define INITIAL_PTR_ARRAYSIZE INITIAL_PTR_ARRAYSIZE_ccbord_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/ccbord.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/ccthin.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/classapp.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/colorcontent.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/coloring.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/colormap.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/colormorph.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/colorquant1.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/colorquant2.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/colorseg.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/colorspace.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/compare.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/conncomp.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/convertfiles.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/convolve.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/convolvelow.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/correlscore.c>
#define INITIAL_PTR_ARRAYSIZE INITIAL_PTR_ARRAYSIZE_dewarp1_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/dewarp1.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/dewarp2.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/dewarp3.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/dewarp4.c>
#define INITIAL_PTR_ARRAYSIZE INITIAL_PTR_ARRAYSIZE_dnabasic_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/dnabasic.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/dwacomb.2.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/dwacomblow.2.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/edge.c>
// main()이 있음: #include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/endiantest.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/enhance.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/fhmtauto.c>
#define NUM_SELS_GENERATED NUM_SELS_GENERATED_fhmtgen_1_BOSS
#define SEL_NAMES SEL_NAMES_fhmtgen_1_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/fhmtgen.1.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/fhmtgenlow.1.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/finditalic.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/flipdetect.c>
#define NUM_SELS_GENERATED NUM_SELS_GENERATED_fliphmtgen_BOSS
#define SEL_NAMES SEL_NAMES_fliphmtgen_BOSS
#define fhmt_1_0 fhmt_1_0_fliphmtgen_BOSS
#define fhmt_1_1 fhmt_1_1_fliphmtgen_BOSS
#define fhmt_1_2 fhmt_1_2_fliphmtgen_BOSS
#define fhmt_1_3 fhmt_1_3_fliphmtgen_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/fliphmtgen.c>
#define wpldecls wpldecls_fmorphauto_BOSS
#define wpldefs wpldefs_fmorphauto_BOSS
#define wplstrp wplstrp_fmorphauto_BOSS
#define wplstrm wplstrm_fmorphauto_BOSS
#define makeBarrelshiftString makeBarrelshiftString_fmorphauto_BOSS
#define sarrayMakeInnerLoopDWACode sarrayMakeInnerLoopDWACode_fmorphauto_BOSS
#define sarrayMakeWplsCode sarrayMakeWplsCode_fmorphauto_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/fmorphauto.c>
#define NUM_SELS_GENERATED NUM_SELS_GENERATED_fmorphgen_1_BOSS
#define SEL_NAMES SEL_NAMES_fmorphgen_1_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/fmorphgen.1.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/fmorphgenlow.1.c>
#define INITIAL_PTR_ARRAYSIZE INITIAL_PTR_ARRAYSIZE_fpix1_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/fpix1.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/fpix2.c>
//#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/freetype.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/gifio.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/gifiostub.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/gplot.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/graphics.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/graymorph.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/graymorphlow.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/grayquant.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/grayquantlow.c>
#define INITIAL_BUFFER_ARRAYSIZE INITIAL_BUFFER_ARRAYSIZE_heap_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/heap.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/jbclass.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/jp2kio.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/jp2kiostub.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/jpegio.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/jpegiostub.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/kernel.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/leptwin.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/libversions.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/list.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/maze.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/morph.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/morphapp.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/morphdwa.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/morphseq.c>
#define INITIAL_PTR_ARRAYSIZE INITIAL_PTR_ARRAYSIZE_numabasic_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/numabasic.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/numafunc1.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/numafunc2.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pageseg.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/paintcmap.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/parseprotos.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/partition.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pdfio.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pdfiostub.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pix1.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pix2.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pix3.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pix4.c>
#define rmask32 rmask32_pix5_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pix5.c>
#define INITIAL_PTR_ARRAYSIZE INITIAL_PTR_ARRAYSIZE_pixabasic_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pixabasic.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pixacc.c>
#define MIN_COMPS_FOR_BIN_SORT MIN_COMPS_FOR_BIN_SORT_pixafunc1_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pixafunc1.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pixafunc2.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pixalloc.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pixarith.c>
#define INITIAL_PTR_ARRAYSIZE INITIAL_PTR_ARRAYSIZE_pixcomp_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pixcomp.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pixconv.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pixlabel.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pixtiling.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pngio.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pngiostub.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pnmio.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/pnmiostub.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/projective.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/psio1.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/psio1stub.c>
#define DEFAULT_INPUT_RES DEFAULT_INPUT_RES_psio2_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/psio2.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/psio2stub.c>
#define INITIAL_PTR_ARRAYSIZE INITIAL_PTR_ARRAYSIZE_ptabasic_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/ptabasic.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/ptafunc1.c>
#define INITIAL_PTR_ARRAYSIZE INITIAL_PTR_ARRAYSIZE_ptra_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/ptra.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/quadtree.c>
#define MIN_BUFFER_SIZE MIN_BUFFER_SIZE_queue_BOSS
#define INITIAL_BUFFER_ARRAYSIZE INITIAL_BUFFER_ARRAYSIZEqueue__BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/queue.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/rank.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/readbarcode.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/readfile.c>
#define INITIAL_PTR_ARRAYSIZE INITIAL_PTR_ARRAYSIZE_recogbasic_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/recogbasic.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/recogdid.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/recogident.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/recogtrain.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/regutils.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/rop.c>
#define rmask32 rmask32_ropiplow_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/ropiplow.c>
#define lmask32 lmask32_roplow_BOSS
#define rmask32 rmask32_roplow_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/roplow.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/rotate.c>
#define MIN_ANGLE_TO_ROTATE MIN_ANGLE_TO_ROTATE_rotateam_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/rotateam.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/rotateamlow.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/rotateorth.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/rotateorthlow.c>
#define MIN_ANGLE_TO_ROTATE MIN_ANGLE_TO_ROTATE_rotateshear_BOSS
#define LIMIT_SHEAR_ANGLE LIMIT_SHEAR_ANGLE_rotateshear_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/rotateshear.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/runlength.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/sarray.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/scale.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/scalelow.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/seedfill.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/seedfilllow.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/sel1.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/sel2.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/selgen.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/shear.c>
#define DEFAULT_SWEEP_RANGE DEFAULT_SWEEP_RANGE_skew_BOSS
#define DEFAULT_SWEEP_DELTA DEFAULT_SWEEP_DELTA_skew_BOSS
#define DEFAULT_MINBS_DELTA DEFAULT_MINBS_DELTA_skew_BOSS
#define DEFAULT_SWEEP_REDUCTION DEFAULT_SWEEP_REDUCTION_skew_BOSS
#define DEFAULT_BS_REDUCTION DEFAULT_BS_REDUCTION_skew_BOSS
#define MIN_ALLOWED_CONFIDENCE MIN_ALLOWED_CONFIDENCE_skew_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/skew.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/spixio.c>
#define INITIAL_PTR_ARRAYSIZE INITIAL_PTR_ARRAYSIZE_stack_BOSS
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/stack.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/sudoku.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/textops.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/tiffio.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/tiffiostub.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/utils.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/viewfiles.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/warper.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/watershed.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/webpio.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/webpiostub.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/writefile.c>
// main()이 있음: #include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/xtractprotos.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/zlibmem.c>
#include <addon/openalpr-2.3.0_for_boss/openalpr-windows-2.2.0/tesseract-ocr/dependencies/liblept/src/zlibmemstub.c>

#endif
