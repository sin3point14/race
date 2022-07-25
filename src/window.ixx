module;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

export module Window;

import Renderer;

import <memory>;

export {
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
}
