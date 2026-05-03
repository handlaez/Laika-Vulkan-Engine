#ifndef LE_CAMERA_HPP
#define LE_CAMERA_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace le {

	class LeCamera {
	public:
		void setOrtographicProjection(float left, float right, float top, float bottom, float near, float far);

		void setPerspectiveProjection(float fovY, float aspect, float near, float far);

		void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });
		void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });
		void setView(glm::vec3 position, glm::quat rotation);

		const glm::mat4& getProjection() const { return projectionMatrix; }
		const glm::mat4& getView() const { return viewMatrix; }

	private:
		glm::mat4 projectionMatrix{ 1.f };
		glm::mat4 viewMatrix{ 1.f };
	};

} //le

#endif