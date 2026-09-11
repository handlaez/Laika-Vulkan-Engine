#ifndef INSTANCE_DATA_HPP
#define INSTANCE_DATA_HPP

#include <glm/glm.hpp>

struct alignas(16) InstanceData {
    glm::mat4 model;
};

#endif