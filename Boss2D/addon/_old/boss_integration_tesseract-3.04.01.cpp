#include <boss.h>
#if BOSS_NEED_ADDON_ALPR | BOSS_NEED_ADDON_TESSERACT

#include <iostream>
#include <functional>
#include <cctype>

#if BOSS_WINDOWS
    #pragma comment(lib, "ws2_32.lib")
#endif

// 도구준비
#include <addon/boss_fakewin.h>
#include "boss_integration_tesseract-3.04.01.h"

#pragma warning(disable : 4566)
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#define WINDLLNAME "tesseract"
#define USE_STD_NAMESPACE
#define cprintf cprintf_callcpp_BOSS
#define fprintf(...) 0
#include <addon/tesseract-3.04.01_for_boss/ccutil/hashfn.h>
#include <addon/tesseract-3.04.01_for_boss/cube/char_samp.h>

#include <addon/tesseract-3.04.01_for_boss/api/baseapi.cpp>
#include <addon/tesseract-3.04.01_for_boss/api/renderer.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/adaptions.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/applybox.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/control.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/cube_control.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/cube_reco_context.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/cubeclassifier.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/docqual.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/equationdetect.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/fixspace.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/fixxht.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/ltrresultiterator.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/osdetect.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/output.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/pageiterator.cpp>
#define kMinCredibleResolution kMinCredibleResolution_pagesegmain_BOSS
#include <addon/tesseract-3.04.01_for_boss/ccmain/pagesegmain.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/pagewalk.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/par_control.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/paragraphs.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/paramsd.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/pgedit.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/recogtraining.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/reject.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/resultiterator.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/superscript.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/tessbox.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/tessedit.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/tesseract_cube_combiner.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/tesseractclass.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/tessvars.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/tfacepp.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/thresholder.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccmain/werdit.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/ambigs.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/basedir.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/bits16.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/bitvector.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/ccutil.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/clst.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/elst.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/elst2.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/errcode.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/globaloc.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/indexmapbidi.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/mainblk.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/memry.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/params.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/scanutils.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/serialis.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/strngs.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/tessdatamanager.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/tprintf.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/unichar.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/unicharmap.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/unicharset.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/unicodes.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccutil/universalambigs.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/alignedblob.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/baselinedetect.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/bbgrid.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/blkocc.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/blobgrid.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/ccnontextdetect.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/cjkpitch.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/colfind.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/colpartition.cpp>
#define kMarginOverlapFraction kMarginOverlapFraction_colpartitiongrid_BOSS
#define kMinColumnWidth kMinColumnWidth_colpartitiongrid_BOSS
#include <addon/tesseract-3.04.01_for_boss/textord/colpartitiongrid.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/colpartitionset.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/devanagari_processing.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/drawedg.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/drawtord.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/edgblob.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/edgloop.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/equationdetectbase.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/fpchop.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/gap_map.cpp>
#define kNoisePadding kNoisePadding_imagefind_BOSS
#include <addon/tesseract-3.04.01_for_boss/textord/imagefind.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/linefind.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/makerow.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/oldbasel.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/pithsync.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/pitsync1.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/scanedg.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/sortflts.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/strokewidth.cpp>
#define kAlignedFraction kAlignedFraction_tabfind_BOSS
#define kMinColumnWidth kMinColumnWidth_tabfind_BOSS
#define kMinBaselineCoverage kMinBaselineCoverage_tabfind_BOSS
#define kMaxBaselineError kMaxBaselineError_tabfind_BOSS
#include <addon/tesseract-3.04.01_for_boss/textord/tabfind.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/tablefind.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/tablerecog.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/tabvector.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/textlineprojection.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/textord.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/topitch.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/tordmain.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/tospace.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/tovars.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/underlin.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/wordseg.cpp>
#include <addon/tesseract-3.04.01_for_boss/textord/workingpartset.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/blamer.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/blobbox.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/blobs.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/blread.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/boxread.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/boxword.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/ccstruct.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/coutln.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/detlinefit.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/dppoint.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/fontinfo.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/genblob.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/imagedata.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/linlsq.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/matrix.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/mod128.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/normalis.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/ocrblock.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/ocrpara.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/ocrrow.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/otsuthr.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/pageres.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/params_training_featdef.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/pdblock.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/points.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/polyaprx.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/polyblk.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/publictypes.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/quadlsq.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/quspline.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/ratngs.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/rect.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/rejctmap.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/seam.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/split.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/statistc.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/stepblob.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/vecfuncs.cpp>
#include <addon/tesseract-3.04.01_for_boss/ccstruct/werd.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/adaptive.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/adaptmatch.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/blobclass.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/classify.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/cluster.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/clusttool.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/cutoffs.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/errorcounter.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/featdefs.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/float2int.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/fpoint.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/intfeaturedist.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/intfeaturemap.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/intfeaturespace.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/intfx.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/intmatcher.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/intproto.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/kdtree.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/mastertrainer.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/mf.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/mfdefs.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/mfoutline.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/mfx.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/normfeat.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/normmatch.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/ocrfeatures.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/outfeat.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/picofeat.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/protos.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/sampleiterator.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/shapeclassifier.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/shapetable.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/tessclassifier.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/trainingsample.cpp>
#include <addon/tesseract-3.04.01_for_boss/classify/trainingsampleset.cpp>
#include <addon/tesseract-3.04.01_for_boss/viewer/scrollview.cpp>
#include <addon/tesseract-3.04.01_for_boss/viewer/svmnode.cpp>
// main()이 있음: #include <addon/tesseract-3.04.01_for_boss/viewer/svpaint.cpp>
#define kMaxMsgSize kMaxMsgSize_svutil_BOSS
#include <addon/tesseract-3.04.01_for_boss/viewer/svutil.cpp>
#include <addon/tesseract-3.04.01_for_boss/dict/context.cpp>
#include <addon/tesseract-3.04.01_for_boss/dict/dawg.cpp>
#include <addon/tesseract-3.04.01_for_boss/dict/dawg_cache.cpp>
#include <addon/tesseract-3.04.01_for_boss/dict/dict.cpp>
#include <addon/tesseract-3.04.01_for_boss/dict/hyphen.cpp>
#include <addon/tesseract-3.04.01_for_boss/dict/permdawg.cpp>
#include <addon/tesseract-3.04.01_for_boss/dict/stopper.cpp>
#include <addon/tesseract-3.04.01_for_boss/dict/trie.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/altlist.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/beam_search.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/bmp_8.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/cached_file.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/char_altlist.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/char_bigrams.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/char_samp.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/char_samp_enum.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/char_samp_set.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/char_set.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/classifier_factory.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/con_comp.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/conv_net_classifier.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/cube_line_object.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/cube_line_segmenter.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/cube_object.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/cube_search_object.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/cube_tuning_params.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/cube_utils.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/feature_bmp.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/feature_chebyshev.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/feature_hybrid.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/hybrid_neural_net_classifier.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/search_column.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/search_node.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/tess_lang_mod_edge.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/tess_lang_model.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/word_altlist.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/word_list_lang_model.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/word_size_model.cpp>
#include <addon/tesseract-3.04.01_for_boss/cube/word_unigrams.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/associate.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/chop.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/chopper.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/drawfx.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/findseam.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/gradechop.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/language_model.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/lm_consistency.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/lm_pain_points.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/lm_state.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/outlines.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/params_model.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/pieces.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/plotedges.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/render.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/segsearch.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/tface.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/wordclass.cpp>
#include <addon/tesseract-3.04.01_for_boss/wordrec/wordrec.cpp>
#include <addon/tesseract-3.04.01_for_boss/cutil/bitvec.cpp>
#include <addon/tesseract-3.04.01_for_boss/cutil/callcpp.cpp>
#include <addon/tesseract-3.04.01_for_boss/cutil/cutil.cpp>
#include <addon/tesseract-3.04.01_for_boss/cutil/cutil_class.cpp>
#include <addon/tesseract-3.04.01_for_boss/cutil/danerror.cpp>
#include <addon/tesseract-3.04.01_for_boss/cutil/efio.cpp>
#include <addon/tesseract-3.04.01_for_boss/cutil/emalloc.cpp>
#include <addon/tesseract-3.04.01_for_boss/cutil/freelist.cpp>
#include <addon/tesseract-3.04.01_for_boss/cutil/listio.cpp>
#include <addon/tesseract-3.04.01_for_boss/cutil/oldlist.cpp>
#include <addon/tesseract-3.04.01_for_boss/cutil/structures.cpp>
#include <addon/tesseract-3.04.01_for_boss/neural_networks/runtime/input_file_buffer.cpp>
#include <addon/tesseract-3.04.01_for_boss/neural_networks/runtime/neural_net.cpp>
#include <addon/tesseract-3.04.01_for_boss/neural_networks/runtime/neuron.cpp>
#include <addon/tesseract-3.04.01_for_boss/neural_networks/runtime/sigmoid_table.cpp>
#include <addon/tesseract-3.04.01_for_boss/vs2010/port/gettimeofday.cpp>

// ■ 대체구현(addon/tesseract-3.04.01_for_boss/api/tesseractmain.cpp)
void PrintVersionInfo() {
    char *versionStrP;

    fprintf(stderr, "tesseract %s\n", tesseract::TessBaseAPI::Version());

    versionStrP = getLeptonicaVersion();
    fprintf(stderr, " %s\n", versionStrP);
    lept_free(versionStrP);

    versionStrP = getImagelibVersions();
    fprintf(stderr, "  %s\n", versionStrP);
    lept_free(versionStrP);

#ifdef USE_OPENCL
    cl_platform_id platform;
    cl_uint num_platforms;
    cl_device_id devices[2];
    cl_uint num_devices;
    char info[256];
    int i;

    fprintf(stderr, " OpenCL info:\n");
    clGetPlatformIDs(1, &platform, &num_platforms);
    fprintf(stderr, "  Found %d platforms.\n", num_platforms);
    clGetPlatformInfo(platform, CL_PLATFORM_NAME, 256, info, 0);
    fprintf(stderr, "  Platform name: %s.\n", info);
    clGetPlatformInfo(platform, CL_PLATFORM_VERSION, 256, info, 0);
    fprintf(stderr, "  Version: %s.\n", info);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 2, devices, &num_devices);
    fprintf(stderr, "  Found %d devices.\n", num_devices);
    for (i = 0; i < num_devices; ++i) {
      clGetDeviceInfo(devices[i], CL_DEVICE_NAME, 256, info, 0);
      fprintf(stderr, "    Device %d name: %s.\n", i+1, info);
    }
#endif
}

void PrintUsage(const char* program) {
  fprintf(stderr,
      "Usage:\n"
      "  %s --help | --help-psm | --version\n"
      "  %s --list-langs [--tessdata-dir PATH]\n"
      "  %s --print-parameters [options...] [configfile...]\n"
      "  %s imagename|stdin outputbase|stdout [options...] [configfile...]\n",
      program, program, program, program);
}

void PrintHelpForPSM() {
  const char* msg =
      "Page segmentation modes:\n"
        "  0    Orientation and script detection (OSD) only.\n"
        "  1    Automatic page segmentation with OSD.\n"
        "  2    Automatic page segmentation, but no OSD, or OCR.\n"
        "  3    Fully automatic page segmentation, but no OSD. (Default)\n"
        "  4    Assume a single column of text of variable sizes.\n"
        "  5    Assume a single uniform block of vertically aligned text.\n"
        "  6    Assume a single uniform block of text.\n"
        "  7    Treat the image as a single text line.\n"
        "  8    Treat the image as a single word.\n"
        "  9    Treat the image as a single word in a circle.\n"
        " 10    Treat the image as a single character.\n"

        //TODO: Consider publishing these modes.
        #if 0
        " 11    Sparse text. Find as much text as possible in no"
          " particular order.\n"
        " 12    Sparse text with OSD.\n"
        " 13    Raw line. Treat the image as a single text line,\n"
          "\t\t\tbypassing hacks that are Tesseract-specific.\n"
        #endif
        ;

  fprintf(stderr, "%s", msg);
}

void PrintHelpMessage(const char* program) {
  PrintUsage(program);

  const char* ocr_options =
      "OCR options:\n"
      "  --tessdata-dir PATH   Specify the location of tessdata path.\n"
      "  --user-words PATH     Specify the location of user words file.\n"
      "  --user-patterns PATH  Specify the location of user patterns file.\n"
      "  -l LANG[+LANG]        Specify language(s) used for OCR.\n"
      "  -c VAR=VALUE          Set value for config variables.\n"
      "                        Multiple -c arguments are allowed.\n"
      "  -psm NUM              Specify page segmentation mode.\n"
      "NOTE: These options must occur before any configfile.\n"
     ;

  fprintf(stderr, "\n%s\n", ocr_options);
  PrintHelpForPSM();

  const char *single_options =
      "Single options:\n"
      "  -h, --help            Show this help message.\n"
      "  --help-psm            Show page segmentation modes.\n"
      "  -v, --version         Show version information.\n"
      "  --list-langs          List available languages for tesseract engine.\n"
      "  --print-parameters    Print tesseract parameters to stdout.\n"
      ;

  fprintf(stderr, "\n%s", single_options);
}

enum class ExitCode {Continue, ReturnForSuccess, ReturnForFailure};

ExitCode SetVariablesFromCLArgs(tesseract::TessBaseAPI* api, int argc, const char** argv) {
  char opt1[256], opt2[255];
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
      strncpy(opt1, argv[i + 1], 255);
      opt1[255] = '\0';
      char *p = strchr(opt1, '=');
      if (!p) {
        fprintf(stderr, "Missing = in configvar assignment\n");
        return ExitCode::ReturnForFailure;
      }
      *p = 0;
      strncpy(opt2, strchr(argv[i + 1], '=') + 1, 255);
      opt2[254] = 0;
      ++i;

      if (!api->SetVariable(opt1, opt2)) {
        fprintf(stderr, "Could not set option: %s=%s\n", opt1, opt2);
      }
    }
  }
  return ExitCode::Continue;
}

void PrintLangsList(tesseract::TessBaseAPI* api) {
  GenericVector<STRING> languages;
  api->GetAvailableLanguagesAsVector(&languages);
  fprintf(stderr, "List of available languages (%d):\n",
          languages.size());
  for (int index = 0; index < languages.size(); ++index) {
    STRING& string = languages[index];
    fprintf(stderr, "%s\n", string.string());
  }
  api->End();
}

void FixPageSegMode(tesseract::TessBaseAPI* api,
              tesseract::PageSegMode pagesegmode) {
  if (api->GetPageSegMode() == tesseract::PSM_SINGLE_BLOCK)
     api->SetPageSegMode(pagesegmode);
}

// NOTE: arg_i is used here to avoid ugly *i so many times in this function
ExitCode ParseArgs(const int argc, const char** argv,
                  const char** lang,
                  const char** image,
                  const char** outputbase,
                  const char** datapath,
                  bool* list_langs,
                  bool* print_parameters,
                  GenericVector<STRING>* vars_vec,
                  GenericVector<STRING>* vars_values,
                  int* arg_i,
                  tesseract::PageSegMode* pagesegmode) {
  if (argc == 1) {
    PrintHelpMessage(argv[0]);
    return ExitCode::ReturnForSuccess;
  }

  if (argc == 2) {
    if ((strcmp(argv[1], "-h") == 0) ||
         (strcmp(argv[1], "--help") == 0)) {
      PrintHelpMessage(argv[0]);
      return ExitCode::ReturnForSuccess;
    }
    if ((strcmp(argv[1], "--help-psm") == 0)) {
      PrintHelpForPSM();
      return ExitCode::ReturnForSuccess;
    }
    if ((strcmp(argv[1], "-v") == 0) ||
         (strcmp(argv[1], "--version") == 0)) {
      PrintVersionInfo();
      return ExitCode::ReturnForSuccess;
    }
  }

  bool noocr = false;
  int i = 1;
  while (i < argc && (*outputbase == NULL || argv[i][0] == '-')) {
    if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) {
      *lang = argv[i + 1];
      ++i;
    } else if (strcmp(argv[i], "--tessdata-dir") == 0 && i + 1 < argc) {
      *datapath = argv[i + 1];
      ++i;
    } else if (strcmp(argv[i], "--user-words") == 0 && i + 1 < argc) {
      vars_vec->push_back("user_words_file");
      vars_values->push_back(argv[i + 1]);
      ++i;
    } else if (strcmp(argv[i], "--user-patterns") == 0 && i + 1 < argc) {
      vars_vec->push_back("user_patterns_file");
      vars_values->push_back(argv[i + 1]);
      ++i;
    } else if (strcmp(argv[i], "--list-langs") == 0) {
      noocr = true;
      *list_langs = true;
    } else if (strcmp(argv[i], "-psm") == 0 && i + 1 < argc) {
      *pagesegmode = static_cast<tesseract::PageSegMode>(atoi(argv[i + 1]));
      ++i;
    } else if (strcmp(argv[i], "--print-parameters") == 0) {
      noocr = true;
      *print_parameters = true;
    } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
      // handled properly after api init
      ++i;
    } else if (*image == NULL) {
      *image = argv[i];
    } else if (*outputbase == NULL) {
      *outputbase = argv[i];
    }
    ++i;
  }

  *arg_i = i;

  if (argc == 2 && strcmp(argv[1], "--list-langs") == 0) {
    *list_langs = true;
    noocr = true;
  }

  if (*outputbase == NULL && noocr == false) {
    PrintHelpMessage(argv[0]);
    return ExitCode::ReturnForFailure;
  }

  if (*outputbase != NULL && strcmp(*outputbase, "-") &&
      strcmp(*outputbase, "stdout")) {
    tprintf("Tesseract Open Source OCR Engine v%s with Leptonica\n",
           tesseract::TessBaseAPI::Version());
  }
  return ExitCode::Continue;
}

void PreloadRenderers(tesseract::TessBaseAPI* api,
          tesseract::PointerVector<tesseract::TessResultRenderer>* renderers,
          tesseract::PageSegMode pagesegmode,
          const char* outputbase) {
  if (pagesegmode == tesseract::PSM_OSD_ONLY) {
    renderers->push_back(new tesseract::TessOsdRenderer(outputbase));
  } else {
    bool b;
    api->GetBoolVariable("tessedit_create_hocr", &b);
    if (b) {
      bool font_info;
      api->GetBoolVariable("hocr_font_info", &font_info);
      renderers->push_back(
                     new tesseract::TessHOcrRenderer(outputbase, font_info));
    }

    api->GetBoolVariable("tessedit_create_pdf", &b);
    if (b) {
      BOSS_ASSERT("TessPDFRenderer를 병합해야 합니다", false);
      //renderers->push_back(new tesseract::TessPDFRenderer(outputbase,
      //                                                  api->GetDatapath()));
    }

    api->GetBoolVariable("tessedit_write_unlv", &b);
    if (b) {
      renderers->push_back(new tesseract::TessUnlvRenderer(outputbase));
    }

    api->GetBoolVariable("tessedit_create_boxfile", &b);
    if (b) {
      renderers->push_back(new tesseract::TessBoxTextRenderer(outputbase));
    }

    api->GetBoolVariable("tessedit_create_txt", &b);
    if (b || renderers->empty()) {
      renderers->push_back(new tesseract::TessTextRenderer(outputbase));
    }
  }

  if (!renderers->empty()) {
    // Since the PointerVector auto-deletes, null-out the renderers that are
    // added to the root, and leave the root in the vector.
    for (int r = 1; r < renderers->size(); ++r) {
      (*renderers)[0]->insert((*renderers)[r]);
      (*renderers)[r] = NULL;
    }
  }
}

bool TesseractProgram(int argc, const char **argv) {
  const char* lang = "eng";
  const char* image = NULL;
  const char* outputbase = NULL;
  const char* datapath = NULL;
  bool list_langs = false;
  bool print_parameters = false;
  GenericVector<STRING> vars_vec, vars_values;
  int arg_i = 1;
  tesseract::PageSegMode pagesegmode = tesseract::PSM_AUTO;

  ExitCode Code1 = ParseArgs(argc, argv,
          &lang, &image, &outputbase, &datapath,
          &list_langs, &print_parameters,
          &vars_vec, &vars_values, &arg_i, &pagesegmode);
  if(Code1 != ExitCode::Continue)
      return (Code1 == ExitCode::ReturnForSuccess);

  PERF_COUNT_START("Tesseract:main")
  tesseract::TessBaseAPI api;

  api.SetOutputName(outputbase);

  int init_failed = api.Init(datapath, lang, tesseract::OEM_DEFAULT,
                &(((char**) argv)[arg_i]), argc - arg_i, &vars_vec, &vars_values, false);
  if (init_failed) {
    fprintf(stderr, "Could not initialize tesseract.\n");
    return false;
  }

  ExitCode Code2 = SetVariablesFromCLArgs(&api, argc, argv);
  if(Code2 != ExitCode::Continue)
      return (Code2 == ExitCode::ReturnForSuccess);

  if (list_langs) {
     PrintLangsList(&api);
     return true;
  }

  if (print_parameters) {
     FILE* fout = stdout;
     fprintf(stdout, "Tesseract parameters:\n");
     api.PrintVariables(fout);
     api.End();
     return true;
  }

  FixPageSegMode(&api, pagesegmode);

  if (pagesegmode == tesseract::PSM_AUTO_ONLY) {
    int ret_val = 0;

    Pix* pixs = pixRead(image);
    if (!pixs) {
      fprintf(stderr, "Cannot open input file: %s\n", image);
      return false;
    }

    api.SetImage(pixs);

    tesseract::Orientation orientation;
    tesseract::WritingDirection direction;
    tesseract::TextlineOrder order;
    float deskew_angle;

    tesseract::PageIterator* it =  api.AnalyseLayout();
    if (it) {
      it->Orientation(&orientation, &direction, &order, &deskew_angle);
      tprintf("Orientation: %d\nWritingDirection: %d\nTextlineOrder: %d\n" \
             "Deskew angle: %.4f\n",
              orientation, direction, order, deskew_angle);
    } else {
      ret_val = 1;
    }

    delete it;

    pixDestroy(&pixs);
    return (ret_val == 0);
  }

  // set in_training_mode to true when using one of these configs:
  // ambigs.train, box.train, box.train.stderr, linebox, rebox
  bool b = false;
  bool in_training_mode =
        (api.GetBoolVariable("tessedit_ambigs_training", &b) && b) ||
        (api.GetBoolVariable("tessedit_resegment_from_boxes", &b) && b) ||
        (api.GetBoolVariable("tessedit_make_boxes_from_boxes", &b) && b);

  tesseract::PointerVector<tesseract::TessResultRenderer> renderers;

  if (in_training_mode) {
    renderers.push_back(NULL);
  } else {
    PreloadRenderers(&api, &renderers, pagesegmode, outputbase);
  }

  if (!renderers.empty()) {
    bool succeed = api.ProcessPages(image, NULL, 0, renderers[0]);
    if (!succeed) {
      fprintf(stderr, "Error during processing.\n");
      return false;
    }
  }

  PERF_COUNT_END
  return true;
}

// ■ 대체구현(addon/tesseract-3.04.01_for_boss/training/unicharset_extractor.cpp)
static int tessoptind = 0;
static char* tessoptarg = nullptr;

int tessopt (                          //parse args
inT32 argc,                            //arg count
const char *argv[],                    //args
const char *arglist                    //string of arg chars
) {
  const char *arg;                     //arg char

  if (tessoptind == 0)
    tessoptind = 1;
  if (tessoptind < argc && argv[tessoptind][0] == '-') {
    arg = strchr (arglist, argv[tessoptind][1]);
    if (arg == NULL || *arg == ':')
      return '?';                //dud option
    tessoptind++;
    tessoptarg = (char*) argv[tessoptind];
    if (arg[1] == ':') {
      if (argv[tessoptind - 1][2] != '\0')
                                 //immediately after
        tessoptarg = (char*) argv[tessoptind - 1] + 2;
      else
        tessoptind++;
    }
    return *arg;
  }
  else
    return EOF;
}

#define USING_WCTYPE
static const char* const kUnicharsetFileName = "unicharset";

UNICHAR_ID wc_to_unichar_id(const UNICHARSET &unicharset, int wc) {
  UNICHAR uch(wc);
  char *unichar = uch.utf8_str();
  UNICHAR_ID unichar_id = unicharset.unichar_to_id(unichar);
  delete[] unichar;
  return unichar_id;
}

// Set character properties using wctype if we have it.
// Contributed by piggy@gmail.com.
// Modified by Ray to use UNICHAR for unicode conversion
// and to check for wctype using autoconf/presence of windows.
void set_properties(UNICHARSET *unicharset, const char* const c_string) {
#ifdef USING_WCTYPE
  UNICHAR_ID id;
  int wc;

  // Convert the string to a unichar id.
  id = unicharset->unichar_to_id(c_string);

  // Set the other_case property to be this unichar id by default.
  unicharset->set_other_case(id, id);

  int step = UNICHAR::utf8_step(c_string);
  if (step == 0)
    return; // Invalid utf-8.

  // Get the next Unicode code point in the string.
  UNICHAR ch(c_string, step);
  wc = ch.first_uni();

  // Copy the properties.
  if (iswalpha(wc)) {
    unicharset->set_isalpha(id, 1);
    if (iswlower(wc)) {
      unicharset->set_islower(id, 1);
      unicharset->set_other_case(id, wc_to_unichar_id(*unicharset,
                                                      towupper(wc)));
    }
    if (iswupper(wc)) {
      unicharset->set_isupper(id, 1);
      unicharset->set_other_case(id, wc_to_unichar_id(*unicharset,
                                                      towlower(wc)));
    }
  }
  if (iswdigit(wc))
    unicharset->set_isdigit(id, 1);
  if(iswpunct(wc))
    unicharset->set_ispunctuation(id, 1);

#endif
}

bool UnicharsetExtractorProgram(int argc, const char **argv) {
  int option;
  const char* output_directory = ".";
  STRING unicharset_file_name;
  // Special characters are now included by default.
  UNICHARSET unicharset;

  setlocale(LC_ALL, "");

  // Print usage
  if (argc <= 1) {
    printf("Usage: %s [-D DIRECTORY] FILE...\n", argv[0]);
#ifdef USING_WCTYPE
    printf("Character properties using wctype is enabled\n");
#else
    printf("WARNING: Character properties using wctype is DISABLED\n");
#endif
    return false;
  }

  // Parse arguments
  tessoptind = 0;
  while ((option = tessopt(argc, argv, "D" )) != EOF) {
    switch (option) {
      case 'D':
        output_directory = tessoptarg;
        ++tessoptind;
        break;
    }
  }

  // Save file name
  unicharset_file_name = output_directory;
  unicharset_file_name += "/";
  unicharset_file_name += kUnicharsetFileName;

  // Load box files
  for (; tessoptind < argc; ++tessoptind) {
    printf("Extracting unicharset from %s\n", argv[tessoptind]);

    FILE* box_file = fopen(argv[tessoptind], "rb");
    if (box_file == NULL) {
      printf("Cannot open box file %s\n", argv[tessoptind]);
      return false;
    }

    TBOX box;
    STRING unichar_string;
    int line_number = 0;
    while (ReadNextBox(&line_number, box_file, &unichar_string, &box)) {
      unicharset.unichar_insert(unichar_string.string());
      set_properties(&unicharset, unichar_string.string());
    }
  }

  // Write unicharset file
  if (unicharset.save_to_file(unicharset_file_name.string())) {
    printf("Wrote unicharset file %s.\n", unicharset_file_name.string());
  }
  else {
    printf("Cannot save unicharset file %s.\n", unicharset_file_name.string());
    return false;
  }
  return true;
}

// ■ 대체구현(addon/tesseract-3.04.01_for_boss/training/mftraining.cpp)
#include <addon/tesseract-3.04.01_for_boss/training/commandlineflags.cpp>
#include <addon/tesseract-3.04.01_for_boss/training/commontraining.cpp>
#include <addon/tesseract-3.04.01_for_boss/training/mergenf.cpp>

#define PROGRAM_FEATURE_TYPE "mf"
static const int kMaxShapeLabelLength = 10;

static void SetupConfigMap(ShapeTable* shape_table, tesseract::IndexMapBiDi* config_map) {
  int num_configs = shape_table->NumShapes();
  config_map->Init(num_configs, true);
  config_map->Setup();
  for (int c1 = 0; c1 < num_configs; ++c1) {
    // Only process ids that are not already merged.
    if (config_map->SparseToCompact(c1) == c1) {
      tesseract::Shape* shape1 = shape_table->MutableShape(c1);
      // Find all the subsequent shapes that are equal.
      for (int c2 = c1 + 1; c2 < num_configs; ++c2) {
        if (shape_table->MutableShape(c2)->IsEqualUnichars(shape1)) {
          config_map->Merge(c1, c2);
        }
      }
    }
  }
  config_map->CompleteMerges();
}

static void DisplayProtoList(const char* ch, LIST protolist) {
  void* window = c_create_window("Char samples", 50, 200,
                                 520, 520, -130.0, 130.0, -130.0, 130.0);
  LIST proto = protolist;
  iterate(proto) {
    PROTOTYPE* prototype = reinterpret_cast<PROTOTYPE *>(first_node(proto));
    if (prototype->Significant)
      c_line_color_index(window, Green);
    else if (prototype->NumSamples == 0)
      c_line_color_index(window, Blue);
    else if (prototype->Merged)
      c_line_color_index(window, Magenta);
    else
      c_line_color_index(window, Red);
    float x = CenterX(prototype->Mean);
    float y = CenterY(prototype->Mean);
    double angle = OrientationOf(prototype->Mean) * 2 * M_PI;
    float dx = static_cast<float>(LengthOf(prototype->Mean) * cos(angle) / 2);
    float dy = static_cast<float>(LengthOf(prototype->Mean) * sin(angle) / 2);
    c_move(window, (x - dx) * 256, (y - dy) * 256);
    c_draw(window, (x + dx) * 256, (y + dy) * 256);
    if (prototype->Significant)
      tprintf("Green proto at (%g,%g)+(%g,%g) %d samples\n",
              x, y, dx, dy, prototype->NumSamples);
    else if (prototype->NumSamples > 0 && !prototype->Merged)
      tprintf("Red proto at (%g,%g)+(%g,%g) %d samples\n",
              x, y, dx, dy, prototype->NumSamples);
  }
  c_make_current(window);
}

static LIST ClusterOneConfig(int shape_id, const char* class_label,
                             LIST mf_classes,
                             const ShapeTable& shape_table,
                             tesseract::MasterTrainer* trainer) {
  int num_samples;
  CLUSTERER  *clusterer = trainer->SetupForClustering(shape_table,
                                                      feature_defs,
                                                      shape_id,
                                                      &num_samples);
  Config.MagicSamples = num_samples;
  LIST proto_list = ClusterSamples(clusterer, &Config);
  CleanUpUnusedData(proto_list);

  // Merge protos where reasonable to make more of them significant by
  // representing almost all samples of the class/font.
  MergeInsignificantProtos(proto_list, class_label, clusterer, &Config);
  #ifndef GRAPHICS_DISABLED
  if (strcmp(FLAGS_test_ch.c_str(), class_label) == 0)
    DisplayProtoList(FLAGS_test_ch.c_str(), proto_list);
  #endif  // GRAPHICS_DISABLED
  // Delete the protos that will not be used in the inttemp output file.
  proto_list = RemoveInsignificantProtos(proto_list, true,
                                         false,
                                         clusterer->SampleSize);
  FreeClusterer(clusterer);
  MERGE_CLASS merge_class = FindClass(mf_classes, class_label);
  if (merge_class == NULL) {
    merge_class = NewLabeledClass(class_label);
    mf_classes = push(mf_classes, merge_class);
  }
  int config_id = AddConfigToClass(merge_class->Class);
  merge_class->Class->font_set.push_back(shape_id);
  LIST proto_it = proto_list;
  iterate(proto_it) {
    PROTOTYPE* prototype = reinterpret_cast<PROTOTYPE*>(first_node(proto_it));
    // See if proto can be approximated by existing proto.
    int p_id = FindClosestExistingProto(merge_class->Class,
                                        merge_class->NumMerged, prototype);
    if (p_id == NO_PROTO) {
      // Need to make a new proto, as it doesn't match anything.
      p_id = AddProtoToClass(merge_class->Class);
      MakeNewFromOld(ProtoIn(merge_class->Class, p_id), prototype);
      merge_class->NumMerged[p_id] = 1;
    } else {
      PROTO_STRUCT dummy_proto;
      MakeNewFromOld(&dummy_proto, prototype);
      // Merge with the similar proto.
      ComputeMergedProto(ProtoIn(merge_class->Class, p_id), &dummy_proto,
                         static_cast<FLOAT32>(merge_class->NumMerged[p_id]),
                         1.0,
                         ProtoIn(merge_class->Class, p_id));
      merge_class->NumMerged[p_id]++;
    }
    AddProtoToConfig(p_id, merge_class->Class->Configurations[config_id]);
  }
  FreeProtoList(&proto_list);
  return mf_classes;
}

bool MfTrainingProgram(int argc, const char **argv) {
  ParseArguments(&argc, (char***) &argv);

  ShapeTable* shape_table = NULL;
  STRING file_prefix;
  // Load the training data.
  tesseract::MasterTrainer* trainer = tesseract::LoadTrainingData(argc, argv,
                                                       false,
                                                       &shape_table,
                                                       &file_prefix);
  if (trainer == NULL)
    return false;  // Failed.

  // Setup an index mapping from the shapes in the shape table to the classes
  // that will be trained. In keeping with the original design, each shape
  // with the same list of unichars becomes a different class and the configs
  // represent the different combinations of fonts.
  tesseract::IndexMapBiDi config_map;
  SetupConfigMap(shape_table, &config_map);

  WriteShapeTable(file_prefix, *shape_table);
  // If the shape_table is flat, then either we didn't run shape clustering, or
  // it did nothing, so we just output the trainer's unicharset.
  // Otherwise shape_set will hold a fake unicharset with an entry for each
  // shape in the shape table, and we will output that instead.
  UNICHARSET shape_set;
  const UNICHARSET* unicharset = &trainer->unicharset();
  // If we ran shapeclustering (and it worked) then at least one shape will
  // have multiple unichars, so we have to build a fake unicharset.
  if (shape_table->AnyMultipleUnichars()) {
    unicharset = &shape_set;
    // Now build a fake unicharset for the compact shape space to keep the
    // output modules happy that we are doing things correctly.
    int num_shapes = config_map.CompactSize();
    for (int s = 0; s < num_shapes; ++s) {
      char shape_label[kMaxShapeLabelLength + 1];
      snprintf(shape_label, kMaxShapeLabelLength, "sh%04d", s);
      shape_set.unichar_insert(shape_label);
    }
  }

  // Now train each config separately.
  int num_configs = shape_table->NumShapes();
  LIST mf_classes = NIL_LIST;
  for (int s = 0; s < num_configs; ++s) {
    int unichar_id, font_id;
    if (unicharset == &shape_set) {
      // Using fake unichar_ids from the config_map/shape_set.
      unichar_id = config_map.SparseToCompact(s);
    } else {
      // Get the real unichar_id from the shape table/unicharset.
      shape_table->GetFirstUnicharAndFont(s, &unichar_id, &font_id);
    }
    const char* class_label = unicharset->id_to_unichar(unichar_id);
    mf_classes = ClusterOneConfig(s, class_label, mf_classes, *shape_table,
                                  trainer);
  }
  STRING inttemp_file = file_prefix;
  inttemp_file += "inttemp";
  STRING pffmtable_file = file_prefix;
  pffmtable_file += "pffmtable";
  CLASS_STRUCT* float_classes = SetUpForFloat2Int(*unicharset, mf_classes);
  // Now write the inttemp and pffmtable.
  trainer->WriteInttempAndPFFMTable(trainer->unicharset(), *unicharset,
                                    *shape_table, float_classes,
                                    inttemp_file.string(),
                                    pffmtable_file.string());
  delete [] float_classes;
  FreeLabeledClassList(mf_classes);
  delete trainer;
  delete shape_table;
  printf("Done!\n");
  if (!FLAGS_test_ch.empty()) {
    // If we are displaying debug window(s), wait for the user to look at them.
    printf("Hit return to exit...\n");
    while (getchar() != '\n');
  }
  return true;
}

// ■ 대체구현(addon/tesseract-3.04.01_for_boss/training/cntraining.cpp)
#define PROGRAM_FEATURE_TYPE "cn"
static CLUSTERCONFIG CNConfig = {elliptical, 0.025f, 0.05f, 0.8f, 1e-3, 0};

static void WriteProtos(
     FILE  *File,
     uinT16  N,
     LIST  ProtoList,
     BOOL8  WriteSigProtos,
     BOOL8  WriteInsigProtos)
{
  PROTOTYPE  *Proto;

  // write prototypes
  iterate(ProtoList)
  {
    Proto = (PROTOTYPE *) first_node ( ProtoList );
    if (( Proto->Significant && WriteSigProtos )  ||
      ( ! Proto->Significant && WriteInsigProtos ) )
      WritePrototype( File, N, Proto );
  }
}

static bool WriteNormProtos (
     const char  *Directory,
     LIST  LabeledProtoList,
     CLUSTERER *Clusterer)
{
  FILE    *File;
  STRING Filename;
  LABELEDLIST LabeledProto;
  int N;

  Filename = "";
  if (Directory != NULL && Directory[0] != '\0')
  {
    Filename += Directory;
    Filename += "/";
  }
  Filename += "normproto";
  printf ("\nWriting %s ...", Filename.string());
  File = Efopen (Filename.string(), "wb");
  fprintf(File,"%0d\n",Clusterer->SampleSize);
  WriteParamDesc(File,Clusterer->SampleSize,Clusterer->ParamDesc);
  iterate(LabeledProtoList)
  {
    LabeledProto = (LABELEDLIST) first_node (LabeledProtoList);
    N = NumberOfProtos(LabeledProto->List, true, false);
    if (N < 1) {
      printf ("\nError! Not enough protos for %s: %d protos"
              " (%d significant protos"
              ", %d insignificant protos)\n",
              LabeledProto->Label, N,
              NumberOfProtos(LabeledProto->List, 1, 0),
              NumberOfProtos(LabeledProto->List, 0, 1));
      return false;
    }
    fprintf(File, "\n%s %d\n", LabeledProto->Label, N);
    WriteProtos(File, Clusterer->SampleSize, LabeledProto->List, true, false);
  }
  fclose (File);
  return true;
}

bool CnTrainingProgram(int argc, const char **argv) {
  // Set the global Config parameters before parsing the command line.
  Config = CNConfig;

  const char  *PageName;
  FILE  *TrainingPage;
  LIST  CharList = NIL_LIST;
  CLUSTERER  *Clusterer = NULL;
  LIST    ProtoList = NIL_LIST;
  LIST    NormProtoList = NIL_LIST;
  LIST pCharList;
  LABELEDLIST CharSample;
  FEATURE_DEFS_STRUCT FeatureDefs;
  InitFeatureDefs(&FeatureDefs);

  ParseArguments(&argc, (char ***) &argv);
  int num_fonts = 0;
  while ((PageName = GetNextFilename(argc, argv)) != NULL) {
    printf("Reading %s ...\n", PageName);
    TrainingPage = Efopen(PageName, "rb");
    ReadTrainingSamples(FeatureDefs, PROGRAM_FEATURE_TYPE,
                        100, NULL, TrainingPage, &CharList);
    fclose(TrainingPage);
    ++num_fonts;
  }
  printf("Clustering ...\n");
  // To allow an individual font to form a separate cluster,
  // reduce the min samples:
  // Config.MinSamples = 0.5 / num_fonts;
  pCharList = CharList;
  iterate(pCharList) {
    //Cluster
    if (Clusterer)
       FreeClusterer(Clusterer);
    CharSample = (LABELEDLIST)first_node(pCharList);
    Clusterer =
      SetUpForClustering(FeatureDefs, CharSample, PROGRAM_FEATURE_TYPE);
    float SavedMinSamples = Config.MinSamples;
    // To disable the tendency to produce a single cluster for all fonts,
    // make MagicSamples an impossible to achieve number:
    // Config.MagicSamples = CharSample->SampleCount * 10;
    Config.MagicSamples = CharSample->SampleCount;
    while (Config.MinSamples > 0.001) {
      ProtoList = ClusterSamples(Clusterer, &Config);
      if (NumberOfProtos(ProtoList, 1, 0) > 0) {
        break;
      } else {
        Config.MinSamples *= 0.95;
        printf("0 significant protos for %s."
               " Retrying clustering with MinSamples = %f%%\n",
               CharSample->Label, Config.MinSamples);
      }
    }
    Config.MinSamples = SavedMinSamples;
    AddToNormProtosList(&NormProtoList, ProtoList, CharSample->Label);
  }
  FreeTrainingSamples(CharList);
  if (Clusterer == NULL) { // To avoid a SIGSEGV
    fprintf(stderr, "Error: NULL clusterer!\n");
    return false;
  }
  if(!WriteNormProtos(FLAGS_D.c_str(), NormProtoList, Clusterer))
      return false;
  FreeNormProtoList(NormProtoList);
  FreeProtoList(&ProtoList);
  FreeClusterer(Clusterer);
  printf ("\n");
  return true;
}

// ■ 대체구현(addon/tesseract-3.04.01_for_boss/training/combine_tessdata.cpp)
bool CombineTessDataProgram(int argc, const char **argv) {
  int i;
  if (argc == 2) {
    printf("Combining tessdata files\n");
    STRING lang = argv[1];
    char* last = (char*) &argv[1][strlen(argv[1])-1];
    if (*last != '.')
      lang += '.';
    STRING output_file = lang;
    output_file += kTrainedDataSuffix;
    if (!tesseract::TessdataManager::CombineDataFiles(
        lang.string(), output_file.string())) {
      printf("Error combining tessdata files into %s\n",
             output_file.string());
    } else {
      printf("Output %s created successfully.\n", output_file.string());
    }
  } else if (argc >= 4 && (strcmp(argv[1], "-e") == 0 ||
                           strcmp(argv[1], "-u") == 0)) {
    // Initialize TessdataManager with the data in the given traineddata file.
    tesseract::TessdataManager tm;
    tm.Init(argv[2], 0);
    printf("Extracting tessdata components from %s\n", argv[2]);
    if (strcmp(argv[1], "-e") == 0) {
      for (i = 3; i < argc; ++i) {
        if (tm.ExtractToFile(argv[i])) {
          printf("Wrote %s\n", argv[i]);
        } else {
          printf("Not extracting %s, since this component"
                 " is not present\n", argv[i]);
        }
      }
    } else {  // extract all the components
      for (i = 0; i < tesseract::TESSDATA_NUM_ENTRIES; ++i) {
        STRING filename = argv[3];
        char* last = (char*) &argv[3][strlen(argv[3])-1];
        if (*last != '.')
          filename += '.';
        filename += tesseract::kTessdataFileSuffixes[i];
        if (tm.ExtractToFile(filename.string())) {
          printf("Wrote %s\n", filename.string());
        }
      }
    }
    tm.End();
  } else if (argc >= 4 && strcmp(argv[1], "-o") == 0) {
    // Rename the current traineddata file to a temporary name.
    const char *new_traineddata_filename = argv[2];
    STRING traineddata_filename = new_traineddata_filename;
    traineddata_filename += ".__tmp__";
    if (rename(new_traineddata_filename, traineddata_filename.string()) != 0) {
      tprintf("Failed to create a temporary file %s\n",
              traineddata_filename.string());
      return false;
    }

    // Initialize TessdataManager with the data in the given traineddata file.
    tesseract::TessdataManager tm;
    tm.Init(traineddata_filename.string(), 0);

    // Write the updated traineddata file.
    tm.OverwriteComponents(new_traineddata_filename, (char**) argv+3, argc-3);
    tm.End();
  } else {
    printf("Usage for combining tessdata components:\n"
           "  %s language_data_path_prefix\n"
           "  (e.g. %s tessdata/eng.)\n\n", argv[0], argv[0]);
    printf("Usage for extracting tessdata components:\n"
           "  %s -e traineddata_file [output_component_file...]\n"
           "  (e.g. %s -e eng.traineddata eng.unicharset)\n\n",
           argv[0], argv[0]);
    printf("Usage for overwriting tessdata components:\n"
           "  %s -o traineddata_file [input_component_file...]\n"
           "  (e.g. %s -o eng.traineddata eng.unicharset)\n\n",
           argv[0], argv[0]);
    printf("Usage for unpacking all tessdata components:\n"
           "  %s -u traineddata_file output_path_prefix\n"
           "  (e.g. %s -u eng.traineddata tmp/eng.)\n", argv[0], argv[0]);
    return false;
  }
  return true;
}

#endif
