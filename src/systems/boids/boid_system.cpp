#include "boid_system.hpp"
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

static inline int CellIndex(int x, int y, int z, int gx, int gy)
{
    return x + y * gx + z * gx * gy;
}

void BoidSystem::AddBoid(const glm::vec3& pos)
{
    _data.position.push_back(pos);

    glm::vec3 v = glm::normalize(glm::vec3(Utils::randf(), Utils::randf(), Utils::randf()));
    _data.velocity.push_back(v * 50.0f);
    _data.acceleration.push_back(glm::vec3(0.0f));

    float randFactor = 1.f + Utils::randf();

    _data.maxSpeed.push_back(25.f * randFactor + 40.0f);
    _data.maxForce.push_back(randFactor + 4.f);
}

void BoidSystem::Update(float deltaTime)
{
    BuildGrid(); // O(N)
    ComputeForces(); // ~ O(N) ??
    Integrate(deltaTime); // O(N)
}

void BoidSystem::BuildGrid()
{
    const int N = (int)_data.Size();
    if (N == 0) return;

    // Ensure boidCell is correctly sized
    _data.boidCell.resize(N);

    const float maxRadius = std::max({ _separationRadius, _alignmentRadius, _cohesionRadius });

    // --- validate cell size FIRST ---
    _grid.cellSize = (maxRadius > 0.0f) ? maxRadius : 1.0f;

    _grid.gridX = int((2 * xLimit) / _grid.cellSize) + 1;
    _grid.gridY = int((yMax - yMin) / _grid.cellSize) + 1;
    _grid.gridZ = int((2 * zLimit) / _grid.cellSize) + 1;

    if (_grid.gridX <= 0 || _grid.gridY <= 0 || _grid.gridZ <= 0)
        return;

    const int numCells = _grid.gridX * _grid.gridY * _grid.gridZ;

    _grid.cellCount.assign(numCells, 0);
    _grid.cellOffset.resize(numCells);
    _grid.sortedIndices.resize(N);

    // --- count phase ---
    for (int i = 0; i < N; i++)
    {
        const glm::vec3& p = _data.position[i];

        float fx = (p.x + xLimit) / _grid.cellSize;
        float fy = (p.y - yMin) / _grid.cellSize;
        float fz = (p.z + zLimit) / _grid.cellSize;

        // guard against NaN / Inf
        if (!std::isfinite(fx) || !std::isfinite(fy) || !std::isfinite(fz))
        {
            _data.boidCell[i] = 0;
            continue;
        }

        int gx = glm::clamp((int)fx, 0, _grid.gridX - 1);
        int gy = glm::clamp((int)fy, 0, _grid.gridY - 1);
        int gz = glm::clamp((int)fz, 0, _grid.gridZ - 1);

        int cell = CellIndex(gx, gy, gz, _grid.gridX, _grid.gridY);

        _data.boidCell[i] = cell;
        _grid.cellCount[cell]++;
    }

    // --- prefix sum ---
    _grid.cellOffset[0] = 0;
    for (int i = 1; i < numCells; i++)
    {
        _grid.cellOffset[i] = _grid.cellOffset[i - 1] + _grid.cellCount[i - 1];
    }

    // --- fill ---
    std::vector<int> currentOffset = _grid.cellOffset;

    for (int i = 0; i < N; i++)
    {
        int cell = _data.boidCell[i];

        // safety (can remove later for perf)
        assert(cell >= 0 && cell < numCells);

        int idx = currentOffset[cell]++;

        assert(idx >= 0 && idx < N);

        _grid.sortedIndices[idx] = i;
    }

#ifdef _DEBUG
    // sanity check (useful)
    for (int i = 0; i < numCells; i++)
    {
        int start = _grid.cellOffset[i];
        int end = start + _grid.cellCount[i];

        assert(start >= 0);
        assert(end <= N);
    }
#endif
}

void BoidSystem::ComputeForces()
{
    const int N = (int)_data.Size();

    const float sepR2 = _separationRadius * _separationRadius;
    const float aliR2 = _alignmentRadius * _alignmentRadius;
    const float cohR2 = _cohesionRadius * _cohesionRadius;

    #pragma omp parallel for schedule (guided, 128) if(Utils::parallelEnabled.load())
    for (int i = 0; i < N; i++)
    {
        const glm::vec3 pos_i = _data.position[i];
        const glm::vec3 vel_i = _data.velocity[i];

        glm::vec3 sep(0.0f);
        glm::vec3 ali(0.0f);
        glm::vec3 coh(0.0f);

        int countSep = 0;
        int countAli = 0;
        int countCoh = 0;

        // current cell
        int cell = _data.boidCell[i];

        int gz = cell / (_grid.gridX * _grid.gridY);
        int gy = (cell / _grid.gridX) % _grid.gridY;
        int gx = cell % _grid.gridX;

        // neighbor cells (const 27)
        for (int dz = -1; dz <= 1; dz++)
        {
            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    int nx = gx + dx;
                    int ny = gy + dy;
                    int nz = gz + dz;

                    if (nx < 0 || ny < 0 || nz < 0 ||
                        nx >= _grid.gridX || ny >= _grid.gridY || nz >= _grid.gridZ)
                        continue;

                    int ncell = CellIndex(nx, ny, nz, _grid.gridX, _grid.gridY);

                    int start = _grid.cellOffset[ncell];
                    int end = start + _grid.cellCount[ncell];

                    for (int k = start; k < end; k++)
                    {
                        int j = _grid.sortedIndices[k];
                        if (j == i) continue;

                        glm::vec3 d = pos_i - _data.position[j];
                        float dist2 = glm::length2(d);

                        if (dist2 < sepR2 && dist2 > 1e-8f)
                        {
                            sep += d / dist2;
                            countSep++;
                        }

                        if (dist2 < aliR2)
                        {
                            ali += _data.velocity[j];
                            countAli++;
                        }

                        if (dist2 < cohR2)
                        {
                            coh += _data.position[j];
                            countCoh++;
                        }
                    }
                }
            }
        }

        glm::vec3 force(0.0f);

        if (countSep > 0)
        {
            sep /= (float)countSep;
            force += (glm::normalize(sep) * _data.maxSpeed[i] - vel_i) * 1.f;
        }

        if (countAli > 0)
        {
            ali /= (float)countAli;

            if (glm::length(ali) > 10.0f)
            {
                force += (glm::normalize(ali) * _data.maxSpeed[i] - vel_i) * 1.5f;
            }
        }

        if (countCoh > 0)
        {
            glm::vec3 centerOfMass = coh / (float)countCoh;
            glm::vec3 dir = centerOfMass - pos_i;
            float dist = glm::length(dir);

            if (dist > 5.0f)
            {
                force += (glm::normalize(dir) * _data.maxSpeed[i] - vel_i) * 0.3f;
            }
        }

        _data.acceleration[i] = force;

        KeepInBounds(i);
    }
}

void BoidSystem::Integrate(float deltaTime)
{
    const int N = (int)_data.Size();

    #pragma omp parallel for schedule(static, 512) if(Utils::parallelEnabled.load())
    for (int i = 0; i < N; i++)
    {
        glm::vec3& vel = _data.velocity[i];
        glm::vec3& pos = _data.position[i];
        glm::vec3& acc = _data.acceleration[i];

        vel += acc * deltaTime;

        vel *= std::pow(0.95f, deltaTime * 60.0f);

        float speed = glm::length(vel);
        float maxS = _data.maxSpeed[i];
        float minS = maxS * 0.6f;
        if (speed > maxS)
        {
            vel = (vel / speed) * maxS;
        }
        else if (speed < maxS * 0.2f)
        {
            vel = (vel / speed) * minS;
        }
        else if (speed < 0.1f)
        {
            vel *= 5.f;
        }

        pos += vel * deltaTime;
        acc = glm::vec3(0.0f);
    }
}

void BoidSystem::KeepInBounds(const int i)
{
    glm::vec3 pos = _data.position[i];
    glm::vec3 steer(0.0f);
    float margin = 20.0f;

    auto checkAxis = [&](float p, float limit) {
        if (p > limit - margin) return -(p - (limit - margin)) / margin;
        if (p < -limit + margin) return ((-limit + margin) - p) / margin;
        return 0.0f;
        };

    steer.x = checkAxis(pos.x, xLimit);
    steer.y = checkAxis(pos.y, yMax);
    steer.z = checkAxis(pos.z, zLimit);

    if (glm::length2(steer) > 0.001f)
    {
        glm::vec3 desired = steer * _data.maxSpeed[i];
        glm::vec3 force = desired - _data.velocity[i];

        _data.acceleration[i] += force * 2.0f;
    }
}

// additional Utils

glm::vec3 BoidSystem::LimitVec(glm::vec3 v, float max)
{
    float len2 = glm::length2(v);
    if (len2 > max * max)
    {
        return glm::normalize(v) * max;
    }
    return v;
}

glm::vec3 BoidSystem::Limit(const glm::vec3& v, float max)
{
    float len2 = glm::length2(v);
    if (len2 > max * max)
    {
        return glm::normalize(v) * max;
    }
    return v;
}