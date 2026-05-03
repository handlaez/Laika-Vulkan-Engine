#ifndef BOID_CLASS_H
#define BOID_CLASS_H

#include <glm/glm.hpp>

namespace le {
	class Boid
	{
	public:
		Boid(const glm::vec3& startPos);

		void Update(float deltaTime);

		void ApplyForce(const glm::vec3& force);

		const glm::vec3& GetPosition() const;
		const glm::vec3& GetVelocity() const;
		const float GetMaxSpeed() const;
		const float GetMaxForce() const;

		void SetPosition(const glm::vec3& pos);
		void SetVelocity(const glm::vec3& vel);

		//helper
		float randf();

	private:
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec3 acceleration;

		float maxSpeed;
		float maxForce;
	};
}

#endif
