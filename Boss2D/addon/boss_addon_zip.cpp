﻿#include <boss.h>

#if !defined(BOSS_NEED_ADDON_ZIP) || (BOSS_NEED_ADDON_ZIP != 0 && BOSS_NEED_ADDON_ZIP != 1)
    #error BOSS_NEED_ADDON_ZIP macro is invalid use
#endif
bool __LINK_ADDON_ZIP__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_ZIP

#ifdef _UNICODE
#undef _UNICODE
#endif
#ifdef UNICODE
#undef UNICODE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C"
{
    #if !BOSS_LINUX & !BOSS_WINDOWS_MINGW
        extern const char* strstr(const char*, char const* const);
    #endif
}

// 도구준비
#include <addon/boss_fakewin.h>

#include "boss_addon_zip.hpp"

#include <boss.hpp>
#include <platform/boss_platform.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(Zip, OpenForBuffer, id_zip, bytes, uint64, sint32*, chars)
    BOSS_DECLARE_ADDON_FUNCTION(Zip, OpenForFD, id_zip, sint32, sint32*, chars)
    BOSS_DECLARE_ADDON_FUNCTION(Zip, Close, void, id_zip)
    BOSS_DECLARE_ADDON_FUNCTION(Zip, ToBuffer, buffer, id_zip, sint32)
    BOSS_DECLARE_ADDON_FUNCTION(Zip, GetFileInfo, chars, id_zip, sint32, uint64*,
        bool*, uint64*, uint64*, uint64*, bool*, bool*, bool*, bool*)

    static autorun Bind_AddOn_Zip()
    {
        Core_AddOn_Zip_OpenForBuffer() = Customized_AddOn_Zip_OpenForBuffer;
        Core_AddOn_Zip_OpenForFD() = Customized_AddOn_Zip_OpenForFD;
        Core_AddOn_Zip_Close() = Customized_AddOn_Zip_Close;
        Core_AddOn_Zip_ToBuffer() = Customized_AddOn_Zip_ToBuffer;
        Core_AddOn_Zip_GetFileInfo() = Customized_AddOn_Zip_GetFileInfo;
        return true;
    }
    static autorun _ = Bind_AddOn_Zip();
}

// THIS FILE is almost entirely based upon code by Jean-loup Gailly
// and Mark Adler. It has been modified by Lucian Wischik.
// The modifications were: incorporate the bugfixes of 1.1.4, allow
// unzipping to/from handles/pipes/files/memory, encryption, unicode,
// a windowsish api, and putting everything into a single .cpp file.
// The original code may be found at http://www.gzip.org/zlib/
// The original copyright text follows.
//
//
//
// zlib.h -- interface of the 'zlib' general purpose compression library
//  version 1.1.3, July 9th, 1998
//
//  Copyright (C) 1995-1998 Jean-loup Gailly and Mark Adler
//
//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any damages
//  arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any purpose,
//  including commercial applications, and to alter it and redistribute it
//  freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software. If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//  2. Altered source versions must be plainly marked as such, and must not be
//     misrepresented as being the original software.
//  3. This notice may not be removed or altered from any source distribution.
//
//  Jean-loup Gailly        Mark Adler
//  jloup@gzip.org          madler@alumni.caltech.edu
//
//
//  The data format used by the zlib library is described by RFCs (Request for
//  Comments) 1950 to 1952 in the files ftp://ds.internic.net/rfc/rfc1950.txt
//  (zlib format), rfc1951.txt (deflate format) and rfc1952.txt (gzip format).
//
//
//     The 'zlib' compression library provides in-memory compression and
//  decompression functions, including integrity checks of the uncompressed
//  data.  This version of the library supports only one compression method
//  (deflation) but other algorithms will be added later and will have the same
//  stream interface.
//
//     Compression can be done in a single step if the buffers are large
//  enough (for example if an input file is mmap'ed), or can be done by
//  repeated calls of the compression function.  In the latter case, the
//  application must provide more input and/or consume the output
//  (providing more output space) before each call.
//
//     The library also supports reading and writing files in gzip (.gz) format
//  with an interface similar to that of stdio.
//
//     The library does not install any signal handler. The decoder checks
//  the consistency of the compressed data, so the library should never
//  crash even in case of corrupted input.
//
// for more info about .ZIP format, see ftp://ftp.cdrom.com/pub/infozip/doc/appnote-970311-iz.zip
//   PkWare has also a specification at ftp://ftp.pkware.com/probdesc.zip

// added 2013.12.12
#ifdef _UNICODE
#undef _UNICODE
#endif
#ifdef UNICODE
#undef UNICODE
#endif

// added 2013.12.12
#define MAX_PATH 1024
#define DECLARE_HANDLE(name) typedef void* name;
#define Int32x32To64(a, b)  ((LONGLONG)(((LONGLONG)((long)(a))) * ((long)(b))))
#define ZeroMemory(a, b) memset(a, 0, b)
#define _T(str) str
#define _tcslen strlen
#define _tcscat strcat
#define _tcsstr strstr
#define _tcscpy strcpy
#define FILE_ATTRIBUTE_READONLY             0x00000001
#define FILE_ATTRIBUTE_HIDDEN               0x00000002
#define FILE_ATTRIBUTE_SYSTEM               0x00000004
#define FILE_ATTRIBUTE_DIRECTORY            0x00000010
#define FILE_ATTRIBUTE_ARCHIVE              0x00000020
#define FILE_ATTRIBUTE_DEVICE               0x00000040
#define FILE_ATTRIBUTE_NORMAL               0x00000080
#define FILE_ATTRIBUTE_TEMPORARY            0x00000100
#define FILE_ATTRIBUTE_SPARSE_FILE          0x00000200
#define FILE_ATTRIBUTE_REPARSE_POINT        0x00000400
#define FILE_ATTRIBUTE_COMPRESSED           0x00000800
#define FILE_ATTRIBUTE_OFFLINE              0x00001000
#define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  0x00002000
#define FILE_ATTRIBUTE_ENCRYPTED            0x00004000
#define FILE_ATTRIBUTE_VIRTUAL              0x00010000
//typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef void* HANDLE;
typedef char TCHAR;
typedef long long LONGLONG;
//typedef struct {DWORD dwLowDateTime; DWORD dwHighDateTime;} FILETIME, *PFILETIME, *LPFILETIME;

DECLARE_HANDLE(HZIP);

typedef DWORD ZRESULT;
// return codes from any of the zip functions. Listed later.

// modified 2013.12.12
typedef struct
{
    int index;                 // index of this file within the zip
    TCHAR name[MAX_PATH];      // filename within the zip
    bool attr_isdir;
    bool attr_archive;
    bool attr_hidden;
    bool attr_readonly;
    bool attr_system;
    FILETIME atime;
    FILETIME ctime;
    FILETIME mtime;
    long comp_size;            // sizes of item, compressed and uncompressed. These
    long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRY;

HZIP OpenZip(void* z, long long len, const char* password);
// OpenZip - opens a zip file and returns a handle with which you can
// subsequently examine its contents. You can open a zip file from:
// from a pipe:             OpenZipHandle(hpipe_read,0);
// from a file (by handle): OpenZipHandle(hfile,0);
// from a file (by name):   OpenZip("c:\\test.zip","password");
// from a memory block:     OpenZip(bufstart, buflen,0);
// If the file is opened through a pipe, then items may only be
// accessed in increasing order, and an item may only be unzipped once,
// although GetZipItem can be called immediately before and after unzipping
// it. If it's opened in any other way, then full random access is possible.
// Note: pipe input is not yet implemented.
// Note: zip passwords are ascii, not unicode.
// Note: for windows-ce, you cannot close the handle until after CloseZip.
// but for real windows, the zip makes its own copy of your handle, so you
// can close yours anytime.

//bx20210730
HZIP OpenZipByFD(sint32 fd, const char* password);

ZRESULT GetZipItem(HZIP hz, int index, ZIPENTRY *ze);
// GetZipItem - call this to get information about an item in the zip.
// If index is -1 and the file wasn't opened through a pipe,
// then it returns information about the whole zipfile
// (and in particular ze.index returns the number of index items).
// Note: the item might be a directory (ze.attr & FILE_ATTRIBUTE_DIRECTORY)
// See below for notes on what happens when you unzip such an item.
// Note: if you are opening the zip through a pipe, then random access
// is not possible and GetZipItem(-1) fails and you can't discover the number
// of items except by calling GetZipItem on each one of them in turn,
// starting at 0, until eventually the call fails. Also, in the event that
// you are opening through a pipe and the zip was itself created into a pipe,
// then then comp_size and sometimes unc_size as well may not be known until
// after the item has been unzipped.

ZRESULT FindZipItem(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRY *ze);
// FindZipItem - finds an item by name. ic means 'insensitive to case'.
// It returns the index of the item, and returns information about it.
// If nothing was found, then index is set to -1 and the function returns
// an error code.

ZRESULT UnzipItem(HZIP hz, int index, void *z, unsigned int len);
// UnzipItem - given an index to an item, unzips it. You can unzip to:
// to a pipe:             UnzipItemHandle(hz,i, hpipe_write);
// to a file (by handle): UnzipItemHandle(hz,i, hfile);
// to a file (by name):   UnzipItem(hz,i, ze.name);
// to a memory block:     UnzipItem(hz,i, buf,buflen);
// In the final case, if the buffer isn't large enough to hold it all,
// then the return code indicates that more is yet to come. If it was
// large enough, and you want to know precisely how big, GetZipItem.
// Note: zip files are normally stored with relative pathnames. If you
// unzip with ZIP_FILENAME a relative pathname then the item gets created
// relative to the current directory - it first ensures that all necessary
// subdirectories have been created. Also, the item may itself be a directory.
// If you unzip a directory with ZIP_FILENAME, then the directory gets created.
// If you unzip it to a handle or a memory block, then nothing gets created
// and it emits 0 bytes.

ZRESULT CloseZip(HZIP hz);
// CloseZip - the zip handle must be closed with this function.

unsigned int FormatZipMessage(ZRESULT code, TCHAR *buf, unsigned int len);
// FormatZipMessage - given an error code, formats it as a string.
// It returns the length of the error message. If buf/len points
// to a real buffer, then it also writes as much as possible into there.

bool IsZipHandle(HZIP hz);

// These are the result codes:
#define ZR_OK         0x00000000     // nb. the pseudo-code zr-recent is never returned,
#define ZR_RECENT     0x00000001     // but can be passed to FormatZipMessage.
// The following come from general system stuff (e.g. files not openable)
#define ZR_GENMASK    0x0000FF00
#define ZR_NODUPH     0x00000100     // couldn't duplicate the handle
#define ZR_NOFILE     0x00000200     // couldn't create/open the file
#define ZR_NOALLOC    0x00000300     // failed to allocate some resource
#define ZR_WRITE      0x00000400     // a general error writing to the file
#define ZR_NOTFOUND   0x00000500     // couldn't find that file in the zip
#define ZR_MORE       0x00000600     // there's still more data to be unzipped
#define ZR_CORRUPT    0x00000700     // the zipfile is corrupt or not a zipfile
#define ZR_READ       0x00000800     // a general error reading the file
#define ZR_PASSWORD   0x00001000     // we didn't get the right password to unzip the file
// The following come from mistakes on the part of the caller
#define ZR_CALLERMASK 0x00FF0000
#define ZR_ARGS       0x00010000     // general mistake with the arguments
#define ZR_NOTMMAP    0x00020000     // tried to ZipGetMemory, but that only works on mmap zipfiles, which yours wasn't
#define ZR_MEMSIZE    0x00030000     // the memory size is too small
#define ZR_FAILED     0x00040000     // the thing was already failed when you called this function
#define ZR_ENDED      0x00050000     // the zip creation has already been closed
#define ZR_MISSIZE    0x00060000     // the indicated input file size turned out mistaken
#define ZR_PARTIALUNZ 0x00070000     // the file had already been partially unzipped
#define ZR_ZMODE      0x00080000     // tried to mix creating/opening a zip 
// The following come from bugs within the zip library itself
#define ZR_BUGMASK    0xFF000000
#define ZR_NOTINITED  0x01000000     // initialisation didn't work
#define ZR_SEEK       0x02000000     // trying to seek in an unseekable file
#define ZR_NOCHANGE   0x04000000     // changed its mind on storage, but not allowed
#define ZR_FLATE      0x05000000     // an internal error in the de/inflation code

// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
// 구현부
namespace BOSS
{
    id_zip Customized_AddOn_Zip_OpenForBuffer(bytes buf, uint64 length, sint32* filecount, chars password)
    {
        id_zip Result = (id_zip) OpenZip((void*) buf, length, password);

        ZIPENTRY ZipEntry;
        GetZipItem((HZIP) Result, -1, &ZipEntry);
        if(filecount) *filecount = ZipEntry.index;
        return Result;
    }

    id_zip Customized_AddOn_Zip_OpenForFD(sint32 fd, sint32* filecount, chars password)
    {
        id_zip Result = (id_zip) OpenZipByFD(fd, password);

        ZIPENTRY ZipEntry;
        GetZipItem((HZIP) Result, -1, &ZipEntry);
        if(filecount) *filecount = ZipEntry.index;
        return Result;
    }

    void Customized_AddOn_Zip_Close(id_zip zip)
    {
        if(zip && IsZipHandle((HZIP) zip))
            CloseZip((HZIP) zip);
    }

    buffer Customized_AddOn_Zip_ToBuffer(id_zip zip, sint32 fileindex)
    {
        if(!zip || !IsZipHandle((HZIP) zip))
            return nullptr;

        ZIPENTRY ZipEntry;
        GetZipItem((HZIP) zip, fileindex, &ZipEntry);

        const sint32 FileSize = ZipEntry.unc_size;
        buffer Result = Buffer::Alloc(BOSS_DBG FileSize);
        UnzipItem((HZIP) zip, fileindex, (uint08*) Result, FileSize);
        return Result;
    }

    chars Customized_AddOn_Zip_GetFileInfo(id_zip zip, sint32 fileindex, uint64* filesize,
        bool* isdir, uint64* ctime, uint64* mtime, uint64* atime,
        bool* archive, bool* hidden, bool* readonly, bool* system)
    {
        if(!zip || !IsZipHandle((HZIP) zip))
            return nullptr;

        static ZIPENTRY ZipEntry;
        GetZipItem((HZIP) zip, fileindex, &ZipEntry);
        if(filesize) *filesize = ZipEntry.unc_size;
        if(isdir) *isdir = ZipEntry.attr_isdir;
        if(ctime) Memory::Copy(ctime, &ZipEntry.ctime, sizeof(uint64));
        if(mtime) Memory::Copy(mtime, &ZipEntry.mtime, sizeof(uint64));
        if(atime) Memory::Copy(atime, &ZipEntry.atime, sizeof(uint64));
        if(archive) *archive = ZipEntry.attr_archive;
        if(hidden) *hidden = ZipEntry.attr_hidden;
        if(readonly) *readonly = ZipEntry.attr_readonly;
        if(system) *system = ZipEntry.attr_system;
        return ZipEntry.name;
    }
}
// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

// modified 2013.12.12
#define zmalloc(len) Memory::Alloc(len)
#define zfree(p) Memory::Free(p)

typedef struct tm_unz_s
{ unsigned int tm_sec;            // seconds after the minute - [0,59]
  unsigned int tm_min;            // minutes after the hour - [0,59]
  unsigned int tm_hour;           // hours since midnight - [0,23]
  unsigned int tm_mday;           // day of the month - [1,31]
  unsigned int tm_mon;            // months since January - [0,11]
  unsigned int tm_year;           // years - [1980..2044]
} tm_unz;

// unz_global_info structure contain global data about the ZIPfile
typedef struct unz_global_info_s
{ unsigned long number_entry;         // total number of entries in the central dir on this disk
  unsigned long size_comment;         // size of the global comment of the zipfile
} unz_global_info;

// unz_file_info contain information about a file in the zipfile
typedef struct unz_file_info_s
{ unsigned long version;              // version made by                 2 bytes
  unsigned long version_needed;       // version needed to extract       2 bytes
  unsigned long flag;                 // general purpose bit flag        2 bytes
  unsigned long compression_method;   // compression method              2 bytes
  unsigned long dosDate;              // last mod file date in Dos fmt   4 bytes
  unsigned long crc;                  // crc-32                          4 bytes
  unsigned long compressed_size;      // compressed size                 4 bytes
  unsigned long uncompressed_size;    // uncompressed size               4 bytes
  unsigned long size_filename;        // filename length                 2 bytes
  unsigned long size_file_extra;      // extra field length              2 bytes
  unsigned long size_file_comment;    // file comment length             2 bytes
  unsigned long disk_num_start;       // disk number start               2 bytes
  unsigned long internal_fa;          // internal file attributes        2 bytes
  unsigned long external_fa;          // external file attributes        4 bytes
  tm_unz tmu_date;
} unz_file_info;

#define UNZ_OK                  (0)
#define UNZ_END_OF_LIST_OF_FILE (-100)
#define UNZ_ERRNO               (Z_ERRNO)
#define UNZ_EOF                 (0)
#define UNZ_PARAMERROR          (-102)
#define UNZ_BADZIPFILE          (-103)
#define UNZ_INTERNALERROR       (-104)
#define UNZ_CRCERROR            (-105)
#define UNZ_PASSWORD            (-106)

#define ZLIB_VERSION "1.1.3"

// Allowed flush values; see deflate() for details
#define Z_NO_FLUSH      0
#define Z_SYNC_FLUSH    2
#define Z_FULL_FLUSH    3
#define Z_FINISH        4

// compression levels
#define Z_NO_COMPRESSION         0
#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)

// compression strategy; see deflateInit2() for details
#define Z_FILTERED            1
#define Z_HUFFMAN_ONLY        2
#define Z_DEFAULT_STRATEGY    0

// Possible values of the data_type field
#define Z_BINARY   0
#define Z_ASCII    1
#define Z_UNKNOWN  2

// The deflate compression method (the only one supported in this version)
#define Z_DEFLATED   8

// for initializing zalloc, zfree, opaque
#define Z_NULL  0

// case sensitivity when searching for filenames
#define CASE_SENSITIVE 1
#define CASE_INSENSITIVE 2

// Return codes for the compression/decompression functions. Negative
// values are errors, positive values are used for special but normal events.
#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)

// Basic data types
typedef unsigned char  Byte;  // 8 bits
typedef unsigned int   uInt;  // 16 bits or more
typedef unsigned long  uLong; // 32 bits or more
typedef unsigned long long  uLongLong; // 64 bits or more
typedef void *voidpf;
typedef void     *voidp;
typedef long z_off_t;

typedef voidpf (*alloc_func) (voidpf opaque, uInt items, uInt size);
typedef void   (*free_func)  (voidpf opaque, voidpf address);

struct internal_state;

typedef struct z_stream_s {
    Byte    *next_in;  // next input byte
    uInt     avail_in;  // number of bytes available at next_in
    uLong    total_in;  // total nb of input bytes read so far

    Byte    *next_out; // next output byte should be put there
    uInt     avail_out; // remaining free space at next_out
    uLong    total_out; // total nb of bytes output so far

    char     *msg;      // last error message, NULL if no error
    struct internal_state *state; // not visible by applications

    alloc_func zalloc;  // used to allocate the internal state
    free_func  zfree;   // used to free the internal state
    voidpf     opaque;  // private data object passed to zalloc and zfree

    int     data_type;  // best guess about the data type: ascii or binary
    uLong   adler;      // adler32 value of the uncompressed data
    uLong   reserved;   // reserved for future use
} z_stream;

typedef z_stream *z_streamp;

//   The application must update next_in and avail_in when avail_in has
//   dropped to zero. It must update next_out and avail_out when avail_out
//   has dropped to zero. The application must initialize zalloc, zfree and
//   opaque before calling the init function. All other fields are set by the
//   compression library and must not be updated by the application.
//
//   The opaque value provided by the application will be passed as the first
//   parameter for calls of zalloc and zfree. This can be useful for custom
//   memory management. The compression library attaches no meaning to the
//   opaque value.
//
//   zalloc must return Z_NULL if there is not enough memory for the object.
//   If zlib is used in a multi-threaded application, zalloc and zfree must be
//   thread safe.
//
//   The fields total_in and total_out can be used for statistics or
//   progress reports. After compression, total_in holds the total size of
//   the uncompressed data and may be saved for use in the decompressor
//   (particularly if the decompressor wants to decompress everything in
//   a single step).
//

// basic functions

const char *zlibVersion ();
// The application can compare zlibVersion and ZLIB_VERSION for consistency.
// If the first character differs, the library code actually used is
// not compatible with the zlib.h header file used by the application.
// This check is automatically made by inflateInit.

int inflate (z_streamp strm, int flush);
//
//    inflate decompresses as much data as possible, and stops when the input
//  buffer becomes empty or the output buffer becomes full. It may some
//  introduce some output latency (reading input without producing any output)
//  except when forced to flush.
//
//  The detailed semantics are as follows. inflate performs one or both of the
//  following actions:
//
//  - Decompress more input starting at next_in and update next_in and avail_in
//    accordingly. If not all input can be processed (because there is not
//    enough room in the output buffer), next_in is updated and processing
//    will resume at this point for the next call of inflate().
//
//  - Provide more output starting at next_out and update next_out and avail_out
//    accordingly.  inflate() provides as much output as possible, until there
//    is no more input data or no more space in the output buffer (see below
//    about the flush parameter).
//
//  Before the call of inflate(), the application should ensure that at least
//  one of the actions is possible, by providing more input and/or consuming
//  more output, and updating the next_* and avail_* values accordingly.
//  The application can consume the uncompressed output when it wants, for
//  example when the output buffer is full (avail_out == 0), or after each
//  call of inflate(). If inflate returns Z_OK and with zero avail_out, it
//  must be called again after making room in the output buffer because there
//  might be more output pending.
//
//    If the parameter flush is set to Z_SYNC_FLUSH, inflate flushes as much
//  output as possible to the output buffer. The flushing behavior of inflate is
//  not specified for values of the flush parameter other than Z_SYNC_FLUSH
//  and Z_FINISH, but the current implementation actually flushes as much output
//  as possible anyway.
//
//    inflate() should normally be called until it returns Z_STREAM_END or an
//  error. However if all decompression is to be performed in a single step
//  (a single call of inflate), the parameter flush should be set to
//  Z_FINISH. In this case all pending input is processed and all pending
//  output is flushed; avail_out must be large enough to hold all the
//  uncompressed data. (The size of the uncompressed data may have been saved
//  by the compressor for this purpose.) The next operation on this stream must
//  be inflateEnd to deallocate the decompression state. The use of Z_FINISH
//  is never required, but can be used to inform inflate that a faster routine
//  may be used for the single inflate() call.
//
//     If a preset dictionary is needed at this point (see inflateSetDictionary
//  below), inflate sets strm-adler to the adler32 checksum of the
//  dictionary chosen by the compressor and returns Z_NEED_DICT; otherwise
//  it sets strm->adler to the adler32 checksum of all output produced
//  so far (that is, total_out bytes) and returns Z_OK, Z_STREAM_END or
//  an error code as described below. At the end of the stream, inflate()
//  checks that its computed adler32 checksum is equal to that saved by the
//  compressor and returns Z_STREAM_END only if the checksum is correct.
//
//    inflate() returns Z_OK if some progress has been made (more input processed
//  or more output produced), Z_STREAM_END if the end of the compressed data has
//  been reached and all uncompressed output has been produced, Z_NEED_DICT if a
//  preset dictionary is needed at this point, Z_DATA_ERROR if the input data was
//  corrupted (input stream not conforming to the zlib format or incorrect
//  adler32 checksum), Z_STREAM_ERROR if the stream structure was inconsistent
//  (for example if next_in or next_out was NULL), Z_MEM_ERROR if there was not
//  enough memory, Z_BUF_ERROR if no progress is possible or if there was not
//  enough room in the output buffer when Z_FINISH is used. In the Z_DATA_ERROR
//  case, the application may then call inflateSync to look for a good
//  compression block.
//

int inflateEnd (z_streamp strm);
//
//     All dynamically allocated data structures for this stream are freed.
//   This function discards any unprocessed input and does not flush any
//   pending output.
//
//     inflateEnd returns Z_OK if success, Z_STREAM_ERROR if the stream state
//   was inconsistent. In the error case, msg may be set but then points to a
//   static string (which must not be deallocated).

                        // Advanced functions 

//  The following functions are needed only in some special applications.

int inflateSetDictionary (z_streamp strm,
                                             const Byte *dictionary,
                                             uInt  dictLength);
//
//     Initializes the decompression dictionary from the given uncompressed byte
//   sequence. This function must be called immediately after a call of inflate
//   if this call returned Z_NEED_DICT. The dictionary chosen by the compressor
//   can be determined from the Adler32 value returned by this call of
//   inflate. The compressor and decompressor must use exactly the same
//   dictionary. 
//
//     inflateSetDictionary returns Z_OK if success, Z_STREAM_ERROR if a
//   parameter is invalid (such as NULL dictionary) or the stream state is
//   inconsistent, Z_DATA_ERROR if the given dictionary doesn't match the
//   expected one (incorrect Adler32 value). inflateSetDictionary does not
//   perform any decompression: this will be done by subsequent calls of
//   inflate().

int inflateSync (z_streamp strm);
// 
//    Skips invalid compressed data until a full flush point can be found, or until all
//  available input is skipped. No output is provided.
//
//    inflateSync returns Z_OK if a full flush point has been found, Z_BUF_ERROR
//  if no more input was provided, Z_DATA_ERROR if no flush point has been found,
//  or Z_STREAM_ERROR if the stream structure was inconsistent. In the success
//  case, the application may save the current current value of total_in which
//  indicates where valid compressed data was found. In the error case, the
//  application may repeatedly call inflateSync, providing more input each time,
//  until success or end of the input data.

int inflateReset (z_streamp strm);
//     This function is equivalent to inflateEnd followed by inflateInit,
//   but does not free and reallocate all the internal decompression state.
//   The stream will keep attributes that may have been set by inflateInit2.
//
//      inflateReset returns Z_OK if success, or Z_STREAM_ERROR if the source
//   stream state was inconsistent (such as zalloc or state being NULL).
//

// checksum functions
// These functions are not related to compression but are exported
// anyway because they might be useful in applications using the
// compression library.

uLong adler32 (uLong adler, const Byte *buf, uInt len);
//     Update a running Adler-32 checksum with the bytes buf[0..len-1] and
//   return the updated checksum. If buf is NULL, this function returns
//   the required initial value for the checksum.
//   An Adler-32 checksum is almost as reliable as a CRC32 but can be computed
//   much faster. Usage example:
//
//     uLong adler = adler32(0L, Z_NULL, 0);
//
//     while (read_buffer(buffer, length) != EOF) {
//       adler = adler32(adler, buffer, length);
//     }
//     if (adler != original_adler) error();

uLong ucrc32   (uLong crc, const Byte *buf, uInt len);
//     Update a running crc with the bytes buf[0..len-1] and return the updated
//   crc. If buf is NULL, this function returns the required initial value
//   for the crc. Pre- and post-conditioning (one's complement) is performed
//   within this function so it shouldn't be done by the application.
//   Usage example:
//
//     uLong crc = crc32(0L, Z_NULL, 0);
//
//     while (read_buffer(buffer, length) != EOF) {
//       crc = crc32(crc, buffer, length);
//     }
//     if (crc != original_crc) error();

const char   *zError           (int err);
int           inflateSyncPoint (z_streamp z);
const uLong *get_crc_table    (void);

typedef unsigned char  uch;
typedef uch uchf;
typedef unsigned short ush;
typedef ush ushf;
typedef unsigned long  ulg;

const char * const z_errmsg[10] = { // indexed by 2-zlib_error
"need dictionary",     // Z_NEED_DICT       2
"stream end",          // Z_STREAM_END      1
"",                    // Z_OK              0
"file error",          // Z_ERRNO         (-1)
"stream error",        // Z_STREAM_ERROR  (-2)
"data error",          // Z_DATA_ERROR    (-3)
"insufficient memory", // Z_MEM_ERROR     (-4)
"buffer error",        // Z_BUF_ERROR     (-5)
"incompatible version",// Z_VERSION_ERROR (-6)
""};

#define ERR_MSG(err) z_errmsg[Z_NEED_DICT-(err)]

#define ERR_RETURN(strm,err) \
  return (strm->msg = (char*)ERR_MSG(err), (err))
// To be used only when the state is known to be valid 

// common constants
#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2
// The three kinds of block type 

#define MIN_MATCH  3
#define MAX_MATCH  258
// The minimum and maximum match lengths 

#define PRESET_DICT 0x20 // preset dictionary flag in zlib header 

// target dependencies 
#define OS_CODE  0x0b  // Window 95 & Windows NT

// functions 
#define zmemzero(dest, len) memset(dest, 0, len)

// Diagnostic functions
#define LuAssert(cond,msg)
#define LuTrace(x)
#define LuTracev(x)
#define LuTracevv(x)
#define LuTracec(c,x)
#define LuTracecv(c,x)

typedef uLong (*check_func) (uLong check, const Byte *buf, uInt len);
voidpf zcalloc (voidpf opaque, unsigned items, unsigned size);
void   zcfree  (voidpf opaque, voidpf ptr);

#define ZALLOC(strm, items, size) \
           (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)  (*((strm)->zfree))((strm)->opaque, (voidpf)(addr))

//void ZFREE(z_streamp strm,voidpf addr)
//{ *((strm)->zfree))((strm)->opaque, addr);
//}

#define TRY_FREE(s, p) {if (p) ZFREE(s, p);}

// Huffman code lookup table entry--this entry is four bytes for machines
// that have 16-bit pointers (e.g. PC's in the small or medium model).

typedef struct inflate_huft_s inflate_huft;

struct inflate_huft_s {
  union {
    struct {
      Byte Exop;        // number of extra bits or operation
      Byte Bits;        // number of bits in this code or subcode
    } what;
    uInt pad;           // pad structure to a power of 2 (4 bytes for
  } word;               //  16-bit, 8 bytes for 32-bit int's)
  uInt base;            // literal, length base, distance base, or table offset
};

// Maximum size of dynamic tree.  The maximum found in a long but non-
//   exhaustive search was 1004 huft structures (850 for length/literals
//   and 154 for distances, the latter actually the result of an
//   exhaustive search).  The actual maximum is not known, but the
//   value below is more than safe.
#define MANY 1440

int inflate_trees_bits (
    uInt *,                    // 19 code lengths
    uInt *,                    // bits tree desired/actual depth
    inflate_huft * *,       // bits tree result
    inflate_huft *,             // space for trees
    z_streamp);                // for messages

int inflate_trees_dynamic (
    uInt,                       // number of literal/length codes
    uInt,                       // number of distance codes
    uInt *,                    // that many (total) code lengths
    uInt *,                    // literal desired/actual bit depth
    uInt *,                    // distance desired/actual bit depth
    inflate_huft * *,       // literal/length tree result
    inflate_huft * *,       // distance tree result
    inflate_huft *,             // space for trees
    z_streamp);                // for messages

int inflate_trees_fixed (
    uInt *,                    // literal desired/actual bit depth
    uInt *,                    // distance desired/actual bit depth
    const inflate_huft * *,       // literal/length tree result
    const inflate_huft * *,       // distance tree result
    z_streamp);                // for memory allocation

struct inflate_blocks_state;
typedef struct inflate_blocks_state inflate_blocks_statef;

inflate_blocks_statef * inflate_blocks_new (
    z_streamp z,
    check_func c,               // check function
    uInt w);                   // window size

int inflate_blocks (
    inflate_blocks_statef *,
    z_streamp ,
    int);                      // initial return code

void inflate_blocks_reset (
    inflate_blocks_statef *,
    z_streamp ,
    uLong *);                  // check value on output

int inflate_blocks_free (
    inflate_blocks_statef *,
    z_streamp);

void inflate_set_dictionary (
    inflate_blocks_statef *s,
    const Byte *d,  // dictionary
    uInt  n);       // dictionary length

int inflate_blocks_sync_point (
    inflate_blocks_statef *s);

struct inflate_codes_state;
typedef struct inflate_codes_state inflate_codes_statef;

inflate_codes_statef *inflate_codes_new (
    uInt, uInt,
    const inflate_huft *, const inflate_huft *,
    z_streamp );

int inflate_codes (
    inflate_blocks_statef *,
    z_streamp ,
    int);

void inflate_codes_free (
    inflate_codes_statef *,
    z_streamp );

typedef enum {
      IBM_TYPE,     // get type bits (3, including end bit)
      IBM_LENS,     // get lengths for stored
      IBM_STORED,   // processing stored block
      IBM_TABLE,    // get table lengths
      IBM_BTREE,    // get bit lengths tree for a dynamic block
      IBM_DTREE,    // get length, distance trees for a dynamic block
      IBM_CODES,    // processing fixed or dynamic block
      IBM_DRY,      // output remaining window bytes
      IBM_DONE,     // finished last block, done 
      IBM_BAD}      // got a data error--stuck here 
inflate_block_mode;

// inflate blocks semi-private state 
struct inflate_blocks_state {

  // mode 
  inflate_block_mode  mode;     // current inflate_block mode 

  // mode dependent information 
  union {
    uInt left;          // if STORED, bytes left to copy 
    struct {
      uInt table;               // table lengths (14 bits) 
      uInt index;               // index into blens (or border)
      uInt *blens;             // bit lengths of codes
      uInt bb;                  // bit length tree depth 
      inflate_huft *tb;         // bit length decoding tree 
    } trees;            // if DTREE, decoding info for trees 
    struct {
      inflate_codes_statef 
         *codes;
    } decode;           // if CODES, current state 
  } sub;                // submode
  uInt last;            // true if this block is the last block 

  // mode independent information 
  uInt bitk;            // bits in bit buffer 
  uLong bitb;           // bit buffer 
  inflate_huft *hufts;  // single malloc for tree space 
  Byte *window;        // sliding window 
  Byte *end;           // one byte after sliding window 
  Byte *read;          // window read pointer 
  Byte *write;         // window write pointer 
  check_func checkfn;   // check function 
  uLong check;          // check on output 

};

// defines for inflate input/output
//   update pointers and return 
#define UPDBITS {s->bitb=b;s->bitk=k;}
#define UPDIN {z->avail_in=n;z->total_in+=(uLong)(p-z->next_in);z->next_in=p;}
#define UPDOUT {s->write=q;}
#define UPDATE {UPDBITS UPDIN UPDOUT}
#define LEAVE {UPDATE return inflate_flush(s,z,r);}
//   get bytes and bits 
#define LOADIN {p=z->next_in;n=z->avail_in;b=s->bitb;k=s->bitk;}
#define NEEDBYTE {if(n)r=Z_OK;else LEAVE}
#define NEXTBYTE (n--,*p++)
#define NEEDBITS(j) {while(k<(j)){NEEDBYTE;b|=((uLong)NEXTBYTE)<<k;k+=8;}}
#define DUMPBITS(j) {b>>=(j);k-=(j);}
//   output bytes 
#define WAVAIL (uInt)(q<s->read?s->read-q-1:s->end-q)
#define LOADOUT {q=s->write;m=(uInt)WAVAIL;m;}
#define WRAP {if(q==s->end&&s->read!=s->window){q=s->window;m=(uInt)WAVAIL;}}
#define FLUSH {UPDOUT r=inflate_flush(s,z,r); LOADOUT}
#define NEEDOUT {if(m==0){WRAP if(m==0){FLUSH WRAP if(m==0) LEAVE}}r=Z_OK;}
#define OUTBYTE(a) {*q++=(Byte)(a);m--;}
//   load local pointers 
#define LOAD {LOADIN LOADOUT}

// masks for lower bits (size given to avoid silly warnings with Visual C++) 
// And'ing with mask[n] masks the lower n bits
const uInt inflate_mask[17] = {
    0x0000,
    0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
    0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};

// copy as much as possible from the sliding window to the output area
int inflate_flush (inflate_blocks_statef *, z_streamp, int);

int inflate_fast (uInt, uInt, const inflate_huft *, const inflate_huft *, inflate_blocks_statef *, z_streamp );

const uInt fixed_bl = 9;
const uInt fixed_bd = 5;
const inflate_huft fixed_tl[] = {
    {{{96,7}},256}, {{{0,8}},80}, {{{0,8}},16}, {{{84,8}},115},
    {{{82,7}},31}, {{{0,8}},112}, {{{0,8}},48}, {{{0,9}},192},
    {{{80,7}},10}, {{{0,8}},96}, {{{0,8}},32}, {{{0,9}},160},
    {{{0,8}},0}, {{{0,8}},128}, {{{0,8}},64}, {{{0,9}},224},
    {{{80,7}},6}, {{{0,8}},88}, {{{0,8}},24}, {{{0,9}},144},
    {{{83,7}},59}, {{{0,8}},120}, {{{0,8}},56}, {{{0,9}},208},
    {{{81,7}},17}, {{{0,8}},104}, {{{0,8}},40}, {{{0,9}},176},
    {{{0,8}},8}, {{{0,8}},136}, {{{0,8}},72}, {{{0,9}},240},
    {{{80,7}},4}, {{{0,8}},84}, {{{0,8}},20}, {{{85,8}},227},
    {{{83,7}},43}, {{{0,8}},116}, {{{0,8}},52}, {{{0,9}},200},
    {{{81,7}},13}, {{{0,8}},100}, {{{0,8}},36}, {{{0,9}},168},
    {{{0,8}},4}, {{{0,8}},132}, {{{0,8}},68}, {{{0,9}},232},
    {{{80,7}},8}, {{{0,8}},92}, {{{0,8}},28}, {{{0,9}},152},
    {{{84,7}},83}, {{{0,8}},124}, {{{0,8}},60}, {{{0,9}},216},
    {{{82,7}},23}, {{{0,8}},108}, {{{0,8}},44}, {{{0,9}},184},
    {{{0,8}},12}, {{{0,8}},140}, {{{0,8}},76}, {{{0,9}},248},
    {{{80,7}},3}, {{{0,8}},82}, {{{0,8}},18}, {{{85,8}},163},
    {{{83,7}},35}, {{{0,8}},114}, {{{0,8}},50}, {{{0,9}},196},
    {{{81,7}},11}, {{{0,8}},98}, {{{0,8}},34}, {{{0,9}},164},
    {{{0,8}},2}, {{{0,8}},130}, {{{0,8}},66}, {{{0,9}},228},
    {{{80,7}},7}, {{{0,8}},90}, {{{0,8}},26}, {{{0,9}},148},
    {{{84,7}},67}, {{{0,8}},122}, {{{0,8}},58}, {{{0,9}},212},
    {{{82,7}},19}, {{{0,8}},106}, {{{0,8}},42}, {{{0,9}},180},
    {{{0,8}},10}, {{{0,8}},138}, {{{0,8}},74}, {{{0,9}},244},
    {{{80,7}},5}, {{{0,8}},86}, {{{0,8}},22}, {{{192,8}},0},
    {{{83,7}},51}, {{{0,8}},118}, {{{0,8}},54}, {{{0,9}},204},
    {{{81,7}},15}, {{{0,8}},102}, {{{0,8}},38}, {{{0,9}},172},
    {{{0,8}},6}, {{{0,8}},134}, {{{0,8}},70}, {{{0,9}},236},
    {{{80,7}},9}, {{{0,8}},94}, {{{0,8}},30}, {{{0,9}},156},
    {{{84,7}},99}, {{{0,8}},126}, {{{0,8}},62}, {{{0,9}},220},
    {{{82,7}},27}, {{{0,8}},110}, {{{0,8}},46}, {{{0,9}},188},
    {{{0,8}},14}, {{{0,8}},142}, {{{0,8}},78}, {{{0,9}},252},
    {{{96,7}},256}, {{{0,8}},81}, {{{0,8}},17}, {{{85,8}},131},
    {{{82,7}},31}, {{{0,8}},113}, {{{0,8}},49}, {{{0,9}},194},
    {{{80,7}},10}, {{{0,8}},97}, {{{0,8}},33}, {{{0,9}},162},
    {{{0,8}},1}, {{{0,8}},129}, {{{0,8}},65}, {{{0,9}},226},
    {{{80,7}},6}, {{{0,8}},89}, {{{0,8}},25}, {{{0,9}},146},
    {{{83,7}},59}, {{{0,8}},121}, {{{0,8}},57}, {{{0,9}},210},
    {{{81,7}},17}, {{{0,8}},105}, {{{0,8}},41}, {{{0,9}},178},
    {{{0,8}},9}, {{{0,8}},137}, {{{0,8}},73}, {{{0,9}},242},
    {{{80,7}},4}, {{{0,8}},85}, {{{0,8}},21}, {{{80,8}},258},
    {{{83,7}},43}, {{{0,8}},117}, {{{0,8}},53}, {{{0,9}},202},
    {{{81,7}},13}, {{{0,8}},101}, {{{0,8}},37}, {{{0,9}},170},
    {{{0,8}},5}, {{{0,8}},133}, {{{0,8}},69}, {{{0,9}},234},
    {{{80,7}},8}, {{{0,8}},93}, {{{0,8}},29}, {{{0,9}},154},
    {{{84,7}},83}, {{{0,8}},125}, {{{0,8}},61}, {{{0,9}},218},
    {{{82,7}},23}, {{{0,8}},109}, {{{0,8}},45}, {{{0,9}},186},
    {{{0,8}},13}, {{{0,8}},141}, {{{0,8}},77}, {{{0,9}},250},
    {{{80,7}},3}, {{{0,8}},83}, {{{0,8}},19}, {{{85,8}},195},
    {{{83,7}},35}, {{{0,8}},115}, {{{0,8}},51}, {{{0,9}},198},
    {{{81,7}},11}, {{{0,8}},99}, {{{0,8}},35}, {{{0,9}},166},
    {{{0,8}},3}, {{{0,8}},131}, {{{0,8}},67}, {{{0,9}},230},
    {{{80,7}},7}, {{{0,8}},91}, {{{0,8}},27}, {{{0,9}},150},
    {{{84,7}},67}, {{{0,8}},123}, {{{0,8}},59}, {{{0,9}},214},
    {{{82,7}},19}, {{{0,8}},107}, {{{0,8}},43}, {{{0,9}},182},
    {{{0,8}},11}, {{{0,8}},139}, {{{0,8}},75}, {{{0,9}},246},
    {{{80,7}},5}, {{{0,8}},87}, {{{0,8}},23}, {{{192,8}},0},
    {{{83,7}},51}, {{{0,8}},119}, {{{0,8}},55}, {{{0,9}},206},
    {{{81,7}},15}, {{{0,8}},103}, {{{0,8}},39}, {{{0,9}},174},
    {{{0,8}},7}, {{{0,8}},135}, {{{0,8}},71}, {{{0,9}},238},
    {{{80,7}},9}, {{{0,8}},95}, {{{0,8}},31}, {{{0,9}},158},
    {{{84,7}},99}, {{{0,8}},127}, {{{0,8}},63}, {{{0,9}},222},
    {{{82,7}},27}, {{{0,8}},111}, {{{0,8}},47}, {{{0,9}},190},
    {{{0,8}},15}, {{{0,8}},143}, {{{0,8}},79}, {{{0,9}},254},
    {{{96,7}},256}, {{{0,8}},80}, {{{0,8}},16}, {{{84,8}},115},
    {{{82,7}},31}, {{{0,8}},112}, {{{0,8}},48}, {{{0,9}},193},
    {{{80,7}},10}, {{{0,8}},96}, {{{0,8}},32}, {{{0,9}},161},
    {{{0,8}},0}, {{{0,8}},128}, {{{0,8}},64}, {{{0,9}},225},
    {{{80,7}},6}, {{{0,8}},88}, {{{0,8}},24}, {{{0,9}},145},
    {{{83,7}},59}, {{{0,8}},120}, {{{0,8}},56}, {{{0,9}},209},
    {{{81,7}},17}, {{{0,8}},104}, {{{0,8}},40}, {{{0,9}},177},
    {{{0,8}},8}, {{{0,8}},136}, {{{0,8}},72}, {{{0,9}},241},
    {{{80,7}},4}, {{{0,8}},84}, {{{0,8}},20}, {{{85,8}},227},
    {{{83,7}},43}, {{{0,8}},116}, {{{0,8}},52}, {{{0,9}},201},
    {{{81,7}},13}, {{{0,8}},100}, {{{0,8}},36}, {{{0,9}},169},
    {{{0,8}},4}, {{{0,8}},132}, {{{0,8}},68}, {{{0,9}},233},
    {{{80,7}},8}, {{{0,8}},92}, {{{0,8}},28}, {{{0,9}},153},
    {{{84,7}},83}, {{{0,8}},124}, {{{0,8}},60}, {{{0,9}},217},
    {{{82,7}},23}, {{{0,8}},108}, {{{0,8}},44}, {{{0,9}},185},
    {{{0,8}},12}, {{{0,8}},140}, {{{0,8}},76}, {{{0,9}},249},
    {{{80,7}},3}, {{{0,8}},82}, {{{0,8}},18}, {{{85,8}},163},
    {{{83,7}},35}, {{{0,8}},114}, {{{0,8}},50}, {{{0,9}},197},
    {{{81,7}},11}, {{{0,8}},98}, {{{0,8}},34}, {{{0,9}},165},
    {{{0,8}},2}, {{{0,8}},130}, {{{0,8}},66}, {{{0,9}},229},
    {{{80,7}},7}, {{{0,8}},90}, {{{0,8}},26}, {{{0,9}},149},
    {{{84,7}},67}, {{{0,8}},122}, {{{0,8}},58}, {{{0,9}},213},
    {{{82,7}},19}, {{{0,8}},106}, {{{0,8}},42}, {{{0,9}},181},
    {{{0,8}},10}, {{{0,8}},138}, {{{0,8}},74}, {{{0,9}},245},
    {{{80,7}},5}, {{{0,8}},86}, {{{0,8}},22}, {{{192,8}},0},
    {{{83,7}},51}, {{{0,8}},118}, {{{0,8}},54}, {{{0,9}},205},
    {{{81,7}},15}, {{{0,8}},102}, {{{0,8}},38}, {{{0,9}},173},
    {{{0,8}},6}, {{{0,8}},134}, {{{0,8}},70}, {{{0,9}},237},
    {{{80,7}},9}, {{{0,8}},94}, {{{0,8}},30}, {{{0,9}},157},
    {{{84,7}},99}, {{{0,8}},126}, {{{0,8}},62}, {{{0,9}},221},
    {{{82,7}},27}, {{{0,8}},110}, {{{0,8}},46}, {{{0,9}},189},
    {{{0,8}},14}, {{{0,8}},142}, {{{0,8}},78}, {{{0,9}},253},
    {{{96,7}},256}, {{{0,8}},81}, {{{0,8}},17}, {{{85,8}},131},
    {{{82,7}},31}, {{{0,8}},113}, {{{0,8}},49}, {{{0,9}},195},
    {{{80,7}},10}, {{{0,8}},97}, {{{0,8}},33}, {{{0,9}},163},
    {{{0,8}},1}, {{{0,8}},129}, {{{0,8}},65}, {{{0,9}},227},
    {{{80,7}},6}, {{{0,8}},89}, {{{0,8}},25}, {{{0,9}},147},
    {{{83,7}},59}, {{{0,8}},121}, {{{0,8}},57}, {{{0,9}},211},
    {{{81,7}},17}, {{{0,8}},105}, {{{0,8}},41}, {{{0,9}},179},
    {{{0,8}},9}, {{{0,8}},137}, {{{0,8}},73}, {{{0,9}},243},
    {{{80,7}},4}, {{{0,8}},85}, {{{0,8}},21}, {{{80,8}},258},
    {{{83,7}},43}, {{{0,8}},117}, {{{0,8}},53}, {{{0,9}},203},
    {{{81,7}},13}, {{{0,8}},101}, {{{0,8}},37}, {{{0,9}},171},
    {{{0,8}},5}, {{{0,8}},133}, {{{0,8}},69}, {{{0,9}},235},
    {{{80,7}},8}, {{{0,8}},93}, {{{0,8}},29}, {{{0,9}},155},
    {{{84,7}},83}, {{{0,8}},125}, {{{0,8}},61}, {{{0,9}},219},
    {{{82,7}},23}, {{{0,8}},109}, {{{0,8}},45}, {{{0,9}},187},
    {{{0,8}},13}, {{{0,8}},141}, {{{0,8}},77}, {{{0,9}},251},
    {{{80,7}},3}, {{{0,8}},83}, {{{0,8}},19}, {{{85,8}},195},
    {{{83,7}},35}, {{{0,8}},115}, {{{0,8}},51}, {{{0,9}},199},
    {{{81,7}},11}, {{{0,8}},99}, {{{0,8}},35}, {{{0,9}},167},
    {{{0,8}},3}, {{{0,8}},131}, {{{0,8}},67}, {{{0,9}},231},
    {{{80,7}},7}, {{{0,8}},91}, {{{0,8}},27}, {{{0,9}},151},
    {{{84,7}},67}, {{{0,8}},123}, {{{0,8}},59}, {{{0,9}},215},
    {{{82,7}},19}, {{{0,8}},107}, {{{0,8}},43}, {{{0,9}},183},
    {{{0,8}},11}, {{{0,8}},139}, {{{0,8}},75}, {{{0,9}},247},
    {{{80,7}},5}, {{{0,8}},87}, {{{0,8}},23}, {{{192,8}},0},
    {{{83,7}},51}, {{{0,8}},119}, {{{0,8}},55}, {{{0,9}},207},
    {{{81,7}},15}, {{{0,8}},103}, {{{0,8}},39}, {{{0,9}},175},
    {{{0,8}},7}, {{{0,8}},135}, {{{0,8}},71}, {{{0,9}},239},
    {{{80,7}},9}, {{{0,8}},95}, {{{0,8}},31}, {{{0,9}},159},
    {{{84,7}},99}, {{{0,8}},127}, {{{0,8}},63}, {{{0,9}},223},
    {{{82,7}},27}, {{{0,8}},111}, {{{0,8}},47}, {{{0,9}},191},
    {{{0,8}},15}, {{{0,8}},143}, {{{0,8}},79}, {{{0,9}},255}
  };
const inflate_huft fixed_td[] = {
    {{{80,5}},1}, {{{87,5}},257}, {{{83,5}},17}, {{{91,5}},4097},
    {{{81,5}},5}, {{{89,5}},1025}, {{{85,5}},65}, {{{93,5}},16385},
    {{{80,5}},3}, {{{88,5}},513}, {{{84,5}},33}, {{{92,5}},8193},
    {{{82,5}},9}, {{{90,5}},2049}, {{{86,5}},129}, {{{192,5}},24577},
    {{{80,5}},2}, {{{87,5}},385}, {{{83,5}},25}, {{{91,5}},6145},
    {{{81,5}},7}, {{{89,5}},1537}, {{{85,5}},97}, {{{93,5}},24577},
    {{{80,5}},4}, {{{88,5}},769}, {{{84,5}},49}, {{{92,5}},12289},
    {{{82,5}},13}, {{{90,5}},3073}, {{{86,5}},193}, {{{192,5}},24577}
  };

// copy as much as possible from the sliding window to the output area
int inflate_flush(inflate_blocks_statef *s,z_streamp z,int r)
{
  uInt n;
  Byte *p;
  Byte *q;

  // local copies of source and destination pointers 
  p = z->next_out;
  q = s->read;

  // compute number of bytes to copy as far as end of window 
  n = (uInt)((q <= s->write ? s->write : s->end) - q);
  if (n > z->avail_out) n = z->avail_out;
  if (n && r == Z_BUF_ERROR) r = Z_OK;

  // update counters
  z->avail_out -= n;
  z->total_out += n;

  // update check information 
  if (s->checkfn != Z_NULL)
    z->adler = s->check = (*s->checkfn)(s->check, q, n);

  // copy as far as end of window 
  if (n!=0)          // check for n!=0 to avoid waking up CodeGuard
  { memcpy(p, q, n);
    p += n;
    q += n;
  }

  // see if more to copy at beginning of window
  if (q == s->end)
  {
    // wrap pointers 
    q = s->window;
    if (s->write == s->end)
      s->write = s->window;

    // compute bytes to copy 
    n = (uInt)(s->write - q);
    if (n > z->avail_out) n = z->avail_out;
    if (n && r == Z_BUF_ERROR) r = Z_OK;

    // update counters 
    z->avail_out -= n;
    z->total_out += n;

    // update check information 
    if (s->checkfn != Z_NULL)
      z->adler = s->check = (*s->checkfn)(s->check, q, n);

    // copy
    if (n!=0) {memcpy(p,q,n); p+=n; q+=n;}
  }

  // update pointers
  z->next_out = p;
  s->read = q;

  // done
  return r;
}

// simplify the use of the inflate_huft type with some defines
#define exop word.what.Exop
#define bits word.what.Bits

typedef enum {        // waiting for "i:"=input, "o:"=output, "x:"=nothing 
      START,    // x: set up for LEN 
      LEN,      // i: get length/literal/eob next 
      LENEXT,   // i: getting length extra (have base) 
      DIST,     // i: get distance next 
      DISTEXT,  // i: getting distance extra 
      COPY,     // o: copying bytes in window, waiting for space
      LIT,      // o: got literal, waiting for output space 
      WASH,     // o: got eob, possibly still output waiting 
      END,      // x: got eob and all data flushed 
      BADCODE}  // x: got error 
inflate_codes_mode;

// inflate codes private state
struct inflate_codes_state {

  // mode 
  inflate_codes_mode mode;      // current inflate_codes mode 

  // mode dependent information 
  uInt len;
  union {
    struct {
      const inflate_huft *tree;       // pointer into tree 
      uInt need;                // bits needed 
    } code;             // if LEN or DIST, where in tree 
    uInt lit;           // if LIT, literal 
    struct {
      uInt get;                 // bits to get for extra 
      uInt dist;                // distance back to copy from 
    } copy;             // if EXT or COPY, where and how much 
  } sub;                // submode

  // mode independent information 
  Byte lbits;           // ltree bits decoded per branch 
  Byte dbits;           // dtree bits decoder per branch 
  const inflate_huft *ltree;          // literal/length/eob tree
  const inflate_huft *dtree;          // distance tree

};

inflate_codes_statef *inflate_codes_new(
uInt bl, uInt bd,
const inflate_huft *tl,
const inflate_huft *td, // need separate declaration for Borland C++
z_streamp z)
{
  inflate_codes_statef *c;

  if ((c = (inflate_codes_statef *)
       ZALLOC(z,1,sizeof(struct inflate_codes_state))) != Z_NULL)
  {
    c->mode = START;
    c->lbits = (Byte)bl;
    c->dbits = (Byte)bd;
    c->ltree = tl;
    c->dtree = td;
    LuTracev((stderr, "inflate:       codes new\n"));
  }
  return c;
}

int inflate_codes(inflate_blocks_statef *s, z_streamp z, int r)
{
  uInt j;               // temporary storage
  const inflate_huft *t;      // temporary pointer
  uInt e;               // extra bits or operation
  uLong b;              // bit buffer
  uInt k;               // bits in bit buffer
  Byte *p;             // input data pointer
  uInt n;               // bytes available there
  Byte *q;             // output window write pointer
  uInt m;               // bytes to end of window or read pointer
  Byte *f;             // pointer to copy strings from
  inflate_codes_statef *c = s->sub.decode.codes;  // codes state

  // copy input/output information to locals (UPDATE macro restores)
  LOAD

  // process input and output based on current state
  for(;;) switch (c->mode)
  {             // waiting for "i:"=input, "o:"=output, "x:"=nothing
    case START:         // x: set up for LEN
#ifndef SLOW
      if (m >= 258 && n >= 10)
      {
        UPDATE
        r = inflate_fast(c->lbits, c->dbits, c->ltree, c->dtree, s, z);
        LOAD
        if (r != Z_OK)
        {
          c->mode = r == Z_STREAM_END ? WASH : BADCODE;
          break;
        }
      }
#endif // !SLOW
      c->sub.code.need = c->lbits;
      c->sub.code.tree = c->ltree;
      c->mode = LEN;
    case LEN:           // i: get length/literal/eob next
      j = c->sub.code.need;
      NEEDBITS(j)
      t = c->sub.code.tree + ((uInt)b & inflate_mask[j]);
      DUMPBITS(t->bits)
      e = (uInt)(t->exop);
      if (e == 0)               // literal 
      {
        c->sub.lit = t->base;
        LuTracevv((stderr, t->base >= 0x20 && t->base < 0x7f ?
                 "inflate:         literal '%c'\n" :
                 "inflate:         literal 0x%02x\n", t->base));
        c->mode = LIT;
        break;
      }
      if (e & 16)               // length 
      {
        c->sub.copy.get = e & 15;
        c->len = t->base;
        c->mode = LENEXT;
        break;
      }
      if ((e & 64) == 0)        // next table 
      {
        c->sub.code.need = e;
        c->sub.code.tree = t + t->base;
        break;
      }
      if (e & 32)               // end of block 
      {
        LuTracevv((stderr, "inflate:         end of block\n"));
        c->mode = WASH;
        break;
      }
      c->mode = BADCODE;        // invalid code 
      z->msg = (char*)"invalid literal/length code";
      r = Z_DATA_ERROR;
      LEAVE
    case LENEXT:        // i: getting length extra (have base) 
      j = c->sub.copy.get;
      NEEDBITS(j)
      c->len += (uInt)b & inflate_mask[j];
      DUMPBITS(j)
      c->sub.code.need = c->dbits;
      c->sub.code.tree = c->dtree;
      LuTracevv((stderr, "inflate:         length %u\n", c->len));
      c->mode = DIST;
    case DIST:          // i: get distance next 
      j = c->sub.code.need;
      NEEDBITS(j)
      t = c->sub.code.tree + ((uInt)b & inflate_mask[j]);
      DUMPBITS(t->bits)
      e = (uInt)(t->exop);
      if (e & 16)               // distance 
      {
        c->sub.copy.get = e & 15;
        c->sub.copy.dist = t->base;
        c->mode = DISTEXT;
        break;
      }
      if ((e & 64) == 0)        // next table 
      {
        c->sub.code.need = e;
        c->sub.code.tree = t + t->base;
        break;
      }
      c->mode = BADCODE;        // invalid code 
      z->msg = (char*)"invalid distance code";
      r = Z_DATA_ERROR;
      LEAVE
    case DISTEXT:       // i: getting distance extra 
      j = c->sub.copy.get;
      NEEDBITS(j)
      c->sub.copy.dist += (uInt)b & inflate_mask[j];
      DUMPBITS(j)
      LuTracevv((stderr, "inflate:         distance %u\n", c->sub.copy.dist));
      c->mode = COPY;
    case COPY:          // o: copying bytes in window, waiting for space 
      f = q - c->sub.copy.dist;
      while (f < s->window)             // modulo window size-"while" instead
        f += s->end - s->window;        // of "if" handles invalid distances 
      while (c->len)
      {
        NEEDOUT
        OUTBYTE(*f++)
        if (f == s->end)
          f = s->window;
        c->len--;
      }
      c->mode = START;
      break;
    case LIT:           // o: got literal, waiting for output space 
      NEEDOUT
      OUTBYTE(c->sub.lit)
      c->mode = START;
      break;
    case WASH:          // o: got eob, possibly more output 
      if (k > 7)        // return unused byte, if any 
      {
        //Assert(k < 16, "inflate_codes grabbed too many bytes")
        k -= 8;
        n++;
        p--;            // can always return one 
      }
      FLUSH
      if (s->read != s->write)
        LEAVE
      c->mode = END;
    case END:
      r = Z_STREAM_END;
      LEAVE
    case BADCODE:       // x: got error
      r = Z_DATA_ERROR;
      LEAVE
    default:
      r = Z_STREAM_ERROR;
      LEAVE
  }
}

void inflate_codes_free(inflate_codes_statef *c,z_streamp z)
{ ZFREE(z, c);
  LuTracev((stderr, "inflate:       codes free\n"));
}

// infblock.c -- interpret and process block types to last block
// Copyright (C) 1995-1998 Mark Adler
// For conditions of distribution and use, see copyright notice in zlib.h

//struct inflate_codes_state {int dummy;}; // for buggy compilers 

// Table for deflate from PKZIP's appnote.txt.
const uInt border[] = { // Order of the bit length code lengths
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

//
// Notes beyond the 1.93a appnote.txt:
//
// 1. Distance pointers never point before the beginning of the output stream.
// 2. Distance pointers can point back across blocks, up to 32k away.
// 3. There is an implied maximum of 7 bits for the bit length table and
//    15 bits for the actual data.
// 4. If only one code exists, then it is encoded using one bit.  (Zero
//    would be more efficient, but perhaps a little confusing.)  If two
//    codes exist, they are coded using one bit each (0 and 1).
// 5. There is no way of sending zero distance codes--a dummy must be
//    sent if there are none.  (History: a pre 2.0 version of PKZIP would
//    store blocks with no distance codes, but this was discovered to be
//    too harsh a criterion.)  Valid only for 1.93a.  2.04c does allow
//    zero distance codes, which is sent as one code of zero bits in
//    length.
// 6. There are up to 286 literal/length codes.  Code 256 represents the
//    end-of-block.  Note however that the static length tree defines
//    288 codes just to fill out the Huffman codes.  Codes 286 and 287
//    cannot be used though, since there is no length base or extra bits
//    defined for them.  Similarily, there are up to 30 distance codes.
//    However, static trees define 32 codes (all 5 bits) to fill out the
//    Huffman codes, but the last two had better not show up in the data.
// 7. Unzip can check dynamic Huffman blocks for complete code sets.
//    The exception is that a single code would not be complete (see #4).
// 8. The five bits following the block type is really the number of
//    literal codes sent minus 257.
// 9. Length codes 8,16,16 are interpreted as 13 length codes of 8 bits
//    (1+6+6).  Therefore, to output three times the length, you output
//    three codes (1+1+1), whereas to output four times the same length,
//    you only need two codes (1+3).  Hmm.
//10. In the tree reconstruction algorithm, Code = Code + Increment
//    only if BitLength(i) is not zero.  (Pretty obvious.)
//11. Correction: 4 Bits: # of Bit Length codes - 4     (4 - 19)
//12. Note: length code 284 can represent 227-258, but length code 285
//    really is 258.  The last length deserves its own, short code
//    since it gets used a lot in very redundant files.  The length
//    258 is special since 258 - 3 (the min match length) is 255.
//13. The literal/length and distance code bit lengths are read as a
//    single stream of lengths.  It is possible (and advantageous) for
//    a repeat code (16, 17, or 18) to go across the boundary between
//    the two sets of lengths.

void inflate_blocks_reset(inflate_blocks_statef *s, z_streamp z, uLong *c)
{
  if (c != Z_NULL)
    *c = s->check;
  if (s->mode == IBM_BTREE || s->mode == IBM_DTREE)
    ZFREE(z, s->sub.trees.blens);
  if (s->mode == IBM_CODES)
    inflate_codes_free(s->sub.decode.codes, z);
  s->mode = IBM_TYPE;
  s->bitk = 0;
  s->bitb = 0;
  s->read = s->write = s->window;
  if (s->checkfn != Z_NULL)
    z->adler = s->check = (*s->checkfn)(0L, (const Byte *)Z_NULL, 0);
  LuTracev((stderr, "inflate:   blocks reset\n"));
}

inflate_blocks_statef *inflate_blocks_new(z_streamp z, check_func c, uInt w)
{
  inflate_blocks_statef *s;

  if ((s = (inflate_blocks_statef *)ZALLOC
       (z,1,sizeof(struct inflate_blocks_state))) == Z_NULL)
    return s;
  if ((s->hufts =
       (inflate_huft *)ZALLOC(z, sizeof(inflate_huft), MANY)) == Z_NULL)
  {
    ZFREE(z, s);
    return Z_NULL;
  }
  if ((s->window = (Byte *)ZALLOC(z, 1, w)) == Z_NULL)
  {
    ZFREE(z, s->hufts);
    ZFREE(z, s);
    return Z_NULL;
  }
  s->end = s->window + w;
  s->checkfn = c;
  s->mode = IBM_TYPE;
  LuTracev((stderr, "inflate:   blocks allocated\n"));
  inflate_blocks_reset(s, z, Z_NULL);
  return s;
}

int inflate_blocks(inflate_blocks_statef *s, z_streamp z, int r)
{
  uInt t;               // temporary storage
  uLong b;              // bit buffer
  uInt k;               // bits in bit buffer
  Byte *p;             // input data pointer
  uInt n;               // bytes available there
  Byte *q;             // output window write pointer
  uInt m;               // bytes to end of window or read pointer 

  // copy input/output information to locals (UPDATE macro restores) 
  LOAD

  // process input based on current state 
  for(;;) switch (s->mode)
  {
    case IBM_TYPE:
      NEEDBITS(3)
      t = (uInt)b & 7;
      s->last = t & 1;
      switch (t >> 1)
      {
        case 0:                         // stored 
          LuTracev((stderr, "inflate:     stored block%s\n",
                 s->last ? " (last)" : ""));
          DUMPBITS(3)
          t = k & 7;                    // go to byte boundary 
          DUMPBITS(t)
          s->mode = IBM_LENS;               // get length of stored block
          break;
        case 1:                         // fixed 
          LuTracev((stderr, "inflate:     fixed codes block%s\n",
                 s->last ? " (last)" : ""));
          {
            uInt bl, bd;
            const inflate_huft *tl, *td;

            inflate_trees_fixed(&bl, &bd, &tl, &td, z);
            s->sub.decode.codes = inflate_codes_new(bl, bd, tl, td, z);
            if (s->sub.decode.codes == Z_NULL)
            {
              r = Z_MEM_ERROR;
              LEAVE
            }
          }
          DUMPBITS(3)
          s->mode = IBM_CODES;
          break;
        case 2:                         // dynamic 
          LuTracev((stderr, "inflate:     dynamic codes block%s\n",
                 s->last ? " (last)" : ""));
          DUMPBITS(3)
          s->mode = IBM_TABLE;
          break;
        case 3:                         // illegal
          DUMPBITS(3)
          s->mode = IBM_BAD;
          z->msg = (char*)"invalid block type";
          r = Z_DATA_ERROR;
          LEAVE
      }
      break;
    case IBM_LENS:
      NEEDBITS(32)
      if ((((~b) >> 16) & 0xffff) != (b & 0xffff))
      {
        s->mode = IBM_BAD;
        z->msg = (char*)"invalid stored block lengths";
        r = Z_DATA_ERROR;
        LEAVE
      }
      s->sub.left = (uInt)b & 0xffff;
      b = k = 0;                      // dump bits 
      LuTracev((stderr, "inflate:       stored length %u\n", s->sub.left));
      s->mode = s->sub.left ? IBM_STORED : (s->last ? IBM_DRY : IBM_TYPE);
      break;
    case IBM_STORED:
      if (n == 0)
        LEAVE
      NEEDOUT
      t = s->sub.left;
      if (t > n) t = n;
      if (t > m) t = m;
      memcpy(q, p, t);
      p += t;  n -= t;
      q += t;  m -= t;
      if ((s->sub.left -= t) != 0)
        break;
      LuTracev((stderr, "inflate:       stored end, %lu total out\n",
              z->total_out + (q >= s->read ? q - s->read :
              (s->end - s->read) + (q - s->window))));
      s->mode = s->last ? IBM_DRY : IBM_TYPE;
      break;
    case IBM_TABLE:
      NEEDBITS(14)
      s->sub.trees.table = t = (uInt)b & 0x3fff;
      // remove this section to workaround bug in pkzip
      if ((t & 0x1f) > 29 || ((t >> 5) & 0x1f) > 29)
      {
        s->mode = IBM_BAD;
        z->msg = (char*)"too many length or distance symbols";
        r = Z_DATA_ERROR;
        LEAVE
      }
      // end remove
      t = 258 + (t & 0x1f) + ((t >> 5) & 0x1f);
      if ((s->sub.trees.blens = (uInt*)ZALLOC(z, t, sizeof(uInt))) == Z_NULL)
      {
        r = Z_MEM_ERROR;
        LEAVE
      }
      DUMPBITS(14)
      s->sub.trees.index = 0;
      LuTracev((stderr, "inflate:       table sizes ok\n"));
      s->mode = IBM_BTREE;
    case IBM_BTREE:
      while (s->sub.trees.index < 4 + (s->sub.trees.table >> 10))
      {
        NEEDBITS(3)
        s->sub.trees.blens[border[s->sub.trees.index++]] = (uInt)b & 7;
        DUMPBITS(3)
      }
      while (s->sub.trees.index < 19)
        s->sub.trees.blens[border[s->sub.trees.index++]] = 0;
      s->sub.trees.bb = 7;
      t = inflate_trees_bits(s->sub.trees.blens, &s->sub.trees.bb,
                             &s->sub.trees.tb, s->hufts, z);
      if (t != Z_OK)
      {
        r = t;
        if (r == Z_DATA_ERROR)
        {
          ZFREE(z, s->sub.trees.blens);
          s->mode = IBM_BAD;
        }
        LEAVE
      }
      s->sub.trees.index = 0;
      LuTracev((stderr, "inflate:       bits tree ok\n"));
      s->mode = IBM_DTREE;
    case IBM_DTREE:
      while (t = s->sub.trees.table,
             s->sub.trees.index < 258 + (t & 0x1f) + ((t >> 5) & 0x1f))
      {
        inflate_huft *h;
        uInt i, j, c;

        t = s->sub.trees.bb;
        NEEDBITS(t)
        h = s->sub.trees.tb + ((uInt)b & inflate_mask[t]);
        t = h->bits;
        c = h->base;
        if (c < 16)
        {
          DUMPBITS(t)
          s->sub.trees.blens[s->sub.trees.index++] = c;
        }
        else // c == 16..18 
        {
          i = c == 18 ? 7 : c - 14;
          j = c == 18 ? 11 : 3;
          NEEDBITS(t + i)
          DUMPBITS(t)
          j += (uInt)b & inflate_mask[i];
          DUMPBITS(i)
          i = s->sub.trees.index;
          t = s->sub.trees.table;
          if (i + j > 258 + (t & 0x1f) + ((t >> 5) & 0x1f) ||
              (c == 16 && i < 1))
          {
            ZFREE(z, s->sub.trees.blens);
            s->mode = IBM_BAD;
            z->msg = (char*)"invalid bit length repeat";
            r = Z_DATA_ERROR;
            LEAVE
          }
          c = c == 16 ? s->sub.trees.blens[i - 1] : 0;
          do {
            s->sub.trees.blens[i++] = c;
          } while (--j);
          s->sub.trees.index = i;
        }
      }
      s->sub.trees.tb = Z_NULL;
      {
        uInt bl, bd;
        inflate_huft *tl, *td;
        inflate_codes_statef *c;

        bl = 9;         // must be <= 9 for lookahead assumptions 
        bd = 6;         // must be <= 9 for lookahead assumptions
        t = s->sub.trees.table;
        t = inflate_trees_dynamic(257 + (t & 0x1f), 1 + ((t >> 5) & 0x1f),
                                  s->sub.trees.blens, &bl, &bd, &tl, &td,
                                  s->hufts, z);
        if (t != Z_OK)
        {
          if (t == (uInt)Z_DATA_ERROR)
          {
            ZFREE(z, s->sub.trees.blens);
            s->mode = IBM_BAD;
          }
          r = t;
          LEAVE
        }
        LuTracev((stderr, "inflate:       trees ok\n"));
        if ((c = inflate_codes_new(bl, bd, tl, td, z)) == Z_NULL)
        {
          r = Z_MEM_ERROR;
          LEAVE
        }
        s->sub.decode.codes = c;
      }
      ZFREE(z, s->sub.trees.blens);
      s->mode = IBM_CODES;
    case IBM_CODES:
      UPDATE
      if ((r = inflate_codes(s, z, r)) != Z_STREAM_END)
        return inflate_flush(s, z, r);
      r = Z_OK;
      inflate_codes_free(s->sub.decode.codes, z);
      LOAD
      LuTracev((stderr, "inflate:       codes end, %lu total out\n",
              z->total_out + (q >= s->read ? q - s->read :
              (s->end - s->read) + (q - s->window))));
      if (!s->last)
      {
        s->mode = IBM_TYPE;
        break;
      }
      s->mode = IBM_DRY;
    case IBM_DRY:
      FLUSH
      if (s->read != s->write)
        LEAVE
      s->mode = IBM_DONE;
    case IBM_DONE:
      r = Z_STREAM_END;
      LEAVE
    case IBM_BAD:
      r = Z_DATA_ERROR;
      LEAVE
    default:
      r = Z_STREAM_ERROR;
      LEAVE
  }
}

int inflate_blocks_free(inflate_blocks_statef *s, z_streamp z)
{
  inflate_blocks_reset(s, z, Z_NULL);
  ZFREE(z, s->window);
  ZFREE(z, s->hufts);
  ZFREE(z, s);
  LuTracev((stderr, "inflate:   blocks freed\n"));
  return Z_OK;
}

// inftrees.c -- generate Huffman trees for efficient decoding
// Copyright (C) 1995-1998 Mark Adler
// For conditions of distribution and use, see copyright notice in zlib.h
//

// If you use the zlib library in a product, an acknowledgment is welcome
// in the documentation of your product. If for some reason you cannot
// include such an acknowledgment, I would appreciate that you keep this
// copyright string in the executable of your product.

int huft_build (
    uInt *,            // code lengths in bits
    uInt,               // number of codes
    uInt,               // number of "simple" codes
    const uInt *,      // list of base values for non-simple codes
    const uInt *,      // list of extra bits for non-simple codes
    inflate_huft **,// result: starting table
    uInt *,            // maximum lookup bits (returns actual) 
    inflate_huft *,     // space for trees 
    uInt *,             // hufts used in space 
    uInt * );         // space for values 

// Tables for deflate from PKZIP's appnote.txt. 
const uInt cplens[31] = { // Copy lengths for literal codes 257..285
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
        35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};
        // see note #13 above about 258
const uInt cplext[31] = { // Extra bits for literal codes 257..285
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
        3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 112, 112}; // 112==invalid
const uInt cpdist[30] = { // Copy offsets for distance codes 0..29
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
        257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
        8193, 12289, 16385, 24577};
const uInt cpdext[30] = { // Extra bits for distance codes 
        0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
        7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
        12, 12, 13, 13};

//
//   Huffman code decoding is performed using a multi-level table lookup.
//   The fastest way to decode is to simply build a lookup table whose
//   size is determined by the longest code.  However, the time it takes
//   to build this table can also be a factor if the data being decoded
//   is not very long.  The most common codes are necessarily the
//   shortest codes, so those codes dominate the decoding time, and hence
//   the speed.  The idea is you can have a shorter table that decodes the
//   shorter, more probable codes, and then point to subsidiary tables for
//   the longer codes.  The time it costs to decode the longer codes is
//   then traded against the time it takes to make longer tables.
//
//   This results of this trade are in the variables lbits and dbits
//   below.  lbits is the number of bits the first level table for literal/
//   length codes can decode in one step, and dbits is the same thing for
//   the distance codes.  Subsequent tables are also less than or equal to
//   those sizes.  These values may be adjusted either when all of the
//   codes are shorter than that, in which case the longest code length in
//   bits is used, or when the shortest code is *longer* than the requested
//   table size, in which case the length of the shortest code in bits is
//   used.
//
//   There are two different values for the two tables, since they code a
//   different number of possibilities each.  The literal/length table
//   codes 286 possible values, or in a flat code, a little over eight
//   bits.  The distance table codes 30 possible values, or a little less
//   than five bits, flat.  The optimum values for speed end up being
//   about one bit more than those, so lbits is 8+1 and dbits is 5+1.
//   The optimum values may differ though from machine to machine, and
//   possibly even between compilers.  Your mileage may vary.
//

// If BMAX needs to be larger than 16, then h and x[] should be uLong. 
#define BMAX 15         // maximum bit length of any code

int huft_build(
uInt *b,               // code lengths in bits (all assumed <= BMAX)
uInt n,                 // number of codes (assumed <= 288)
uInt s,                 // number of simple-valued codes (0..s-1)
const uInt *d,         // list of base values for non-simple codes
const uInt *e,         // list of extra bits for non-simple codes
inflate_huft * *t,  // result: starting table
uInt *m,               // maximum lookup bits, returns actual
inflate_huft *hp,       // space for trees
uInt *hn,               // hufts used in space
uInt *v)               // working area: values in order of bit length
// Given a list of code lengths and a maximum table size, make a set of
// tables to decode that set of codes.  Return Z_OK on success, Z_BUF_ERROR
// if the given code set is incomplete (the tables are still built in this
// case), or Z_DATA_ERROR if the input is invalid.
{

  uInt a;                         // counter for codes of length k
  uInt c[BMAX+1];                 // bit length count table
  uInt f;                         // i repeats in table every f entries 
  int g;                          // maximum code length 
  int h;                          // table level 
  register uInt i;                // counter, current code 
  register uInt j;                // counter
  register int k;                 // number of bits in current code 
  int l;                          // bits per table (returned in m) 
  uInt mask;                      // (1 << w) - 1, to avoid cc -O bug on HP 
  register uInt *p;               // pointer into c[], b[], or v[]
  inflate_huft *q;                // points to current table 
  struct inflate_huft_s r;        // table entry for structure assignment 
  inflate_huft *u[BMAX];          // table stack 
  register int w;                 // bits before this table == (l * h) 
  uInt x[BMAX+1];                 // bit offsets, then code stack 
  uInt *xp;                       // pointer into x 
  int y;                          // number of dummy codes added 
  uInt z;                         // number of entries in current table
  r.word.pad = 0;
  r.base = 0;

  // Generate counts for each bit length 
  p = c;
#define C0 *p++ = 0;
#define C2 C0 C0 C0 C0
#define C4 C2 C2 C2 C2
  C4; p;                          // clear c[]--assume BMAX+1 is 16
  p = b;  i = n;
  do {
    c[*p++]++;                  // assume all entries <= BMAX 
  } while (--i);
  if (c[0] == n)                // null input--all zero length codes 
  {
    *t = (inflate_huft *)Z_NULL;
    *m = 0;
    return Z_OK;
  }


  // Find minimum and maximum length, bound *m by those 
  l = *m;
  for (j = 1; j <= BMAX; j++)
    if (c[j])
      break;
  k = j;                        // minimum code length 
  if ((uInt)l < j)
    l = j;
  for (i = BMAX; i; i--)
    if (c[i])
      break;
  g = i;                        // maximum code length 
  if ((uInt)l > i)
    l = i;
  *m = l;


  // Adjust last length count to fill out codes, if needed 
  for (y = 1 << j; j < i; j++, y <<= 1)
    if ((y -= c[j]) < 0)
      return Z_DATA_ERROR;
  if ((y -= c[i]) < 0)
    return Z_DATA_ERROR;
  c[i] += y;


  // Generate starting offsets into the value table for each length 
  x[1] = j = 0;
  p = c + 1;  xp = x + 2;
  while (--i) {                 // note that i == g from above 
    *xp++ = (j += *p++);
  }


  // Make a table of values in order of bit lengths 
  p = b;  i = 0;
  do {
    if ((j = *p++) != 0)
      v[x[j]++] = i;
  } while (++i < n);
  n = x[g];                     // set n to length of v 


  // Generate the Huffman codes and for each, make the table entries 
  x[0] = i = 0;                 // first Huffman code is zero 
  p = v;                        // grab values in bit order 
  h = -1;                       // no tables yet--level -1 
  w = -l;                       // bits decoded == (l * h) 
  u[0] = (inflate_huft *)Z_NULL;        // just to keep compilers happy 
  q = (inflate_huft *)Z_NULL;   // ditto 
  z = 0;                        // ditto 

  // go through the bit lengths (k already is bits in shortest code) 
  for (; k <= g; k++)
  {
    a = c[k];
    while (a--)
    {
      // here i is the Huffman code of length k bits for value *p 
      // make tables up to required level 
      while (k > w + l)
      {
        h++;
        w += l;                 // previous table always l bits 

        // compute minimum size table less than or equal to l bits
        z = g - w;
        z = z > (uInt)l ? l : z;        // table size upper limit 
        if ((f = 1 << (j = k - w)) > a + 1)     // try a k-w bit table 
        {                       // too few codes for k-w bit table 
          f -= a + 1;           // deduct codes from patterns left 
          xp = c + k;
          if (j < z)
            while (++j < z)     // try smaller tables up to z bits 
            {
              if ((f <<= 1) <= *++xp)
                break;          // enough codes to use up j bits 
              f -= *xp;         // else deduct codes from patterns
            }
        }
        z = 1 << j;             // table entries for j-bit table 

        // allocate new table 
        if (*hn + z > MANY)     // (note: doesn't matter for fixed) 
          return Z_DATA_ERROR;  // overflow of MANY 
        u[h] = q = hp + *hn;
        *hn += z;

        // connect to last table, if there is one 
        if (h)
        {
          x[h] = i;             // save pattern for backing up
          r.bits = (Byte)l;     // bits to dump before this table 
          r.exop = (Byte)j;     // bits in this table 
          j = i >> (w - l);
          r.base = (uInt)(q - u[h-1] - j);   // offset to this table 
          u[h-1][j] = r;        // connect to last table 
        }
        else
          *t = q;               // first table is returned result 
      }

      // set up table entry in r 
      r.bits = (Byte)(k - w);
      if (p >= v + n)
        r.exop = 128 + 64;      // out of values--invalid code 
      else if (*p < s)
      {
        r.exop = (Byte)(*p < 256 ? 0 : 32 + 64);     // 256 is end-of-block 
        r.base = *p++;          // simple code is just the value 
      }
      else
      {
        r.exop = (Byte)(e[*p - s] + 16 + 64);// non-simple--look up in lists 
        r.base = d[*p++ - s];
      }

      // fill code-like entries with r
      f = 1 << (k - w);
      for (j = i >> w; j < z; j += f)
        q[j] = r;

      // backwards increment the k-bit code i 
      for (j = 1 << (k - 1); i & j; j >>= 1)
        i ^= j;
      i ^= j;

      // backup over finished tables 
      mask = (1 << w) - 1;      // needed on HP, cc -O bug 
      while ((i & mask) != x[h])
      {
        h--;                    // don't need to update q
        w -= l;
        mask = (1 << w) - 1;
      }
    }
  }

  // Return Z_BUF_ERROR if we were given an incomplete table 
  return y != 0 && g != 1 ? Z_BUF_ERROR : Z_OK;
}

int inflate_trees_bits(
uInt *c,               // 19 code lengths
uInt *bb,              // bits tree desired/actual depth
inflate_huft * *tb, // bits tree result
inflate_huft *hp,       // space for trees
z_streamp z)            // for messages
{
  int r;
  uInt hn = 0;          // hufts used in space 
  uInt *v;             // work area for huft_build 

  if ((v = (uInt*)ZALLOC(z, 19, sizeof(uInt))) == Z_NULL)
    return Z_MEM_ERROR;
  r = huft_build(c, 19, 19, (uInt*)Z_NULL, (uInt*)Z_NULL,
                 tb, bb, hp, &hn, v);
  if (r == Z_DATA_ERROR)
    z->msg = (char*)"oversubscribed dynamic bit lengths tree";
  else if (r == Z_BUF_ERROR || *bb == 0)
  {
    z->msg = (char*)"incomplete dynamic bit lengths tree";
    r = Z_DATA_ERROR;
  }
  ZFREE(z, v);
  return r;
}

int inflate_trees_dynamic(
uInt nl,                // number of literal/length codes
uInt nd,                // number of distance codes
uInt *c,               // that many (total) code lengths
uInt *bl,              // literal desired/actual bit depth
uInt *bd,              // distance desired/actual bit depth
inflate_huft * *tl, // literal/length tree result
inflate_huft * *td, // distance tree result
inflate_huft *hp,       // space for trees
z_streamp z)            // for messages
{
  int r;
  uInt hn = 0;          // hufts used in space 
  uInt *v;             // work area for huft_build 

  // allocate work area 
  if ((v = (uInt*)ZALLOC(z, 288, sizeof(uInt))) == Z_NULL)
    return Z_MEM_ERROR;

  // build literal/length tree 
  r = huft_build(c, nl, 257, cplens, cplext, tl, bl, hp, &hn, v);
  if (r != Z_OK || *bl == 0)
  {
    if (r == Z_DATA_ERROR)
      z->msg = (char*)"oversubscribed literal/length tree";
    else if (r != Z_MEM_ERROR)
    {
      z->msg = (char*)"incomplete literal/length tree";
      r = Z_DATA_ERROR;
    }
    ZFREE(z, v);
    return r;
  }

  // build distance tree 
  r = huft_build(c + nl, nd, 0, cpdist, cpdext, td, bd, hp, &hn, v);
  if (r != Z_OK || (*bd == 0 && nl > 257))
  {
    if (r == Z_DATA_ERROR)
      z->msg = (char*)"oversubscribed distance tree";
    else if (r == Z_BUF_ERROR) {
      z->msg = (char*)"incomplete distance tree";
      r = Z_DATA_ERROR;
    }
    else if (r != Z_MEM_ERROR)
    {
      z->msg = (char*)"empty distance tree with lengths";
      r = Z_DATA_ERROR;
    }
    ZFREE(z, v);
    return r;
  }

  // done 
  ZFREE(z, v);
  return Z_OK;
}

int inflate_trees_fixed(
uInt *bl,               // literal desired/actual bit depth
uInt *bd,               // distance desired/actual bit depth
const inflate_huft * * tl,     // literal/length tree result
const inflate_huft * *td,     // distance tree result
z_streamp )             // for memory allocation
{
  *bl = fixed_bl;
  *bd = fixed_bd;
  *tl = fixed_tl;
  *td = fixed_td;
  return Z_OK;
}

// inffast.c -- process literals and length/distance pairs fast
// Copyright (C) 1995-1998 Mark Adler
// For conditions of distribution and use, see copyright notice in zlib.h
//

//struct inflate_codes_state {int dummy;}; // for buggy compilers 

// macros for bit input with no checking and for returning unused bytes 
#define GRABBITS(j) {while(k<(j)){b|=((uLong)NEXTBYTE)<<k;k+=8;}}
#define UNGRAB {c=z->avail_in-n;c=(k>>3)<c?k>>3:c;n+=c;p-=c;k-=c<<3;}

// Called with number of bytes left to write in window at least 258
// (the maximum string length) and number of input bytes available
// at least ten.  The ten bytes are six bytes for the longest length/
// distance pair plus four bytes for overloading the bit buffer. 

int inflate_fast(
uInt bl, uInt bd,
const inflate_huft *tl,
const inflate_huft *td, // need separate declaration for Borland C++
inflate_blocks_statef *s,
z_streamp z)
{
  const inflate_huft *t;      // temporary pointer 
  uInt e;               // extra bits or operation 
  uLong b;              // bit buffer 
  uInt k;               // bits in bit buffer 
  Byte *p;             // input data pointer 
  uInt n;               // bytes available there 
  Byte *q;             // output window write pointer 
  uInt m;               // bytes to end of window or read pointer 
  uInt ml;              // mask for literal/length tree
  uInt md;              // mask for distance tree 
  uInt c;               // bytes to copy 
  uInt d;               // distance back to copy from 
  Byte *r;             // copy source pointer 

  // load input, output, bit values 
  LOAD

  // initialize masks 
  ml = inflate_mask[bl];
  md = inflate_mask[bd];

  // do until not enough input or output space for fast loop 
  do {                          // assume called with m >= 258 && n >= 10 
    // get literal/length code 
    GRABBITS(20)                // max bits for literal/length code 
    if ((e = (t = tl + ((uInt)b & ml))->exop) == 0)
    {
      DUMPBITS(t->bits)
      LuTracevv((stderr, t->base >= 0x20 && t->base < 0x7f ?
                "inflate:         * literal '%c'\n" :
                "inflate:         * literal 0x%02x\n", t->base));
      *q++ = (Byte)t->base;
      m--;
      continue;
    }
    for (;;) {
      DUMPBITS(t->bits)
      if (e & 16)
      {
        // get extra bits for length 
        e &= 15;
        c = t->base + ((uInt)b & inflate_mask[e]);
        DUMPBITS(e)
        LuTracevv((stderr, "inflate:         * length %u\n", c));

        // decode distance base of block to copy 
        GRABBITS(15);           // max bits for distance code 
        e = (t = td + ((uInt)b & md))->exop;
        for (;;) {
          DUMPBITS(t->bits)
          if (e & 16)
          {
            // get extra bits to add to distance base 
            e &= 15;
            GRABBITS(e)         // get extra bits (up to 13) 
            d = t->base + ((uInt)b & inflate_mask[e]);
            DUMPBITS(e)
            LuTracevv((stderr, "inflate:         * distance %u\n", d));

            // do the copy
            m -= c;
            r = q - d;
            if (r < s->window)                  // wrap if needed
            {
              do {
                r += s->end - s->window;        // force pointer in window
              } while (r < s->window);          // covers invalid distances
              e = (uInt) (s->end - r);
              if (c > e)
              {
                c -= e;                         // wrapped copy
                do {
                    *q++ = *r++;
                } while (--e);
                r = s->window;
                do {
                    *q++ = *r++;
                } while (--c);
              }
              else                              // normal copy
              {
                *q++ = *r++;  c--;
                *q++ = *r++;  c--;
                do {
                    *q++ = *r++;
                } while (--c);
              }
            }
            else                                /* normal copy */
            {
              *q++ = *r++;  c--;
              *q++ = *r++;  c--;
              do {
                *q++ = *r++;
              } while (--c);
            }
            break;
          }
          else if ((e & 64) == 0)
          {
            t += t->base;
            e = (t += ((uInt)b & inflate_mask[e]))->exop;
          }
          else
          {
            z->msg = (char*)"invalid distance code";
            UNGRAB
            UPDATE
            return Z_DATA_ERROR;
          }
        };
        break;
      }
      if ((e & 64) == 0)
      {
        t += t->base;
        if ((e = (t += ((uInt)b & inflate_mask[e]))->exop) == 0)
        {
          DUMPBITS(t->bits)
          LuTracevv((stderr, t->base >= 0x20 && t->base < 0x7f ?
                    "inflate:         * literal '%c'\n" :
                    "inflate:         * literal 0x%02x\n", t->base));
          *q++ = (Byte)t->base;
          m--;
          break;
        }
      }
      else if (e & 32)
      {
        LuTracevv((stderr, "inflate:         * end of block\n"));
        UNGRAB
        UPDATE
        return Z_STREAM_END;
      }
      else
      {
        z->msg = (char*)"invalid literal/length code";
        UNGRAB
        UPDATE
        return Z_DATA_ERROR;
      }
    };
  } while (m >= 258 && n >= 10);

  // not enough input or output--restore pointers and return
  UNGRAB
  UPDATE
  return Z_OK;
}

// crc32.c -- compute the CRC-32 of a data stream
// Copyright (C) 1995-1998 Mark Adler
// For conditions of distribution and use, see copyright notice in zlib.h

// @(#) $Id$

// Table of CRC-32's of all single-byte values (made by make_crc_table)
const uLong crc_table[256] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};

const uLong * get_crc_table()
{ return (const uLong *)crc_table;
}

#define CRC_DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define CRC_DO2(buf)  CRC_DO1(buf); CRC_DO1(buf);
#define CRC_DO4(buf)  CRC_DO2(buf); CRC_DO2(buf);
#define CRC_DO8(buf)  CRC_DO4(buf); CRC_DO4(buf);

uLong ucrc32(uLong crc, const Byte *buf, uInt len)
{ if (buf == Z_NULL) return 0L;
  crc = crc ^ 0xffffffffL;
  while (len >= 8)  {CRC_DO8(buf); len -= 8;}
  if (len) do {CRC_DO1(buf);} while (--len);
  return crc ^ 0xffffffffL;
}

// =============================================================
// some decryption routines
#define CRC32(c, b) (crc_table[((int)(c)^(b))&0xff]^((c)>>8))
void Uupdate_keys(unsigned long *keys, char c)
{ keys[0] = CRC32(keys[0],c);
  keys[1] += keys[0] & 0xFF;
  keys[1] = keys[1]*134775813L +1;
  keys[2] = CRC32(keys[2], keys[1] >> 24);
}
char Udecrypt_byte(unsigned long *keys)
{ unsigned temp = ((unsigned)keys[2] & 0xffff) | 2;
  return (char)(((temp * (temp ^ 1)) >> 8) & 0xff);
}
char zdecode(unsigned long *keys, char c)
{ c^=Udecrypt_byte(keys);
  Uupdate_keys(keys,c);
  return c;
}

// adler32.c -- compute the Adler-32 checksum of a data stream
// Copyright (C) 1995-1998 Mark Adler
// For conditions of distribution and use, see copyright notice in zlib.h

// @(#) $Id$

#define BASE 65521L // largest prime smaller than 65536
#define NMAX 5552
// NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1

#define AD_DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define AD_DO2(buf,i)  AD_DO1(buf,i); AD_DO1(buf,i+1);
#define AD_DO4(buf,i)  AD_DO2(buf,i); AD_DO2(buf,i+2);
#define AD_DO8(buf,i)  AD_DO4(buf,i); AD_DO4(buf,i+4);
#define AD_DO16(buf)   AD_DO8(buf,0); AD_DO8(buf,8);

// =========================================================================
uLong adler32(uLong adler, const Byte *buf, uInt len)
{
    unsigned long s1 = adler & 0xffff;
    unsigned long s2 = (adler >> 16) & 0xffff;
    int k;

    if (buf == Z_NULL) return 1L;

    while (len > 0) {
        k = len < NMAX ? len : NMAX;
        len -= k;
        while (k >= 16) {
            AD_DO16(buf);
        buf += 16;
            k -= 16;
        }
        if (k != 0) do {
            s1 += *buf++;
        s2 += s1;
        } while (--k);
        s1 %= BASE;
        s2 %= BASE;
    }
    return (s2 << 16) | s1;
}

// zutil.c -- target dependent utility functions for the compression library
// Copyright (C) 1995-1998 Jean-loup Gailly.
// For conditions of distribution and use, see copyright notice in zlib.h
// @(#) $Id$

const char * zlibVersion()
{
    return ZLIB_VERSION;
}

// exported to allow conversion of error code to string for compress() and
// uncompress()
const char * zError(int err)
{ return ERR_MSG(err);
}

voidpf zcalloc (voidpf opaque, unsigned items, unsigned size)
{
    if (opaque) items += size - size; // make compiler happy
    return (voidpf)calloc(items, size);
}

void  zcfree (voidpf opaque, voidpf ptr)
{
    zfree(ptr);
    if (opaque) return; // make compiler happy
}

// inflate.c -- zlib interface to inflate modules
// Copyright (C) 1995-1998 Mark Adler
// For conditions of distribution and use, see copyright notice in zlib.h

//struct inflate_blocks_state {int dummy;}; // for buggy compilers

typedef enum {
      IM_METHOD,   // waiting for method byte
      IM_FLAG,     // waiting for flag byte
      IM_DICT4,    // four dictionary check bytes to go
      IM_DICT3,    // three dictionary check bytes to go
      IM_DICT2,    // two dictionary check bytes to go
      IM_DICT1,    // one dictionary check byte to go
      IM_DICT0,    // waiting for inflateSetDictionary
      IM_BLOCKS,   // decompressing blocks
      IM_CHECK4,   // four check bytes to go
      IM_CHECK3,   // three check bytes to go
      IM_CHECK2,   // two check bytes to go
      IM_CHECK1,   // one check byte to go
      IM_DONE,     // finished check, done
      IM_BAD}      // got an error--stay here
inflate_mode;

// inflate private state
struct internal_state {

  // mode
  inflate_mode  mode;   // current inflate mode

  // mode dependent information
  union {
    uInt method;        // if IM_FLAGS, method byte
    struct {
      uLong was;                // computed check value
      uLong need;               // stream check value
    } check;            // if CHECK, check values to compare
    uInt marker;        // if IM_BAD, inflateSync's marker bytes count
  } sub;        // submode

  // mode independent information
  int  nowrap;          // flag for no wrapper
  uInt wbits;           // log2(window size)  (8..15, defaults to 15)
  inflate_blocks_statef
    *blocks;            // current inflate_blocks state

};

int inflateReset(z_streamp z)
{
  if (z == Z_NULL || z->state == Z_NULL)
    return Z_STREAM_ERROR;
  z->total_in = z->total_out = 0;
  z->msg = Z_NULL;
  z->state->mode = z->state->nowrap ? IM_BLOCKS : IM_METHOD;
  inflate_blocks_reset(z->state->blocks, z, Z_NULL);
  LuTracev((stderr, "inflate: reset\n"));
  return Z_OK;
}

int inflateEnd(z_streamp z)
{
  if (z == Z_NULL || z->state == Z_NULL || z->zfree == Z_NULL)
    return Z_STREAM_ERROR;
  if (z->state->blocks != Z_NULL)
    inflate_blocks_free(z->state->blocks, z);
  ZFREE(z, z->state);
  z->state = Z_NULL;
  LuTracev((stderr, "inflate: end\n"));
  return Z_OK;
}

int inflateInit2(z_streamp z)
{ const char *version = ZLIB_VERSION; int stream_size = sizeof(z_stream);
  if (version == Z_NULL || version[0] != ZLIB_VERSION[0] || stream_size != sizeof(z_stream)) return Z_VERSION_ERROR;

  int w = -15; // MAX_WBITS: 32K LZ77 window.
  // Warning: reducing MAX_WBITS makes minigzip unable to extract .gz files created by gzip.
  // The memory requirements for deflate are (in bytes):
  //            (1 << (windowBits+2)) +  (1 << (memLevel+9))
  // that is: 128K for windowBits=15  +  128K for memLevel = 8  (default values)
  // plus a few kilobytes for small objects. For example, if you want to reduce
  // the default memory requirements from 256K to 128K, compile with
  //     make CFLAGS="-O -DMAX_WBITS=14 -DMAX_MEM_LEVEL=7"
  // Of course this will generally degrade compression (there's no free lunch).
  //
  //   The memory requirements for inflate are (in bytes) 1 << windowBits
  // that is, 32K for windowBits=15 (default value) plus a few kilobytes
  // for small objects.

  // initialize state
  if (z == Z_NULL) return Z_STREAM_ERROR;
  z->msg = Z_NULL;
  if (z->zalloc == Z_NULL)
  {
    z->zalloc = zcalloc;
    z->opaque = (voidpf)0;
  }
  if (z->zfree == Z_NULL) z->zfree = zcfree;
  if ((z->state = (struct internal_state *)
       ZALLOC(z,1,sizeof(struct internal_state))) == Z_NULL)
    return Z_MEM_ERROR;
  z->state->blocks = Z_NULL;

  // handle undocumented nowrap option (no zlib header or check)
  z->state->nowrap = 0;
  if (w < 0)
  {
    w = - w;
    z->state->nowrap = 1;
  }

  // set window size
  if (w < 8 || w > 15)
  {
    inflateEnd(z);
    return Z_STREAM_ERROR;
  }
  z->state->wbits = (uInt)w;

  // create inflate_blocks state
  if ((z->state->blocks =
      inflate_blocks_new(z, z->state->nowrap ? Z_NULL : adler32, (uInt)1 << w))
      == Z_NULL)
  {
    inflateEnd(z);
    return Z_MEM_ERROR;
  }
  LuTracev((stderr, "inflate: allocated\n"));

  // reset state 
  inflateReset(z);
  return Z_OK;
}

#define IM_NEEDBYTE {if(z->avail_in==0)return r;r=f;}
#define IM_NEXTBYTE (z->avail_in--,z->total_in++,*z->next_in++)

int inflate(z_streamp z, int f)
{
  int r;
  uInt b;

  if (z == Z_NULL || z->state == Z_NULL || z->next_in == Z_NULL)
    return Z_STREAM_ERROR;
  f = f == Z_FINISH ? Z_BUF_ERROR : Z_OK;
  r = Z_BUF_ERROR;
  for (;;) switch (z->state->mode)
  {
    case IM_METHOD:
      IM_NEEDBYTE
      if (((z->state->sub.method = IM_NEXTBYTE) & 0xf) != Z_DEFLATED)
      {
        z->state->mode = IM_BAD;
        z->msg = (char*)"unknown compression method";
        z->state->sub.marker = 5;       // can't try inflateSync
        break;
      }
      if ((z->state->sub.method >> 4) + 8 > z->state->wbits)
      {
        z->state->mode = IM_BAD;
        z->msg = (char*)"invalid window size";
        z->state->sub.marker = 5;       // can't try inflateSync
        break;
      }
      z->state->mode = IM_FLAG;
    case IM_FLAG:
      IM_NEEDBYTE
      b = IM_NEXTBYTE;
      if (((z->state->sub.method << 8) + b) % 31)
      {
        z->state->mode = IM_BAD;
        z->msg = (char*)"incorrect header check";
        z->state->sub.marker = 5;       // can't try inflateSync 
        break;
      }
      LuTracev((stderr, "inflate: zlib header ok\n"));
      if (!(b & PRESET_DICT))
      {
        z->state->mode = IM_BLOCKS;
        break;
      }
      z->state->mode = IM_DICT4;
    case IM_DICT4:
      IM_NEEDBYTE
      z->state->sub.check.need = (uLong)IM_NEXTBYTE << 24;
      z->state->mode = IM_DICT3;
    case IM_DICT3:
      IM_NEEDBYTE
      z->state->sub.check.need += (uLong)IM_NEXTBYTE << 16;
      z->state->mode = IM_DICT2;
    case IM_DICT2:
      IM_NEEDBYTE
      z->state->sub.check.need += (uLong)IM_NEXTBYTE << 8;
      z->state->mode = IM_DICT1;
    case IM_DICT1:
      IM_NEEDBYTE; r;
      z->state->sub.check.need += (uLong)IM_NEXTBYTE;
      z->adler = z->state->sub.check.need;
      z->state->mode = IM_DICT0;
      return Z_NEED_DICT;
    case IM_DICT0:
      z->state->mode = IM_BAD;
      z->msg = (char*)"need dictionary";
      z->state->sub.marker = 0;       // can try inflateSync 
      return Z_STREAM_ERROR;
    case IM_BLOCKS:
      r = inflate_blocks(z->state->blocks, z, r);
      if (r == Z_DATA_ERROR)
      {
        z->state->mode = IM_BAD;
        z->state->sub.marker = 0;       // can try inflateSync 
        break;
      }
      if (r == Z_OK)
        r = f;
      if (r != Z_STREAM_END)
        return r;
      r = f;
      inflate_blocks_reset(z->state->blocks, z, &z->state->sub.check.was);
      if (z->state->nowrap)
      {
        z->state->mode = IM_DONE;
        break;
      }
      z->state->mode = IM_CHECK4;
    case IM_CHECK4:
      IM_NEEDBYTE
      z->state->sub.check.need = (uLong)IM_NEXTBYTE << 24;
      z->state->mode = IM_CHECK3;
    case IM_CHECK3:
      IM_NEEDBYTE
      z->state->sub.check.need += (uLong)IM_NEXTBYTE << 16;
      z->state->mode = IM_CHECK2;
    case IM_CHECK2:
      IM_NEEDBYTE
      z->state->sub.check.need += (uLong)IM_NEXTBYTE << 8;
      z->state->mode = IM_CHECK1;
    case IM_CHECK1:
      IM_NEEDBYTE
      z->state->sub.check.need += (uLong)IM_NEXTBYTE;

      if (z->state->sub.check.was != z->state->sub.check.need)
      {
        z->state->mode = IM_BAD;
        z->msg = (char*)"incorrect data check";
        z->state->sub.marker = 5;       // can't try inflateSync 
        break;
      }
      LuTracev((stderr, "inflate: zlib check ok\n"));
      z->state->mode = IM_DONE;
    case IM_DONE:
      return Z_STREAM_END;
    case IM_BAD:
      return Z_DATA_ERROR;
    default:
      return Z_STREAM_ERROR;
  }
}

// unzip.c -- IO on .zip files using zlib
// Version 0.15 beta, Mar 19th, 1998,
// Read unzip.h for more info

#define UNZ_BUFSIZE (16384)
#define UNZ_MAXFILENAMEINZIP (256)
#define SIZECENTRALDIRITEM (0x2e)
#define SIZEZIPLOCALHEADER (0x1e)

const char unz_copyright[] = " unzip 0.15 Copyright 1998 Gilles Vollant ";

// unz_file_info_interntal contain internal info about a file in zipfile
typedef struct unz_file_info_internal_s
{
    uLong offset_curfile;// relative offset of local header 4 bytes
} unz_file_info_internal;

// modified 2013.12.12
typedef struct
{
  bool canseek;
  // for fd:
  sint32 fd; long long initial_offset; bool mustclosefd; //bx20210730
  // for memory:
  void* buf; long long len; long long pos; // if it's a memory block
} LUFILE;

// modified 2013.12.12
LUFILE *lufopen(void *z, long long len, ZRESULT *err)
{
    LUFILE *lf = new LUFILE;
    lf->canseek = true;
    lf->fd = -1;
    lf->initial_offset = 0;
    lf->mustclosefd = false;
    lf->buf = z;
    lf->len = len;
    lf->pos = 0;
    *err = ZR_OK;
    return lf;
}

//bx20210730
LUFILE* lufopen_fd(sint32 fd, ZRESULT *err)
{
    uint64 FileSize = 0;
    Platform::File::FDAttributes(fd, &FileSize);

    LUFILE *lf = new LUFILE;
    lf->canseek = true;
    lf->fd = fd;
    lf->initial_offset = 0;
    lf->mustclosefd = true;
    lf->buf = nullptr;
    lf->len = FileSize;
    lf->pos = 0;
    *err = ZR_OK;
    return lf;
}

// modified 2013.12.12
int lufclose(LUFILE *stream)
{
    if(stream == NULL)
        return EOF;

    if(stream->mustclosefd)
        Platform::File::FDClose(stream->fd);
    delete stream;
    return 0;
}

// modified 2013.12.12
int luferror(LUFILE *stream)
{
    return 0;
}

// modified 2013.12.12
long long luftell(LUFILE *stream)
{
    return stream->pos;
}

// modified 2013.12.12
int lufseek(LUFILE *stream, long long offset, int whence)
{
    if (whence==SEEK_SET) stream->pos=offset;
    else if (whence==SEEK_CUR) stream->pos+=offset;
    else if (whence==SEEK_END) stream->pos=stream->len+offset;
	if(stream->mustclosefd)
        Platform::File::FDSeek(stream->fd, stream->pos, 0);
    return 0;
}

// modified 2013.12.12
size_t lufread(void *ptr, size_t size, size_t n, LUFILE *stream)
{
    long long toread = (long long) (size * n);
    if(stream->len < stream->pos + toread)
        toread = stream->len - stream->pos;

    if(stream->mustclosefd)
        Platform::File::FDRead(stream->fd, ptr, toread);
    else memcpy(ptr, (char*) stream->buf + stream->pos, toread);

    stream->pos += toread;
    return toread / size;
}

// file_in_zip_read_info_s contain internal information about a file in zipfile,
//  when reading and decompress it
typedef struct
{
    char  *read_buffer;         // internal buffer for compressed data
    z_stream stream;            // zLib stream structure for inflate

    uLong pos_in_zipfile;       // position in byte on the zipfile, for fseek
    uLong stream_initialised;   // flag set if stream structure is initialised

    uLong offset_local_extrafield;// offset of the local extra field
    uInt  size_local_extrafield;// size of the local extra field
    uLong pos_local_extrafield;   // position in the local extra field in read

    uLong crc32;                // crc32 of all data uncompressed
    uLong crc32_wait;           // crc32 we must obtain after decompress all
    uLong rest_read_compressed; // number of byte to be decompressed
    uLong rest_read_uncompressed;//number of byte to be obtained after decomp
    LUFILE* file;                 // io structore of the zipfile
    uLong compression_method;   // compression method (0==store)
    uLong byte_before_the_zipfile;// byte before the zipfile, (>0 for sfx)
  bool encrypted;               // is it encrypted?
  unsigned long keys[3];        // decryption keys, initialized by unzOpenCurrentFile
  int encheadleft;              // the first call(s) to unzReadCurrentFile will read this many encryption-header bytes first
  char crcenctest;              // if encrypted, we'll check the encryption buffer against this
} file_in_zip_read_info_s;

// unz_s contain internal information about the zipfile
typedef struct
{
    LUFILE* file;               // io structore of the zipfile
    unz_global_info gi;         // public global information
    uLong byte_before_the_zipfile;// byte before the zipfile, (>0 for sfx)
    uLong num_file;             // number of the current file in the zipfile
    uLong pos_in_central_dir;   // pos of the current file in the central dir
    uLong current_file_ok;      // flag about the usability of the current file
    uLong central_pos;          // position of the beginning of the central dir

    uLong size_central_dir;     // size of the central directory
    uLong offset_central_dir;   // offset of start of central directory with respect to the starting disk number

    unz_file_info cur_file_info; // public info about the current file in zip
    unz_file_info_internal cur_file_info_internal; // private info about it
    file_in_zip_read_info_s* pfile_in_zip_read; // structure about the current file if we are decompressing it
} unz_s, *unzFile;

int unzStringFileNameCompare (const char* fileName1,const char* fileName2,int iCaseSensitivity);
//   Compare two filename (fileName1,fileName2).

z_off_t unztell (unzFile file);
//  Give the current position in uncompressed data

int unzeof (unzFile file);
//  return 1 if the end of file was reached, 0 elsewhere

int unzGetLocalExtrafield (unzFile file, voidp buf, unsigned len);
//  Read extra field from the current file (opened by unzOpenCurrentFile)
//  This is the local-header version of the extra field (sometimes, there is
//    more info in the local-header version than in the central-header)
//
//  if buf==NULL, it return the size of the local extra field
//
//  if buf!=NULL, len is the size of the buffer, the extra header is copied in
//    buf.
//  the return value is the number of bytes copied in buf, or (if <0)
//    the error code

// ===========================================================================
//   Read a byte from a gz_stream; update next_in and avail_in. Return EOF
// for end of file.
// IN assertion: the stream s has been sucessfully opened for reading.

int unzlocal_getByte(LUFILE *fin,int *pi)
{ unsigned char c;
  int err = (int)lufread(&c, 1, 1, fin);
  if (err==1)
  { *pi = (int)c;
    return UNZ_OK;
  }
  else
  { if (luferror(fin)) return UNZ_ERRNO;
    else return UNZ_EOF;
  }
}

// ===========================================================================
// Reads a long in LSB order from the given gz_stream. Sets
int unzlocal_getShort (LUFILE *fin,uLong *pX)
{
    uLong x ;
    int i = 0;
    int err;

    err = unzlocal_getByte(fin,&i);
    x = (uLong)i;

    if (err==UNZ_OK)
        err = unzlocal_getByte(fin,&i);
    x += ((uLong)i)<<8;

    if (err==UNZ_OK)
        *pX = x;
    else
        *pX = 0;
    return err;
}

int unzlocal_getLong (LUFILE *fin,uLong *pX)
{
    uLong x ;
    int i = 0;
    int err;

    err = unzlocal_getByte(fin,&i);
    x = (uLong)i;
    
    if (err==UNZ_OK)
        err = unzlocal_getByte(fin,&i);
    x += ((uLong)i)<<8;

    if (err==UNZ_OK)
        err = unzlocal_getByte(fin,&i);
    x += ((uLong)i)<<16;

    if (err==UNZ_OK)
        err = unzlocal_getByte(fin,&i);
    x += ((uLong)i)<<24;
   
    if (err==UNZ_OK)
        *pX = x;
    else
        *pX = 0;
    return err;
}

// My own strcmpi / strcasecmp 
int strcmpcasenosensitive_internal (const char* fileName1,const char *fileName2)
{
    for (;;)
    {
        char c1=*(fileName1++);
        char c2=*(fileName2++);
        if ((c1>='a') && (c1<='z'))
            c1 -= (char)0x20;
        if ((c2>='a') && (c2<='z'))
            c2 -= (char)0x20;
        if (c1=='\0')
            return ((c2=='\0') ? 0 : -1);
        if (c2=='\0')
            return 1;
        if (c1<c2)
            return -1;
        if (c1>c2)
            return 1;
    }
}

//
// Compare two filename (fileName1,fileName2).
// If iCaseSenisivity = 1, comparision is case sensitivity (like strcmp)
// If iCaseSenisivity = 2, comparision is not case sensitivity (like strcmpi or strcasecmp)
//
int unzStringFileNameCompare (const char*fileName1,const char*fileName2,int iCaseSensitivity)
{ if (iCaseSensitivity==1) return strcmp(fileName1,fileName2);
  else return strcmpcasenosensitive_internal(fileName1,fileName2);
} 

#define BUFREADCOMMENT (0x400)


//  Locate the Central directory of a zipfile (at the end, just before
// the global comment). Lu bugfix 2005.07.26 - returns oxFFFFFFFFFFFFFFFF if not found,
// rather than 0, since 0 is a valid central-dir-location for an empty zipfile.
uLongLong unzlocal_SearchCentralDir(LUFILE *fin)
{
  if (lufseek(fin,0,SEEK_END) != 0) return oxFFFFFFFFFFFFFFFF;
  uLongLong uSizeFile = luftell(fin);

  uLongLong uMaxBack = 0xFFFF; // maximum size of global comment
  if (uMaxBack>uSizeFile) uMaxBack = uSizeFile;

  unsigned char *buf = (unsigned char*)zmalloc(BUFREADCOMMENT+4);
  if (buf==NULL) return oxFFFFFFFFFFFFFFFF;
  uLongLong uPosFound = oxFFFFFFFFFFFFFFFF;

  uLongLong uBackRead = 4;
  while (uBackRead<uMaxBack)
  {
    uLongLong uReadSize,uReadPos ;
    int i;
    if (uBackRead+BUFREADCOMMENT>uMaxBack) uBackRead = uMaxBack;
    else uBackRead+=BUFREADCOMMENT;
    uReadPos = uSizeFile-uBackRead ;
    uReadSize = ((BUFREADCOMMENT+4) < (uSizeFile-uReadPos)) ? (BUFREADCOMMENT+4) : (uSizeFile-uReadPos);
    if (lufseek(fin,uReadPos,SEEK_SET)!=0) break;
    if (lufread(buf,(uInt)uReadSize,1,fin)!=1) break;
    for (i=(int)uReadSize-3; (i--)>=0;)
    {
      if (buf[i]==0x50 && buf[i+1]==0x4b && buf[i+2]==0x05 && buf[i+3]==0x06)
      {
        uPosFound = uReadPos+i;
        break;
      }
    }
    if (uPosFound!=0)
        break;
  }
  if (buf) zfree(buf);
  return uPosFound;
}

int unzGoToFirstFile (unzFile file);
int unzCloseCurrentFile (unzFile file);

// Open a Zip file.
// If the zipfile cannot be opened (file don't exist or in not valid), return NULL.
// Otherwise, the return value is a unzFile Handle, usable with other unzip functions
unzFile unzOpenInternal(LUFILE *fin)
{ if (fin==NULL) return NULL;
  if (unz_copyright[0]!=' ') {lufclose(fin); return NULL;}

  int err=UNZ_OK;
  unz_s us;
  uLongLong central_pos;
  uLong uL;
  central_pos = unzlocal_SearchCentralDir(fin);
  if (central_pos==oxFFFFFFFFFFFFFFFF) err=UNZ_ERRNO;
  if (lufseek(fin,central_pos,SEEK_SET)!=0) err=UNZ_ERRNO;
  // the signature, already checked
  if (unzlocal_getLong(fin,&uL)!=UNZ_OK) err=UNZ_ERRNO;
  // number of this disk
  uLong number_disk;          // number of the current dist, used for spanning ZIP, unsupported, always 0
  if (unzlocal_getShort(fin,&number_disk)!=UNZ_OK) err=UNZ_ERRNO;
  // number of the disk with the start of the central directory
  uLong number_disk_with_CD;  // number the the disk with central dir, used for spaning ZIP, unsupported, always 0
  if (unzlocal_getShort(fin,&number_disk_with_CD)!=UNZ_OK) err=UNZ_ERRNO;
  // total number of entries in the central dir on this disk
  if (unzlocal_getShort(fin,&us.gi.number_entry)!=UNZ_OK) err=UNZ_ERRNO;
  // total number of entries in the central dir
  uLong number_entry_CD;      // total number of entries in the central dir (same than number_entry on nospan)
  if (unzlocal_getShort(fin,&number_entry_CD)!=UNZ_OK) err=UNZ_ERRNO;
  if ((number_entry_CD!=us.gi.number_entry) || (number_disk_with_CD!=0) || (number_disk!=0)) err=UNZ_BADZIPFILE;
  // size of the central directory
  if (unzlocal_getLong(fin,&us.size_central_dir)!=UNZ_OK) err=UNZ_ERRNO;
  // offset of start of central directory with respect to the starting disk number
  if (unzlocal_getLong(fin,&us.offset_central_dir)!=UNZ_OK) err=UNZ_ERRNO;
  // zipfile comment length
  if (unzlocal_getShort(fin,&us.gi.size_comment)!=UNZ_OK) err=UNZ_ERRNO;
  if ((central_pos+fin->initial_offset<us.offset_central_dir+us.size_central_dir) && (err==UNZ_OK)) err=UNZ_BADZIPFILE;
  if (err!=UNZ_OK) {lufclose(fin);return NULL;}

  us.file=fin;
  us.byte_before_the_zipfile = central_pos+fin->initial_offset - (us.offset_central_dir+us.size_central_dir);
  us.central_pos = central_pos;
  us.pfile_in_zip_read = NULL;
  fin->initial_offset = 0; // since the zipfile itself is expected to handle this

  unz_s *s = (unz_s*)zmalloc(sizeof(unz_s));
  *s=us;
  unzGoToFirstFile((unzFile)s);
  return (unzFile)s;
}

//  Close a ZipFile opened with unzipOpen.
//  If there is files inside the .Zip opened with unzipOpenCurrentFile (see later),
//    these files MUST be closed with unzipCloseCurrentFile before call unzipClose.
//  return UNZ_OK if there is no problem.
int unzClose (unzFile file)
{
    unz_s* s;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;

    if (s->pfile_in_zip_read!=NULL)
        unzCloseCurrentFile(file);

    lufclose(s->file);
    if (s) zfree(s); // unused s=0;
    return UNZ_OK;
}

//  Write info about the ZipFile in the *pglobal_info structure.
//  No preparation of the structure is needed
//  return UNZ_OK if there is no problem. 
int unzGetGlobalInfo (unzFile file,unz_global_info *pglobal_info)
{
    unz_s* s;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    *pglobal_info=s->gi;
    return UNZ_OK;
}

//   Translate date/time from Dos format to tm_unz (readable more easilty)
void unzlocal_DosDateToTmuDate (uLong ulDosDate, tm_unz* ptm)
{
    uLong uDate;
    uDate = (uLong)(ulDosDate>>16);
    ptm->tm_mday = (uInt)(uDate&0x1f) ;
    ptm->tm_mon =  (uInt)((((uDate)&0x1E0)/0x20)-1) ;
    ptm->tm_year = (uInt)(((uDate&0x0FE00)/0x0200)+1980) ;

    ptm->tm_hour = (uInt) ((ulDosDate &0xF800)/0x800);
    ptm->tm_min =  (uInt) ((ulDosDate&0x7E0)/0x20) ;
    ptm->tm_sec =  (uInt) (2*(ulDosDate&0x1f)) ;
}

//  Get Info about the current file in the zipfile, with internal only info
int unzlocal_GetCurrentFileInfoInternal (unzFile file,
                                                  unz_file_info *pfile_info,
                                                  unz_file_info_internal
                                                  *pfile_info_internal,
                                                  char *szFileName,
                                                  uLong fileNameBufferSize,
                                                  void *extraField,
                                                  uLong extraFieldBufferSize,
                                                  char *szComment,
                                                  uLong commentBufferSize);

int unzlocal_GetCurrentFileInfoInternal (unzFile file, unz_file_info *pfile_info,
   unz_file_info_internal *pfile_info_internal, char *szFileName,
   uLong fileNameBufferSize, void *extraField, uLong extraFieldBufferSize,
   char *szComment, uLong commentBufferSize)
{
    unz_s* s;
    unz_file_info file_info;
    unz_file_info_internal file_info_internal;
    int err=UNZ_OK;
    uLong uMagic;
    long lSeek=0;

    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    if (lufseek(s->file,s->pos_in_central_dir+s->byte_before_the_zipfile,SEEK_SET)!=0)
        err=UNZ_ERRNO;


    // we check the magic
    if (err==UNZ_OK)
        if (unzlocal_getLong(s->file,&uMagic) != UNZ_OK)
            err=UNZ_ERRNO;
        else if (uMagic!=0x02014b50)
            err=UNZ_BADZIPFILE;

    if (unzlocal_getShort(s->file,&file_info.version) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(s->file,&file_info.version_needed) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(s->file,&file_info.flag) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(s->file,&file_info.compression_method) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getLong(s->file,&file_info.dosDate) != UNZ_OK)
        err=UNZ_ERRNO;

    unzlocal_DosDateToTmuDate(file_info.dosDate,&file_info.tmu_date);

    if (unzlocal_getLong(s->file,&file_info.crc) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getLong(s->file,&file_info.compressed_size) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getLong(s->file,&file_info.uncompressed_size) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(s->file,&file_info.size_filename) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(s->file,&file_info.size_file_extra) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(s->file,&file_info.size_file_comment) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(s->file,&file_info.disk_num_start) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(s->file,&file_info.internal_fa) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getLong(s->file,&file_info.external_fa) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getLong(s->file,&file_info_internal.offset_curfile) != UNZ_OK)
        err=UNZ_ERRNO;

    lSeek+=file_info.size_filename;
    if ((err==UNZ_OK) && (szFileName!=NULL))
    {
        uLong uSizeRead ;
        if (file_info.size_filename<fileNameBufferSize)
        {
            *(szFileName+file_info.size_filename)='\0';
            uSizeRead = file_info.size_filename;
        }
        else
            uSizeRead = fileNameBufferSize;

        if ((file_info.size_filename>0) && (fileNameBufferSize>0))
            if (lufread(szFileName,(uInt)uSizeRead,1,s->file)!=1)
                err=UNZ_ERRNO;
        lSeek -= uSizeRead;
    }


    if ((err==UNZ_OK) && (extraField!=NULL))
    {
        uLong uSizeRead ;
        if (file_info.size_file_extra<extraFieldBufferSize)
            uSizeRead = file_info.size_file_extra;
        else
            uSizeRead = extraFieldBufferSize;

        if (lSeek!=0)
            if (lufseek(s->file,lSeek,SEEK_CUR)==0)
                lSeek=0;
            else
                err=UNZ_ERRNO;
        if ((file_info.size_file_extra>0) && (extraFieldBufferSize>0))
            if (lufread(extraField,(uInt)uSizeRead,1,s->file)!=1)
                err=UNZ_ERRNO;
        lSeek += file_info.size_file_extra - uSizeRead;
    }
    else
        lSeek+=file_info.size_file_extra;


    if ((err==UNZ_OK) && (szComment!=NULL))
    {
        uLong uSizeRead ;
        if (file_info.size_file_comment<commentBufferSize)
        {
            *(szComment+file_info.size_file_comment)='\0';
            uSizeRead = file_info.size_file_comment;
        }
        else
            uSizeRead = commentBufferSize;

        if (lSeek!=0)
            if (lufseek(s->file,lSeek,SEEK_CUR)==0)
                {} // unused lSeek=0;
            else
                err=UNZ_ERRNO;
        if ((file_info.size_file_comment>0) && (commentBufferSize>0))
            if (lufread(szComment,(uInt)uSizeRead,1,s->file)!=1)
                err=UNZ_ERRNO;
        //unused lSeek+=file_info.size_file_comment - uSizeRead;
    }
    else {} //unused lSeek+=file_info.size_file_comment;

    if ((err==UNZ_OK) && (pfile_info!=NULL))
        *pfile_info=file_info;

    if ((err==UNZ_OK) && (pfile_info_internal!=NULL))
        *pfile_info_internal=file_info_internal;

    return err;
}

//  Write info about the ZipFile in the *pglobal_info structure.
//  No preparation of the structure is needed
//  return UNZ_OK if there is no problem.
int unzGetCurrentFileInfo (unzFile file, unz_file_info *pfile_info,
  char *szFileName, uLong fileNameBufferSize, void *extraField, uLong extraFieldBufferSize,
  char *szComment, uLong commentBufferSize)
{ return unzlocal_GetCurrentFileInfoInternal(file,pfile_info,NULL,szFileName,fileNameBufferSize,
      extraField,extraFieldBufferSize, szComment,commentBufferSize);
}

//  Set the current file of the zipfile to the first file.
//  return UNZ_OK if there is no problem
int unzGoToFirstFile (unzFile file)
{
    int err;
    unz_s* s;
    if (file==NULL) return UNZ_PARAMERROR;
    s=(unz_s*)file;
    s->pos_in_central_dir=s->offset_central_dir;
    s->num_file=0;
    err=unzlocal_GetCurrentFileInfoInternal(file,&s->cur_file_info,
                                             &s->cur_file_info_internal,
                                             NULL,0,NULL,0,NULL,0);
    s->current_file_ok = (err == UNZ_OK);
    return err;
}

//  Set the current file of the zipfile to the next file.
//  return UNZ_OK if there is no problem
//  return UNZ_END_OF_LIST_OF_FILE if the actual file was the latest.
int unzGoToNextFile (unzFile file)
{
    unz_s* s;
    int err;

    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    if (!s->current_file_ok)
        return UNZ_END_OF_LIST_OF_FILE;
    if (s->num_file+1==s->gi.number_entry)
        return UNZ_END_OF_LIST_OF_FILE;

    s->pos_in_central_dir += SIZECENTRALDIRITEM + s->cur_file_info.size_filename +
            s->cur_file_info.size_file_extra + s->cur_file_info.size_file_comment ;
    s->num_file++;
    err = unzlocal_GetCurrentFileInfoInternal(file,&s->cur_file_info,
                                               &s->cur_file_info_internal,
                                               NULL,0,NULL,0,NULL,0);
    s->current_file_ok = (err == UNZ_OK);
    return err;
}

//  Try locate the file szFileName in the zipfile.
//  For the iCaseSensitivity signification, see unzStringFileNameCompare
//  return value :
//  UNZ_OK if the file is found. It becomes the current file.
//  UNZ_END_OF_LIST_OF_FILE if the file is not found
int unzLocateFile (unzFile file, const char *szFileName, int iCaseSensitivity)
{
    unz_s* s;
    int err;


    uLong num_fileSaved;
    uLong pos_in_central_dirSaved;


    if (file==NULL)
        return UNZ_PARAMERROR;

    if (strlen(szFileName)>=UNZ_MAXFILENAMEINZIP)
        return UNZ_PARAMERROR;

    s=(unz_s*)file;
    if (!s->current_file_ok)
        return UNZ_END_OF_LIST_OF_FILE;

    num_fileSaved = s->num_file;
    pos_in_central_dirSaved = s->pos_in_central_dir;

    err = unzGoToFirstFile(file);

    while (err == UNZ_OK)
    {
        char szCurrentFileName[UNZ_MAXFILENAMEINZIP+1];
        unzGetCurrentFileInfo(file,NULL,
                                szCurrentFileName,sizeof(szCurrentFileName)-1,
                                NULL,0,NULL,0);
        if (unzStringFileNameCompare(szCurrentFileName,szFileName,iCaseSensitivity)==0)
            return UNZ_OK;
        err = unzGoToNextFile(file);
    }

    s->num_file = num_fileSaved ;
    s->pos_in_central_dir = pos_in_central_dirSaved ;
    return err;
}

//  Read the local header of the current zipfile
//  Check the coherency of the local header and info in the end of central
//        directory about this file
//  store in *piSizeVar the size of extra info in local header
//        (filename and size of extra field data)
int unzlocal_CheckCurrentFileCoherencyHeader (unz_s *s,uInt *piSizeVar,
  uLong *poffset_local_extrafield, uInt  *psize_local_extrafield)
{
    uLong uMagic,uData,uFlags;
    uLong size_filename;
    uLong size_extra_field;
    int err=UNZ_OK;

    *piSizeVar = 0;
    *poffset_local_extrafield = 0;
    *psize_local_extrafield = 0;

    if (lufseek(s->file,s->cur_file_info_internal.offset_curfile + s->byte_before_the_zipfile,SEEK_SET)!=0)
        return UNZ_ERRNO;


    if (err==UNZ_OK)
        if (unzlocal_getLong(s->file,&uMagic) != UNZ_OK)
            err=UNZ_ERRNO;
        else if (uMagic!=0x04034b50)
            err=UNZ_BADZIPFILE;

    if (unzlocal_getShort(s->file,&uData) != UNZ_OK)
        err=UNZ_ERRNO;
//    else if ((err==UNZ_OK) && (uData!=s->cur_file_info.wVersion))
//        err=UNZ_BADZIPFILE;
    if (unzlocal_getShort(s->file,&uFlags) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(s->file,&uData) != UNZ_OK)
        err=UNZ_ERRNO;
    else if ((err==UNZ_OK) && (uData!=s->cur_file_info.compression_method))
        err=UNZ_BADZIPFILE;

    if ((err==UNZ_OK) && (s->cur_file_info.compression_method!=0) &&
                         (s->cur_file_info.compression_method!=Z_DEFLATED))
        err=UNZ_BADZIPFILE;

    if (unzlocal_getLong(s->file,&uData) != UNZ_OK) // date/time
        err=UNZ_ERRNO;

    if (unzlocal_getLong(s->file,&uData) != UNZ_OK) // crc
        err=UNZ_ERRNO;
    else if ((err==UNZ_OK) && (uData!=s->cur_file_info.crc) &&
                              ((uFlags & 8)==0))
        err=UNZ_BADZIPFILE;

    if (unzlocal_getLong(s->file,&uData) != UNZ_OK) // size compr
        err=UNZ_ERRNO;
    else if ((err==UNZ_OK) && (uData!=s->cur_file_info.compressed_size) &&
                              ((uFlags & 8)==0))
        err=UNZ_BADZIPFILE;

    if (unzlocal_getLong(s->file,&uData) != UNZ_OK) // size uncompr
        err=UNZ_ERRNO;
    else if ((err==UNZ_OK) && (uData!=s->cur_file_info.uncompressed_size) &&
                              ((uFlags & 8)==0))
        err=UNZ_BADZIPFILE;


    if (unzlocal_getShort(s->file,&size_filename) != UNZ_OK)
        err=UNZ_ERRNO;
    else if ((err==UNZ_OK) && (size_filename!=s->cur_file_info.size_filename))
        err=UNZ_BADZIPFILE;

    *piSizeVar += (uInt)size_filename;

    if (unzlocal_getShort(s->file,&size_extra_field) != UNZ_OK)
        err=UNZ_ERRNO;
    *poffset_local_extrafield= s->cur_file_info_internal.offset_curfile +
                                    SIZEZIPLOCALHEADER + size_filename;
    *psize_local_extrafield = (uInt)size_extra_field;

    *piSizeVar += (uInt)size_extra_field;

    return err;
}

//  Open for reading data the current file in the zipfile.
//  If there is no error and the file is opened, the return value is UNZ_OK.
int unzOpenCurrentFile (unzFile file, const char *password)
{
    int err;
    int Store;
    uInt iSizeVar;
    unz_s* s;
    file_in_zip_read_info_s* pfile_in_zip_read_info;
    uLong offset_local_extrafield;  // offset of the local extra field
    uInt  size_local_extrafield;    // size of the local extra field

    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    if (!s->current_file_ok)
        return UNZ_PARAMERROR;

    if (s->pfile_in_zip_read != NULL)
        unzCloseCurrentFile(file);

    if (unzlocal_CheckCurrentFileCoherencyHeader(s,&iSizeVar,
                &offset_local_extrafield,&size_local_extrafield)!=UNZ_OK)
        return UNZ_BADZIPFILE;

    pfile_in_zip_read_info = (file_in_zip_read_info_s*)zmalloc(sizeof(file_in_zip_read_info_s));
    if (pfile_in_zip_read_info==NULL)
        return UNZ_INTERNALERROR;

    pfile_in_zip_read_info->read_buffer=(char*)zmalloc(UNZ_BUFSIZE);
    pfile_in_zip_read_info->offset_local_extrafield = offset_local_extrafield;
    pfile_in_zip_read_info->size_local_extrafield = size_local_extrafield;
    pfile_in_zip_read_info->pos_local_extrafield=0;

    if (pfile_in_zip_read_info->read_buffer==NULL)
    {
        if (pfile_in_zip_read_info!=0) zfree(pfile_in_zip_read_info); //unused pfile_in_zip_read_info=0;
        return UNZ_INTERNALERROR;
    }

    pfile_in_zip_read_info->stream_initialised=0;

    if ((s->cur_file_info.compression_method!=0) && (s->cur_file_info.compression_method!=Z_DEFLATED))
        { // unused err=UNZ_BADZIPFILE;
        }
    Store = s->cur_file_info.compression_method==0;

    pfile_in_zip_read_info->crc32_wait=s->cur_file_info.crc;
    pfile_in_zip_read_info->crc32=0;
    pfile_in_zip_read_info->compression_method = s->cur_file_info.compression_method;
    pfile_in_zip_read_info->file=s->file;
    pfile_in_zip_read_info->byte_before_the_zipfile=s->byte_before_the_zipfile;

    pfile_in_zip_read_info->stream.total_out = 0;

    if (!Store)
    {
      pfile_in_zip_read_info->stream.zalloc = (alloc_func)0;
      pfile_in_zip_read_info->stream.zfree = (free_func)0;
      pfile_in_zip_read_info->stream.opaque = (voidpf)0;

          err=inflateInit2(&pfile_in_zip_read_info->stream);
      if (err == Z_OK)
        pfile_in_zip_read_info->stream_initialised=1;
        // windowBits is passed < 0 to tell that there is no zlib header.
        // Note that in this case inflate *requires* an extra "dummy" byte
        // after the compressed stream in order to complete decompression and
        // return Z_STREAM_END.
        // In unzip, i don't wait absolutely Z_STREAM_END because I known the
        // size of both compressed and uncompressed data
    }
    pfile_in_zip_read_info->rest_read_compressed = s->cur_file_info.compressed_size ;
    pfile_in_zip_read_info->rest_read_uncompressed = s->cur_file_info.uncompressed_size ;
  pfile_in_zip_read_info->encrypted = (s->cur_file_info.flag&1)!=0;
  bool extlochead = (s->cur_file_info.flag&8)!=0;
  if (extlochead) pfile_in_zip_read_info->crcenctest = (char)((s->cur_file_info.dosDate>>8)&0xff);
  else pfile_in_zip_read_info->crcenctest = (char)(s->cur_file_info.crc >> 24);
  pfile_in_zip_read_info->encheadleft = (pfile_in_zip_read_info->encrypted?12:0);
  pfile_in_zip_read_info->keys[0] = 305419896L;
  pfile_in_zip_read_info->keys[1] = 591751049L;
  pfile_in_zip_read_info->keys[2] = 878082192L;
  for (const char *cp=password; cp!=0 && *cp!=0; cp++) Uupdate_keys(pfile_in_zip_read_info->keys,*cp);

    pfile_in_zip_read_info->pos_in_zipfile =
            s->cur_file_info_internal.offset_curfile + SIZEZIPLOCALHEADER +
              iSizeVar;

    pfile_in_zip_read_info->stream.avail_in = (uInt)0;

    s->pfile_in_zip_read = pfile_in_zip_read_info;

  return UNZ_OK;
}

//  Read bytes from the current file.
//  buf contain buffer where data must be copied
//  len the size of buf.
//  return the number of byte copied if somes bytes are copied (and also sets *reached_eof)
//  return 0 if the end of file was reached. (and also sets *reached_eof).
//  return <0 with error code if there is an error. (in which case *reached_eof is meaningless)
//    (UNZ_ERRNO for IO error, or zLib error for uncompress error)
int unzReadCurrentFile  (unzFile file, voidp buf, unsigned len, bool *reached_eof)
{ int err=UNZ_OK;
  uInt iRead = 0;
  if (reached_eof!=0) *reached_eof=false;

  unz_s *s = (unz_s*)file;
  if (s==NULL) return UNZ_PARAMERROR;

  file_in_zip_read_info_s* pfile_in_zip_read_info = s->pfile_in_zip_read;
  if (pfile_in_zip_read_info==NULL) return UNZ_PARAMERROR;
  if ((pfile_in_zip_read_info->read_buffer == NULL)) return UNZ_END_OF_LIST_OF_FILE;
  if (len==0) return 0;

  pfile_in_zip_read_info->stream.next_out = (Byte*)buf;
  pfile_in_zip_read_info->stream.avail_out = (uInt)len;

  if (len>pfile_in_zip_read_info->rest_read_uncompressed)
  { pfile_in_zip_read_info->stream.avail_out = (uInt)pfile_in_zip_read_info->rest_read_uncompressed;
  }

  while (pfile_in_zip_read_info->stream.avail_out>0)
  { if ((pfile_in_zip_read_info->stream.avail_in==0) && (pfile_in_zip_read_info->rest_read_compressed>0))
    { uInt uReadThis = UNZ_BUFSIZE;
      if (pfile_in_zip_read_info->rest_read_compressed<uReadThis) uReadThis = (uInt)pfile_in_zip_read_info->rest_read_compressed;
      if (uReadThis == 0) {if (reached_eof!=0) *reached_eof=true; return UNZ_EOF;}
      if (lufseek(pfile_in_zip_read_info->file, pfile_in_zip_read_info->pos_in_zipfile + pfile_in_zip_read_info->byte_before_the_zipfile,SEEK_SET)!=0) return UNZ_ERRNO;
      if (lufread(pfile_in_zip_read_info->read_buffer,uReadThis,1,pfile_in_zip_read_info->file)!=1) return UNZ_ERRNO;
      pfile_in_zip_read_info->pos_in_zipfile += uReadThis;
      pfile_in_zip_read_info->rest_read_compressed-=uReadThis;
      pfile_in_zip_read_info->stream.next_in = (Byte*)pfile_in_zip_read_info->read_buffer;
      pfile_in_zip_read_info->stream.avail_in = (uInt)uReadThis;
      //
      if (pfile_in_zip_read_info->encrypted)
      { char *buf = (char*)pfile_in_zip_read_info->stream.next_in;
        for (unsigned int i=0; i<uReadThis; i++) buf[i]=zdecode(pfile_in_zip_read_info->keys,buf[i]);
      }
    }

    unsigned int uDoEncHead = pfile_in_zip_read_info->encheadleft;
    if (uDoEncHead>pfile_in_zip_read_info->stream.avail_in) uDoEncHead=pfile_in_zip_read_info->stream.avail_in;
    if (uDoEncHead>0)
    { char bufcrc=pfile_in_zip_read_info->stream.next_in[uDoEncHead-1];
      pfile_in_zip_read_info->rest_read_uncompressed-=uDoEncHead;
      pfile_in_zip_read_info->stream.avail_in -= uDoEncHead;
      pfile_in_zip_read_info->stream.next_in += uDoEncHead;
      pfile_in_zip_read_info->encheadleft -= uDoEncHead;
      if (pfile_in_zip_read_info->encheadleft==0)
      { if (bufcrc!=pfile_in_zip_read_info->crcenctest) return UNZ_PASSWORD;
      }
    }

    if (pfile_in_zip_read_info->compression_method==0)
    { uInt uDoCopy,i ;
      if (pfile_in_zip_read_info->stream.avail_out < pfile_in_zip_read_info->stream.avail_in)
      { uDoCopy = pfile_in_zip_read_info->stream.avail_out ;
      }
      else
      { uDoCopy = pfile_in_zip_read_info->stream.avail_in ;
      }
      for (i=0;i<uDoCopy;i++) *(pfile_in_zip_read_info->stream.next_out+i) = *(pfile_in_zip_read_info->stream.next_in+i);
      pfile_in_zip_read_info->crc32 = ucrc32(pfile_in_zip_read_info->crc32,pfile_in_zip_read_info->stream.next_out,uDoCopy);
      pfile_in_zip_read_info->rest_read_uncompressed-=uDoCopy;
      pfile_in_zip_read_info->stream.avail_in -= uDoCopy;
      pfile_in_zip_read_info->stream.avail_out -= uDoCopy;
      pfile_in_zip_read_info->stream.next_out += uDoCopy;
      pfile_in_zip_read_info->stream.next_in += uDoCopy;
      pfile_in_zip_read_info->stream.total_out += uDoCopy;
      iRead += uDoCopy;
      if (pfile_in_zip_read_info->rest_read_uncompressed==0) {if (reached_eof!=0) *reached_eof=true;}
    }
    else
    { uLong uTotalOutBefore,uTotalOutAfter;
      const Byte *bufBefore;
      uLong uOutThis;
      int flush=Z_SYNC_FLUSH;
      uTotalOutBefore = pfile_in_zip_read_info->stream.total_out;
      bufBefore = pfile_in_zip_read_info->stream.next_out;
      //
      err=inflate(&pfile_in_zip_read_info->stream,flush);
      //
      uTotalOutAfter = pfile_in_zip_read_info->stream.total_out;
      uOutThis = uTotalOutAfter-uTotalOutBefore;
      pfile_in_zip_read_info->crc32 = ucrc32(pfile_in_zip_read_info->crc32,bufBefore,(uInt)(uOutThis));
      pfile_in_zip_read_info->rest_read_uncompressed -= uOutThis;
      iRead += (uInt)(uTotalOutAfter - uTotalOutBefore);
      if (err==Z_STREAM_END || pfile_in_zip_read_info->rest_read_uncompressed==0)
      { if (reached_eof!=0) *reached_eof=true;
        return iRead;
      }
      if (err!=Z_OK) break;
    }
  }

  if (err==Z_OK) return iRead;
  return err;
}

//  Give the current position in uncompressed data
z_off_t unztell (unzFile file)
{
    unz_s* s;
    file_in_zip_read_info_s* pfile_in_zip_read_info;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

    if (pfile_in_zip_read_info==NULL)
        return UNZ_PARAMERROR;

    return (z_off_t)pfile_in_zip_read_info->stream.total_out;
}

//  return 1 if the end of file was reached, 0 elsewhere
int unzeof (unzFile file)
{
    unz_s* s;
    file_in_zip_read_info_s* pfile_in_zip_read_info;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

    if (pfile_in_zip_read_info==NULL)
        return UNZ_PARAMERROR;

    if (pfile_in_zip_read_info->rest_read_uncompressed == 0)
        return 1;
    else
        return 0;
}

//  Read extra field from the current file (opened by unzOpenCurrentFile)
//  This is the local-header version of the extra field (sometimes, there is
//    more info in the local-header version than in the central-header)
//  if buf==NULL, it return the size of the local extra field that can be read
//  if buf!=NULL, len is the size of the buffer, the extra header is copied in buf.
//  the return value is the number of bytes copied in buf, or (if <0) the error code
int unzGetLocalExtrafield (unzFile file,voidp buf,unsigned len)
{
    unz_s* s;
    file_in_zip_read_info_s* pfile_in_zip_read_info;
    uInt read_now;
    uLong size_to_read;

    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

    if (pfile_in_zip_read_info==NULL)
        return UNZ_PARAMERROR;

    size_to_read = (pfile_in_zip_read_info->size_local_extrafield -
                pfile_in_zip_read_info->pos_local_extrafield);

    if (buf==NULL)
        return (int)size_to_read;

    if (len>size_to_read)
        read_now = (uInt)size_to_read;
    else
        read_now = (uInt)len ;

    if (read_now==0)
        return 0;

    if (lufseek(pfile_in_zip_read_info->file, pfile_in_zip_read_info->offset_local_extrafield +  pfile_in_zip_read_info->pos_local_extrafield,SEEK_SET)!=0)
        return UNZ_ERRNO;

    if (lufread(buf,(uInt)size_to_read,1,pfile_in_zip_read_info->file)!=1)
        return UNZ_ERRNO;

    return (int)read_now;
}

//  Close the file in zip opened with unzipOpenCurrentFile
//  Return UNZ_CRCERROR if all the file was read but the CRC is not good
int unzCloseCurrentFile (unzFile file)
{
    int err=UNZ_OK;

    unz_s* s;
    file_in_zip_read_info_s* pfile_in_zip_read_info;
    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;

    if (pfile_in_zip_read_info==NULL)
        return UNZ_PARAMERROR;


    if (pfile_in_zip_read_info->rest_read_uncompressed == 0)
    {
        if (pfile_in_zip_read_info->crc32 != pfile_in_zip_read_info->crc32_wait)
            err=UNZ_CRCERROR;
    }


    if (pfile_in_zip_read_info->read_buffer!=0)
        { void *buf = pfile_in_zip_read_info->read_buffer;
          zfree(buf);
          pfile_in_zip_read_info->read_buffer=0;
        }
    pfile_in_zip_read_info->read_buffer = NULL;
    if (pfile_in_zip_read_info->stream_initialised)
        inflateEnd(&pfile_in_zip_read_info->stream);

    pfile_in_zip_read_info->stream_initialised = 0;
        if (pfile_in_zip_read_info!=0) zfree(pfile_in_zip_read_info); // unused pfile_in_zip_read_info=0;

    s->pfile_in_zip_read=NULL;

    return err;
}

//  Get the global comment string of the ZipFile, in the szComment buffer.
//  uSizeBuf is the size of the szComment buffer.
//  return the number of byte copied or an error code <0
int unzGetGlobalComment (unzFile file, char *szComment, uLong uSizeBuf)
{ //int err=UNZ_OK;
  unz_s* s;
  uLong uReadThis ;
  if (file==NULL) return UNZ_PARAMERROR;
  s=(unz_s*)file;
  uReadThis = uSizeBuf;
  if (uReadThis>s->gi.size_comment) uReadThis = s->gi.size_comment;
  if (lufseek(s->file,s->central_pos+22,SEEK_SET)!=0) return UNZ_ERRNO;
  if (uReadThis>0)
  { *szComment='\0';
    if (lufread(szComment,(uInt)uReadThis,1,s->file)!=1) return UNZ_ERRNO;
  }
  if ((szComment != NULL) && (uSizeBuf > s->gi.size_comment)) *(szComment+s->gi.size_comment)='\0';
  return (int)uReadThis;
}

int unzOpenCurrentFile (unzFile file, const char *password);
int unzReadCurrentFile (unzFile file, void *buf, unsigned len);
int unzCloseCurrentFile (unzFile file);

FILETIME timet2filetime(const unsigned t)
{
#ifdef __GNUC__
    LONGLONG i = Int32x32To64(t,10000000) + 116444736000000000LL;
#elif __ARMCC_VERSION
    LONGLONG i = Int32x32To64(t,10000000) + 116444736000000000LL;
#else
    LONGLONG i = Int32x32To64(t,10000000) + 116444736000000000i64;
#endif
    FILETIME ft;
    ft.dwLowDateTime = (DWORD) i;
    ft.dwHighDateTime = (DWORD) (i >>32);
    return ft;
}

// modified 2013.12.12
class TUnzip
{
public:
    TUnzip(const char *pwd) : uf(0), currentfile(-1), czei(-1), password(0) {if (pwd!=0) {password=(char*)zmalloc(strlen(pwd)+1); strcpy(password,pwd);}}
    ~TUnzip() {if (password!=0) zfree(password); password=0;}

    unzFile uf; int currentfile; ZIPENTRY cze; int czei;
    char *password;

    ZRESULT Open(void* z, long long len);
    ZRESULT OpenByFD(sint32 fd); //bx20210730
    ZRESULT Get(int index,ZIPENTRY *ze);
    ZRESULT Find(const TCHAR *name,bool ic,int *index,ZIPENTRY *ze);
    ZRESULT Unzip(int index,void *dst,unsigned int len);
    ZRESULT Close();
};

// modified 2013.12.12
ZRESULT TUnzip::Open(void* z, long long len)
{
    if (uf!=0 || currentfile!=-1)
        return ZR_NOTINITED;
    ZRESULT e; LUFILE *f = lufopen(z, len, &e);
    if (f==NULL) return e;
    uf = unzOpenInternal(f);
    if (uf==0) return ZR_NOFILE;
    return ZR_OK;
}

//bx20210730
ZRESULT TUnzip::OpenByFD(sint32 fd)
{
    if (uf!=0 || currentfile!=-1)
        return ZR_NOTINITED;
    ZRESULT e; LUFILE *f = lufopen_fd(fd, &e);
    if (f==NULL) return e;
    uf = unzOpenInternal(f);
    if (uf==0) return ZR_NOFILE;
    return ZR_OK;
}

// modified 2013.12.12
ZRESULT TUnzip::Get(int index,ZIPENTRY *ze)
{
    if (index<-1 || index>=(int)uf->gi.number_entry) return ZR_ARGS;
    if (currentfile!=-1) unzCloseCurrentFile(uf); currentfile=-1;
    if (index==czei && index!=-1) {memcpy(ze,&cze,sizeof(ZIPENTRY)); return ZR_OK;}
    if (index==-1)
    {
        ze->index = uf->gi.number_entry;
        ze->name[0]=0;
        ze->attr_isdir = false;
        ze->attr_archive = false;
        ze->attr_hidden = false;
        ze->attr_readonly = false;
        ze->attr_system = false;
        ze->atime.dwLowDateTime=0; ze->atime.dwHighDateTime=0;
        ze->ctime.dwLowDateTime=0; ze->ctime.dwHighDateTime=0;
        ze->mtime.dwLowDateTime=0; ze->mtime.dwHighDateTime=0;
        ze->comp_size=0;
        ze->unc_size=0;
        return ZR_OK;
    }
    if (index<(int)uf->num_file) unzGoToFirstFile(uf);
    while ((int)uf->num_file<index) unzGoToNextFile(uf);
    unz_file_info ufi; char fn[MAX_PATH];
    unzGetCurrentFileInfo(uf,&ufi,fn,MAX_PATH,NULL,0,NULL,0);
    // now get the extra header. We do this ourselves, instead of
    // calling unzOpenCurrentFile &c., to avoid allocating more than necessary.
    unsigned int extralen,iSizeVar; unsigned long offset;
    int res = unzlocal_CheckCurrentFileCoherencyHeader(uf,&iSizeVar,&offset,&extralen);
    if (res!=UNZ_OK) return ZR_CORRUPT;
    if (lufseek(uf->file,offset,SEEK_SET)!=0) return ZR_READ;
    unsigned char *extra = (unsigned char*) zmalloc(extralen);
    if (lufread(extra,1,(uInt)extralen,uf->file)!=extralen) {zfree(extra); return ZR_READ;}
    //
    ze->index=uf->num_file;
    TCHAR tfn[MAX_PATH];
#ifdef UNICODE
    MultiByteToWideChar(CP_UTF8,0,fn,-1,tfn,MAX_PATH);
#else
    strcpy(tfn,fn);
#endif
    // As a safety feature: if the zip filename had sneaky stuff
    // like "c:\windows\file.txt" or "\windows\file.txt" or "fred\..\..\..\windows\file.txt"
    // then we get rid of them all. That way, when the programmer does UnzipItem(hz,i,ze.name),
    // it won't be a problem. (If the programmer really did want to get the full evil information,
    // then they can edit out this security feature from here).
    // In particular, we chop off any prefixes that are "c:\" or "\" or "/" or "[stuff]\.." or "[stuff]/.."
    const TCHAR *sfn=tfn;
    for (;;)
    {
        if (sfn[0]!=0 && sfn[1]==':') {sfn+=2; continue;}
        if (sfn[0]=='\\') {sfn++; continue;}
        if (sfn[0]=='/') {sfn++; continue;}
        const TCHAR *c;
        c=_tcsstr(sfn,_T("\\..\\")); if (c!=0) {sfn=c+4; continue;}
        c=_tcsstr(sfn,_T("\\../")); if (c!=0) {sfn=c+4; continue;}
        c=_tcsstr(sfn,_T("/../")); if (c!=0) {sfn=c+4; continue;}
        c=_tcsstr(sfn,_T("/..\\")); if (c!=0) {sfn=c+4; continue;}
        break;
    }
    _tcscpy(ze->name, sfn);

    // zip has an 'attribute' 32bit value. Its lower half is windows stuff
    // its upper half is standard unix stat.st_mode. We'll start trying
    // to read it in unix mode
    unsigned long a = ufi.external_fa;
    bool isdir  =   (a&0x40000000)!=0;
    bool readonly=  (a&0x00800000)==0;
    //bool readable=  (a&0x01000000)!=0; // unused
    //bool executable=(a&0x00400000)!=0; // unused
    bool hidden=false, system=false, archive=true;
    // but in normal hostmodes these are overridden by the lower half...
    int host = ufi.version>>8;
    if (host==0 || host==7 || host==11 || host==14)
    {
        readonly=  (a&0x00000001)!=0;
        hidden=    (a&0x00000002)!=0;
        system=    (a&0x00000004)!=0;
        isdir=     (a&0x00000010)!=0;
        archive=   (a&0x00000020)!=0;
    }

    ze->attr_isdir = isdir;
    ze->attr_archive = archive;
    ze->attr_hidden = hidden;
    ze->attr_readonly = readonly;
    ze->attr_system = system;
    ze->comp_size = ufi.compressed_size;
    ze->unc_size = ufi.uncompressed_size;
    //
    ze->atime.dwLowDateTime = 0;
    ze->atime.dwHighDateTime = 0;
    ze->ctime.dwLowDateTime = 0;
    ze->ctime.dwHighDateTime = 0;
    ze->mtime.dwLowDateTime = 0;
    ze->mtime.dwHighDateTime = 0;
    // the zip will always have at least that dostime. But if it also has
    // an extra header, then we'll instead get the info from that.
    unsigned int epos=0;
    while (epos+4<extralen)
    {
        char etype[3]; etype[0]=extra[epos+0]; etype[1]=extra[epos+1]; etype[2]=0;
        int size = extra[epos+2];
        if (strcmp(etype,"UT")!=0) {epos += 4+size; continue;}
        int flags = extra[epos+4];
        bool hasmtime = (flags&1)!=0;
        bool hasatime = (flags&2)!=0;
        bool hasctime = (flags&4)!=0;
        epos+=5;
        if (hasmtime)
        {
            unsigned mtime = ((extra[epos+0])<<0) | ((extra[epos+1])<<8) |((extra[epos+2])<<16) | ((extra[epos+3])<<24);
            epos+=4;
            ze->mtime = timet2filetime(mtime);
        }
        if (hasatime)
        {
            unsigned atime = ((extra[epos+0])<<0) | ((extra[epos+1])<<8) |((extra[epos+2])<<16) | ((extra[epos+3])<<24);
            epos+=4;
            ze->atime = timet2filetime(atime);
        }
        if (hasctime)
        {
            unsigned ctime = ((extra[epos+0])<<0) | ((extra[epos+1])<<8) |((extra[epos+2])<<16) | ((extra[epos+3])<<24);
            epos+=4;
            ze->ctime = timet2filetime(ctime);
        }
        break;
    }
    //
    if (extra!=0) zfree(extra);
    memcpy(&cze,ze,sizeof(ZIPENTRY)); czei=index;
    return ZR_OK;
}

ZRESULT TUnzip::Find(const TCHAR *tname,bool ic,int *index,ZIPENTRY *ze)
{ char name[MAX_PATH];
#ifdef UNICODE
  WideCharToMultiByte(CP_UTF8,0,tname,-1,name,MAX_PATH,0,0);
#else
  strcpy(name,tname);
#endif
  int res = unzLocateFile(uf,name,ic?CASE_INSENSITIVE:CASE_SENSITIVE);
  if (res!=UNZ_OK)
  { if (index!=0) *index=-1;
    if (ze!=NULL) {ZeroMemory(ze,sizeof(ZIPENTRY)); ze->index=-1;}
    return ZR_NOTFOUND;
  }
  if (currentfile!=-1) unzCloseCurrentFile(uf); currentfile=-1;
  int i = (int)uf->num_file;
  if (index!=NULL) *index=i;
  if (ze!=NULL)
  { ZRESULT zres = Get(i,ze);
    if (zres!=ZR_OK) return zres;
  }
  return ZR_OK;
}

// modified 2013.12.12
void EnsureDirectory(const TCHAR *dir)
{
    const TCHAR *lastslash=dir, *c=lastslash;
    while (*c!=0) {if (*c=='/' || *c=='\\') lastslash=c; c++;}
    const TCHAR *name=lastslash;
    if (lastslash!=dir)
    { TCHAR tmp[MAX_PATH]; memcpy(tmp,dir,sizeof(TCHAR)*(lastslash-dir));
    tmp[lastslash-dir]=0;
    EnsureDirectory(tmp);
    name++;
    }
    TCHAR cd[MAX_PATH]; *cd=0; _tcscat(cd,dir);
}

// modified 2013.12.12
ZRESULT TUnzip::Unzip(int index,void *dst,unsigned int len)
{
    if (index!=currentfile)
    {
        if (currentfile!=-1) unzCloseCurrentFile(uf);
        currentfile=-1;
        if (index>=(int)uf->gi.number_entry) return ZR_ARGS;
        if (index<(int)uf->num_file) unzGoToFirstFile(uf);
        while ((int)uf->num_file<index) unzGoToNextFile(uf);
        unzOpenCurrentFile(uf,password); currentfile=index;
    }
    bool reached_eof;
    int res = unzReadCurrentFile(uf,dst,len,&reached_eof);
    if (res<=0) {unzCloseCurrentFile(uf); currentfile=-1;}
    if (reached_eof) return ZR_OK;
    if (res>0) return ZR_MORE;
    if (res==UNZ_PASSWORD) return ZR_PASSWORD;
    return ZR_FLATE;
}

ZRESULT TUnzip::Close()
{ if (currentfile!=-1) unzCloseCurrentFile(uf); currentfile=-1;
  if (uf!=0) unzClose(uf); uf=0;
  return ZR_OK;
}

ZRESULT lasterrorU=ZR_OK;

// modified 2013.12.12
unsigned int FormatZipMessage(ZRESULT code, TCHAR *buf,unsigned int len)
{
    if (code==ZR_RECENT) code=lasterrorU;
    const TCHAR *msg=_T("unknown zip result code");
    switch (code)
    { case ZR_OK: msg=_T("Success"); break;
    case ZR_NODUPH: msg=_T("Culdn't duplicate handle"); break;
    case ZR_NOFILE: msg=_T("Couldn't create/open file"); break;
    case ZR_NOALLOC: msg=_T("Failed to allocate memory"); break;
    case ZR_WRITE: msg=_T("Error writing to file"); break;
    case ZR_NOTFOUND: msg=_T("File not found in the zipfile"); break;
    case ZR_MORE: msg=_T("Still more data to unzip"); break;
    case ZR_CORRUPT: msg=_T("Zipfile is corrupt or not a zipfile"); break;
    case ZR_READ: msg=_T("Error reading file"); break;
    case ZR_PASSWORD: msg=_T("Correct password required"); break;
    case ZR_ARGS: msg=_T("Caller: faulty arguments"); break;
    case ZR_PARTIALUNZ: msg=_T("Caller: the file had already been partially unzipped"); break;
    case ZR_NOTMMAP: msg=_T("Caller: can only get memory of a memory zipfile"); break;
    case ZR_MEMSIZE: msg=_T("Caller: not enough space allocated for memory zipfile"); break;
    case ZR_FAILED: msg=_T("Caller: there was a previous error"); break;
    case ZR_ENDED: msg=_T("Caller: additions to the zip have already been ended"); break;
    case ZR_ZMODE: msg=_T("Caller: mixing creation and opening of zip"); break;
    case ZR_NOTINITED: msg=_T("Zip-bug: internal initialisation not completed"); break;
    case ZR_SEEK: msg=_T("Zip-bug: trying to seek the unseekable"); break;
    case ZR_MISSIZE: msg=_T("Zip-bug: the anticipated size turned out wrong"); break;
    case ZR_NOCHANGE: msg=_T("Zip-bug: tried to change mind, but not allowed"); break;
    case ZR_FLATE: msg=_T("Zip-bug: an internal error during flation"); break;
    }
    unsigned int mlen=(unsigned int)_tcslen(msg);
    if (buf==0 || len==0) return mlen;
    unsigned int n=mlen; if (n+1>len) n=len-1;
    memcpy(buf,msg,n); buf[n]=0;
    return mlen;
}

typedef struct
{
    DWORD flag;
    TUnzip *unz;
} TUnzipHandleData;

HZIP OpenZip(void* z, long long len, const char* password)
{
    TUnzip *unz = new TUnzip(password);
    lasterrorU = unz->Open(z, len);
    if (lasterrorU!=ZR_OK) {delete unz; return 0;}
    TUnzipHandleData *han = new TUnzipHandleData;
    han->flag=1; han->unz=unz; return (HZIP)han;
}

HZIP OpenZipByFD(sint32 fd, const char* password)
{
    TUnzip *unz = new TUnzip(password);
    lasterrorU = unz->OpenByFD(fd);
    if (lasterrorU!=ZR_OK) {delete unz; return 0;}
    TUnzipHandleData *han = new TUnzipHandleData;
    han->flag=1; han->unz=unz; return (HZIP)han;
}

ZRESULT GetZipItem(HZIP hz, int index, ZIPENTRY *ze)
{ ze->index=0; *ze->name=0; ze->unc_size=0;
  if (hz==0) {lasterrorU=ZR_ARGS;return ZR_ARGS;}
  TUnzipHandleData *han = (TUnzipHandleData*)hz;
  if (han->flag!=1) {lasterrorU=ZR_ZMODE;return ZR_ZMODE;}
  TUnzip *unz = han->unz;
  lasterrorU = unz->Get(index,ze);
  return lasterrorU;
}

ZRESULT FindZipItem(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRY *ze)
{ if (hz==0) {lasterrorU=ZR_ARGS;return ZR_ARGS;}
  TUnzipHandleData *han = (TUnzipHandleData*)hz;
  if (han->flag!=1) {lasterrorU=ZR_ZMODE;return ZR_ZMODE;}
  TUnzip *unz = han->unz;
  lasterrorU = unz->Find(name,ic,index,ze);
  return lasterrorU;
}

ZRESULT UnzipItemInternal(HZIP hz, int index, void *dst, unsigned int len)
{ if (hz==0) {lasterrorU=ZR_ARGS;return ZR_ARGS;}
  TUnzipHandleData *han = (TUnzipHandleData*)hz;
  if (han->flag!=1) {lasterrorU=ZR_ZMODE;return ZR_ZMODE;}
  TUnzip *unz = han->unz;
  lasterrorU = unz->Unzip(index,dst,len);
  return lasterrorU;
}

ZRESULT UnzipItem(HZIP hz, int index, void *z,unsigned int len) {return UnzipItemInternal(hz,index,z,len);}

// modified 2013.12.12
ZRESULT CloseZip(HZIP hz)
{
    if (hz==0) {lasterrorU=ZR_ARGS;return ZR_ARGS;}
    TUnzipHandleData *han = (TUnzipHandleData*)hz;
    if (han->flag!=1) {lasterrorU=ZR_ZMODE;return ZR_ZMODE;}
    TUnzip *unz = han->unz;
    lasterrorU = unz->Close();
    delete unz;
    delete han;
    return lasterrorU;
}

// modified 2013.12.12
bool IsZipHandle(HZIP hz)
{
    if (hz==0) return false;
    TUnzipHandleData *han = (TUnzipHandleData*)hz;
    return (han->flag==1);
}

#endif
