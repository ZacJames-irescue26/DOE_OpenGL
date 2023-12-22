#pragma once

#include "Engine/Global illumination/Math/AxisAlignedBox3D.h"


namespace Utils {
	namespace Log {

		void Vector(const glm::vec2& v, const std::string& prefix = "");

		void Vector(const glm::vec3& v, const std::string& prefix = "");

		void Vector(const glm::vec4& v, const std::string& prefix = "");

		void Vector(const AxisAlignedBox3D& b, const std::string& prefix = "");

		void GLError();

	}
}