#include "MemoryUtils.h"

#include <cstdio>
#include <cstdint>

namespace Utils {
	namespace Memory {

		size_t Padding(size_t objectSize, size_t alignment) {
			uint16_t fullAlignments = objectSize / alignment;
			size_t tail = objectSize - alignment * fullAlignments;
			size_t padding = tail > 0 ? alignment - tail : 0;
			return padding;
		}

	}
}