// Copyright 2018 The Abseil Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include BOSS_ABSEILCPP_U_absl__debugging__symbolize_h //original-code:"absl/debugging/symbolize.h"

#ifdef _WIN32
#include <winapifamily.h>
#if !(WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)) || \
    WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
// UWP doesn't have access to win32 APIs.
#define ABSL_INTERNAL_HAVE_SYMBOLIZE_WIN32
#endif
#endif

#if defined(ABSL_INTERNAL_HAVE_ELF_SYMBOLIZE)
#include BOSS_ABSEILCPP_U_absl__debugging__symbolize_elf_inc //original-code:"absl/debugging/symbolize_elf.inc"
#elif defined(ABSL_INTERNAL_HAVE_SYMBOLIZE_WIN32)
// The Windows Symbolizer only works if PDB files containing the debug info
// are available to the program at runtime.
#include BOSS_ABSEILCPP_U_absl__debugging__symbolize_win32_inc //original-code:"absl/debugging/symbolize_win32.inc"
#elif defined(__APPLE__)
#include "absl/debugging/symbolize_darwin.inc"
#elif defined(__EMSCRIPTEN__)
#include "absl/debugging/symbolize_emscripten.inc"
#else
#include BOSS_ABSEILCPP_U_absl__debugging__symbolize_unimplemented_inc //original-code:"absl/debugging/symbolize_unimplemented.inc"
#endif
