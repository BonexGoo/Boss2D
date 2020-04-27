#include <boss.h>

#if !defined(BOSS_NEED_ADDON_JPG) || (BOSS_NEED_ADDON_JPG != 0 && BOSS_NEED_ADDON_JPG != 1)
    #error BOSS_NEED_ADDON_JPG macro is invalid use
#endif
bool __LINK_ADDON_JPG__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_JPG

#include "boss_addon_jpg.hpp"

#include <boss.hpp>
#include <format/boss_bmp.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(Jpg, Create, id_jpg, id_bitmap_read, sint32)
    BOSS_DECLARE_ADDON_FUNCTION(Jpg, Release, void, id_jpg)
    BOSS_DECLARE_ADDON_FUNCTION(Jpg, GetLength, sint32, id_jpg)
    BOSS_DECLARE_ADDON_FUNCTION(Jpg, GetBits, bytes, id_jpg)
    BOSS_DECLARE_ADDON_FUNCTION(Jpg, ToBmp, id_bitmap, bytes, sint32)

    static autorun Bind_AddOn_Jpg()
    {
        Core_AddOn_Jpg_Create() = Customized_AddOn_Jpg_Create;
        Core_AddOn_Jpg_Release() = Customized_AddOn_Jpg_Release;
        Core_AddOn_Jpg_GetLength() = Customized_AddOn_Jpg_GetLength;
        Core_AddOn_Jpg_GetBits() = Customized_AddOn_Jpg_GetBits;
        Core_AddOn_Jpg_ToBmp() = Customized_AddOn_Jpg_ToBmp;
        return true;
    }
    static autorun _ = Bind_AddOn_Jpg();
}

// 구현부
namespace BOSS
{
    id_jpg Customized_AddOn_Jpg_Create(id_bitmap_read bmp, sint32 quality)
    {
        static uint08s JpgBuffer;
        JpgBuffer.SubtractionAll();

        const sint32 BmpWidth = Bmp::GetWidth(bmp);
        const sint32 BmpHeight = Bmp::GetHeight(bmp);
        const sint32 BmpBitCount = Bmp::GetBitCount(bmp);

        JPEGEncoder Encoder;
        Encoder.setWidth(BmpWidth);
        Encoder.setHeight(BmpHeight);
        Encoder.setQuality(quality);
        Encoder.setColorSpace("rgb");

        if(BmpBitCount == 32)
        {
            const sint32 DstScanlineSize = 3 * BmpWidth;
            uint08s DstBits;
            uint08* Dst = DstBits.AtDumpingAdded(DstScanlineSize * BmpHeight);
            uint08* CurDst = Dst;
            Bmp::bitmappixel* Src = (Bmp::bitmappixel*) Bmp::GetBits(bmp);
            for(sint32 y = 0; y < BmpHeight; ++y)
            {
                Bmp::bitmappixel* CurSrc = &Src[BmpWidth * (BmpHeight - 1 - y)];
                for(sint32 x = 0; x < BmpWidth; ++x)
                {
                    *(CurDst++) = CurSrc->r;
                    *(CurDst++) = CurSrc->g;
                    *(CurDst++) = CurSrc->b;
                    CurSrc++;
                }
            }

            Encoder.encode(Dst, DstScanlineSize * BmpHeight,
                [](bytes buffer, int length, payload data)->void
                {
                    uint08s& JpgBuffer = *((uint08s*) data);
                    Memory::Copy(JpgBuffer.AtDumpingAdded(length), buffer, length);
                }, &JpgBuffer);
        }
        else return nullptr;
        Encoder.end();
        if(JpgBuffer.Count() == 0)
            return nullptr;

        buffer NewJpg = Buffer::Alloc(BOSS_DBG JpgBuffer.Count());
        Memory::Copy(NewJpg, &JpgBuffer[0], JpgBuffer.Count());
        return (id_jpg) NewJpg;
    }

    void Customized_AddOn_Jpg_Release(id_jpg jpg)
    {
        Buffer::Free((buffer) jpg);
    }

    sint32 Customized_AddOn_Jpg_GetLength(id_jpg jpg)
    {
        if(!jpg) return 0;
        return Buffer::CountOf((buffer) jpg);
    }

    bytes Customized_AddOn_Jpg_GetBits(id_jpg jpg)
    {
        if(!jpg) return nullptr;
        return (bytes)(buffer) jpg;
    }

    id_bitmap Customized_AddOn_Jpg_ToBmp(bytes jpg, sint32 length)
    {
        JPEGDecoder Decoder;
        Decoder.decode(jpg, length);

        if(!Decoder.readHeader())
        {
            BOSS_ASSERT("jpeg디코딩에(readHeader) 실패하였습니다", false);
            return nullptr;
        }

        if(!Decoder.startDecompress())
        {
            BOSS_ASSERT("jpeg디코딩에(startDecompress) 실패하였습니다", false);
            return nullptr;
        }

        const int Width = Decoder.getWidth();
        const int Height = Decoder.getHeight();
        id_bitmap NewBitmap = Bmp::Create(4, Width, Height);
        auto BitmapFocus = (Bmp::bitmappixel*) (Bmp::GetBits(NewBitmap) + sizeof(Bmp::bitmappixel) * Width * (Height - 1));

        if(!Decoder.decompress((unsigned int*) BitmapFocus))
        {
            BOSS_ASSERT("jpeg디코딩에(decompress) 실패하였습니다", false);
            Memory::Set(Bmp::GetBits(NewBitmap), 0xFF, sizeof(Bmp::bitmappixel) * Width * Height);
            return NewBitmap;
        }

        if(!Decoder.endDecompress())
        {
            BOSS_ASSERT("jpeg디코딩에(endDecompress) 실패하였습니다", false);
            Memory::Set(Bmp::GetBits(NewBitmap), 0xFF, sizeof(Bmp::bitmappixel) * Width * Height);
            return NewBitmap;
        }

        // added by BOSS : 회전방향을 적용
        const int Orientation = Decoder.getOrientation();
        if(1 < Orientation)
        {
            const int Width2 = (Orientation < 5)? Width : Height;
            const int Height2 = (Orientation < 5)? Height : Width;
            id_bitmap NewBitmap2 = Bmp::Create(4, Width2, Height2);
            auto SrcFocus = (Bmp::bitmappixel*) Bmp::GetBits(NewBitmap);
            auto DstFocus = (Bmp::bitmappixel*) Bmp::GetBits(NewBitmap2);
            switch(Orientation)
            {
            case 2:
                for(int y = 0; y < Height; ++y)
                {
                    int pos = (Width2 - 1) + y * Width2;
                    for(int x = 0; x < Width; ++x)
                    {
                        DstFocus[pos] = *(SrcFocus++);
                        pos -= 1;
                    }
                }
                break;
            case 3:
                for(int y = 0; y < Height; ++y)
                {
                    int pos = (Width2 - 1) + y * Width2;
                    for(int x = 0; x < Width; ++x)
                    {
                        DstFocus[pos] = *(SrcFocus++);
                        pos -= 1;
                    }
                }
                break;
            case 4:
                for(int y = 0; y < Height; ++y)
                {
                    int pos = 0 + (Height2 - y - 1) * Width2;
                    Memory::Copy(&DstFocus[pos], SrcFocus, sizeof(Bmp::bitmappixel) * Width);
                    SrcFocus += Width;
                }
                break;
            case 5:
                for(int y = 0; y < Height; ++y)
                {
                    int pos = (Width2 - y - 1) + (Height2 - 1) * Width2;
                    for(int x = 0; x < Width; ++x)
                    {
                        DstFocus[pos] = *(SrcFocus++);
                        pos -= Width2;
                    }
                }
                break;
            case 6:
                for(int y = 0; y < Height; ++y)
                {
                    int pos = y + (Height2 - 1) * Width2;
                    for(int x = 0; x < Width; ++x)
                    {
                        DstFocus[pos] = *(SrcFocus++);
                        pos -= Width2;
                    }
                }
                break;
            case 7:
                for(int y = 0; y < Height; ++y)
                {
                    int pos = y + 0;
                    for(int x = 0; x < Width; ++x)
                    {
                        DstFocus[pos] = *(SrcFocus++);
                        pos += Width2;
                    }
                }
                break;
            case 8:
                for(int y = 0; y < Height; ++y)
                {
                    int pos = (Width2 - y - 1) + 0;
                    for(int x = 0; x < Width; ++x)
                    {
                        DstFocus[pos] = *(SrcFocus++);
                        pos += Width2;
                    }
                }
                break;
            }
            Bmp::Remove(NewBitmap);
            NewBitmap = NewBitmap2;
        }
        return NewBitmap;
    }
}

////////////////////////////////////////////////////////////////////////////////
// JPEGEncoder, JPEGDecoder
////////////////////////////////////////////////////////////////////////////////

static void error_exit(j_common_ptr cinfo)
{
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message) (cinfo, buffer);

    JPEGDecoder *decoder = static_cast<JPEGDecoder *>(cinfo->client_data);
    decoder->error(buffer);
}

////////////////////////////////////////////////////////////////////////////////
// JPEGEncoder
////////////////////////////////////////////////////////////////////////////////
#define BUF_SIZE 8192

static void initDestination(jpeg_compress_struct* enc) {}
static void termDestination(jpeg_compress_struct* enc) {}
static boolean emptyOutputBuffer(jpeg_compress_struct* enc)
{
    JPEGEncoder *encoder = static_cast<JPEGEncoder*>(enc->client_data);
    encoder->emptyOutput();
    return TRUE;
}

JPEGEncoder::JPEGEncoder()
{
    enc.err = jpeg_std_error(&err);
    err.error_exit = error_exit;
    jpeg_create_compress(&enc);

    enc.image_width = 0;
    enc.image_height = 0;
    enc.input_components = 3;
    enc.in_color_space = JCS_RGB;

    // set up the destination manager
    jpeg_destination_mgr* dest = (jpeg_destination_mgr*) Memory::Alloc(sizeof(jpeg_destination_mgr));
    Memory::Set(dest, 0x00, sizeof(jpeg_destination_mgr));
    enc.dest = dest;
    enc.client_data = this;

    dest->init_destination = initDestination;
    dest->empty_output_buffer = emptyOutputBuffer;
    dest->term_destination = termDestination;

    output = (uint08*)Memory::Alloc(BUF_SIZE);
    dest->next_output_byte = output;
    dest->free_in_buffer = BUF_SIZE;
    callback = nullptr;
    callbackdata = nullptr;

    quality = 100;
    scanlineLength = 0;
}

JPEGEncoder::~JPEGEncoder()
{
    Memory::Free(enc.dest);
    jpeg_destroy_compress(&enc);
    Memory::Free(output);
}

void JPEGEncoder::encode(bytes buffer, int length, EncodeCB cb, payload cbdata)
{
    callback = cb;
    callbackdata = cbdata;

    jpeg_set_defaults(&enc);
    jpeg_set_quality(&enc, quality, TRUE);
    jpeg_start_compress(&enc, TRUE);

    while (length >= scanlineLength) {
        jpeg_write_scanlines(&enc, (JSAMPARRAY) &buffer, 1);
        buffer += scanlineLength;
        length -= scanlineLength;
    }
}

void JPEGEncoder::emptyOutput()
{
    callback(output, BUF_SIZE, callbackdata);
    enc.dest->next_output_byte = output;
    enc.dest->free_in_buffer = BUF_SIZE;
}

void JPEGEncoder::end()
{
    jpeg_finish_compress(&enc);
    size_t remaining = BUF_SIZE - enc.dest->free_in_buffer;
    if(0 < remaining)
        callback(output, remaining, callbackdata);
}

void JPEGEncoder::error(char *message)
{
    BOSS_ASSERT(String::Format("jpeg인코딩에 실패하였습니다(message:%s)", message), false);
}

////////////////////////////////////////////////////////////////////////////////
// JPEGDecoder
////////////////////////////////////////////////////////////////////////////////
#define EXIF_MARKER (JPEG_APP0 + 1)

static void init_source(j_decompress_ptr) {}
static void term_source(j_decompress_ptr jd) {}
static void skip_input_data(j_decompress_ptr jd, long numBytes)
{
    JPEGDecoder *decoder = static_cast<JPEGDecoder *>(jd->client_data);
    decoder->skipBytes(numBytes);
}
static boolean fill_input_buffer(j_decompress_ptr)
{
    return FALSE;
}

JPEGDecoder::JPEGDecoder()
{
    dec.err = jpeg_std_error(&err);
    err.error_exit = error_exit;
    jpeg_create_decompress(&dec);

    jpeg_source_mgr* src = (jpeg_source_mgr*) Memory::Alloc(sizeof(jpeg_source_mgr));
    Memory::Set(src, 0x00, sizeof(jpeg_source_mgr));
    dec.src = src;
    dec.client_data = this;

    src->init_source = init_source;
    src->fill_input_buffer = fill_input_buffer;
    src->skip_input_data = skip_input_data;
    src->resync_to_restart = jpeg_resync_to_restart;
    src->term_source = term_source;

    // Keep APP1 blocks, for obtaining exif data.
    jpeg_save_markers(&dec, EXIF_MARKER, 0xFFFF);

    imageWidth = 0;
    imageHeight = 0;
    desiredWidth = 0;
    desiredHeight = 0;
    outputWidth = 0;
    outputHeight = 0;
    orientationCode = 0;
    output = nullptr;
}

JPEGDecoder::~JPEGDecoder()
{
    Memory::Free(dec.src);
    jpeg_destroy_decompress(&dec);
    Memory::Free(output);
}

void JPEGDecoder::skipBytes(long numBytes)
{
    long skip = (numBytes < (long)dec.src->bytes_in_buffer) ?
        numBytes : (long)dec.src->bytes_in_buffer;
    dec.src->bytes_in_buffer -= (size_t)skip;
    dec.src->next_input_byte += skip;
}

bool JPEGDecoder::decode(bytes buffer, int length)
{
    dec.src->bytes_in_buffer += length;
    dec.src->next_input_byte = buffer;
    return true;
}

bool JPEGDecoder::readHeader()
{
    if (jpeg_read_header(&dec, TRUE) == JPEG_SUSPENDED)
        return false; // I/O suspension.

    imageWidth = dec.image_width;
    imageHeight = dec.image_height;

    // added by BOSS : 회전방향을 알기 위한 코드추가
    if(dec.marker_list && !boss_strcmp((chars) dec.marker_list->data, "Exif"))
        orientationCode = ((bytes) dec.marker_list->data)[48];
    return true;
}

bool JPEGDecoder::startDecompress()
{
    switch (dec.jpeg_color_space) {
    case JCS_YCbCr:
    case JCS_RGB:
        dec.out_color_space = JCS_RGB;
        break;

    case JCS_GRAYSCALE:
        dec.out_color_space = JCS_GRAYSCALE;
        break;

    case JCS_CMYK:
    case JCS_YCCK:
        dec.out_color_space = JCS_CMYK;
        break;

    default:
        return false;
    }

    // Calculate scale so we only decode what we need
    if (desiredWidth && desiredHeight) {
        int wdeg = imageWidth / desiredWidth;
        int hdeg = imageHeight / desiredHeight;
        dec.scale_num = 1;
        dec.scale_denom = Math::Max(1, Math::Min(Math::Min(wdeg, hdeg), 8));
    }

    jpeg_calc_output_dimensions(&dec);
    outputWidth = dec.output_width;
    outputHeight = dec.output_height;

    findExif();

    if (!jpeg_start_decompress(&dec))
        return false; // I/O suspension.

        // Allocate output buffer for a single scanline
    output = (uint08*)Memory::Alloc(dec.output_width * dec.output_components);
    return true;
}

bool JPEGDecoder::decompress(unsigned int* dest)
{
    while (dec.output_scanline < dec.output_height) {
        if (jpeg_read_scanlines(&dec, &output, 1) != 1)
            return false;

        bytes CurOutput = output;
        Bmp::bitmappixel CurPixel;
        switch (dec.out_color_space)
        {
        case JCS_RGB:
            BOSS_ASSERT(String::Format("JCS_RGB이면서 components는 %d인 예외상황입니다", dec.output_components),
                dec.output_components == 3);
            if (dec.output_components == 3)
                for (int i = 0; i < outputWidth; ++i)
                {
                    CurPixel.a = 0xFF;
                    CurPixel.r = *(CurOutput++);
                    CurPixel.g = *(CurOutput++);
                    CurPixel.b = *(CurOutput++);
                    *(dest++) = *((unsigned int*)&CurPixel);
                }
            break;
        case JCS_GRAYSCALE:
            BOSS_ASSERT(String::Format("JCS_GRAYSCALE이면서 components는 %d인 예외상황입니다", dec.output_components),
                dec.output_components == 1);
            if (dec.output_components == 1)
                for (int i = 0; i < outputWidth; ++i)
                {
                    CurPixel.a = 0xFF;
                    CurPixel.r = CurPixel.g = CurPixel.b = *(CurOutput++);
                    *(dest++) = *((unsigned int*)&CurPixel);
                }
            break;
        case JCS_CMYK:
            BOSS_ASSERT(String::Format("JCS_CMYK이면서 components는 %d인 예외상황입니다", dec.output_components),
                dec.output_components == 4);
            if (dec.output_components == 4)
                for (int i = 0; i < outputWidth; ++i)
                {
                    const int C = *(CurOutput++);
                    const int M = *(CurOutput++);
                    const int Y = *(CurOutput++);
                    const int K = *(CurOutput++);
                    CurPixel.a = 0xFF;
                    CurPixel.r = (uint08)((255 - C) * (255 - K) / 255);
                    CurPixel.g = (uint08)((255 - M) * (255 - K) / 255);
                    CurPixel.b = (uint08)((255 - Y) * (255 - K) / 255);
                    *(dest++) = *((unsigned int*)&CurPixel);
                }
            break;
        }
        dest -= outputWidth * 2;
    }
    return true;
}

bool JPEGDecoder::endDecompress()
{
    return jpeg_finish_decompress(&dec);
}

void JPEGDecoder::findExif()
{
    for (jpeg_saved_marker_ptr marker = dec.marker_list; marker; marker = marker->next) {
        if (marker->marker == EXIF_MARKER && marker->data_length >= 14 && !Memory::Compare(marker->data, "Exif", 5)) {
        }
    }
}

void JPEGDecoder::error(char *message)
{
    BOSS_ASSERT(String::Format("jpeg디코딩에 실패하였습니다(message:%s)", message), false);
}

#endif
