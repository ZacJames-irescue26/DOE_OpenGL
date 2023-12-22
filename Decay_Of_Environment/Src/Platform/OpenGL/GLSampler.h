#pragma once
#include "GLNamedObject.h"
#include "Sampling.h"

class GLSampler : public GLNamedObject {
public:
	GLSampler(Sampling::Filter filter, Sampling::WrapMode wrapMode, Sampling::ComparisonMode comparisonMode);

	~GLSampler();

	void setFilter(Sampling::Filter filter);

	void setWrapMode(Sampling::WrapMode wrapMode);

	void setComparisonMode(Sampling::ComparisonMode comparisonMode);
};