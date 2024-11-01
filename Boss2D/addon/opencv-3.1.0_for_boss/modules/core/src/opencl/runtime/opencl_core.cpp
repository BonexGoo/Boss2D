/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2010-2013, Advanced Micro Devices, Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the OpenCV Foundation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "../../precomp.hpp"

#if defined(HAVE_OPENCL) && !defined(HAVE_OPENCL_STATIC)

#include BOSS_OPENCV_U_opencv2__core_hpp //original-code:"opencv2/core.hpp" // CV_Error

#include BOSS_OPENCV_U_opencv2__core__opencl__runtime__opencl_core_hpp //original-code:"opencv2/core/opencl/runtime/opencl_core.hpp"

#define OPENCL_FUNC_TO_CHECK_1_1 "clEnqueueReadBufferRect"
#define ERROR_MSG_CANT_LOAD "Failed to load OpenCL runtime\n"
#define ERROR_MSG_INVALID_VERSION "Failed to load OpenCL runtime (expected version 1.1+)\n"

#if defined(__APPLE__)
#include <dlfcn.h>

static void* AppleCLGetProcAddress(const char* name)
{
    static bool initialized = false;
    static void* handle = NULL;
    if (!handle)
    {
        if(!initialized)
        {
            initialized = true;
            const char* path = "/System/Library/Frameworks/OpenCL.framework/Versions/Current/OpenCL";
            const char* envPath = getenv("OPENCV_OPENCL_RUNTIME");
            if (envPath)
                path = envPath;
            handle = dlopen(oclpath, RTLD_LAZY | RTLD_GLOBAL);
            if (handle == NULL)
            {
                if (envPath)
                    fprintf(stderr, ERROR_MSG_CANT_LOAD);
            }
            else if (dlsym(handle, OPENCL_FUNC_TO_CHECK_1_1) == NULL)
            {
                fprintf(stderr, ERROR_MSG_INVALID_VERSION);
                handle = NULL;
            }
        }
        if (!handle)
            return NULL;
    }
    return dlsym(handle, name);
}
#define CV_CL_GET_PROC_ADDRESS(name) AppleCLGetProcAddress(name)
#endif // __APPLE__

#if defined(_WIN32)
#include BOSS_FAKEWIN_V_windows_h //original-code:<windows.h>

static void* WinGetProcAddress(const char* name)
{
    static bool initialized = false;
    static HMODULE handle = NULL;
    if (!handle)
    {
        if(!initialized)
        {
            initialized = true;
            handle = GetModuleHandleA("OpenCL.dll");
            if (!handle)
            {
                const char* path = "OpenCL.dll";
                const char* envPath = getenv("OPENCV_OPENCL_RUNTIME");
                if (envPath)
                    path = envPath;
                handle = LoadLibraryA(path);
                if (!handle)
                {
                    if (envPath)
                        fprintf(stderr, ERROR_MSG_CANT_LOAD);
                }
                else if (GetProcAddress(handle, OPENCL_FUNC_TO_CHECK_1_1) == NULL)
                {
                    fprintf(stderr, ERROR_MSG_INVALID_VERSION);
                    handle = NULL;
                }
            }
        }
        if (!handle)
            return NULL;
    }
    return (void*)GetProcAddress(handle, name);
}
#define CV_CL_GET_PROC_ADDRESS(name) WinGetProcAddress(name)
#endif // _WIN32

#if defined(__linux__)
#include <dlfcn.h>
#include <stdio.h>

static void* GetProcAddress(const char* name)
{
    static bool initialized = false;
    static void* handle = NULL;
    if (!handle)
    {
        if(!initialized)
        {
            initialized = true;
            const char* path = "libOpenCL.so";
            const char* envPath = getenv("OPENCV_OPENCL_RUNTIME");
            if (envPath)
                path = envPath;
            handle = dlopen(path, RTLD_LAZY | RTLD_GLOBAL);
            if (handle == NULL)
            {
                if (envPath)
                    fprintf(stderr, ERROR_MSG_CANT_LOAD);
            }
            else if (dlsym(handle, OPENCL_FUNC_TO_CHECK_1_1) == NULL)
            {
                fprintf(stderr, ERROR_MSG_INVALID_VERSION);
                handle = NULL;
            }
        }
        if (!handle)
            return NULL;
    }
    return dlsym(handle, name);
}
#define CV_CL_GET_PROC_ADDRESS(name) GetProcAddress(name)
#endif

#ifndef CV_CL_GET_PROC_ADDRESS
#ifdef __GNUC__
#warning("OPENCV: OpenCL dynamic library loader: check configuration")
#else
#pragma message("WARNING: OPENCV: OpenCL dynamic library loader: check configuration")
#endif
#define CV_CL_GET_PROC_ADDRESS(name) NULL
#endif

static void* opencl_check_fn(int ID);

#include "runtime_common.hpp"

#include "autogenerated/opencl_core_impl.hpp"

//
// BEGIN OF CUSTOM FUNCTIONS
//

#define CUSTOM_FUNCTION_ID 1000

#ifdef HAVE_OPENCL_SVM
#include "opencv2/core/opencl/runtime/opencl_svm_20.hpp"
#define SVM_FUNCTION_ID_START CUSTOM_FUNCTION_ID
#define SVM_FUNCTION_ID_END CUSTOM_FUNCTION_ID + 100

enum OPENCL_FN_SVM_ID
{
    OPENCL_FN_clSVMAlloc = SVM_FUNCTION_ID_START,
    OPENCL_FN_clSVMFree,
    OPENCL_FN_clSetKernelArgSVMPointer,
    OPENCL_FN_clSetKernelExecInfo,
    OPENCL_FN_clEnqueueSVMFree,
    OPENCL_FN_clEnqueueSVMMemcpy,
    OPENCL_FN_clEnqueueSVMMemFill,
    OPENCL_FN_clEnqueueSVMMap,
    OPENCL_FN_clEnqueueSVMUnmap,
};

void* (CL_API_CALL *clSVMAlloc)(cl_context context, cl_svm_mem_flags flags, size_t size, unsigned int alignment) =
        opencl_fn4<OPENCL_FN_clSVMAlloc, void*, cl_context, cl_svm_mem_flags, size_t, unsigned int>::switch_fn;
static const struct DynamicFnEntry _clSVMAlloc_definition = { "clSVMAlloc", (void**)&clSVMAlloc};
void (CL_API_CALL *clSVMFree)(cl_context context, void* svm_pointer) =
        opencl_fn2<OPENCL_FN_clSVMFree, void, cl_context, void*>::switch_fn;
static const struct DynamicFnEntry _clSVMFree_definition = { "clSVMFree", (void**)&clSVMFree};
cl_int (CL_API_CALL *clSetKernelArgSVMPointer)(cl_kernel kernel, cl_uint arg_index, const void* arg_value) =
        opencl_fn3<OPENCL_FN_clSetKernelArgSVMPointer, cl_int, cl_kernel, cl_uint, const void*>::switch_fn;
static const struct DynamicFnEntry _clSetKernelArgSVMPointer_definition = { "clSetKernelArgSVMPointer", (void**)&clSetKernelArgSVMPointer};
//void* (CL_API_CALL *clSetKernelExecInfo)(cl_kernel kernel, cl_kernel_exec_info param_name, size_t param_value_size, const void* param_value) =
//        opencl_fn4<OPENCL_FN_clSetKernelExecInfo, void*, cl_kernel, cl_kernel_exec_info, size_t, const void*>::switch_fn;
//static const struct DynamicFnEntry _clSetKernelExecInfo_definition = { "clSetKernelExecInfo", (void**)&clSetKernelExecInfo};
//cl_int (CL_API_CALL *clEnqueueSVMFree)(...) =
//        opencl_fn8<OPENCL_FN_clEnqueueSVMFree, cl_int, ...>::switch_fn;
//static const struct DynamicFnEntry _clEnqueueSVMFree_definition = { "clEnqueueSVMFree", (void**)&clEnqueueSVMFree};
cl_int (CL_API_CALL *clEnqueueSVMMemcpy)(cl_command_queue command_queue, cl_bool blocking_copy, void* dst_ptr, const void* src_ptr, size_t size, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event) =
        opencl_fn8<OPENCL_FN_clEnqueueSVMMemcpy, cl_int, cl_command_queue, cl_bool, void*, const void*, size_t, cl_uint, const cl_event*, cl_event*>::switch_fn;
static const struct DynamicFnEntry _clEnqueueSVMMemcpy_definition = { "clEnqueueSVMMemcpy", (void**)&clEnqueueSVMMemcpy};
cl_int (CL_API_CALL *clEnqueueSVMMemFill)(cl_command_queue command_queue, void* svm_ptr, const void* pattern, size_t pattern_size, size_t size, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event) =
        opencl_fn8<OPENCL_FN_clEnqueueSVMMemFill, cl_int, cl_command_queue, void*, const void*, size_t, size_t, cl_uint, const cl_event*, cl_event*>::switch_fn;
static const struct DynamicFnEntry _clEnqueueSVMMemFill_definition = { "clEnqueueSVMMemFill", (void**)&clEnqueueSVMMemFill};
cl_int (CL_API_CALL *clEnqueueSVMMap)(cl_command_queue command_queue, cl_bool blocking_map, cl_map_flags map_flags, void* svm_ptr, size_t size, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event) =
        opencl_fn8<OPENCL_FN_clEnqueueSVMMap, cl_int, cl_command_queue, cl_bool, cl_map_flags, void*, size_t, cl_uint, const cl_event*, cl_event*>::switch_fn;
static const struct DynamicFnEntry _clEnqueueSVMMap_definition = { "clEnqueueSVMMap", (void**)&clEnqueueSVMMap};
cl_int (CL_API_CALL *clEnqueueSVMUnmap)(cl_command_queue command_queue, void* svm_ptr, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event) =
        opencl_fn5<OPENCL_FN_clEnqueueSVMUnmap, cl_int, cl_command_queue, void*, cl_uint, const cl_event*, cl_event*>::switch_fn;
static const struct DynamicFnEntry _clEnqueueSVMUnmap_definition = { "clEnqueueSVMUnmap", (void**)&clEnqueueSVMUnmap};

static const struct DynamicFnEntry* opencl_svm_fn_list[] = {
    &_clSVMAlloc_definition,
    &_clSVMFree_definition,
    &_clSetKernelArgSVMPointer_definition,
    NULL/*&_clSetKernelExecInfo_definition*/,
    NULL/*&_clEnqueueSVMFree_definition*/,
    &_clEnqueueSVMMemcpy_definition,
    &_clEnqueueSVMMemFill_definition,
    &_clEnqueueSVMMap_definition,
    &_clEnqueueSVMUnmap_definition,
};
#endif // HAVE_OPENCL_SVM

//
// END OF CUSTOM FUNCTIONS HERE
//

static void* opencl_check_fn(int ID)
{
    const struct DynamicFnEntry* e = NULL;
    if (ID < CUSTOM_FUNCTION_ID)
    {
        assert(ID >= 0 && ID < (int)(sizeof(opencl_fn_list)/sizeof(opencl_fn_list[0])));
        e = opencl_fn_list[ID];
    }
#ifdef HAVE_OPENCL_SVM
    else if (ID >= SVM_FUNCTION_ID_START && ID < SVM_FUNCTION_ID_END)
    {
        ID = ID - SVM_FUNCTION_ID_START;
        assert(ID >= 0 && ID < (int)(sizeof(opencl_svm_fn_list)/sizeof(opencl_svm_fn_list[0])));
        e = opencl_svm_fn_list[ID];
    }
#endif
    else
    {
        CV_ErrorNoReturn(cv::Error::StsBadArg, "Invalid function ID");
    }
    void* func = CV_CL_GET_PROC_ADDRESS(e->fnName);
    if (!func)
    {
        throw cv::Exception(cv::Error::OpenCLApiCallError,
                cv::format("OpenCL function is not available: [%s]", e->fnName),
                CV_Func, __FILE__, __LINE__);
    }
    *(e->ppFn) = func;
    return func;
}

#ifdef HAVE_OPENGL

#include "opencv2/core/opencl/runtime/opencl_gl.hpp"

static void* opencl_gl_check_fn(int ID);

#include "autogenerated/opencl_gl_impl.hpp"

static void* opencl_gl_check_fn(int ID)
{
    const struct DynamicFnEntry* e = NULL;
    assert(ID >= 0 && ID < (int)(sizeof(opencl_gl_fn_list)/sizeof(opencl_gl_fn_list[0])));
    e = opencl_gl_fn_list[ID];
    void* func = CV_CL_GET_PROC_ADDRESS(e->fnName);
    if (!func)
    {
        throw cv::Exception(cv::Error::OpenCLApiCallError,
                cv::format("OpenCL function is not available: [%s]", e->fnName),
                CV_Func, __FILE__, __LINE__);
    }
    *(e->ppFn) = func;
    return func;
}

#endif // HAVE_OPENGL

#endif
