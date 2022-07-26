module;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

module Window;

import <iostream>;

Window::Window() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_Window = glfwCreateWindow(m_Width, m_Height, "Vulkan window", nullptr, nullptr);
    if (m_Window == nullptr) {
        std::cerr << "Couldn't initialise GLFW window" << std::endl;
        exit(-1);
    }

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    auto surfaceCreator = [&](const VkInstance& instance, VkSurfaceKHR& surface) -> bool {
        return glfwCreateWindowSurface(instance, m_Window, nullptr, &surface) == VK_SUCCESS;
    };

    int width, height;
    glfwGetFramebufferSize(m_Window, &width, &height);

    m_Renderer.reset(new Renderer(glfwExtensions, glfwExtensionCount, surfaceCreator, width, height));

}

void Window::run() {
    while (!glfwWindowShouldClose(m_Window)) {
        glfwPollEvents();
    }
}

Window::~Window() {
    if (m_Window) {
        glfwDestroyWindow(m_Window);
    }

    glfwTerminate();
}
