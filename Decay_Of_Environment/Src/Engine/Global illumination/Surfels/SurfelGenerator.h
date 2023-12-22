#pragma once

#include "Engine/config.h"

#include "Engine/Global illumination/Math/Triangle3D.h"
#include "Engine/Global illumination/Math/Triangle2D.h"
#include "Engine/Global illumination/LogarithmicBin/LogarithmicBin.h"
#include "Surfel.h"
#include "../SpatialHash/SpatialHash.h"
#include "SurfelData.h"
#include "../Geometry/Mesh.h"
#include "../../Assimp/Model.h"

class SurfelGenerator
{
	struct TransformedTriangleData
	{
		Triangle3D positions;
		Triangle3D normals;
		Triangle2D UVs;

		TransformedTriangleData(const Triangle3D& positions, const Triangle3D& normals, const Triangle2D& UVs);

		std::array<TransformedTriangleData, 4> split() const;
	};

	struct SurfelCandidate
	{
		using BinIterator = LogarithmicBin<TransformedTriangleData>::Iterator;

		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 barycentricCoordinate;
		BinIterator logarithmicBinIterator;

		SurfelCandidate(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& barycentric, BinIterator iterator);

	};

	float mSurfelSpacing;
	size_t mMaximumSurfelClusterSize = 256;

	std::mt19937 mEngine;
	std::uniform_real_distribution<float> mDistribution;
	PackedLookupTable<Surfel> mSurfelFlatStorage;
	SpatialHash<Surfel> mSurfelSpatialHash;
	std::unique_ptr<SurfelData> mSurfelDataContainer;
	/*const SharedResourceStorage* mResourcePool = nullptr;
	const Scene* mScene = nullptr;*/

    /**
     Calculates an optimal minimum area of subdivided triangles based on the minimum desired distance between surfels

     @return A minimum triangle area which shouldn't be exceeded when splitting triangle into 4 smaller children
     */
    float optimalMinimumSubdivisionArea() const;

    /**
     Calculates an optimal space division resolution to be used in a spatial hash data structure

     @param surfelCountPerCellDimension desired surfel density per one cell of the spatial hash
     @param workingVolume extents of a spatial hash structure
     @return space division resolution to passed to the spatial hash
     */
    uint32_t spaceDivisionResolution(float surfelCountPerCellDimension, const AxisAlignedBox3D& workingVolume) const;

    /**
     Generates 3 random numbers between 0 and 1

     @return Normalized triple of random numbers
     */
    glm::vec3 randomBarycentricCoordinates();

    /**
     Creates LogarithmicBin data structure and fills it with sub mesh's transformed triangle data

     @param subMesh Sub mesh object holding geometry data
     @param containingInstance Mesh instance that applies transformation and materials to underlying sub mesh
     @return A logarithmic bin containing sub mesh's triangle data (positions, normals, albedo values and texture coordinates)
     */
    LogarithmicBin<TransformedTriangleData> constructSubMeshVertexDataBin(Mesh& subMesh, const std::shared_ptr<Entity>  entity);

    /**
     Checks to see whether triangle is completely covered by any surfel from existing surfel set

     @param triangle Test subject
     @return Bool value indicating whether triangle is covered
     */
    bool triangleCompletelyCovered(Triangle3D& triangle);

    /**
     Checks to see whether surfel candidate is far enough from all the already generated surfels

     @param candidate Test subject
     @return Bool value indicating whether surfel candidate is far enough to be accepted as a full-fledged surfel
     */
    bool surfelCandidateMeetsMinimumDistanceRequirement(SurfelCandidate& candidate);

    /**
     Generates a surfel candidate with minimum amount of data required to perform routines deciding
     whether this candidate is worthy to be added to a full-fledged surfel set

     @param subMesh A sub mesh on which candidate is generated
     @param transformedVerticesBin Bin that holds all transformed triangle data of the sub mesh
     @return A surfel candidate ready to participate in validity tests
     */
    SurfelCandidate generateSurfelCandidate(const Mesh& subMesh, LogarithmicBin<TransformedTriangleData>& transformedVerticesBin);

    /**
     Computes all necessary data for a surfel candidate (normal, albedo, uv and an area) to transform it into a full-fledged surfel

     @param surfelCandidate Candidate to be transformed
     @param transformedVerticesBin Bin that holds all transformed triangle data of the sub mesh on which candidate was generated on
     @return Surfel ready to be added to a scene and participate in rendering
     */
    template<class TextureFormat, TextureFormat Format>
    Surfel generateSurfel(SurfelCandidate& surfelCandidate,
        LogarithmicBin<TransformedTriangleData>& transformedVerticesBin,
        const GLTexture2DSampler<TextureFormat, Format>& albedoMapSampler);

    /**
     Generates surfels for a single mesh instance

     @param instance An instance on which surfels will be generated on
     */
    void generateSurflesOnMeshInstance(const Model& instance);

    /**
     Determines resemblance of two surfels to decide whether thay belong to the same cluster

     @param first surfel one
     @param second surfel two
     @param workingVolumeMaximumExtent2 squared largest length of scene's light baking volume
     @return returns a true if surfels are alike and belong to the same surfel cluster
     */
    bool surfelsAlike(const Surfel& first, const Surfel& second, float workingVolumeMaximumExtent2);

    /**
     Gathers similar surfels into clusters
     */
    void formClusters();

public:
    SurfelGenerator(/*const SharedResourceStorage* resourcePool, const Scene* scene*/);

    std::unique_ptr<SurfelData> generateStaticGeometrySurfels();
};

};

