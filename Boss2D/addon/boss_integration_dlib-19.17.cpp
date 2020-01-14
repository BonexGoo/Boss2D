#include <boss.h>
#if BOSS_NEED_ADDON_DLIB

#include "boss_integration_dlib-19.17.h"

// ISO C++ code
#include <addon/dlib-19.17_for_boss/dlib/base64/base64_kernel_1.cpp>
#include <addon/dlib-19.17_for_boss/dlib/bigint/bigint_kernel_1.cpp>
#include <addon/dlib-19.17_for_boss/dlib/bigint/bigint_kernel_2.cpp>
#include <addon/dlib-19.17_for_boss/dlib/bit_stream/bit_stream_kernel_1.cpp>
#include <addon/dlib-19.17_for_boss/dlib/entropy_decoder/entropy_decoder_kernel_1.cpp>
#include <addon/dlib-19.17_for_boss/dlib/entropy_decoder/entropy_decoder_kernel_2.cpp>
#include <addon/dlib-19.17_for_boss/dlib/entropy_encoder/entropy_encoder_kernel_1.cpp>
#include <addon/dlib-19.17_for_boss/dlib/entropy_encoder/entropy_encoder_kernel_2.cpp>
#include <addon/dlib-19.17_for_boss/dlib/md5/md5_kernel_1.cpp>
#include <addon/dlib-19.17_for_boss/dlib/tokenizer/tokenizer_kernel_1.cpp>
#include <addon/dlib-19.17_for_boss/dlib/unicode/unicode.cpp>
#include <addon/dlib-19.17_for_boss/dlib/test_for_odr_violations.cpp>

#ifndef DLIB_ISO_CPP_ONLY
    #include <addon/dlib-19.17_for_boss/dlib/sockets/sockets_kernel_1.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/bsp/bsp.cpp>

    #include <addon/dlib-19.17_for_boss/dlib/dir_nav/dir_nav_kernel_1.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/dir_nav/dir_nav_kernel_2.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/dir_nav/dir_nav_extensions.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/linker/linker_kernel_1.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/logger/extra_logger_headers.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/logger/logger_kernel_1.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/logger/logger_config_file.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/misc_api/misc_api_kernel_1.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/misc_api/misc_api_kernel_2.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/sockets/sockets_extensions.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/sockets/sockets_kernel_2.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/sockstreambuf/sockstreambuf.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/sockstreambuf/sockstreambuf_unbuffered.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/server/server_kernel.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/server/server_iostream.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/server/server_http.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/threads/multithreaded_object_extension.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/threads/threaded_object_extension.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/threads/threads_kernel_1.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/threads/threads_kernel_2.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/threads/threads_kernel_shared.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/threads/thread_pool_extension.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/threads/async.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/timer/timer.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/stack_trace.cpp>

    #ifdef DLIB_PNG_SUPPORT
        #include <addon/dlib-19.17_for_boss/dlib/image_loader/png_loader.cpp>
        #include <addon/dlib-19.17_for_boss/dlib/image_saver/save_png.cpp>
    #endif

    #ifdef DLIB_JPEG_SUPPORT
        #include <addon/dlib-19.17_for_boss/dlib/image_loader/jpeg_loader.cpp>
        #include <addon/dlib-19.17_for_boss/dlib/image_saver/save_jpeg.cpp>
    #endif

    #ifndef DLIB_NO_GUI_SUPPORT
        #include <addon/dlib-19.17_for_boss/dlib/gui_widgets/fonts.cpp>
        #include <addon/dlib-19.17_for_boss/dlib/gui_widgets/widgets.cpp>
        #include <addon/dlib-19.17_for_boss/dlib/gui_widgets/drawable.cpp>
        #include <addon/dlib-19.17_for_boss/dlib/gui_widgets/canvas_drawing.cpp>
        #include <addon/dlib-19.17_for_boss/dlib/gui_widgets/style.cpp>
        #include <addon/dlib-19.17_for_boss/dlib/gui_widgets/base_widgets.cpp>
        #include <addon/dlib-19.17_for_boss/dlib/gui_core/gui_core_kernel_1.cpp>
        #include <addon/dlib-19.17_for_boss/dlib/gui_core/gui_core_kernel_2.cpp>
    #endif // DLIB_NO_GUI_SUPPORT

    #include <addon/dlib-19.17_for_boss/dlib/cuda/cpu_dlib.cpp>
    #include <addon/dlib-19.17_for_boss/dlib/cuda/tensor_tools.cpp>
#endif // DLIB_ISO_CPP_ONLY

#include <addon/dlib-19.17_for_boss/dlib/data_io/image_dataset_metadata.cpp>
#include <addon/dlib-19.17_for_boss/dlib/data_io/mnist.cpp>
#include <addon/dlib-19.17_for_boss/dlib/global_optimization/global_function_search.cpp>
#include <addon/dlib-19.17_for_boss/dlib/filtering/kalman_filter.cpp>
#include <addon/dlib-19.17_for_boss/dlib/svm/auto.cpp>

#endif
