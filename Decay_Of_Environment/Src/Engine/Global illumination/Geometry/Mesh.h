#pragma once
#include "Engine/Global illumination/Math/AxisAlignedBox3D.h"
#include "Engine/Global illumination/PackedLookupTable/PackedLookupTable.h"


//class Mesh {
//private:
//	std::string mName;
//	Transformation mBaseTransform;
//	AxisAlignedBox3D mBoundingBox;
//	PackedLookupTable<SubMesh> mSubMeshes;
//
//public:
//	Mesh(const std::string& filePath);
//
//	void swap(Mesh&);
//
//	const std::string& name() const;
//
//	const AxisAlignedBox3D& boundingBox() const;
//
//	const Transformation& baseTransform() const;
//
//	const PackedLookupTable<SubMesh>& subMeshes() const;
//
//	PackedLookupTable<SubMesh>& subMeshes();
//
//	void setName(const std::string& name);
//
//	void setTransformID(ID transformID);
//
//	void setBoundingBox(const AxisAlignedBox3D& box);
//};
//
//void swap(Mesh&, Mesh&);