#pragma once
#include "Engine/config.h"

struct Transformation {
	glm::vec3 scale;
	glm::vec3 translation;
	glm::quat rotation;

	Transformation();

	Transformation(const glm::mat4& matrix);

	Transformation(glm::vec3 scale, glm::vec3 translation, glm::quat rotation);

	Transformation combinedWith(const Transformation& other) const;

	glm::mat4 modelMatrix() const;

	glm::mat4 scaleMatrix() const;

	glm::mat4 rotationMatrix() const;

	glm::mat4 translationMatrix() const;

	glm::mat4 normalMatrix() const;

	glm::mat4 inverseScaleMatrix() const;

	glm::mat4 inverseRotationMatrix() const;

	glm::mat4 inverseTranslationMatrix() const;
};
