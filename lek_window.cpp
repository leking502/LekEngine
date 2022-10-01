//
// Created by leking on 22-9-28.
//
#include "lek_window.hpp"

#include <stdexcept>

using namespace std;

namespace leking{

    LekWindow::LekWindow(int width, int height, string name) : width(width), height(height), windowName(name){
        initWindow();
    }

    void LekWindow::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE,GLFW_TRUE);

        window = glfwCreateWindow(width,height,windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
    }

    LekWindow::~LekWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    bool LekWindow::shouldClose() {
        return glfwWindowShouldClose(window);
    }

    void LekWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
        if(glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw runtime_error("创建窗体失败");
        }
    }

    VkExtent2D LekWindow::getExtent() {
        return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    }

    void LekWindow::frameBufferResizeCallback(GLFWwindow *window, int wight, int height) {
        auto lekWindow = reinterpret_cast<LekWindow*>(glfwGetWindowUserPointer(window));
        lekWindow->frameBufferResized = true;
        lekWindow->width = wight;
        lekWindow->height = height;
    }

}

