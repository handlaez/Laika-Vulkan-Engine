#ifndef LE_FRAME_INFO_HPP
#define LE_FRAME_INFO_HPP

typedef struct GLFWwindow GLFWwindow;

namespace le {

    struct FrameInfo {
        float aspect;
        float deltaTime;
        GLFWwindow* window = nullptr;
    };

} // namespace le

#endif