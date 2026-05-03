#include "Boid.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace le {
    float Boid::randf()
    {
        return -1.0f + (rand() / (RAND_MAX / 2.0f));
    }

    Boid::Boid(const glm::vec3& startPosition)
        : position(startPosition),
        acceleration(glm::vec3(0.0f)),
        maxSpeed(500.0f),
        maxForce(20.0f)
    {
        // Random direction
        velocity = glm::normalize(glm::vec3(randf(), randf(), randf())) * 2.0f;
    }

    void Boid::ApplyForce(const glm::vec3& force)
    {
        acceleration += force;
    }

    void Boid::Update(float deltaTime)
    {
        velocity += acceleration * deltaTime;

        float speed = glm::length(velocity);

        if (speed > maxSpeed)
        {
            velocity = glm::normalize(velocity) * maxSpeed;
        }

        // optional damping
        velocity *= 0.995f;

        position += velocity * 10.f * deltaTime;

        acceleration = glm::vec3(0.0f);
    }

    const glm::vec3& Boid::GetPosition() const
    {
        return position;
    }

    const glm::vec3& Boid::GetVelocity() const
    {
        return velocity;
    }

    const float Boid::GetMaxSpeed() const
    {
        return maxSpeed;
    }

    const float Boid::GetMaxForce() const
    {
        return maxForce;
    }

    void Boid::SetPosition(const glm::vec3& pos)
    {
        position = pos;
    }

    void Boid::SetVelocity(const glm::vec3& vel)
    {
        velocity = vel;
    }
}