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
	VkSwapchainKHR m_SwapChain;
	std::vector<VkImage> m_SwapChainImages;
	std::vector<VkImageView> m_SwapChainImageViews;
	VkFormat m_SwapChainImageFormat;
	VkExtent2D m_SwapChainExtent;

	static inline const std::vector<const char*> s_DeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	static constexpr bool s_FPSLock = false;

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

	typedef std::tuple<VkSurfaceCapabilitiesKHR, std::vector<VkSurfaceFormatKHR>, std::vector<VkPresentModeKHR>> swap_chain_details;

	bool checkValidationLayerSupport();
	void createInstance(const char** extensions, uint32_t extensionsCount);
	void pickPhysicalDevice();
	std::optional<swap_chain_details> querySwapChainSupport(const VkPhysicalDevice& device);
	void createSurface(const std::function<bool(const VkInstance&, VkSurfaceKHR&)>& surfaceCreator);
	bool checkDeviceExtensionSupport(const VkPhysicalDevice& device);
	std::optional<uint32_t> findQueueFamilies(const VkPhysicalDevice& device);
	int rateDeviceSuitability(const VkPhysicalDevice& device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height);
	void createLogicalDevice();
	void createSwapChain(int width, int height);
	void createImageViews();
public:
	Renderer(const char** extensions, uint32_t extensionsCount, const std::function<bool(const VkInstance&, VkSurfaceKHR&)>& surfaceCreator, int width, int height);
	Renderer() = delete;
	Renderer(const Renderer&) = delete;
	~Renderer();

	void render(float time);
};
