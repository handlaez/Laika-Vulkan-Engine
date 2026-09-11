#ifndef I_TERRAIN_GENERATOR_HPP
#define I_TERRAIN_GENERATOR_HPP

#include "le_chunk.hpp"

class ITerrainGenerator {
public:
    virtual ~ITerrainGenerator() = default;

    virtual void generateChunk(const le::LeChunk& chunk, VkCommandBuffer cmd, const float seed, int indexBuffer) = 0;
};

#endif
