#include "vk/mesh.hpp"
#include <iostream>

namespace democollection::vk
{
#if VALIDATION_LAYER_ENABLED
	static const char* const g_ValidationLayers[] = {
		"VK_LAYER_KHRONOS_validation"
	};

	static bool CheckValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : g_ValidationLayers)
		{
			bool layerFound = false;

			for (const VkLayerProperties& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
				return false;
		}

		return true;
	}

	static std::vector<const char*> GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions;
		extensions.reserve(glfwExtensionCount + 1);

		for (uint32_t i = 0; i < glfwExtensionCount; ++i)
			extensions.push_back(glfwExtensions[i]);
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
			void *pUserData)
	{

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	static VkResult CreateDebugUtilsMessengerEXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	static void DestroyDebugUtilsMessengerEXT(
			VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator)
	{
	    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
	    if (func != nullptr)
	        func(instance, debugMessenger, pAllocator);
	}
#endif

	VulkanResources::VulkanResources()
		: m_instance{VK_NULL_HANDLE}
#if VALIDATION_LAYER_ENABLED
		, m_debugMessenger{VK_NULL_HANDLE}
#endif
		, m_surface{VK_NULL_HANDLE}
		, m_device{VK_NULL_HANDLE}
		, m_swapchain{VK_NULL_HANDLE}
		, m_swapchainImages{}
		, m_swapchainImageViews{}
		, m_swapchainFrameBuffers{}
		, m_colorImage{VK_NULL_HANDLE}
		, m_colorImageMemory{VK_NULL_HANDLE}
		, m_colorImageView{VK_NULL_HANDLE}
		, m_depthImage{VK_NULL_HANDLE}
		, m_depthImageMemory{VK_NULL_HANDLE}
		, m_depthImageView{VK_NULL_HANDLE}
		, m_renderPass{VK_NULL_HANDLE}
		, m_descriptorSetLayout{VK_NULL_HANDLE}
		, m_pipelineLayout{VK_NULL_HANDLE}
		, m_graphicsPipeline{VK_NULL_HANDLE}
		, m_commandPool{VK_NULL_HANDLE}
		, m_commandBuffers{}
		, m_imageAvailableSemaphore{}
		, m_renderFinishedSemaphore{}
		, m_inFlightFences{}
	{
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			m_commandBuffers[i] = VK_NULL_HANDLE;
			m_imageAvailableSemaphore[i] = VK_NULL_HANDLE;
			m_renderFinishedSemaphore[i] = VK_NULL_HANDLE;
			m_inFlightFences[i] = VK_NULL_HANDLE;
		}
	}

	VulkanResources::~VulkanResources()
	{
		CleanupScreenResources();
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			SAFE_DESTROY(vkDestroySemaphore, m_imageAvailableSemaphore[i], m_device, m_imageAvailableSemaphore[i], Allocator());
			SAFE_DESTROY(vkDestroySemaphore, m_renderFinishedSemaphore[i], m_device, m_renderFinishedSemaphore[i], Allocator());
			SAFE_DESTROY(vkDestroyFence, m_inFlightFences[i], m_device, m_inFlightFences[i], Allocator());
		}
		if (m_commandBuffers[0])
		{
			vkFreeCommandBuffers(m_device, m_commandPool, MAX_FRAMES_IN_FLIGHT, m_commandBuffers);
			for (VkCommandBuffer& cb : m_commandBuffers)
				cb = VK_NULL_HANDLE;
		}
		SAFE_DESTROY(vkDestroyCommandPool, m_commandPool, m_device, m_commandPool, Allocator());
		SAFE_DESTROY(vkDestroyPipeline, m_graphicsPipeline, m_device, m_graphicsPipeline, Allocator());
		SAFE_DESTROY(vkDestroyPipelineLayout, m_pipelineLayout, m_device, m_pipelineLayout, Allocator());
		SAFE_DESTROY(vkDestroyDescriptorSetLayout, m_descriptorSetLayout, m_device, m_descriptorSetLayout, Allocator());
		SAFE_DESTROY(vkDestroyRenderPass, m_renderPass, m_device, m_renderPass, Allocator());

		SAFE_DESTROY(vkDestroyDevice, m_device, m_device, Allocator());
		SAFE_DESTROY(vkDestroySurfaceKHR, m_surface, m_instance, m_surface, Allocator());
#if VALIDATION_LAYER_ENABLED
		SAFE_DESTROY(DestroyDebugUtilsMessengerEXT, m_debugMessenger, m_instance, m_debugMessenger, Allocator());
#endif
		SAFE_DESTROY(vkDestroyInstance, m_instance, m_instance, Allocator());
	}

	void VulkanResources::CleanupScreenResources()
	{
		SAFE_DESTROY(vkDestroyImage, m_colorImage, m_device, m_colorImage, Allocator());
		SAFE_DESTROY(vkFreeMemory, m_colorImageMemory, m_device, m_colorImageMemory, Allocator());
		SAFE_DESTROY(vkDestroyImageView, m_colorImageView, m_device, m_colorImageView, Allocator());
		SAFE_DESTROY(vkDestroyImage, m_depthImage, m_device, m_depthImage, Allocator());
		SAFE_DESTROY(vkFreeMemory, m_depthImageMemory, m_device, m_depthImageMemory, Allocator());
		SAFE_DESTROY(vkDestroyImageView, m_depthImageView, m_device, m_depthImageView, Allocator());
		for (VkFramebuffer& frameBuffer: m_swapchainFrameBuffers)
			SAFE_DESTROY(vkDestroyFramebuffer, frameBuffer, m_device, frameBuffer, Allocator());
		m_swapchainFrameBuffers.clear();
		for (VkImageView& imgView : m_swapchainImageViews)
			SAFE_DESTROY(vkDestroyImageView, imgView, m_device, imgView, Allocator());
		m_swapchainImageViews.clear();
		SAFE_DESTROY(vkDestroySwapchainKHR, m_swapchain, m_device, m_swapchain, Allocator());
		m_swapchainImages.clear();
	}

	Vulkan::ShaderModule::ShaderModule(const Vulkan& gfx)
		: m_gfx{gfx}
		, m_shaderModule{VK_NULL_HANDLE}{}

	Vulkan::ShaderModule::ShaderModule(ShaderModule&& rhs)
		: m_gfx{rhs.m_gfx}
		, m_shaderModule{rhs.m_shaderModule}
	{
		rhs.m_shaderModule = VK_NULL_HANDLE;
	}

	Vulkan::ShaderModule::~ShaderModule()
	{
		SAFE_DESTROY(vkDestroyShaderModule, m_shaderModule, m_gfx.m_device, m_shaderModule, m_gfx.Allocator());
	}

	void Vulkan::CreateInstance(const char* name)
	{
#if VALIDATION_LAYER_ENABLED
		ThrowIfFalse(CheckValidationLayerSupport());
#endif

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = name;
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = name;
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo instanceInfo{};
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pApplicationInfo = &appInfo;

#if VALIDATION_LAYER_ENABLED
		const std::vector<const char*> extensions = GetRequiredExtensions();
		instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instanceInfo.ppEnabledExtensionNames = extensions.data();
		instanceInfo.enabledLayerCount = ARRAY_SIZE(g_ValidationLayers);
		instanceInfo.ppEnabledLayerNames = g_ValidationLayers;
#else
		instanceInfo.ppEnabledExtensionNames = glfwGetRequiredInstanceExtensions(&instanceInfo.enabledExtensionCount);
#endif

#if VALIDATION_LAYER_ENABLED
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = DebugCallback;
		debugCreateInfo.pUserData = static_cast<void*>(this);
		instanceInfo.pNext = static_cast<void*>(&debugCreateInfo);
#endif

		ThrowIfFailed(vkCreateInstance(&instanceInfo, Allocator(), &m_instance));

#if VALIDATION_LAYER_ENABLED
		ThrowIfFailed(CreateDebugUtilsMessengerEXT(m_instance, &debugCreateInfo, Allocator(), &m_debugMessenger));
#endif
	}

	PhysicalDevice Vulkan::SelectPhysicalDevice() const
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
		ThrowIfFalse(deviceCount);
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

		std::vector<PhysicalDevice> devData;
		devData.reserve(deviceCount);
		uint32_t bestDeviceIndex = 0;
		uint32_t bestDeviceScore = 0;

		for (uint32_t i = 0; i < deviceCount; ++i)
		{
			devData.emplace_back(devices[i]);
			const uint32_t score = devData[i].ScoreDevice(m_surface);
			if (bestDeviceScore < score)
			{
				bestDeviceScore = score;
				bestDeviceIndex = i;
			}
		}
		ThrowIfFalse(bestDeviceScore);

		return devData[bestDeviceIndex];
	}

	void Vulkan::CreateLogicalDevice()
	{
		VkDeviceQueueCreateInfo queueCreateInfos[2]{};

		const uint32_t queueCount = m_physicalDevice.GraphicsQueueIndex() == m_physicalDevice.PresentQueueIndex() ? 1 : 2;

		float queuePriority = 1.0f;
		queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[0].queueFamilyIndex = m_physicalDevice.GraphicsQueueIndex();
		queueCreateInfos[0].queueCount = 1;
		queueCreateInfos[0].pQueuePriorities = &queuePriority;
		if (queueCount > 1)
		{
			queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfos[1].queueFamilyIndex = m_physicalDevice.PresentQueueIndex();
			queueCreateInfos[1].queueCount = 1;
			queueCreateInfos[1].pQueuePriorities = &queuePriority;
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = m_physicalDevice.Features().samplerAnisotropy;
		deviceFeatures.sampleRateShading = m_physicalDevice.Features().sampleRateShading;

		VkDeviceCreateInfo deviceInfo{};
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceInfo.queueCreateInfoCount = queueCount;
		deviceInfo.pQueueCreateInfos = queueCreateInfos;
		deviceInfo.pEnabledFeatures = &deviceFeatures;
		deviceInfo.enabledExtensionCount = ARRAY_SIZE(PhysicalDevice::sDeviceExtensions);
		deviceInfo.ppEnabledExtensionNames = PhysicalDevice::sDeviceExtensions;
#if VALIDATION_LAYER_ENABLED
		deviceInfo.enabledLayerCount = ARRAY_SIZE(g_ValidationLayers);
		deviceInfo.ppEnabledLayerNames = g_ValidationLayers;
#endif
		ThrowIfFailed(vkCreateDevice(m_physicalDevice.Device(), &deviceInfo, Allocator(), &m_device));
		vkGetDeviceQueue(m_device, m_physicalDevice.GraphicsQueueIndex(), 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_device, m_physicalDevice.PresentQueueIndex(), 0, &m_presentQueue);
	}

	uint32_t Vulkan::ChooseImageCount() const
	{
		uint32_t imageCount = m_physicalDevice.SurfaceCapabilities().minImageCount + 1;
		const uint32_t maxImageCount = m_physicalDevice.SurfaceCapabilities().maxImageCount;
		if (maxImageCount && imageCount > maxImageCount)
			return maxImageCount;
		return imageCount;
	}

	VkExtent2D Vulkan::ChooseExtent() const
	{
		const VkSurfaceCapabilitiesKHR& capabilities = m_physicalDevice.SurfaceCapabilities();
		if (capabilities.currentExtent.width != UINT32_MAX)
			return capabilities.currentExtent;

		int width, height;
		glfwGetFramebufferSize(m_window, &width, &height);

		return VkExtent2D{
			std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
			std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
		};
	}

	void Vulkan::CreateSwapchain()
	{
		const uint32_t queueFamilyIndices[] = {
				m_physicalDevice.GraphicsQueueIndex(),
				m_physicalDevice.PresentQueueIndex()
		};

		VkSwapchainCreateInfoKHR swapchainInfo{};
		swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainInfo.surface = m_surface;
		swapchainInfo.minImageCount = ChooseImageCount();
		swapchainInfo.imageFormat = m_physicalDevice.SurfaceFormat().format;
		swapchainInfo.imageColorSpace = m_physicalDevice.SurfaceFormat().colorSpace;
		swapchainInfo.imageExtent = ChooseExtent();
		swapchainInfo.imageArrayLayers = 1;
		swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if (queueFamilyIndices[0] != queueFamilyIndices[1])
		{
			swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainInfo.queueFamilyIndexCount = ARRAY_SIZE(queueFamilyIndices);
			swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		swapchainInfo.preTransform = m_physicalDevice.SurfaceCapabilities().currentTransform;
		swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainInfo.presentMode = m_physicalDevice.PresentMode();
		swapchainInfo.clipped = VK_TRUE;
		swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
		ThrowIfFailed(vkCreateSwapchainKHR(m_device, &swapchainInfo, Allocator(), &m_swapchain));

		m_swapchainExtent = swapchainInfo.imageExtent;
	}

	void Vulkan::CreateSwapchainImageResources()
	{
		uint32_t imageCount = 0;
		vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);
		m_swapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, m_swapchainImages.data());
		m_swapchainImageViews.resize(imageCount);

		VkImageViewCreateInfo imageViewInfo{};
		imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewInfo.format = m_physicalDevice.SurfaceFormat().format;
		imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewInfo.subresourceRange.baseMipLevel = 0;
		imageViewInfo.subresourceRange.levelCount = 1;
		imageViewInfo.subresourceRange.baseArrayLayer = 0;
		imageViewInfo.subresourceRange.layerCount = 1;
		for (uint32_t i = 0; i < imageCount; ++i)
		{
			imageViewInfo.image = m_swapchainImages[i];
			ThrowIfFailed(vkCreateImageView(m_device, &imageViewInfo, Allocator(), &m_swapchainImageViews[i]));
		}
	}

	void Vulkan::CreateColorImageResources()
	{
		const VkSampleCountFlagBits sampleCount = m_physicalDevice.MsaaSampleCount();
		if (VK_SAMPLE_COUNT_1_BIT != sampleCount)
		{
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = m_swapchainExtent.width;
			imageInfo.extent.height = m_swapchainExtent.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = m_physicalDevice.SurfaceFormat().format;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.samples = sampleCount;
			imageInfo.flags = 0;
			ThrowIfFailed(vkCreateImage(m_device, &imageInfo, Allocator(), &m_colorImage));

			m_colorImageMemory = AllocateMemory(m_colorImage);

			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_colorImage;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = imageInfo.format;
			viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;
			ThrowIfFailed(vkCreateImageView(m_device, &viewInfo, Allocator(), &m_colorImageView));
		}
	}

	void Vulkan::CreateDepthResources()
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = m_swapchainExtent.width;
		imageInfo.extent.height = m_swapchainExtent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = m_physicalDevice.DepthFormat();
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = m_physicalDevice.MsaaSampleCount();
		imageInfo.flags = 0;
		ThrowIfFailed(vkCreateImage(m_device, &imageInfo, Allocator(), &m_depthImage));

		m_depthImageMemory = AllocateMemory(m_depthImage);

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_depthImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = imageInfo.format;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		ThrowIfFailed(vkCreateImageView(m_device, &viewInfo, Allocator(), &m_depthImageView));
	}
	void Vulkan::CreateRenderPass()
	{
		VkAttachmentDescription attachments[3]{};

		const VkSampleCountFlagBits sampleCount = m_physicalDevice.MsaaSampleCount();
		const bool multiSampling = sampleCount != VK_SAMPLE_COUNT_1_BIT;

		VkAttachmentDescription& colorAttachment = attachments[0];
		VkAttachmentReference colorAttachmentRef{};
		colorAttachment.format = m_physicalDevice.SurfaceFormat().format;
		colorAttachment.samples = sampleCount;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = multiSampling ?
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription& depthAttachment = attachments[1];
		VkAttachmentReference depthAttachmentRef{};
		depthAttachment.format = m_physicalDevice.DepthFormat();
		depthAttachment.samples = sampleCount;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription& colorAttachmentResolve = attachments[2];
		VkAttachmentReference colorAttachmentResolveRef{};
		if (multiSampling)
		{
			colorAttachmentResolve.format = colorAttachment.format;
			colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			colorAttachmentResolveRef.attachment = 2;
			colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		if (multiSampling)
			subpass.pResolveAttachments = &colorAttachmentResolveRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = multiSampling ? 3 : 2;
		renderPassInfo.pAttachments = attachments;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;
		ThrowIfFailed(vkCreateRenderPass(m_device, &renderPassInfo, Allocator(), &m_renderPass));
	}

	void Vulkan::CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding bindings[5]{};

		VkDescriptorSetLayoutBinding& sceneBufferVertexShaderLayoutBinding = bindings[0];
		sceneBufferVertexShaderLayoutBinding.binding = 0;
		sceneBufferVertexShaderLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		sceneBufferVertexShaderLayoutBinding.descriptorCount = 1;
		sceneBufferVertexShaderLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding& modelBufferVertexShaderLayoutBinding = bindings[1];
		modelBufferVertexShaderLayoutBinding.binding = 1;
		modelBufferVertexShaderLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		modelBufferVertexShaderLayoutBinding.descriptorCount = 1;
		modelBufferVertexShaderLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding& sceneBufferFragmentShaderLayoutBinding = bindings[2];
		sceneBufferFragmentShaderLayoutBinding.binding = 2;
		sceneBufferFragmentShaderLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		sceneBufferFragmentShaderLayoutBinding.descriptorCount = 1;
		sceneBufferFragmentShaderLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding& modelBufferFragmentShaderLayoutBinding = bindings[3];
		modelBufferFragmentShaderLayoutBinding.binding = 3;
		modelBufferFragmentShaderLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		modelBufferFragmentShaderLayoutBinding.descriptorCount = 1;
		modelBufferFragmentShaderLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding& samplerLayoutBinding = bindings[4];
		samplerLayoutBinding.binding = 4;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = ARRAY_SIZE(bindings);
		layoutInfo.pBindings = bindings;
		ThrowIfFailed(vkCreateDescriptorSetLayout(m_device, &layoutInfo, Allocator(), &m_descriptorSetLayout));
	}

	Vulkan::ShaderModule Vulkan::CreateShaderModule(const std::vector<char>& code) const
	{
		VkShaderModuleCreateInfo moduleInfo{};
		moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		moduleInfo.codeSize = code.size();
		moduleInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        ShaderModule shaderModule(*this);
		ThrowIfFailed(vkCreateShaderModule(m_device, &moduleInfo, Allocator(), &shaderModule));
		return shaderModule;
	}

	void Vulkan::CreateGraphicsPipeline()
	{
        const std::vector<char> vertShaderCode = ReadFile((GetProgramFolder() + "shader_vert.spv").c_str());
        const std::vector<char> fragShaderCode = ReadFile((GetProgramFolder() + "shader_frag.spv").c_str());

		ShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
		ShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo shaderStages[2]{};
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = vertShaderModule;
		shaderStages[0].pName = "main";
		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = fragShaderModule;
		shaderStages[1].pName = "main";


		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		VkVertexInputAttributeDescription attributeDescriptions[3]{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, texcoord);
		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, normal);

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = ARRAY_SIZE(attributeDescriptions);
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		VkDynamicState dynamicStates[] = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.dynamicStateCount = ARRAY_SIZE(dynamicStates);
		dynamicStateInfo.pDynamicStates = dynamicStates;

		VkPipelineViewportStateCreateInfo viewportStateInfo{};
		viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateInfo.viewportCount = 1;
		viewportStateInfo.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizerInfo{};
		rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerInfo.depthClampEnable = VK_FALSE;
		rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerInfo.lineWidth = 1.0f;
		rasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizerInfo.depthBiasEnable = VK_FALSE;
		rasterizerInfo.depthBiasConstantFactor = 0.0f;
		rasterizerInfo.depthBiasClamp = 0.0f;
		rasterizerInfo.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multiSamplingInfo{};
		multiSamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multiSamplingInfo.rasterizationSamples = m_physicalDevice.MsaaSampleCount();
		multiSamplingInfo.pSampleMask = nullptr;
		multiSamplingInfo.alphaToCoverageEnable = VK_FALSE;
		multiSamplingInfo.alphaToOneEnable = VK_FALSE;
		if (VK_SAMPLE_COUNT_1_BIT != multiSamplingInfo.rasterizationSamples)
		{
			multiSamplingInfo.sampleShadingEnable = VK_TRUE;
			multiSamplingInfo.minSampleShading = 0.2f;
		}
		else
		{
			multiSamplingInfo.sampleShadingEnable = VK_FALSE;
			multiSamplingInfo.minSampleShading = 1.0f;
		}

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlendingInfo{};
		colorBlendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendingInfo.logicOpEnable = VK_FALSE;
		colorBlendingInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendingInfo.attachmentCount = 1;
		colorBlendingInfo.pAttachments = &colorBlendAttachment;
		colorBlendingInfo.blendConstants[0] = 0.0f;
		colorBlendingInfo.blendConstants[1] = 0.0f;
		colorBlendingInfo.blendConstants[2] = 0.0f;
		colorBlendingInfo.blendConstants[3] = 0.0f;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		ThrowIfFailed(vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, Allocator(), &m_pipelineLayout));

		VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
		depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilInfo.depthTestEnable = VK_TRUE;
		depthStencilInfo.depthWriteEnable = VK_TRUE;
		depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		depthStencilInfo.minDepthBounds = 0.0f;
		depthStencilInfo.maxDepthBounds = 1.0f;
		depthStencilInfo.stencilTestEnable = VK_FALSE;
		depthStencilInfo.front = {};
		depthStencilInfo.back = {};

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportStateInfo;
		pipelineInfo.pRasterizationState = &rasterizerInfo;
		pipelineInfo.pMultisampleState = &multiSamplingInfo;
		pipelineInfo.pDepthStencilState = &depthStencilInfo;
		pipelineInfo.pColorBlendState = &colorBlendingInfo;
		pipelineInfo.pDynamicState = &dynamicStateInfo;
		pipelineInfo.layout = m_pipelineLayout;
		pipelineInfo.renderPass = m_renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;
		ThrowIfFailed(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, Allocator(), &m_graphicsPipeline));
	}

	void Vulkan::CreateFrameBuffers()
	{
		const bool multiSampling = m_physicalDevice.MsaaSampleCount() != VK_SAMPLE_COUNT_1_BIT;
		m_swapchainFrameBuffers.resize(m_swapchainImageViews.size());

		VkImageView attachments[] = {
				m_colorImageView,
				m_depthImageView,
				VK_NULL_HANDLE
		};
		VkImageView& swapchainImageView = attachments[multiSampling ? 2 : 0];

		VkFramebufferCreateInfo frameBufferInfo{};
		frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferInfo.renderPass = m_renderPass;
		frameBufferInfo.attachmentCount = multiSampling ? 3 : 2;
		frameBufferInfo.pAttachments = attachments;
		frameBufferInfo.width = m_swapchainExtent.width;
		frameBufferInfo.height = m_swapchainExtent.height;
		frameBufferInfo.layers = 1;

		for (size_t i = 0; i < m_swapchainFrameBuffers.size(); ++i)
		{
			swapchainImageView = m_swapchainImageViews[i];
			ThrowIfFailed(vkCreateFramebuffer(m_device, &frameBufferInfo, Allocator(), &m_swapchainFrameBuffers[i]));
		}
	}

	void Vulkan::CreateCommandPool()
	{
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = m_physicalDevice.GraphicsQueueIndex();
		ThrowIfFailed(vkCreateCommandPool(m_device, &poolInfo, Allocator(), &m_commandPool));
	}

	void Vulkan::CreateCommandBuffers()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
		ThrowIfFailed(vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers));
	}

	void Vulkan::CreateSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			ThrowIfFailed(vkCreateSemaphore(m_device, &semaphoreInfo, Allocator(), &m_imageAvailableSemaphore[i]));
			ThrowIfFailed(vkCreateSemaphore(m_device, &semaphoreInfo, Allocator(), &m_renderFinishedSemaphore[i]));
			ThrowIfFailed(vkCreateFence(m_device, &fenceInfo, Allocator(), &m_inFlightFences[i]));
		}
	}

	void Vulkan::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) const
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;
		ThrowIfFailed(vkBeginCommandBuffer(commandBuffer, &beginInfo));

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_swapchainExtent.width);
		viewport.height = static_cast<float>(m_swapchainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_swapchainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		VkClearValue clearValues[2]{};
		clearValues[0].color = {{ 1.0f, 0.5f, 0.0f, 1.0f }};
		clearValues[1].depthStencil = { 1.0f, 0 };
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_renderPass;
		renderPassInfo.framebuffer = m_swapchainFrameBuffers[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_swapchainExtent;
		renderPassInfo.clearValueCount = ARRAY_SIZE(clearValues);
		renderPassInfo.pClearValues = clearValues;
		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
	}

	Vulkan::Vulkan(const char* name, GLFWwindow* window)
		: m_window{window}
		, m_graphicsQueue{VK_NULL_HANDLE}
		, m_presentQueue{VK_NULL_HANDLE}
		, m_physicalDevice{}
		, m_currentFrame{0}
		, m_imageIndex{}
		, m_resizeRequested{false}
	{
		CreateInstance(name);
		ThrowIfFailed(glfwCreateWindowSurface(m_instance, window, Allocator(), &m_surface));
		m_physicalDevice = SelectPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapchain();
		CreateSwapchainImageResources();
		CreateColorImageResources();
		CreateDepthResources();
		CreateRenderPass();
		CreateDescriptorSetLayout();
		CreateGraphicsPipeline();
		CreateFrameBuffers();
		CreateCommandPool();
		CreateCommandBuffers();
		CreateSyncObjects();
	}

	void Vulkan::RecreateSwapchain()
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(m_window, &width, &height);
		if (width && height)
		{
			vkDeviceWaitIdle(m_device);
			CleanupScreenResources();

			m_physicalDevice.UpdateSurfaceDetails(m_surface);
			CreateSwapchain();
			CreateSwapchainImageResources();
			CreateColorImageResources();
			CreateDepthResources();
			CreateFrameBuffers();
		}
	}

	bool Vulkan::BeginRender()
	{
		vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
		const VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_imageAvailableSemaphore[m_currentFrame], VK_NULL_HANDLE, &m_imageIndex);
		if (VK_ERROR_OUT_OF_DATE_KHR == result)
		{
			RecreateSwapchain();
			return false;
		}
		else
		{
			if (VK_SUBOPTIMAL_KHR != result)
				ThrowIfFailed(result);
		}

		vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);
		vkResetCommandBuffer(m_commandBuffers[m_currentFrame], 0);
		RecordCommandBuffer(m_commandBuffers[m_currentFrame], m_imageIndex);

		return true;
	}

	void Vulkan::EndRender()
	{
		vkCmdEndRenderPass(m_commandBuffers[m_currentFrame]);
		ThrowIfFailed(vkEndCommandBuffer(m_commandBuffers[m_currentFrame]));

		const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_imageAvailableSemaphore[m_currentFrame];
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_renderFinishedSemaphore[m_currentFrame];
		ThrowIfFailed(vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]));

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_renderFinishedSemaphore[m_currentFrame];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_swapchain;
		presentInfo.pImageIndices = &m_imageIndex;
		presentInfo.pResults = nullptr;

		const VkResult result = vkQueuePresentKHR(m_presentQueue, &presentInfo);
		if (m_resizeRequested || VK_ERROR_OUT_OF_DATE_KHR == result || VK_SUBOPTIMAL_KHR == result)
		{
			m_resizeRequested = false;
			RecreateSwapchain();
		}
		else
		{
			ThrowIfFailed(result);
		}

		m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	uint32_t Vulkan::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
	{
		VkPhysicalDeviceMemoryProperties memProperties{};
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice.Device(), &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		Throw("failed to find suitable memory type");
	}

	VkDeviceMemory Vulkan::AllocateMemory(VkImage image) const
	{
		VkDeviceMemory memory = VK_NULL_HANDLE;

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		ThrowIfFailed(vkAllocateMemory(m_device, &allocInfo, Allocator(), &memory));

		ThrowIfFailed(vkBindImageMemory(m_device, image, memory, 0));

		return memory;
	}
}
