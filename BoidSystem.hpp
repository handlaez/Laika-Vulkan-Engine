#ifndef BOIDSYSTEM_CLASS_H
#define BOIDSYSTEM_CLASS_H

#include <vector>
#include "Boid.hpp"

namespace le {

    class BoidSystem
    {
    public:
        void AddBoid(const Boid& boid);
        void Update(float deltaTime);
        void KeepInBounds(Boid& b, float deltaTime);

        std::vector<Boid>& GetBoids();

    private:
        std::vector<Boid> boids;

        glm::vec3 Limit(const glm::vec3& v, float max);

        glm::vec3 Separation(Boid& boid);
        glm::vec3 Alignment(Boid& boid);
        glm::vec3 Cohesion(Boid& boid);

        float separationRadius = 15.0f;
        float alignmentRadius = 20.0f;
        float cohesionRadius = 16.0f;
    };
}

#endif