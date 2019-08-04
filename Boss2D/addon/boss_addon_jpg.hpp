#pragma once
#include <boss.h>

extern "C"
{
    #define JPEG_INTERNALS
    #include <addon/opencv-3.1.0_for_boss/3rdparty/libjpeg/jinclude.h>
    #include <addon/opencv-3.1.0_for_boss/3rdparty/libjpeg/jpeglib.h>
}

#include <boss.hpp>

class JPEGEncoder {
public:
    typedef void (*EncodeCB)(bytes buffer, int length, payload data);

public:
    JPEGEncoder();
    ~JPEGEncoder();
    void encode(bytes buffer, int length, EncodeCB cb, payload cbdata);
    void emptyOutput();
    void end();

    int getWidth() const {
        return enc.image_width;
    }

    void setWidth(int w) {
        enc.image_width = w;
    }

    int getHeight() const {
        return enc.image_height;
    }

    void setHeight(int h) {
        enc.image_height = h;
    }

    int getQuality() const {
        return quality;
    }

    void setQuality(int q) {
        quality = q;
    }

    chars getColorSpace() const {
        switch (enc.in_color_space) {
        case JCS_RGB:
            return "rgb";

        case JCS_GRAYSCALE:
            return "gray";

        case JCS_CMYK:
            return "cmyk";

        default:
            return "rgb";
        }
    }

    void setColorSpace(chars cs) {
        if (boss_strcmp(cs, "gray") == 0) {
            enc.input_components = 1;
            enc.in_color_space = JCS_GRAYSCALE;
        }
        else if (boss_strcmp(cs, "cmyk") == 0) {
            enc.input_components = 4;
            enc.in_color_space = JCS_CMYK;
        }
        else {
            enc.input_components = 3;
            enc.in_color_space = JCS_RGB;
        }

        scanlineLength = enc.image_width * enc.input_components;
    }

    void error(char *message);

private:
    struct jpeg_compress_struct enc;
    struct jpeg_error_mgr err;

    uint08* output;
    EncodeCB callback;
    payload callbackdata;
    int quality;
    int scanlineLength;
};

class JPEGDecoder {
public:
    JPEGDecoder();
    ~JPEGDecoder();
    void setDesiredSize(int dw, int dh) {
        desiredWidth = dw;
        desiredHeight = dh;
    }
    void skipBytes(long numBytes);
    bool decode(bytes buffer, int length);

    int getWidth() const {
        return outputWidth;
    }

    int getHeight() const {
        return outputHeight;
    }

    int getOrientation() const {
        return orientationCode;
    }

    chars getColorSpace() const {
        switch (dec.out_color_space) {
        case JCS_RGB:
            return "rgb";

        case JCS_GRAYSCALE:
            return "gray";

        case JCS_CMYK:
            return "cmyk";

        default:
            return "rgb";
        }
    }

    void error(char *message);

public:
    bool readHeader();
    bool startDecompress();
    bool decompress(unsigned int* dest);
    bool endDecompress();

private:
    void findExif();

    jpeg_error_mgr err;
    jpeg_decompress_struct dec;
    int imageWidth, imageHeight;
    int desiredWidth, desiredHeight;
    int outputWidth, outputHeight;
    int orientationCode; // added by BOSS : 회전방향
    uint08* output;
};
