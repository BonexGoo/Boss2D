/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_rtc_base__fileutils_h //original-code:"rtc_base/fileutils.h"

#include BOSS_WEBRTC_U_rtc_base__arraysize_h //original-code:"rtc_base/arraysize.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__pathutils_h //original-code:"rtc_base/pathutils.h"
#include BOSS_WEBRTC_U_rtc_base__stringutils_h //original-code:"rtc_base/stringutils.h"

#if defined(WEBRTC_WIN)
#include BOSS_WEBRTC_U_rtc_base__win32filesystem_h //original-code:"rtc_base/win32filesystem.h"
#else
#include BOSS_WEBRTC_U_rtc_base__unixfilesystem_h //original-code:"rtc_base/unixfilesystem.h"
#endif

#if !defined(WEBRTC_WIN)
#define MAX_PATH 260
#endif

namespace rtc {

//////////////////////////
// Directory Iterator   //
//////////////////////////

// A DirectoryIterator is created with a given directory. It originally points
// to the first file in the directory, and can be advanecd with Next(). This
// allows you to get information about each file.

// Constructor
DirectoryIterator::DirectoryIterator()
#ifdef WEBRTC_WIN
    : handle_(INVALID_HANDLE_VALUE) {
#else
    : dir_(nullptr),
      dirent_(nullptr){
#endif
}

// Destructor
DirectoryIterator::~DirectoryIterator() {
#if defined(WEBRTC_WIN)
  if (handle_ != INVALID_HANDLE_VALUE)
    ::FindClose(handle_);
#else
  if (dir_)
    closedir(dir_);
#endif
}

// Starts traversing a directory.
// dir is the directory to traverse
// returns true if the directory exists and is valid
bool DirectoryIterator::Iterate(const Pathname& dir) {
  directory_ = dir.pathname();
#if defined(WEBRTC_WIN)
  if (handle_ != INVALID_HANDLE_VALUE)
    ::FindClose(handle_);
  std::string d = dir.pathname() + '*';
  handle_ = ::FindFirstFile(ToUtf16(d).c_str(), &data_);
  if (handle_ == INVALID_HANDLE_VALUE)
    return false;
#else
  if (dir_ != nullptr)
    closedir(dir_);
  dir_ = ::opendir(directory_.c_str());
  if (dir_ == nullptr)
    return false;
  dirent_ = readdir(dir_);
  if (dirent_ == nullptr)
    return false;

  if (::stat(std::string(directory_ + Name()).c_str(), &stat_) != 0)
    return false;
#endif
  return true;
}

// Advances to the next file
// returns true if there were more files in the directory.
bool DirectoryIterator::Next() {
#if defined(WEBRTC_WIN)
  return ::FindNextFile(handle_, &data_) == TRUE;
#else
  dirent_ = ::readdir(dir_);
  if (dirent_ == nullptr)
    return false;

  return ::stat(std::string(directory_ + Name()).c_str(), &stat_) == 0;
#endif
}

// returns true if the file currently pointed to is a directory
bool DirectoryIterator::IsDirectory() const {
#if defined(WEBRTC_WIN)
  return (data_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FALSE;
#else
  return S_ISDIR(stat_.st_mode);
#endif
}

// returns the name of the file currently pointed to
std::string DirectoryIterator::Name() const {
#if defined(WEBRTC_WIN)
  return ToUtf8(data_.cFileName);
#else
  RTC_DCHECK(dirent_);
  return dirent_->d_name;
#endif
}

FilesystemInterface* Filesystem::GetFilesystem() {
#if defined(WEBRTC_WIN)
  static FilesystemInterface* const filesystem = new Win32Filesystem();
#else
  static FilesystemInterface* const filesystem = new UnixFilesystem();
#endif

  return filesystem;
}

}  // namespace rtc
