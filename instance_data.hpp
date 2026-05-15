#ifndef INSTANCE_DATA_HPP
#define INSTANCE_DATA_HPP

#include <glm/glm.hpp>

struct alignas(16) InstanceData {
    alignas(16) glm::vec4 position;
    alignas(16) glm::vec4 forward;
};

#endif