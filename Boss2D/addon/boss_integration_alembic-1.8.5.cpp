#include <boss.h>
#if BOSS_NEED_ADDON_ABC

#include "boss_integration_alembic-1.8.5.h"

#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Abc/ArchiveInfo.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Abc/ErrorHandler.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Abc/IArchive.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Abc/IArrayProperty.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Abc/ICompoundProperty.cpp>
#define g_ohd g_ohd_IObject_BOSS
    #include <addon/alembic-1.8.5_for_boss/lib/Alembic/Abc/IObject.cpp>
#undef g_ohd
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Abc/ISampleSelector.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Abc/IScalarProperty.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Abc/OArchive.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Abc/OArrayProperty.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Abc/OCompoundProperty.cpp>
#define g_ohd g_ohd_OObject_BOSS
    #include <addon/alembic-1.8.5_for_boss/lib/Alembic/Abc/OObject.cpp>
#undef g_ohd
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Abc/OScalarProperty.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Abc/Reference.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Abc/SourceName.cpp>

#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCollection/ICollections.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCollection/OCollections.cpp>

#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/ArchiveReader.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/ArchiveWriter.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/ArrayPropertyReader.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/ArrayPropertyWriter.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/ArraySample.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/BasePropertyReader.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/BasePropertyWriter.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/CompoundPropertyReader.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/CompoundPropertyWriter.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/Foundation.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/ObjectReader.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/ObjectWriter.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/ReadArraySampleCache.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/ScalarPropertyReader.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/ScalarPropertyWriter.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/ScalarSample.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/TimeSampling.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreAbstract/TimeSamplingType.cpp>

#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreFactory/IFactory.cpp>

//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/AprImpl.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/ApwImpl.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/ArImpl.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/AwImpl.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/CacheImpl.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/CprData.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/CprImpl.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/CpwData.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/CpwImpl.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/DataTypeRegistry.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/HDF5Hierarchy.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/HDF5HierarchyReader.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/HDF5HierarchyWriter.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/HDF5Util.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/OrData.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/OrImpl.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/OwData.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/OwImpl.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/ReadUtil.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/ReadWrite.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/SprImpl.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/SpwImpl.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/StringReadUtil.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/StringWriteUtil.cpp>
//#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreHDF5/WriteUtil.cpp>

#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreLayer/ArImpl.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreLayer/CprImpl.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreLayer/OrImpl.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreLayer/Read.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreLayer/Util.cpp>

#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/AprImpl.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/ApwImpl.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/ArImpl.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/AwImpl.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/CprData.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/CprImpl.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/CpwData.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/CpwImpl.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/MetaDataMap.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/OrData.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/OrImpl.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/OwData.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/OwImpl.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/ReadUtil.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/ReadWrite.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/SprImpl.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/SpwImpl.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/StreamManager.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/WriteUtil.cpp>

#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/ArchiveBounds.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/Basis.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/CameraSample.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/FilmBackXformOp.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/GeometryScope.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/ICamera.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/ICurves.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/IFaceSet.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/ILight.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/INuPatch.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/IPoints.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/IPolyMesh.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/ISubD.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/IXform.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/OCamera.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/OCurves.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/OFaceSet.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/OLight.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/ONuPatch.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/OPoints.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/OPolyMesh.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/OSubD.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/OXform.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/Visibility.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/XformOp.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcGeom/XformSample.cpp>

#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcMaterial/IMaterial.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcMaterial/InternalUtil.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcMaterial/MaterialAssignment.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcMaterial/MaterialFlatten.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcMaterial/OMaterial.cpp>

#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Ogawa/IArchive.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Ogawa/IData.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Ogawa/IGroup.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Ogawa/IStreams.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Ogawa/OArchive.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Ogawa/OData.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Ogawa/OGroup.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Ogawa/OStream.cpp>

#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Util/Murmur3.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Util/Naming.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Util/SpookyV2.cpp>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Util/TokenMap.cpp>

#endif
