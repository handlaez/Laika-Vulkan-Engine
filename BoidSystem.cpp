#include "BoidSystem.hpp"
#include <glm/glm.hpp>

namespace le {

    void BoidSystem::AddBoid(const Boid& boid)
    {
        boids.push_back(boid);
    }

    std::vector<Boid>& BoidSystem::GetBoids()
    {
        return boids;
    }

    void BoidSystem::Update(float deltaTime)
    {
        for (auto& b : boids)
        {
            glm::vec3 sep = Separation(b);
            glm::vec3 ali = Alignment(b);
            glm::vec3 coh = Cohesion(b);

            sep = Limit(sep, b.GetMaxForce());
            ali = Limit(ali, b.GetMaxForce());
            coh = Limit(coh, b.GetMaxForce());

            // weights matter a lot
            b.ApplyForce(sep * 5.0f * deltaTime);
            b.ApplyForce(ali * 3.5f * deltaTime);
            b.ApplyForce(coh * 3.0f * deltaTime);

            KeepInBounds(b, deltaTime);
        }

        // update AFTER applying all forces
        for (auto& b : boids)
        {
            b.Update(deltaTime);
        }
    }

    void BoidSystem::KeepInBounds(Boid& b, float deltaTime)
    {
        float xLimit = 100.0f;
        float yMin = -25.0f;
        float yMax = 25.0f;
        float zLimit = 100.0f;

        float margin = 10.0f;

        glm::vec3 steer(0.0f);
        glm::vec3 pos = b.GetPosition();

        // X axis
        if (pos.x > xLimit - margin)
        {
            float t = (pos.x - (xLimit - margin)) / margin;
            steer.x -= t;
        }
        else if (pos.x < -xLimit + margin)
        {
            float t = ((-xLimit + margin) - pos.x) / margin;
            steer.x += t;
        }

        // Y axis
        if (pos.y > yMax - margin)
        {
            float t = (pos.y - (yMax - margin)) / margin;
            steer.y -= t;
        }
        else if (pos.y < yMin + margin)
        {
            float t = ((yMin + margin) - pos.y) / margin;
            steer.y += t;
        }

        // Z axis
        if (pos.z > zLimit - margin)
        {
            float t = (pos.z - (zLimit - margin)) / margin;
            steer.z -= t;
        }
        else if (pos.z < -zLimit + margin)
        {
            float t = ((-zLimit + margin) - pos.z) / margin;
            steer.z += t;
        }

        if (glm::length(steer) > 0.0f)
        {
            steer = glm::normalize(steer) * b.GetMaxSpeed();
            steer -= b.GetVelocity();

            steer = Limit(steer, b.GetMaxForce());

            b.ApplyForce(steer * 5.f * deltaTime);
        }
    }

    glm::vec3 BoidSystem::Limit(const glm::vec3& v, float max)
    {
        if (glm::length(v) > max)
        {
            return glm::normalize(v) * max;
        }
        return v;
    }

    glm::vec3 BoidSystem::Separation(Boid& boid)
    {
        glm::vec3 steer(0.0f);
        int count = 0;
        float dist = 0.f;

        for (auto& other : boids)
        {
            dist = glm::distance(boid.GetPosition(), other.GetPosition());

            if (&other != &boid && dist < separationRadius && dist > 0.0f)
            {
                glm::vec3 diff = boid.GetPosition() - other.GetPosition();

                // stronger when closer
                diff /= dist * dist;

                steer += diff;
                count++;
            }
        }

        if (count > 0)
        {
            steer /= (float)count;

            float factor = glm::clamp(glm::length(steer), 0.2f, 1.0f);
            glm::vec3 desired = glm::normalize(steer) * (boid.GetMaxSpeed() * factor);
            return desired - boid.GetVelocity();
        }

        return glm::vec3(0.0f);
    }

    glm::vec3 BoidSystem::Alignment(Boid& boid)
    {
        glm::vec3 avgVelocity(0.0f);
        int count = 0;

        for (auto& other : boids)
        {
            float dist = glm::distance(boid.GetPosition(), other.GetPosition());

            if (&other != &boid && dist < alignmentRadius)
            {
                avgVelocity += other.GetVelocity();
                count++;
            }
        }

        if (count > 0)
        {
            avgVelocity /= (float)count;

            glm::vec3 desired = glm::normalize(avgVelocity) * (boid.GetMaxSpeed() * 0.8f);
            return desired - boid.GetVelocity();
        }

        return glm::vec3(0.0f);
    }

    glm::vec3 BoidSystem::Cohesion(Boid& boid)
    {
        glm::vec3 center(0.0f);
        int count = 0;

        for (auto& other : boids)
        {
            float dist = glm::distance(boid.GetPosition(), other.GetPosition());

            if (&other != &boid && dist < cohesionRadius)
            {
                center += other.GetPosition();
                count++;
            }
        }

        if (count > 0)
        {
            center /= (float)count;

            float factor = glm::clamp(glm::length(center - boid.GetPosition()) / cohesionRadius, 0.3f, 1.0f);
            glm::vec3 desired = glm::normalize(center - boid.GetPosition()) * (boid.GetMaxSpeed() * factor);
            return desired - boid.GetVelocity();
        }

        return glm::vec3(0.0f);
    }
}