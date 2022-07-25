#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include <functional>

class Renderer {
	VkInstance m_Instance;
	VkPhysicalDevice m_PhysicalDevice;
	VkDevice m_Device;
	VkQueue m_GraphicsPresentQueue;
	VkSurfaceKHR m_Surface;
	VkQueue m_PresentQueue;

	static inline const std::vector<const char*> s_DeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

#ifdef NDEBUG
	static inline const bool s_EnableValidationLayers = false;
	static inline const std::vector<const char*> s_ValidationLayers = {
	};
#else
	static inline const bool s_EnableValidationLayers = true;
	static inline const std::vector<const char*> s_ValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
#endif

	bool checkValidationLayerSupport();
	void createInstance(const char** extensions, uint32_t extensionsCount);
	void pickPhysicalDevice();
	void createSurface(const std::function<bool(const VkInstance&, VkSurfaceKHR&)>& surfaceCreator);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	std::optional<uint32_t> findQueueFamilies(const VkPhysicalDevice& device);
	int rateDeviceSuitability(const VkPhysicalDevice& device);
	void createLogicalDevice();
public:
	Renderer(const char** extensions, uint32_t extensionsCount, const std::function<bool(const VkInstance&, VkSurfaceKHR&)>& surfaceCreator);
	Renderer() = delete;
	Renderer(const Renderer&) = delete;
	~Renderer();

	void render(float time);
};
