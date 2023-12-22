#include "LogUtils.h"

#include <cstdio>

namespace Utils {
	namespace Log {

		void Vector(const glm::vec2& v, const std::string& prefix) {
			printf("%s x %f y %f\n", prefix.c_str(), v.x, v.y);
		}

		void Vector(const glm::vec3& v, const std::string& prefix) {
			printf("%s x %f y %f z %f\n", prefix.c_str(), v.x, v.y, v.z);
		}

		void Vector(const glm::vec4& v, const std::string& prefix) {
			printf("%s x %f y %f z %f w %f\n", prefix.c_str(), v.x, v.y, v.z, v.w);
		}

		void Vector(const AxisAlignedBox3D& b, const std::string& prefix) {
			printf("%s\n", prefix.c_str());
			Log::Vector(b.min, "Min: ");
			Log::Vector(b.max, "Max: ");
		}

		void GLError() {
			printf("GL Error: %d \n", glGetError());
		}

	}
}