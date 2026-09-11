#ifndef BOIDSYSTEM_CLASS_H
#define BOIDSYSTEM_CLASS_H

#include <vector>
#include <glm/glm.hpp>

#include "le_utils.hpp"

struct BoidData {
    // boid data
    std::vector<glm::vec3> position;
    std::vector<glm::vec3> velocity;
    std::vector<glm::vec3> acceleration;

    std::vector<int> boidCell;

    std::vector<float> maxSpeed;
    std::vector<float> maxForce;

    size_t Size() const { return position.size(); }
};

struct GridData {
    // grid data
    float cellSize;
    int gridX, gridY, gridZ;

    std::vector<int> cellCount;
    std::vector<int> cellOffset;
    std::vector<int> sortedIndices;
};

class BoidSystem
{
public:
    void AddBoid(const glm::vec3& pos);
    void Update(float deltaTime);
    void KeepInBounds(const int i);
    glm::vec3 LimitVec(glm::vec3 v, float max);

    const std::vector<glm::vec3>& GetPositions() const { return _data.position; }
    const std::vector<glm::vec3>& GetVelocities() const { return _data.velocity; }

private:
    BoidData _data;
    GridData _grid;

    void BuildGrid();
    void ComputeForces();
    void Integrate(float deltaTime);

    glm::vec3 Limit(const glm::vec3& v, float max);

    // params
    const float xLimit = 150.0f;
    const float yMin = -50.f;
    const float yMax = 50.f;
    const float zLimit = 150.0f;
    // border margin
    const float margin = 30.f;

    // radius
    const float _separationRadius = 4.5f;
    const float _alignmentRadius = 15.0f;
    const float _cohesionRadius = 20.0f;
};

#endif