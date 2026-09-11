#ifndef LE_UTILS_HPP
#define LE_UTILS_HPP

#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <random>
#include <atomic>

class Utils
{
public:
    inline static std::atomic<bool> parallelEnabled{ false };

    inline static bool instancingEnabled{ false };
    inline static bool texturesEnabled{ true };
    inline static bool lightingEnabled{ true };
    inline static bool wireframeEnabled{ false };
    inline static bool hitboxesEnabled{ false };
    inline static bool skyboxEnabled{ true };

    static bool isKeyPressed(GLFWwindow* window, int key)
    {
        static bool keyStates[1024] = { false };

        bool current = glfwGetKey(window, key) == GLFW_PRESS;

        bool pressedThisFrame = current && !keyStates[key];

        keyStates[key] = current;

        return pressedThisFrame;
    }

    static void checkKeys(GLFWwindow* window)
    {
        if (isKeyPressed(window, GLFW_KEY_1))
        {
            parallelEnabled = !parallelEnabled;
        }

        if (isKeyPressed(window, GLFW_KEY_2))
        {
            instancingEnabled = !instancingEnabled;
        }

        if (isKeyPressed(window, GLFW_KEY_3))
        {
            lightingEnabled = !lightingEnabled;
        }

        if (isKeyPressed(window, GLFW_KEY_4))
        {
            texturesEnabled = !texturesEnabled;
        }

        if (isKeyPressed(window, GLFW_KEY_5))
        {
            wireframeEnabled = !wireframeEnabled;
        }

        if (isKeyPressed(window, GLFW_KEY_6))
        {
            hitboxesEnabled = !hitboxesEnabled;
        }

        if (isKeyPressed(window, GLFW_KEY_7))
        {
            skyboxEnabled = !skyboxEnabled;
        }
    }

    // stolen from: https://stackoverflow.com/a/57595105
    template <typename T, typename... Rest>
    static void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
        seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hashCombine(seed, rest), ...);
    }

    static float randf()
    {
        thread_local static std::mt19937 rng([]()
            {
                std::random_device rd;
                return rd();
            }());

        thread_local static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

        return dist(rng);
    }
};

#endif