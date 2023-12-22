#pragma once
#include "Engine/config.h"
namespace Sampling {

	enum class Filter : uint16_t {
		None = 1 << 0, Bilinear = 1 << 1, Trilinear = 1 << 2, Anisotropic = 1 << 3
	};

	enum class WrapMode : uint16_t {
		Repeat = 1 << 4, ClampToEdge = 1 << 5, ClampToBorder = 1 << 6
	};

	// https://www.khronos.org/opengl/wiki/Sampler_Object#Comparison_mode
	enum class ComparisonMode : uint16_t {
		None = 1 << 7, ReferenceToTexture = 1 << 8
	};

};