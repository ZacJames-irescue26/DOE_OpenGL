#pragma once
#include "Platform\OpenGL\GLTexture2D.h"
#include "Platform\OpenGL\GLTexture.h"
#include "Platform\OpenGL\GLTexture2DArray.h"
#include "SurfelCluster.h"
#include "Platform\OpenGL\GLBufferTexture.h"


class SurfelGenerator;

class SurfelData {
private:
	friend SurfelGenerator;

	std::vector<Surfel> mSurfels;
	std::vector<SurfelCluster> mSurfelClusters;

	std::shared_ptr<GLFloatTexture2DArray<GLTexture::Float::RGB32F>> mSurfelsGBuffer;
	std::shared_ptr<GLIntegerTexture2D<GLTexture::Integer::R32UI>> mSurfelClustersGBuffer;
	std::shared_ptr<GLFloatBufferTexture<GLTexture::Float::RGB32F, glm::vec3>> mSurfelClusterCentersBufferTexture;

public:
	void initializeBuffers();

	void serialize(const std::string& filePath);

	bool deserialize(const std::string& filePath);

	const std::vector<Surfel>& surfels() const;

	const std::vector<SurfelCluster>& surfelClusters() const;

	std::shared_ptr<GLFloatTexture2DArray<GLTexture::Float::RGB32F>> surfelsGBuffer() const;

	std::shared_ptr<GLIntegerTexture2D<GLTexture::Integer::R32UI>> surfelClustersGBuffer() const;

	std::shared_ptr<GLFloatBufferTexture<GLTexture::Float::RGB32F, glm::vec3>> surfelClusterCentersBufferTexture() const;
};