#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include "renderer.h"

class Window {
	GLFWwindow* m_Window;
	std::unique_ptr<Renderer> m_Renderer;

	const uint32_t m_Width = 800;
	const uint32_t m_Height = 600;
	
public:
	Window();
	Window(const Window&) = delete;
	~Window();

	void run();
};