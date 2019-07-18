#ifndef DATA_GENERATOR_H__
#define DATA_GENERATOR_H__

#include BOSS_OPENH264_U_typedefs_h //original-code:"typedefs.h"

bool YUVPixelDataGenerator (uint8_t* pPointer, int32_t iWidth, int32_t iHeight, int32_t iStride);

void RandomPixelDataGenerator (uint8_t* pPointer, int32_t iWidth, int32_t iHeight, int32_t iStride);

void RandomResidueDataGenerator (uint16_t* pPointer, int32_t iWidth, int32_t iHeight, int32_t iStride);

void RandomCoeffDataGenerator (uint16_t* pPointer, int32_t iWidth, int32_t iHeight, int32_t iStride);


#endif//DATA_GENERATOR_H__
