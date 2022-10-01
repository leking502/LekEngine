//
// Created by leking on 22-9-28.
//

#ifndef LEKENGINE_LEK_WINDOW_HPP
#define LEKENGINE_LEK_WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <string>

using namespace std;
namespace leking {
    class LekWindow{
    public:
        LekWindow(int width, int height, string name);
        ~LekWindow();

        LekWindow(const LekWindow &) = delete;
        LekWindow& operator=(const LekWindow&) = delete;

        bool shouldClose();
        VkExtent2D getExtent();
        bool wasWindowResized() { return frameBufferResized; }
        void resetWindowResizedFlag() { frameBufferResized = false; }
        GLFWwindow* getGLFWwindow() const {return window;}

        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    private:
        static void frameBufferResizeCallback(GLFWwindow* window, int wight, int height);
        void initWindow();
        int width;
        int height;
        bool frameBufferResized = false;

        string windowName;
        GLFWwindow* window;
    };
}

#endif //LEKENGINE_LEK_WINDOW_HPP
