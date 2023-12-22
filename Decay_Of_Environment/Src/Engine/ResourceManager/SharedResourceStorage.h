#pragma once
#include "Engine/config.h"
#include "Engine/Global illumination/PackedLookupTable/PackedLookupTable.h"
#include "Mesh.h"
#include "SubMesh.h"
#include "CookTorranceMaterial.h"
#include "EmissiveMaterial.h"
#include "GLVertexArray.h"
#include "MaterialType.h"
#include "GLUniformBuffer.h"

class SharedResourceStorage {
private:
	int32_t mTotalVertexCount = 0;

	std::unique_ptr<GLVertexArray<Vertex1P1N2UV1T1BT>> mVAO;
	std::unique_ptr<GLUniformBuffer> mUniformBuffer;

	PackedLookupTable<Mesh> mMeshes;
	PackedLookupTable<CookTorranceMaterial> mCookTorranceMaterials;
	PackedLookupTable<EmissiveMaterial> mEmissiveMaterials;

public:
	SharedResourceStorage();

	const GLVertexArray<Vertex1P1N2UV1T1BT>* meshVAO() const;

	int32_t totalVertexCount() const;

	ID addMesh(Mesh&& mesh);

	MaterialReference addMaterial(CookTorranceMaterial&& material);

	MaterialReference addMaterial(EmissiveMaterial&& material);

	const Mesh& mesh(ID meshID) const;

	const CookTorranceMaterial& cookTorranceMaterial(ID materialID) const;

	const EmissiveMaterial& emissiveMaterial(ID materialID) const;

	CookTorranceMaterial& cookTorranceMaterial(ID materialID);

	EmissiveMaterial& emissiveMaterial(ID materialID);

	template <typename F>
	void iterateMeshes(F f) const {
		std::for_each(std::begin(mMeshes), std::end(mMeshes), f);
	}

	template <typename F>
	void iterateMaterials(F f) const {
		//            std::for_each(std::begin(mMeshes), std::end(mMeshes), f);
	}
};