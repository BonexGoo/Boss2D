#include <boss.h>
#if BOSS_NEED_ADDON_PSD

#include "boss_integration_psd_sdk-commit23.h"

#undef DIFFERENCE

#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdExport.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdPch.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdDecompressRle.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdInterleave.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdLayerCanvasCopy.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdAllocator.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdFile.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdMallocAllocator.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdNativeFile.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdParseColorModeDataSection.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdParseDocument.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdParseImageDataSection.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdParseImageResourcesSection.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdParseLayerMaskSection.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdBlendMode.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdColorMode.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdFixedSizeString.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdSyncFileReader.cpp>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/Psdminiz.c>
#include <addon/psd_sdk-commit23_for_boss/src/Psd/PsdSyncFileWriter.cpp>

#endif
