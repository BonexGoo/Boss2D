#include <boss.h>

#if !defined(BOSS_NEED_ADDON_TIF) || (BOSS_NEED_ADDON_TIF != 0 && BOSS_NEED_ADDON_TIF != 1)
    #error BOSS_NEED_ADDON_TIF macro is invalid use
#endif
bool __LINK_ADDON_TIF__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_TIF

#include <math.h>
#include <float.h>

// 도구준비
#include <addon/boss_fakewin.h>

#include "boss_addon_tif.hpp"

#include <boss.hpp>
#include <format/boss_bmp.hpp>
#include <platform/boss_platform.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(Tif, Create, id_tif, id_bitmap_read)
    BOSS_DECLARE_ADDON_FUNCTION(Tif, Release, void, id_tif)
    BOSS_DECLARE_ADDON_FUNCTION(Tif, ToBmp, id_bitmap, bytes, sint32)

    static autorun Bind_AddOn_Tif()
    {
        Core_AddOn_Tif_Create() = Customized_AddOn_Tif_Create;
        Core_AddOn_Tif_Release() = Customized_AddOn_Tif_Release;
        Core_AddOn_Tif_ToBmp() = Customized_AddOn_Tif_ToBmp;
        return true;
    }
    static autorun _ = Bind_AddOn_Tif();
}

struct TinyTIFFFile;
struct TinyTIFFReaderFile;
static int TinyTIFFWriter_getMaxDescriptionTextSize();
static TinyTIFFFile* TinyTIFFWriter_open(const char* filename, uint16 bitsPerSample, uint32 width, uint32 height);
static void TinyTIFFWriter_writeImage(TinyTIFFFile* tiff, void* data);
static void TinyTIFFWriter_writeImage(TinyTIFFFile* tiff, float* data);
static void TinyTIFFWriter_writeImage(TinyTIFFFile* tiff, double* data);
static void TinyTIFFWriter_close(TinyTIFFFile* tiff, double pixel_width=0, double pixel_height=0, double frametime=0, double deltaz=0);
static void TinyTIFFWriter_close(TinyTIFFFile* tiff, char* imageDescription);
static TinyTIFFReaderFile* TinyTIFFReader_open(const char* filename);
static void TinyTIFFReader_close(TinyTIFFReaderFile* tiff);
static const char* TinyTIFFReader_getLastError(TinyTIFFReaderFile* tiff);
static int TinyTIFFReader_wasError(TinyTIFFReaderFile* tiff);
static int TinyTIFFReader_success(TinyTIFFReaderFile* tiff);
static int TinyTIFFReader_hasNext(TinyTIFFReaderFile* tiff);
static int TinyTIFFReader_readNext(TinyTIFFReaderFile* tiff);
static uint32 TinyTIFFReader_getWidth(TinyTIFFReaderFile* tiff);
static uint32 TinyTIFFReader_getHeight(TinyTIFFReaderFile* tiff);
static String TinyTIFFReader_getImageDescription(TinyTIFFReaderFile* tiff);
#define TINYTIFFREADER_SAMPLEFORMAT_UINT 1
#define TINYTIFFREADER_SAMPLEFORMAT_INT 2
#define TINYTIFFREADER_SAMPLEFORMAT_FLOAT 3
#define TINYTIFFREADER_SAMPLEFORMAT_UNDEFINED 4
static uint16 TinyTIFFReader_getBitsPerSample(TinyTIFFReaderFile* tiff, int sample=0);
static uint16 TinyTIFFReader_getSamplesPerPixel(TinyTIFFReaderFile* tiff);
static int TinyTIFFReader_getSampleData(TinyTIFFReaderFile* tiff, void* buffer, uint16 sample);
static uint32 TinyTIFFReader_countFrames(TinyTIFFReaderFile* tiff);
static uint16 TinyTIFFReader_getSampleFormat(TinyTIFFReaderFile* tiff);

// 구현부
namespace BOSS
{
    chars TempTifName = nullptr;//////////////////////////////////

    id_tif Customized_AddOn_Tif_Create(id_bitmap_read bmp)
    {
        const sint32 Width = Bmp::GetWidth(bmp);
        const sint32 Height = Bmp::GetHeight(bmp);
        auto SrcBits = (const Bmp::bitmappixel*) Bmp::GetBits(bmp);
        uint08* NewBits = new uint08[Width * Height];
        for(sint32 y = 0; y < Height; ++y)
        for(sint32 x = 0; x < Width; ++x)
            NewBits[x + y * Width] = SrcBits[x + (Height - 1 - y) * Width].r;

        TinyTIFFFile* tiff = TinyTIFFWriter_open(TempTifName, 8, Width, Height);
        TinyTIFFWriter_writeImage(tiff, (uint08*) NewBits);
        //TinyTIFFWriter_close(tiff, "THIS IS MY OWN TIFF COMMET");
        TinyTIFFWriter_close(tiff);
        delete[] NewBits;
        return nullptr;
    }

    void Customized_AddOn_Tif_Release(id_tif tif)
    {
        Buffer::Free((buffer) tif);
    }

    id_bitmap Customized_AddOn_Tif_ToBmp(bytes tif, sint32 length)
    {
        return nullptr;
    }
}

// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
#define TIFF_ORDER_UNKNOWN 0
#define TIFF_ORDER_BIGENDIAN 1
#define TIFF_ORDER_LITTLEENDIAN 2

#define TINYTIFFWRITER_DESCRIPTION_SIZE 1024

int TinyTIFFWriter_getMaxDescriptionTextSize() {
    return TINYTIFFWRITER_DESCRIPTION_SIZE;
}

/*! \brief determines the byte order of the system
    \ingroup tinytiffwriter
    \internal

    \return TIFF_ORDER_BIGENDIAN or TIFF_ORDER_LITTLEENDIAN, or TIFF_ORDER_UNKNOWN if the byte order cannot be determined
 */
int TIFF_get_byteorder()
{
        union {
                long l;
                char c[4];
        } test;
        test.l = 1;
        if( test.c[3] && !test.c[2] && !test.c[1] && !test.c[0] )
                return TIFF_ORDER_BIGENDIAN;

        if( !test.c[3] && !test.c[2] && !test.c[1] && test.c[0] )
                return TIFF_ORDER_LITTLEENDIAN;

        return TIFF_ORDER_UNKNOWN;
}


/*! \brief this struct represents a TIFF file
    \ingroup tinytiffwriter
    \internal
 */
struct TinyTIFFFile {
#ifdef __USE_WINAPI_FOR_TIFF__
    /* \brief the windows API file handle */
    HANDLE hFile;
#else
    /* \brief the libc file handle */
    FILE* file;
#endif // __USE_WINAPI_FOR_TIFF__
    /* \brief position of the field in the previously written IFD/header, which points to the next frame. This is set to 0, when closing the file to indicate, the last frame! */
    uint32 lastIFDOffsetField;
    /* \brief file position (from ftell) of the first byte of the previous IFD/frame header */
    long int lastStartPos;
    //uint32 lastIFDEndAdress;
    uint32 lastIFDDATAAdress;
    /* \brief counts the entries in the current IFD/frame header */
    uint16 lastIFDCount;
    /* \brief temporary data array for the current header */
    uint08* lastHeader;
    int lastHeaderSize;
    /* \brief current write position in lastHeader */
    uint32 pos;
    /* \brief width of the frames */
    uint32 width;
    /* \brief height of the frames */
    uint32 height;
    /* \brief bits per sample of the frames */
    uint16 bitspersample;
    uint32 descriptionOffset;
    uint32 descriptionSizeOffset;
    /* \brief counter for the frames, written into the file */
    uint64 frames;
    /* \brief specifies the byte order of the system (and the written file!) */
    uint08 byteorder;
};

/*! \brief wrapper around fopen
    \ingroup tinytiffwriter
    \internal
 */
inline void TinyTIFFWriter_fopen(TinyTIFFFile* tiff, const char* filename) {
#ifdef __USE_WINAPI_FOR_TIFF__
    tiff->hFile = CreateFile(filename,               // name of the write
                       GENERIC_WRITE,          // open for writing
                       0,                      // do not share
                       NULL,                   // default security
                       CREATE_NEW,             // create new file only
                       FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH,  // normal file
                       NULL);                  // no attr. template
#else
    tiff->file=fopen(filename, "wb");
#endif
}

/*! \brief checks whether a file was opened successfully
    \ingroup tinytiffwriter
    \internal
 */
inline int TinyTIFFWriter_fOK(TinyTIFFFile* tiff) {
#ifdef __USE_WINAPI_FOR_TIFF__
   if (tiff->hFile == INVALID_HANDLE_VALUE) return FALSE;
   else return TRUE;
#else
   if (tiff->file) return TRUE;
   else return FALSE;
#endif
}

/*! \brief wrapper around fclose
    \ingroup tinytiffwriter
    \internal
 */
inline int TinyTIFFWriter_fclose(TinyTIFFFile* tiff) {
#ifdef __USE_WINAPI_FOR_TIFF__
    CloseHandle(tiff->hFile);
    return 0;
#else
    int r=fclose(tiff->file);
    tiff->file=NULL;
    return r;
#endif
}

/*! \brief wrapper around fwrite
    \ingroup tinytiffwriter
    \internal
 */
inline size_t TinyTIFFWriter_fwrite(void * ptr, size_t size, size_t count, TinyTIFFFile* tiff) {
#ifdef __USE_WINAPI_FOR_TIFF__
   DWORD dwBytesWritten = 0;
    WriteFile(
                    tiff->hFile,           // open file handle
                    ptr,      // start of data to write
                    size*count,  // number of bytes to write
                    &dwBytesWritten, // number of bytes that were written
                    NULL);
    return dwBytesWritten;
#else
    return fwrite(ptr, size, count, tiff->file);
#endif
}

/*! \brief wrapper around ftell
    \ingroup tinytiffwriter
    \internal
 */
inline long int TinyTIFFWriter_ftell ( TinyTIFFFile * tiff ) {
#ifdef __USE_WINAPI_FOR_TIFF__
DWORD dwPtr = SetFilePointer( tiff->hFile,
                                0,
                                NULL,
                                FILE_CURRENT );
    return dwPtr;
#else
    return ftell(tiff->file);
#endif
}


/*! \brief wrapper around fseek
    \ingroup tinytiffwriter
    \internal
 */
inline int TinyTIFFWriter_fseek_set(TinyTIFFFile* tiff, size_t offset) {
#ifdef __USE_WINAPI_FOR_TIFF__
   DWORD res = SetFilePointer (tiff->hFile,
                                offset,
                                NULL,
                                FILE_BEGIN);


   return res;
#else
    return fseek(tiff->file, offset, SEEK_SET);
#endif // __USE_WINAPI_FOR_TIFF__
}

/*! \brief wrapper around fseek(..., FILE_CURRENT)
    \ingroup tinytiffwriter
    \internal
 */
inline int TinyTIFFWriter_fseek_cur(TinyTIFFFile* tiff, size_t offset) {
#ifdef __USE_WINAPI_FOR_TIFF__
   DWORD res = SetFilePointer (tiff->hFile,
                                offset,
                                NULL,
                                FILE_CURRENT);


   return res;
#else
    return fseek(tiff->file, offset, SEEK_CUR);
#endif // __USE_WINAPI_FOR_TIFF__
}

#define TIFF_FIELD_IMAGEWIDTH 256
#define TIFF_FIELD_IMAGELENGTH 257
#define TIFF_FIELD_BITSPERSAMPLE 258
#define TIFF_FIELD_COMPRESSION 259
#define TIFF_FIELD_PHOTOMETRICINTERPRETATION 262
#define TIFF_FIELD_IMAGEDESCRIPTION 270
#define TIFF_FIELD_STRIPOFFSETS 273
#define TIFF_FIELD_SAMPLESPERPIXEL 277
#define TIFF_FIELD_ROWSPERSTRIP 278
#define TIFF_FIELD_STRIPBYTECOUNTS 279
#define TIFF_FIELD_XRESOLUTION 282
#define TIFF_FIELD_YRESOLUTION 283
#define TIFF_FIELD_PLANARCONFIG 284
#define TIFF_FIELD_RESOLUTIONUNIT 296
#define TIFF_FIELD_SAMPLEFORMAT 339

#define TIFF_TYPE_BYTE 1
#define TIFF_TYPE_ASCII 2
#define TIFF_TYPE_SHORT 3
#define TIFF_TYPE_LONG 4
#define TIFF_TYPE_RATIONAL 5


/*! \brief fixed size of the TIFF frame header in bytes
    \ingroup tinytiffwriter
    \internal
 */
#define TIFF_HEADER_SIZE 510
/*! \brief maximum number of field entries in a TIFF header
    \ingroup tinytiffwriter
    \internal
 */
#define TIFF_HEADER_MAX_ENTRIES 16





/*! \brief write a 4-byte word \a data directly into a file \a fileno
    \ingroup tinytiffwriter
    \internal
 */
#define WRITE32DIRECT(filen, data)  { \
    TinyTIFFWriter_fwrite((void*)(&(data)), 4, 1, filen); \
}

/*! \brief write a data word \a data , which is first cast into a 4-byte word directly into a file \a fileno
    \ingroup tinytiffwriter
    \internal
 */
#define WRITE32DIRECT_CAST(filen, data)  { \
    uint32 d=data; \
    WRITE32DIRECT((filen), d); \
}






/*! \brief write a 2-byte word \a data directly into a file \a fileno
    \ingroup tinytiffwriter
    \internal
 */
#define WRITE16DIRECT(filen, data)    { \
    TinyTIFFWriter_fwrite((void*)(&(data)), 2, 1, filen); \
}

/*! \brief write a data word \a data , which is first cast into a 2-byte word directly into a file \a fileno
    \ingroup tinytiffwriter
    \internal
 */
#define WRITE16DIRECT_CAST(filen, data)    { \
    uint16 d=data; \
    WRITE16DIRECT((filen), d); \
}




/*! \brief write a data word \a data , which is first cast into a 1-byte word directly into a file \a fileno
    \ingroup tinytiffwriter
    \internal
 */
#define WRITE8DIRECT(filen, data) {\
    uint08 ch=data; \
    TinyTIFFWriter_fwrite(&ch, 1, 1, filen);\
}














/*! \brief writes a 32-bit word at the current position into the current file header and advances the position by 4 bytes
    \ingroup tinytiffwriter
    \internal
 */
#define WRITEH32DIRECT_LE(filen, data)  { \
    *((uint32*)(&filen->lastHeader[filen->pos]))=data; \
    filen->pos+=4;\
}
/*! \brief writes a value, which is cast to a 32-bit word at the current position into the current file header and advances the position by 4 bytes
    \ingroup tinytiffwriter
    \internal
 */
#define WRITEH32_LE(filen, data)  { \
    uint32 d=data; \
    WRITEH32DIRECT_LE(filen, d); \
}

// write 2-bytes in big endian
/*! \brief writes a 16-bit word at the current position into the current file header and advances the position by 4 bytes
    \ingroup tinytiffwriter
    \internal
 */
#define WRITEH16DIRECT_LE(filen, data)    { \
    *((uint16*)(&filen->lastHeader[filen->pos]))=data; \
    filen->pos+=2; \
}

/*! \brief writes a value, which is cast to a 16-bit word at the current position into the current file header and advances the position by 4 bytes
    \ingroup tinytiffwriter
    \internal
 */
#define WRITEH16_LE(filen, data)    { \
    uint16 d=data; \
    WRITEH16DIRECT_LE(filen, d); \
}


// write byte
/*! \brief writes an 8-bit word at the current position into the current file header and advances the position by 4 bytes
    \ingroup tinytiffwriter
    \internal
 */
#define WRITEH8(filen, data) { filen->lastHeader[filen->pos]=data; filen->pos+=1; }
/*! \brief writes an 8-bit word at the current position into the current file header and advances the position by 4 bytes
    \ingroup tinytiffwriter
    \internal
 */
#define WRITEH8DIRECT(filen, data) { filen->lastHeader[filen->pos]=data; filen->pos+=1; }

// write 2 bytes
#define WRITEH16(filen, data)  WRITEH16_LE(filen, data)
#define WRITEH32(filen, data)  WRITEH32_LE(filen, data)

#define WRITEH16DIRECT(filen, data)  WRITEH16DIRECT_LE(filen, data)
#define WRITEH32DIRECT(filen, data)  WRITEH32DIRECT_LE(filen, data)

/*! \brief starts a new IFD (TIFF frame header)
    \ingroup tinytiffwriter
    \internal
 */
inline void TinyTIFFWriter_startIFD(TinyTIFFFile* tiff, int hsize=TIFF_HEADER_SIZE) {
    if (!tiff) return;
    tiff->lastStartPos=TinyTIFFWriter_ftell(tiff);//ftell(tiff->file);
    //tiff->lastIFDEndAdress=startPos+2+TIFF_HEADER_SIZE;
    tiff->lastIFDDATAAdress=2+TIFF_HEADER_MAX_ENTRIES*12;
    tiff->lastIFDCount=0;
    if (tiff->lastHeader && hsize!=tiff->lastHeaderSize) {
        free(tiff->lastHeader);
        tiff->lastHeader=NULL;
        tiff->lastHeaderSize=0;
    }
    if (!tiff->lastHeader) {
        tiff->lastHeader=(uint08*)calloc(hsize+2, 1);
        tiff->lastHeaderSize=hsize;
    } else {
        memset(tiff->lastHeader, 0, hsize+2);
    }
    tiff->pos=2;
}

/*! \brief ends the current IFD (TIFF frame header) and writes the header (as a single block of size TIFF_HEADER_SIZE) into the file
    \ingroup tinytiffwriter
    \internal

    This function also sets the pointer to the next IFD, based on the known header size and frame data size.
 */
inline void TinyTIFFWriter_endIFD(TinyTIFFFile* tiff, int hsize=TIFF_HEADER_SIZE) {
    if (!tiff) return;
    //long startPos=ftell(tiff->file);

    tiff->pos=0;
    WRITEH16DIRECT(tiff, tiff->lastIFDCount);

    tiff->pos=2+tiff->lastIFDCount*12; // header start (2byte) + 12 bytes per IFD entry
    WRITEH32(tiff, tiff->lastStartPos+2+hsize+tiff->width*tiff->height*(tiff->bitspersample/8));
    //printf("imagesize = %d\n", tiff->width*tiff->height*(tiff->bitspersample/8));

    //fwrite((void*)tiff->lastHeader, TIFF_HEADER_SIZE+2, 1, tiff->file);
    TinyTIFFWriter_fwrite((void*)tiff->lastHeader, tiff->lastHeaderSize+2, 1, tiff);
    tiff->lastIFDOffsetField=tiff->lastStartPos+2+tiff->lastIFDCount*12;
    //free(tiff->lastHeader);
    //tiff->lastHeader=NULL;
}

/*! \brief write an arbitrary IFD entry
    \ingroup tinytiffwriter
    \internal

    \note This function writes into TinyTIFFFile::lastHeader, starting at the position TinyTIFFFile::pos
 */
inline void TinyTIFFWriter_writeIFDEntry(TinyTIFFFile* tiff, uint16 tag, uint16 type, uint32 count, uint32 data) {
    if (!tiff) return;
    if (tiff->lastIFDCount<TIFF_HEADER_MAX_ENTRIES) {
        tiff->lastIFDCount++;
        WRITEH16DIRECT(tiff, tag);
        WRITEH16DIRECT(tiff, type);
        WRITEH32DIRECT(tiff, count);
        WRITEH32DIRECT(tiff, data);
    }
}

/*! \brief write an 8-bit word IFD entry
    \ingroup tinytiffwriter
    \internal

    \note This function writes into TinyTIFFFile::lastHeader, starting at the position TinyTIFFFile::pos
 */
 inline void TinyTIFFWriter_writeIFDEntryBYTE(TinyTIFFFile* tiff, uint16 tag, uint08 data) {
    if (!tiff) return;
    if (tiff->lastIFDCount<TIFF_HEADER_MAX_ENTRIES) {
        tiff->lastIFDCount++;
        WRITEH16DIRECT(tiff, tag);
        WRITEH16(tiff, TIFF_TYPE_BYTE);
        WRITEH32(tiff, 1);
        WRITEH8DIRECT(tiff, data);
        WRITEH8(tiff, 0);
        WRITEH16(tiff, 0);
    }
}

/*! \brief write an 16-bit word IFD entry
    \ingroup tinytiffwriter
    \internal

    \note This function writes into TinyTIFFFile::lastHeader, starting at the position TinyTIFFFile::pos
 */
 void TinyTIFFWriter_writeIFDEntrySHORT(TinyTIFFFile* tiff, uint16 tag, uint16 data) {
    if (!tiff) return;
    if (tiff->lastIFDCount<TIFF_HEADER_MAX_ENTRIES) {
        tiff->lastIFDCount++;
        WRITEH16DIRECT(tiff, tag);
        WRITEH16(tiff, TIFF_TYPE_SHORT);
        WRITEH32(tiff, 1);
        WRITEH16DIRECT(tiff, data);
        WRITEH16(tiff, 0);
    }
}

/*! \brief write an 32-bit word IFD entry
    \ingroup tinytiffwriter
    \internal

    \note This function writes into TinyTIFFFile::lastHeader, starting at the position TinyTIFFFile::pos
 */
 inline void TinyTIFFWriter_writeIFDEntryLONG(TinyTIFFFile* tiff, uint16 tag, uint32 data) {
    if (!tiff) return;
    if (tiff->lastIFDCount<TIFF_HEADER_MAX_ENTRIES) {
        tiff->lastIFDCount++;
        WRITEH16DIRECT(tiff, tag);
        WRITEH16(tiff, TIFF_TYPE_LONG);
        WRITEH32(tiff, 1);
        WRITEH32DIRECT(tiff, data);
    }
}

/*! \brief write an array of 32-bit words as IFD entry
    \ingroup tinytiffwriter
    \internal

    \note This function writes into TinyTIFFFile::lastHeader, starting at the position TinyTIFFFile::pos
 */
 inline void TinyTIFFWriter_writeIFDEntryLONGARRAY(TinyTIFFFile* tiff, uint16 tag, uint32* data, uint32 N) {
    if (!tiff) return;
    if (tiff->lastIFDCount<TIFF_HEADER_MAX_ENTRIES) {
        tiff->lastIFDCount++;
        WRITEH16DIRECT(tiff, tag);
        WRITEH16(tiff, TIFF_TYPE_LONG);
        WRITEH32(tiff, N);
        if (N==1) {
            WRITEH32DIRECT(tiff, *data);
        } else {
            WRITEH32DIRECT(tiff, tiff->lastIFDDATAAdress+tiff->lastStartPos);
            int pos=tiff->pos;
            tiff->pos=tiff->lastIFDDATAAdress;
            for (uint32 i=0; i<N; i++) {
                WRITEH32DIRECT(tiff, data[i]);
            }
            tiff->lastIFDDATAAdress=tiff->pos;
            tiff->pos=pos;
        }
    }
}

/*! \brief write an array of 16-bit words as IFD entry
    \ingroup tinytiffwriter
    \internal

    \note This function writes into TinyTIFFFile::lastHeader, starting at the position TinyTIFFFile::pos
 */
inline void TinyTIFFWriter_writeIFDEntrySHORTARRAY(TinyTIFFFile* tiff, uint16 tag, uint16* data, uint32 N) {
    if (!tiff) return;
    if (tiff->lastIFDCount<TIFF_HEADER_MAX_ENTRIES) {
        tiff->lastIFDCount++;
        WRITEH16DIRECT(tiff, tag);
        WRITEH16(tiff, TIFF_TYPE_SHORT);
        WRITEH32(tiff, N);
        if (N==1) {
            WRITEH32DIRECT(tiff, *data);
        } else {
            WRITEH32DIRECT(tiff, tiff->lastIFDDATAAdress+tiff->lastStartPos);
            int pos=tiff->pos;
            tiff->pos=tiff->lastIFDDATAAdress;
            for (uint32 i=0; i<N; i++) {
                WRITEH16DIRECT(tiff, data[i]);
            }
            tiff->lastIFDDATAAdress=tiff->pos;
            tiff->pos=pos;
        }


    }
}

/*! \brief write an array of characters (ASCII TEXT) as IFD entry
    \ingroup tinytiffwriter
    \internal

    \note This function writes into TinyTIFFFile::lastHeader, starting at the position TinyTIFFFile::pos
 */
inline void TinyTIFFWriter_writeIFDEntryASCIIARRAY(TinyTIFFFile* tiff, uint16 tag, char* data, uint32 N, int* datapos=NULL, int* sizepos=NULL) {
    if (!tiff) return;
    if (tiff->lastIFDCount<TIFF_HEADER_MAX_ENTRIES) {
        tiff->lastIFDCount++;
        WRITEH16DIRECT(tiff, tag);
        WRITEH16(tiff, TIFF_TYPE_ASCII);
        if (sizepos) *sizepos=tiff->pos;
        WRITEH32(tiff, N);
        if (N<4) {
            if (datapos) *datapos=tiff->pos;
            for (uint32 i=0; i<4; i++) {
                if (i<N) {
                    WRITEH8DIRECT(tiff, data[i]);
                } else {
                    WRITEH8DIRECT(tiff, 0);
                }
            }
        } else {
            WRITEH32DIRECT(tiff, tiff->lastIFDDATAAdress+tiff->lastStartPos);
            int pos=tiff->pos;
            tiff->pos=tiff->lastIFDDATAAdress;
            if (datapos) *datapos=tiff->pos;
            for (uint32 i=0; i<N; i++) {
                WRITEH8DIRECT(tiff, data[i]);
            }
            tiff->lastIFDDATAAdress=tiff->pos;
            tiff->pos=pos;
        }


    }
}

/*! \brief write a rational number as IFD entry
    \ingroup tinytiffwriter
    \internal

    \note This function writes into TinyTIFFFile::lastHeader, starting at the position TinyTIFFFile::pos
 */
inline void TinyTIFFWriter_writeIFDEntryRATIONAL(TinyTIFFFile* tiff, uint16 tag, uint32 numerator, uint32 denominator) {
    if (!tiff) return;
    if (tiff->lastIFDCount<TIFF_HEADER_MAX_ENTRIES) {
        tiff->lastIFDCount++;
        WRITEH16DIRECT(tiff, tag);
        WRITEH16(tiff, TIFF_TYPE_RATIONAL);
        WRITEH32(tiff, 1);
        WRITEH32DIRECT(tiff, tiff->lastIFDDATAAdress+tiff->lastStartPos);
        //printf("1 - %lx\n", tiff->pos);
        int pos=tiff->pos;
        tiff->pos=tiff->lastIFDDATAAdress;
        //printf("2 - %lx\n", tiff->pos);
        WRITEH32DIRECT(tiff, numerator);
        //printf("3 - %lx\n", tiff->pos);
        WRITEH32DIRECT(tiff, denominator);
        tiff->lastIFDDATAAdress=tiff->pos;
        tiff->pos=pos;
        //printf("4 - %lx\n", tiff->pos);
    }
}



TinyTIFFFile* TinyTIFFWriter_open(const char* filename, uint16 bitsPerSample, uint32 width, uint32 height) {
    TinyTIFFFile* tiff=(TinyTIFFFile*)malloc(sizeof(TinyTIFFFile));

    //tiff->file=fopen(filename, "wb");
    TinyTIFFWriter_fopen(tiff, filename);
    tiff->width=width;
    tiff->height=height;
    tiff->bitspersample=bitsPerSample;
    tiff->lastHeader=NULL;
    tiff->lastHeaderSize=0;
    tiff->byteorder=TIFF_get_byteorder();
    tiff->frames=0;

    if (TinyTIFFWriter_fOK(tiff)) {
        if (TIFF_get_byteorder()==TIFF_ORDER_BIGENDIAN) {
            WRITE8DIRECT(tiff, 'M');   // write TIFF header for big-endian
            WRITE8DIRECT(tiff, 'M');
        } else {
            WRITE8DIRECT(tiff, 'I');   // write TIFF header for little-endian
            WRITE8DIRECT(tiff, 'I');
        }
        WRITE16DIRECT_CAST(tiff, 42);
        tiff->lastIFDOffsetField=TinyTIFFWriter_ftell(tiff);//ftell(tiff->file);
        WRITE32DIRECT_CAST(tiff, 8);      // now write offset to first IFD, which is simply 8 here (in little-endian order)
        return tiff;
    } else {
        free(tiff);
        return NULL;
    }
}
#ifdef TINYTIFF_WRITE_COMMENTS
void TinyTIFFWriter_close(TinyTIFFFile* tiff, char* imageDescription) {
#else
void TinyTIFFWriter_close(TinyTIFFFile* tiff, char* /*imageDescription*/) {
#endif
   if (tiff) {
        TinyTIFFWriter_fseek_set(tiff, tiff->lastIFDOffsetField);
        WRITE32DIRECT_CAST(tiff, 0);
#ifdef TINYTIFF_WRITE_COMMENTS
        if (tiff->descriptionOffset>0) {
          size_t dlen;
          char description[TINYTIFFWRITER_DESCRIPTION_SIZE+1];

          if (imageDescription) {
              strcpy(description, imageDescription);
          } else {
              for (int i=0; i<TINYTIFFWRITER_DESCRIPTION_SIZE+1; i++) description[i]='\0';
              sprintf(description, "TinyTIFFWriter_version=1.1\nimages=%ld", tiff->frames);
          }
          description[TINYTIFFWRITER_DESCRIPTION_SIZE-1]='\0';
          dlen=strlen(description);
          printf("WRITING COMMENT\n***");
          printf(description);
          printf("***\nlen=%ld\n\n", dlen);
          TinyTIFFWriter_fseek_set(tiff, tiff->descriptionOffset);
          TinyTIFFWriter_fwrite(description, 1, dlen+1, tiff);//<<" / "<<dlen<<"\n";
          TinyTIFFWriter_fseek_set(tiff, tiff->descriptionSizeOffset);
          WRITE32DIRECT_CAST(tiff, (dlen+1));
        }
#endif // TINYTIFF_WRITE_COMMENTS
        TinyTIFFWriter_fclose(tiff);
        free(tiff->lastHeader);
        free(tiff);
    }
}

void TinyTIFFWriter_close(TinyTIFFFile* tiff, double pixel_width, double pixel_height, double frametime, double deltaz) {
    if (tiff) {
      char description[TINYTIFFWRITER_DESCRIPTION_SIZE+1];
      for (int i=0; i<TINYTIFFWRITER_DESCRIPTION_SIZE+1; i++) description[i]='\0';
      char spw[256];
      sprintf(description, "TinyTIFFWriter_version=1.1\nimages=%lu", (unsigned long int)tiff->frames);
      if (fabs(pixel_width)>10.0*DBL_MIN) {
          sprintf(spw, "\npixel_width=%lf ", pixel_width);
          strcat(description, spw);
      }
      if (fabs(pixel_height)>10.0*DBL_MIN) {
          sprintf(spw, "\npixel_height=%lf ", pixel_height);
          strcat(description,spw);
      }
      if (fabs(deltaz)>10.0*DBL_MIN) {
          sprintf(spw, "\ndeltaz=%lf ", deltaz);
          strcat(description,spw);
      }
      if (fabs(frametime)>10.0*DBL_MIN) {
          sprintf(spw, "\nframetime=%lg ", frametime);
          strcat(description,spw);
      }
      description[TINYTIFFWRITER_DESCRIPTION_SIZE-1]='\0';
      TinyTIFFWriter_close(tiff, description);
    }
}

#define TINTIFFWRITER_WRITEImageDescriptionTemplate(tiff) \
    if (tiff->frames<=0) {\
        int datapos=0;\
        int sizepos=0;\
        char description[TINYTIFFWRITER_DESCRIPTION_SIZE+1];\
        for (int i=0; i<TINYTIFFWRITER_DESCRIPTION_SIZE+1; i++) description[i]='\0';\
        sprintf(description, "TinyTIFFWriter_version=1.1\n");\
        description[TINYTIFFWRITER_DESCRIPTION_SIZE]='\0';\
        TinyTIFFWriter_writeIFDEntryASCIIARRAY(tiff, TIFF_FIELD_IMAGEDESCRIPTION, description, TINYTIFFWRITER_DESCRIPTION_SIZE, &datapos, &sizepos);\
        tiff->descriptionOffset=tiff->lastStartPos+datapos;\
        tiff->descriptionSizeOffset=tiff->lastStartPos+sizepos;\
     }


void TinyTIFFWriter_writeImage(TinyTIFFFile* tiff, void* data) {
     if (!tiff) return;
     long pos=TinyTIFFWriter_ftell(tiff);
     int hsize=TIFF_HEADER_SIZE;
#ifdef TINYTIFF_WRITE_COMMENTS
     if (tiff->frames<=0) {
        hsize=TIFF_HEADER_SIZE+TINYTIFFWRITER_DESCRIPTION_SIZE+16;
      }
#endif // TINYTIFF_WRITE_COMMENTS
    TinyTIFFWriter_startIFD(tiff,hsize);
    TinyTIFFWriter_writeIFDEntryLONG(tiff, TIFF_FIELD_IMAGEWIDTH, tiff->width);
    TinyTIFFWriter_writeIFDEntryLONG(tiff, TIFF_FIELD_IMAGELENGTH, tiff->height);
    TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_BITSPERSAMPLE, tiff->bitspersample);
    TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_COMPRESSION, 1);
    TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_PHOTOMETRICINTERPRETATION, 1);
#ifdef TINYTIFF_WRITE_COMMENTS
    TINTIFFWRITER_WRITEImageDescriptionTemplate(tiff);
#endif // TINYTIFF_WRITE_COMMENTS
    TinyTIFFWriter_writeIFDEntryLONG(tiff, TIFF_FIELD_STRIPOFFSETS, pos+2+hsize);
    TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_SAMPLESPERPIXEL, 1);
    TinyTIFFWriter_writeIFDEntryLONG(tiff, TIFF_FIELD_ROWSPERSTRIP, tiff->height);
    TinyTIFFWriter_writeIFDEntryLONG(tiff, TIFF_FIELD_STRIPBYTECOUNTS, tiff->width*tiff->height*(tiff->bitspersample/8));
    TinyTIFFWriter_writeIFDEntryRATIONAL(tiff, TIFF_FIELD_XRESOLUTION, 1,1);
    TinyTIFFWriter_writeIFDEntryRATIONAL(tiff, TIFF_FIELD_YRESOLUTION, 1,1);
    TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_PLANARCONFIG, 1);
    TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_RESOLUTIONUNIT, 1);
    TinyTIFFWriter_endIFD(tiff);
    TinyTIFFWriter_fwrite(data, tiff->width*tiff->height*(tiff->bitspersample/8), 1, tiff);
    tiff->frames=tiff->frames+1;
}



void TinyTIFFWriter_writeImage(TinyTIFFFile* tiff, float* data) {
     if (!tiff) return;
     long pos=ftell(tiff->file);
     int hsize=TIFF_HEADER_SIZE;
#ifdef TINYTIFF_WRITE_COMMENTS
     if (tiff->frames<=0) {
        hsize=TIFF_HEADER_SIZE+TINYTIFFWRITER_DESCRIPTION_SIZE+16;
      }
#endif // TINYTIFF_WRITE_COMMENTS

     TinyTIFFWriter_startIFD(tiff,hsize);
     TinyTIFFWriter_writeIFDEntryLONG(tiff, TIFF_FIELD_IMAGEWIDTH, tiff->width);
     TinyTIFFWriter_writeIFDEntryLONG(tiff, TIFF_FIELD_IMAGELENGTH, tiff->height);
     TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_BITSPERSAMPLE, tiff->bitspersample);
     TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_COMPRESSION, 1);
     TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_PHOTOMETRICINTERPRETATION, 1);
#ifdef TINYTIFF_WRITE_COMMENTS
    TINTIFFWRITER_WRITEImageDescriptionTemplate(tiff);
#endif // TINYTIFF_WRITE_COMMENTS
     TinyTIFFWriter_writeIFDEntryLONG(tiff, TIFF_FIELD_STRIPOFFSETS, pos+2+hsize);
     TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_SAMPLESPERPIXEL, 1);
     TinyTIFFWriter_writeIFDEntryLONG(tiff, TIFF_FIELD_ROWSPERSTRIP, tiff->height);
     TinyTIFFWriter_writeIFDEntryLONG(tiff, TIFF_FIELD_STRIPBYTECOUNTS, tiff->width*tiff->height*(tiff->bitspersample/8));
     TinyTIFFWriter_writeIFDEntryRATIONAL(tiff, TIFF_FIELD_XRESOLUTION, 1,1);
     TinyTIFFWriter_writeIFDEntryRATIONAL(tiff, TIFF_FIELD_YRESOLUTION, 1,1);
     TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_PLANARCONFIG, 1);
     TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_RESOLUTIONUNIT, 1);
     TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_SAMPLEFORMAT, 3);
     TinyTIFFWriter_endIFD(tiff);
     TinyTIFFWriter_fwrite(data, tiff->width*tiff->height*(tiff->bitspersample/8), 1, tiff);
     tiff->frames=tiff->frames+1;
}







void TinyTIFFWriter_writeImage(TinyTIFFFile* tiff, double* data) {
     if (!tiff) return;
     long pos=ftell(tiff->file);
     int hsize=TIFF_HEADER_SIZE;
#ifdef TINYTIFF_WRITE_COMMENTS
     if (tiff->frames<=0) {
        hsize=TIFF_HEADER_SIZE+TINYTIFFWRITER_DESCRIPTION_SIZE+16;
      }
#endif // TINYTIFF_WRITE_COMMENTS

     TinyTIFFWriter_startIFD(tiff,hsize);
     TinyTIFFWriter_writeIFDEntryLONG(tiff, TIFF_FIELD_IMAGEWIDTH, tiff->width);
     TinyTIFFWriter_writeIFDEntryLONG(tiff, TIFF_FIELD_IMAGELENGTH, tiff->height);
     TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_BITSPERSAMPLE, tiff->bitspersample);
     TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_COMPRESSION, 1);
     TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_PHOTOMETRICINTERPRETATION, 1);
#ifdef TINYTIFF_WRITE_COMMENTS
    TINTIFFWRITER_WRITEImageDescriptionTemplate(tiff);
#endif // TINYTIFF_WRITE_COMMENTS
     TinyTIFFWriter_writeIFDEntryLONG(tiff, TIFF_FIELD_STRIPOFFSETS, pos+2+hsize);
     TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_SAMPLESPERPIXEL, 1);
     TinyTIFFWriter_writeIFDEntryLONG(tiff, TIFF_FIELD_ROWSPERSTRIP, tiff->height);
     TinyTIFFWriter_writeIFDEntryLONG(tiff, TIFF_FIELD_STRIPBYTECOUNTS, tiff->width*tiff->height*(tiff->bitspersample/8));
     TinyTIFFWriter_writeIFDEntryRATIONAL(tiff, TIFF_FIELD_XRESOLUTION, 1,1);
     TinyTIFFWriter_writeIFDEntryRATIONAL(tiff, TIFF_FIELD_YRESOLUTION, 1,1);
     TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_PLANARCONFIG, 1);
     TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_RESOLUTIONUNIT, 1);
     TinyTIFFWriter_writeIFDEntrySHORT(tiff, TIFF_FIELD_SAMPLEFORMAT, 3);
     TinyTIFFWriter_endIFD(tiff);
     TinyTIFFWriter_fwrite(data, tiff->width*tiff->height*(tiff->bitspersample/8), 1, tiff);
     tiff->frames=tiff->frames+1;
}

// ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
#define TinyTIFFReader_POSTYPE fpos_t
#define TIFF_LAST_ERROR_SIZE 1024

#define TIFF_FIELD_IMAGEWIDTH 256
#define TIFF_FIELD_IMAGELENGTH 257
#define TIFF_FIELD_BITSPERSAMPLE 258
#define TIFF_FIELD_COMPRESSION 259
#define TIFF_FIELD_PHOTOMETRICINTERPRETATION 262
#define TIFF_FIELD_IMAGEDESCRIPTION 270
#define TIFF_FIELD_STRIPOFFSETS 273
#define TIFF_FIELD_SAMPLESPERPIXEL 277
#define TIFF_FIELD_ROWSPERSTRIP 278
#define TIFF_FIELD_STRIPBYTECOUNTS 279
#define TIFF_FIELD_XRESOLUTION 282
#define TIFF_FIELD_YRESOLUTION 283
#define TIFF_FIELD_PLANARCONFIG 284
#define TIFF_FIELD_RESOLUTIONUNIT 296
#define TIFF_FIELD_SAMPLEFORMAT 339

#define TIFF_TYPE_BYTE 1
#define TIFF_TYPE_ASCII 2
#define TIFF_TYPE_SHORT 3
#define TIFF_TYPE_LONG 4
#define TIFF_TYPE_RATIONAL 5

#define TIFF_COMPRESSION_NONE 1
#define TIFF_COMPRESSION_CCITT 2
#define TIFF_COMPRESSION_PACKBITS 32773

#define TIFF_PLANARCONFIG_CHUNKY 1
#define TIFF_PLANARCONFIG_PLANAR 2




#define TIFF_HEADER_SIZE 510
#define TIFF_HEADER_MAX_ENTRIES 16


int TIFFReader_get_byteorder() {
    union {
        long l;
        char c[4];
    } test;
    test.l = 1;
    if( test.c[3] && !test.c[2] && !test.c[1] && !test.c[0] )
        return TIFF_ORDER_BIGENDIAN;

    if( !test.c[3] && !test.c[2] && !test.c[1] && test.c[0] )
        return TIFF_ORDER_LITTLEENDIAN;

    return TIFF_ORDER_UNKNOWN;
}

struct TinyTIFFReaderFrame {
    uint32 width;
    uint32 height;
    uint16 compression;

    uint32 rowsperstrip;
    uint32* stripoffsets;
    uint32* stripbytecounts;
    uint32 stripcount;
    uint16 samplesperpixel;
    uint16* bitspersample;
    uint16 planarconfiguration;
    uint16 sampleformat;
    uint32 imagelength;
    
    char* description;
};

inline TinyTIFFReaderFrame TinyTIFFReader_getEmptyFrame() {
    TinyTIFFReaderFrame d;
    d.width=0;
    d.height=0;
    d.stripcount=0;
    d.compression=TIFF_COMPRESSION_NONE;
    d.rowsperstrip=0;
    d.stripoffsets=0;
    d.stripbytecounts=0;
    d.samplesperpixel=1;
    d.bitspersample=0;
    d.planarconfiguration=TIFF_PLANARCONFIG_PLANAR;
    d.sampleformat=TINYTIFFREADER_SAMPLEFORMAT_UINT;
    d.imagelength=0;
    d.description=0;
    return d;
}

inline void TinyTIFFReader_freeEmptyFrame(TinyTIFFReaderFrame f) {
    if (f.stripoffsets) free(f.stripoffsets);
    f.stripoffsets=NULL;
    if (f.stripbytecounts) free(f.stripbytecounts);
    f.stripbytecounts=NULL;
    if (f.bitspersample) free(f.bitspersample);
    f.bitspersample=NULL;
    if (f.description) free(f.description);
    f.description=NULL;
}


struct TinyTIFFReaderFile {
#ifdef __USE_WINAPI_FIR_TIFF__
    HANDLE hFile;
#else
    FILE* file;
#endif // __USE_WINAPI_FIR_TIFF__

    char lastError[TIFF_LAST_ERROR_SIZE];
    int wasError;

    uint08 systembyteorder;
    uint08 filebyteorder;

    uint32 firstrecord_offset;
    uint32 nextifd_offset;

    uint64 filesize;
    
    TinyTIFFReaderFrame currentFrame;
};


void TinyTIFFReader_fopen(TinyTIFFReaderFile* tiff, const char* filename) {
#ifdef __USE_WINAPI_FIR_TIFF__
    tiff->hFile = CreateFile(filename,               // name of the write
                       GENERIC_READ,          // open for writing
                       FILE_SHARE_READ,
                       NULL,                   // default security
                       OPEN_EXISTING,             // create new file only
                       FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,  // normal file
                       NULL);                  // no attr. template
#else
    tiff->file=fopen(filename, "rb");
#endif // __USE_WINAPI_FIR_TIFF__
}

int TinyTIFFReader_fclose(TinyTIFFReaderFile* tiff) {
#ifdef __USE_WINAPI_FIR_TIFF__
    CloseHandle(tiff->hFile);
    return 0;
#else
    int r=fclose(tiff->file);
    tiff->file=NULL;
    return r;
#endif // __USE_WINAPI_FIR_TIFF__
}

int TinyTIFFReader_fOK(const TinyTIFFReaderFile* tiff)  {
#ifdef __USE_WINAPI_FIR_TIFF__
   if (tiff->hFile == INVALID_HANDLE_VALUE) return FALSE;
   else return TRUE;
#else
    if (tiff->file) {
        return TRUE;
    }
    return FALSE;
#endif // __USE_WINAPI_FIR_TIFF__
}

int TinyTIFFReader_fseek_set(TinyTIFFReaderFile* tiff, size_t offset) {
#ifdef __USE_WINAPI_FIR_TIFF__
   DWORD res = SetFilePointer (tiff->hFile,
                                offset,
                                NULL,
                                FILE_BEGIN);


   return res;
#else
    return fseek(tiff->file, offset, SEEK_SET);
#endif // __USE_WINAPI_FIR_TIFF__
}

int TinyTIFFReader_fseek_cur(TinyTIFFReaderFile* tiff, size_t offset) {
#ifdef __USE_WINAPI_FIR_TIFF__
   DWORD res = SetFilePointer (tiff->hFile,
                                offset,
                                NULL,
                                FILE_CURRENT);


   return res;
#else
    return fseek(tiff->file, offset, SEEK_CUR);
#endif // __USE_WINAPI_FIR_TIFF__
}

size_t TinyTIFFReader_fread(void * ptr, size_t size, size_t count, TinyTIFFReaderFile* tiff) {
#ifdef __USE_WINAPI_FIR_TIFF__
    DWORD  dwBytesRead = 0;
    if(!ReadFile(tiff->hFile, ptr, size*count, &dwBytesRead, NULL)) {
        return 0;
    }
    return dwBytesRead;
#else
    return fread(ptr, size, count, tiff->file);
#endif // __USE_WINAPI_FIR_TIFF__
}


inline long int TinyTIFFReader_ftell ( TinyTIFFReaderFile * tiff ) {
#ifdef __USE_WINAPI_FIR_TIFF__
DWORD dwPtr = SetFilePointer( tiff->hFile,
                                0,
                                NULL,
                                FILE_CURRENT );
    return dwPtr;
#else
    return ftell(tiff->file);
#endif
}

int TinyTIFFReader_fgetpos(TinyTIFFReaderFile* tiff, TinyTIFFReader_POSTYPE* pos) {
#ifdef __USE_WINAPI_FIR_TIFF__
    *pos= SetFilePointer( tiff->hFile,
                                0,
                                NULL,
                                FILE_CURRENT );
    return 0;
#else
    return fgetpos(tiff->file, pos);
#endif // __USE_WINAPI_FIR_TIFF__
}

int TinyTIFFReader_fsetpos(TinyTIFFReaderFile* tiff, const TinyTIFFReader_POSTYPE* pos) {
#ifdef __USE_WINAPI_FIR_TIFF__
    SetFilePointer( tiff->hFile,
                                *pos,
                                NULL,
                                FILE_BEGIN );
    return 0;
#else
    return fsetpos(tiff->file, pos);
#endif // __USE_WINAPI_FIR_TIFF__
}




const char* TinyTIFFReader_getLastError(TinyTIFFReaderFile* tiff) {
    if (tiff) return tiff->lastError;
    return NULL;
}

int TinyTIFFReader_wasError(TinyTIFFReaderFile* tiff) {
    if (tiff) return tiff->wasError;
    return TRUE;
}

int TinyTIFFReader_success(TinyTIFFReaderFile* tiff) {
    if (tiff) return !tiff->wasError;
    return FALSE;
}



inline static uint32 TinyTIFFReader_Byteswap32(uint32 nLongNumber)
{
   return (((nLongNumber&0x000000FF)<<24)+((nLongNumber&0x0000FF00)<<8)+
   ((nLongNumber&0x00FF0000)>>8)+((nLongNumber&0xFF000000)>>24));
}

inline static uint16 TinyTIFFReader_Byteswap16(uint16 nValue)
{
   return (((nValue>> 8)) | (nValue << 8));
}

inline uint32 TinyTIFFReader_readuint32(TinyTIFFReaderFile* tiff) {
    uint32 res=0;
    //fread(&res, 4,1,tiff->file);
    TinyTIFFReader_fread(&res, 4,1,tiff);
    if (tiff->systembyteorder!=tiff->filebyteorder) {
        res=TinyTIFFReader_Byteswap32(res);
    }
    return res;
}


inline uint16 TinyTIFFReader_readuint16(TinyTIFFReaderFile* tiff) {
    uint16 res=0;
    //fread(&res, 2,1,tiff->file);
    TinyTIFFReader_fread(&res, 2,1,tiff);
    if (tiff->systembyteorder!=tiff->filebyteorder) {
        res=TinyTIFFReader_Byteswap16(res);
    }
    return res;
}

inline uint08 TinyTIFFReader_readuint8(TinyTIFFReaderFile* tiff) {
    uint16 res=0;
    //fread(&res, 1,1,tiff->file);
    TinyTIFFReader_fread(&res, 1,1,tiff);
    return res;
}


struct TinyTIFFReader_IFD {
    uint16 tag;
    uint16 type;
    uint32 count;
    uint32 value;
    uint32 value2;

    uint32* pvalue;
    uint32* pvalue2;
};

inline void TinyTIFFReader_freeIFD(TinyTIFFReader_IFD d) {
    if (d.pvalue /*&& d.count>1*/) { free(d.pvalue);  d.pvalue=NULL; }
    if (d.pvalue2 /*&& d.count>1*/) { free(d.pvalue2);  d.pvalue2=NULL; }
}

inline TinyTIFFReader_IFD TinyTIFFReader_readIFD(TinyTIFFReaderFile* tiff) {
    TinyTIFFReader_IFD d;

    d.value=0;
    d.value2=0;

    d.pvalue=0;
    d.pvalue2=0;

    d.count=1;
    d.tag=TinyTIFFReader_readuint16(tiff);
    d.type=TinyTIFFReader_readuint16(tiff);
    d.count=TinyTIFFReader_readuint32(tiff);
    //uint32 val=TinyTIFFReader_readuint32(tiff);
    TinyTIFFReader_POSTYPE pos;
    //fgetpos(tiff->file, &pos);
    TinyTIFFReader_fgetpos(tiff, &pos);
    int changedpos=FALSE;
    //printf("    - pos=0x%X   tag=%d type=%d count=%u \n",pos, d.tag, d.type, d.count);
    switch(d.type) {
        case TIFF_TYPE_BYTE:
        case TIFF_TYPE_ASCII:
        if (d.count>0) {
          d.pvalue=(uint32*)calloc(d.count, sizeof(uint32));
            if (d.count<=4) {
            unsigned int i;
            for (i=0; i<4; i++) {
              uint32 v=TinyTIFFReader_readuint8(tiff);
              if (i<d.count) d.pvalue[i]=v;
            }
          } else {
            changedpos=TRUE;
            uint32 offset=TinyTIFFReader_readuint32(tiff);
            
            if (offset+d.count*1<=tiff->filesize) {
              //fseek(tiff->file, offset, SEEK_SET);
              TinyTIFFReader_fseek_set(tiff, offset);
              unsigned int i;
              for (i=0; i<d.count; i++) {
                d.pvalue[i]=TinyTIFFReader_readuint8(tiff);
              }
            }
          }
        }
            d.pvalue2=NULL;
            //printf("    - BYTE/CHAR: tag=%d count=%u   val[0]=%u\n",d.tag,d.count, d.pvalue[0]);
            break;
        case TIFF_TYPE_SHORT:
            d.pvalue=(uint32*)calloc(d.count, sizeof(uint32));
            if (d.count<=2) {
                unsigned int i;
                for (i=0; i<2; i++) {
                    uint32 v=TinyTIFFReader_readuint16(tiff);
                    if (i<d.count) d.pvalue[i]=v;
                }
            } else {
                changedpos=TRUE;
                uint32 offset=TinyTIFFReader_readuint32(tiff);
                if (offset+d.count*2<tiff->filesize) {
                    //fseek(tiff->file, offset, SEEK_SET);
                    TinyTIFFReader_fseek_set(tiff, offset);
                    unsigned int i;
                    for (i=0; i<d.count; i++) {
                        d.pvalue[i]=TinyTIFFReader_readuint16(tiff);
                    }
                }
            }
            d.pvalue2=NULL;
            //printf("    - SHORT: tag=%d count=%u   val[0]=%u\n",d.tag,d.count, d.pvalue[0]);
            break;

        case TIFF_TYPE_LONG:
            d.pvalue=(uint32*)calloc(d.count, sizeof(uint32));
            if (d.count<=1) {
                d.pvalue[0]=TinyTIFFReader_readuint32(tiff);
            } else {
                changedpos=TRUE;
                uint32 offset=TinyTIFFReader_readuint32(tiff);
                if (offset+d.count*4<tiff->filesize) {
                    //fseek(tiff->file, offset, SEEK_SET);
                    TinyTIFFReader_fseek_set(tiff, offset);
                    uint32 i;
                    for (i=0; i<d.count; i++) {
                        d.pvalue[i]=TinyTIFFReader_readuint32(tiff);
                    }
                }
                //printf("    - LONG: pos=0x%X   offset=0x%X   tag=%d count=%u   val[0]=%u\n",pos, offset,d.tag,d.count, d.pvalue[0]);
            }
            d.pvalue2=NULL;
            //printf("    - LONG: tag=%d count=%u   val[0]=%u\n",d.tag,d.count, d.pvalue[0]);
            break;
        case TIFF_TYPE_RATIONAL: {
            d.pvalue=(uint32*)calloc(d.count, sizeof(uint32));
            d.pvalue2=(uint32*)calloc(d.count, sizeof(uint32));

            changedpos=TRUE;
            uint32 offset=TinyTIFFReader_readuint32(tiff);
            if (offset+d.count*4<tiff->filesize) {
                //fseek(tiff->file, offset, SEEK_SET);
                TinyTIFFReader_fseek_set(tiff, offset);
                uint32 i;
                for (i=0; i<d.count; i++) {
                    d.pvalue[i]=TinyTIFFReader_readuint32(tiff);
                    d.pvalue2[i]=TinyTIFFReader_readuint32(tiff);
                }
            }
            //printf("    - RATIONAL: pos=0x%X   offset=0x%X   tag=%d count=%u   val[0]=%u/%u\n",pos, offset,d.tag,d.count, d.pvalue[0], d.pvalue[1]);
            } break;

       default: d.value=TinyTIFFReader_readuint32(tiff); break;
    }
    if (d.pvalue) d.value=d.pvalue[0];
    if (d.pvalue2) d.value2=d.pvalue2[0];

    if (changedpos) {
        //fsetpos(tiff->file, &pos);
        TinyTIFFReader_fsetpos(tiff, &pos);
        //fseek(tiff->file, 4, SEEK_CUR);
        TinyTIFFReader_fseek_cur(tiff, 4);
    }
    return d;
}


inline void TinyTIFFReader_readNextFrame(TinyTIFFReaderFile* tiff) {

    TinyTIFFReader_freeEmptyFrame(tiff->currentFrame);
    tiff->currentFrame=TinyTIFFReader_getEmptyFrame();
    #ifdef DEBUG_IFDTIMING
    HighResTimer timer;
    timer.start();
    #endif
    if (tiff->nextifd_offset!=0 && tiff->nextifd_offset+2<tiff->filesize) {
        //printf("    - seeking=0x%X\n", tiff->nextifd_offset);
        //fseek(tiff->file, tiff->nextifd_offset, SEEK_SET);
        TinyTIFFReader_fseek_set(tiff, tiff->nextifd_offset);
        uint16 ifd_count=TinyTIFFReader_readuint16(tiff);
        //printf("    - tag_count=%u\n", ifd_count);
        uint16 i;
        for ( i=0; i<ifd_count; i++) {
    #ifdef DEBUG_IFDTIMING
            timer.start();
    #endif
            TinyTIFFReader_IFD ifd=TinyTIFFReader_readIFD(tiff);
    #ifdef DEBUG_IFDTIMING
            //printf("    - readIFD %d (tag: %u, type: %u, count: %u): %lf us\n", i, ifd.tag, ifd.type, ifd.count, timer.get_time());
    #endif
            //printf("    - readIFD %d (tag: %u, type: %u, count: %u)\n", i, ifd.tag, ifd.type, ifd.count);
            switch(ifd.tag) {
                case TIFF_FIELD_IMAGEWIDTH: tiff->currentFrame.width=ifd.value;  break;
                case TIFF_FIELD_IMAGELENGTH: tiff->currentFrame.imagelength=ifd.value;  break;
                case TIFF_FIELD_BITSPERSAMPLE: {
                        tiff->currentFrame.bitspersample=(uint16*)malloc(ifd.count*sizeof(uint16));
                        memcpy(tiff->currentFrame.bitspersample, ifd.pvalue, ifd.count*sizeof(uint16));
                     } break;
                case TIFF_FIELD_COMPRESSION: tiff->currentFrame.compression=ifd.value; break;
                case TIFF_FIELD_STRIPOFFSETS: {
                        tiff->currentFrame.stripcount=ifd.count;
                        tiff->currentFrame.stripoffsets=(uint32*)calloc(ifd.count, sizeof(uint32));
                        memcpy(tiff->currentFrame.stripoffsets, ifd.pvalue, ifd.count*sizeof(uint32));
                    } break;
                case TIFF_FIELD_SAMPLESPERPIXEL: tiff->currentFrame.samplesperpixel=ifd.value; break;
                case TIFF_FIELD_ROWSPERSTRIP: tiff->currentFrame.rowsperstrip=ifd.value; break;
                case TIFF_FIELD_SAMPLEFORMAT: tiff->currentFrame.sampleformat=ifd.value; break;
                case TIFF_FIELD_IMAGEDESCRIPTION: {
                        //printf("TIFF_FIELD_IMAGEDESCRIPTION: (tag: %u, type: %u, count: %u)\n", ifd.tag, ifd.type, ifd.count);
                        if (ifd.count>0) {
                            if (tiff->currentFrame.description) free(tiff->currentFrame.description);
                            tiff->currentFrame.description=(char*)calloc(ifd.count+1, sizeof(char));
                            for (uint32 ji=0; ji<ifd.count+1; ji++) {
                                tiff->currentFrame.description[ji]='\0';
                            }
                            for (uint32 ji=0; ji<ifd.count; ji++) {
                                tiff->currentFrame.description[ji]=(char)ifd.pvalue[ji];
                                //printf(" %d[%d]", int(tiff->currentFrame.description[ji]), int(ifd.pvalue[ji]));
                            }
                            //printf("\n  %s\n", tiff->currentFrame.description);
                        }
                    } break;
                case TIFF_FIELD_STRIPBYTECOUNTS: {
                        tiff->currentFrame.stripcount=ifd.count;
                        tiff->currentFrame.stripbytecounts=(uint32*)calloc(ifd.count, sizeof(uint32));
                        memcpy(tiff->currentFrame.stripbytecounts, ifd.pvalue, ifd.count*sizeof(uint32));
                    } break;
                case TIFF_FIELD_PLANARCONFIG: tiff->currentFrame.planarconfiguration=ifd.value; break;
                default: break;
            }
            TinyTIFFReader_freeIFD(ifd);
            //printf("    - tag=%u\n", ifd.tag);
        }
        tiff->currentFrame.height=tiff->currentFrame.imagelength;
        //printf("      - width=%u\n", tiff->currentFrame.width);
        //printf("      - height=%u\n", tiff->currentFrame.height);
        //fseek(tiff->file, tiff->nextifd_offset+2+12*ifd_count, SEEK_SET);
        TinyTIFFReader_fseek_set(tiff, tiff->nextifd_offset+2+12*ifd_count);
        tiff->nextifd_offset=TinyTIFFReader_readuint32(tiff);
        //printf("      - nextifd_offset=%lu\n", tiff->nextifd_offset);
    } else {
        tiff->wasError=TRUE;
        strcpy(tiff->lastError, "no more images in TIF file\0");
    }
}

int TinyTIFFReader_getSampleData(TinyTIFFReaderFile* tiff, void* buffer, uint16 sample) {
    if (tiff) {
        if (tiff->currentFrame.compression!=TIFF_COMPRESSION_NONE) {
            tiff->wasError=TRUE;
            strcpy(tiff->lastError, "the compression of the file is not supported by this library\0");
            return FALSE;
        }
        if (tiff->currentFrame.samplesperpixel>1 && tiff->currentFrame.planarconfiguration!=TIFF_PLANARCONFIG_PLANAR) {
            tiff->wasError=TRUE;
            strcpy(tiff->lastError, "only planar TIFF files are supported by this library\0");
            return FALSE;
        }
        if (tiff->currentFrame.width==0 || tiff->currentFrame.height==0 ) {
            tiff->wasError=TRUE;
            strcpy(tiff->lastError, "the current frame does not contain images\0");
            return FALSE;
        }
        if (tiff->currentFrame.bitspersample[sample]!=8 && tiff->currentFrame.bitspersample[sample]!=16 && tiff->currentFrame.bitspersample[sample]!=32) {
            tiff->wasError=TRUE;
            strcpy(tiff->lastError, "this library only support 8,16 and 32 bits per sample\0");
            return FALSE;
        }
        TinyTIFFReader_POSTYPE pos;
        //fgetpos(tiff->file, &pos);
        TinyTIFFReader_fgetpos(tiff, &pos);
        tiff->wasError=FALSE;

        //printf("    - stripcount=%u\n", tiff->currentFrame.stripcount);
        if (tiff->currentFrame.stripcount>0 && tiff->currentFrame.stripbytecounts && tiff->currentFrame.stripoffsets) {
            uint32 s;
            //printf("    - bitspersample[sample]=%u\n", tiff->currentFrame.bitspersample[sample]);
            if (tiff->currentFrame.bitspersample[sample]==8) {
                for (s=0; s<tiff->currentFrame.stripcount; s++) {
                    //printf("      - s=%u: stripoffset=0x%X stripbytecounts=%u\n", s, tiff->currentFrame.stripoffsets[s], tiff->currentFrame.stripbytecounts[s]);
                    uint08* tmp=(uint08*)calloc(tiff->currentFrame.stripbytecounts[s], sizeof(uint08));
                    //fseek(tiff->file, tiff->currentFrame.stripoffsets[s], SEEK_SET);
                    TinyTIFFReader_fseek_set(tiff, tiff->currentFrame.stripoffsets[s]);
                    //fread(tmp, tiff->currentFrame.stripbytecounts[s], 1, tiff->file);
                    TinyTIFFReader_fread(tmp, tiff->currentFrame.stripbytecounts[s], 1, tiff);
                    uint32 offset=s*tiff->currentFrame.rowsperstrip*tiff->currentFrame.width;
                    //printf("          bufferoffset=%u\n", offset);
                    memcpy(&(((uint08*)buffer)[offset]), tmp, tiff->currentFrame.stripbytecounts[s]);
                    free(tmp);
                }
            } else if (tiff->currentFrame.bitspersample[sample]==16) {
                for (s=0; s<tiff->currentFrame.stripcount; s++) {
                    //printf("      - s=%u: stripoffset=0x%X stripbytecounts=%u\n", s, tiff->currentFrame.stripoffsets[s], tiff->currentFrame.stripbytecounts[s]);
                    uint16* tmp=(uint16*)calloc(tiff->currentFrame.stripbytecounts[s], sizeof(uint08));
                    //fseek(tiff->file, tiff->currentFrame.stripoffsets[s], SEEK_SET);
                    TinyTIFFReader_fseek_set(tiff, tiff->currentFrame.stripoffsets[s]);
                    //fread(tmp, tiff->currentFrame.stripbytecounts[s], 1, tiff->file);
                    TinyTIFFReader_fread(tmp, tiff->currentFrame.stripbytecounts[s], 1, tiff);
                    uint32 offset=s*tiff->currentFrame.rowsperstrip*tiff->currentFrame.width;
                    //memcpy(&(((uint08*)buffer)[offset*2]), tmp, tiff->currentFrame.stripbytecounts[s]);
                    uint32 pixels=tiff->currentFrame.rowsperstrip*tiff->currentFrame.width;
                    uint32 imagesize=tiff->currentFrame.width*tiff->currentFrame.height;
                    if (offset+pixels>imagesize) pixels=imagesize-offset;
                    uint32 x;
                    if (tiff->systembyteorder==tiff->filebyteorder) {
                        memcpy(&(((uint16*)buffer)[offset]), tmp, tiff->currentFrame.stripbytecounts[s]);
                    } else {
                        for (x=0; x<pixels; x++) {
                            ((uint16*)buffer)[offset+x]=TinyTIFFReader_Byteswap16(tmp[x]);
                        }
                    }
                    free(tmp);
                }
            } else if (tiff->currentFrame.bitspersample[sample]==32) {
                for (s=0; s<tiff->currentFrame.stripcount; s++) {
                    //printf("      - s=%u: stripoffset=0x%X stripbytecounts=%u\n", s, tiff->currentFrame.stripoffsets[s], tiff->currentFrame.stripbytecounts[s]);
                    uint32* tmp=(uint32*)calloc(tiff->currentFrame.stripbytecounts[s], sizeof(uint08));
                    //fseek(tiff->file, tiff->currentFrame.stripoffsets[s], SEEK_SET);
                    TinyTIFFReader_fseek_set(tiff, tiff->currentFrame.stripoffsets[s]);
                    //fread(tmp, tiff->currentFrame.stripbytecounts[s], 1, tiff->file);
                    uint32 offset=s*tiff->currentFrame.rowsperstrip*tiff->currentFrame.width;
                    //memcpy(&(((uint08*)buffer)[offset*2]), tmp, tiff->currentFrame.stripbytecounts[s]);
                    uint32 pixels=tiff->currentFrame.rowsperstrip*tiff->currentFrame.width;
                    uint32 imagesize=tiff->currentFrame.width*tiff->currentFrame.height;
                    if (offset+pixels>imagesize) pixels=imagesize-offset;
                    uint32 x;
                    for (x=0; x<pixels; x++) {
                        ((uint32*)buffer)[offset+x]=TinyTIFFReader_readuint32(tiff);
                    }
                    free(tmp);
                }
            }

        } else {
            tiff->wasError=TRUE;
            strcpy(tiff->lastError, "TIFF format not recognized\0");
        }

        //fsetpos(tiff->file, &pos);
        TinyTIFFReader_fsetpos(tiff, &pos);
        return !tiff->wasError;
    }
    tiff->wasError=TRUE;
    strcpy(tiff->lastError, "TIFF file not opened\0");
    return FALSE;
}
















TinyTIFFReaderFile* TinyTIFFReader_open(const char* filename) {
    TinyTIFFReaderFile* tiff=(TinyTIFFReaderFile*)malloc(sizeof(TinyTIFFReaderFile));

    tiff->filesize=0;
    struct stat file;
    if(stat(filename,&file)==0) {
         tiff->filesize=file.st_size;
    }
    tiff->currentFrame=TinyTIFFReader_getEmptyFrame();


    //tiff->file=v(filename, "rb");
    TinyTIFFReader_fopen(tiff, filename);
    tiff->systembyteorder=TIFFReader_get_byteorder();
    memset(tiff->lastError, 0, TIFF_LAST_ERROR_SIZE);
    tiff->wasError=FALSE;
    if (TinyTIFFReader_fOK(tiff) && tiff->filesize>0) {
        uint08 tiffid[3]={0,0,0};
        //fread(tiffid, 1,2,tiff->file);
        TinyTIFFReader_fread(tiffid, 1,2,tiff);

        //printf("      - head=%s\n", tiffid);
        if (tiffid[0]=='I' && tiffid[1]=='I') tiff->filebyteorder=TIFF_ORDER_LITTLEENDIAN;
        else if (tiffid[0]=='M' && tiffid[1]=='M') tiff->filebyteorder=TIFF_ORDER_BIGENDIAN;
        else {
            free(tiff);
            return NULL;
        }
        uint16 magic=TinyTIFFReader_readuint16(tiff);
        //printf("      - magic=%u\n", magic);
        if (magic!=42) {
            free(tiff);
            return NULL;
        }
        tiff->firstrecord_offset=TinyTIFFReader_readuint32(tiff);
        tiff->nextifd_offset=tiff->firstrecord_offset;
        //printf("      - filesize=%u\n", tiff->filesize);
        //printf("      - firstrecord_offset=%4X\n", tiff->firstrecord_offset);
        TinyTIFFReader_readNextFrame(tiff);
    } else {
        free(tiff);
        return NULL;
    }

    return tiff;
}

void TinyTIFFReader_close(TinyTIFFReaderFile* tiff) {
    if (tiff) {
        TinyTIFFReader_freeEmptyFrame(tiff->currentFrame);
        //fclose(tiff->file);
        TinyTIFFReader_fclose(tiff);
        free(tiff);
    }
}

int TinyTIFFReader_hasNext(TinyTIFFReaderFile* tiff) {
    if (tiff) {
        if (tiff->nextifd_offset>0 && tiff->nextifd_offset<tiff->filesize) return TRUE;
        else return FALSE;
    } else {
        return FALSE;
    }
}

int TinyTIFFReader_readNext(TinyTIFFReaderFile* tiff) {
    int hasNext=TinyTIFFReader_hasNext(tiff);
    if (hasNext) {
        TinyTIFFReader_readNextFrame(tiff);
    }
    return hasNext;
}

uint32 TinyTIFFReader_getWidth(TinyTIFFReaderFile* tiff) {
    if (tiff) {
        return tiff->currentFrame.width;
    }
    return 0;
}

uint32 TinyTIFFReader_getHeight(TinyTIFFReaderFile* tiff) {
    if (tiff) {
        return tiff->currentFrame.height;
    }
    return 0;
}

String TinyTIFFReader_getImageDescription(TinyTIFFReaderFile* tiff) {
    if (tiff) {
        if (tiff->currentFrame.description) return String(tiff->currentFrame.description);
    }
    return String();
}

uint16 TinyTIFFReader_getSampleFormat(TinyTIFFReaderFile* tiff) {
    if (tiff) {
        return tiff->currentFrame.sampleformat;
    }
    return 0;
}

uint16 TinyTIFFReader_getBitsPerSample(TinyTIFFReaderFile* tiff, int sample) {
    if (tiff) {
        return tiff->currentFrame.bitspersample[sample];
    }
    return 0;
}

uint16 TinyTIFFReader_getSamplesPerPixel(TinyTIFFReaderFile* tiff) {
    if (tiff) {
        return tiff->currentFrame.samplesperpixel;
    }
    return 0;
}


uint32 TinyTIFFReader_countFrames(TinyTIFFReaderFile* tiff) {

    if (tiff) {
        uint32 frames=0;
        TinyTIFFReader_POSTYPE pos;
        //printf("    -> countFrames: pos before %ld\n", ftell(tiff->file));
        //fgetpos(tiff->file, &pos);
        TinyTIFFReader_fgetpos(tiff, &pos);

        uint32 nextOffset=tiff->firstrecord_offset;
        while (nextOffset>0) {
            //fseek(tiff->file, nextOffset, SEEK_SET);
            TinyTIFFReader_fseek_set(tiff, nextOffset);
            uint16 count=TinyTIFFReader_readuint16(tiff);
            //fseek(tiff->file, count*12, SEEK_CUR);
            TinyTIFFReader_fseek_cur(tiff, count*12);
            nextOffset=TinyTIFFReader_readuint32(tiff);
            frames++;
        }


        //fsetpos(tiff->file, &pos);
        TinyTIFFReader_fsetpos(tiff, &pos);
        //printf("    -> countFrames: pos after %ld\n", ftell(tiff->file));
        return frames;
    }
    return 0;
}

#endif
