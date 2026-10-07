struct TransformComponent
{
    glm::vec3 translation{};
    glm::quat rotation{ 1.f, 0.f, 0.f, 0.f };
    glm::vec3 scale{ 1.f, 1.f, 1.f };

    TransformComponent() = default;

    TransformComponent(glm::vec3 translation, glm::quat rotation, glm::vec3 scale)
        : translation(translation), rotation(rotation), scale(scale)
    {
    }
};