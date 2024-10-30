#include "vk/physicaldevice.hpp"

namespace democollection::vk
{
	constexpr uint32_t ACCEPTABLE_SCORE = 1;
	constexpr uint32_t COMMON_GFX_PRESENT_QUEUE_SCORE = 5;
	constexpr uint32_t DISCETE_GPU_SCORE = 20;
	constexpr uint32_t INTEGRATED_GPU_SCORE = 10;
	constexpr uint32_t VIRTUAL_GPU_SCORE = 5;
	constexpr uint32_t FORMAT_SCORE = 2;
	constexpr uint32_t PRESENT_MODE_SCORE = 3;
	constexpr uint32_t MULTI_SAMPLING_8_BIT_SCORE = 3;
	constexpr uint32_t MULTI_SAMPLING_4_BIT_SCORE = 2;
	constexpr uint32_t MULTI_SAMPLING_2_BIT_SCORE = 2;

	const char* PhysicalDevice::sDeviceExtensions[1] = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	bool PhysicalDevice::CheckDeviceExtensionSupport() const
	{
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(m_device, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(m_device, nullptr, &extensionCount, availableExtensions.data());

		for (const char* devExtension : PhysicalDevice::sDeviceExtensions)
		{
			bool extensionFound = false;
			for (const VkExtensionProperties& ext : availableExtensions)
			{
				if (0 == strcmp(devExtension, ext.extensionName))
				{
					extensionFound = true;
					break;
				}
			}
			if (!extensionFound)
				return false;
		}

		return true;
	}

	uint32_t PhysicalDevice::ScorePhysicalDeviceQueues(VkSurfaceKHR surface)
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_device, &queueFamilyCount, queueFamilies.data());

		m_graphicsQueueIndex = UINT32_MAX;
		m_presentQueueIndex = UINT32_MAX;
		for (uint32_t i = 0; i < queueFamilyCount; ++i)
		{
			uint32_t gfxQueueSupport = queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;
			VkBool32 presentQueueSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(m_device, i, surface, &presentQueueSupport);
			if (gfxQueueSupport && presentQueueSupport)
			{
				m_graphicsQueueIndex = i;
				m_presentQueueIndex = i;
				return COMMON_GFX_PRESENT_QUEUE_SCORE;
			}
			if (m_graphicsQueueIndex != UINT32_MAX && gfxQueueSupport)
				m_graphicsQueueIndex = i;
			if (m_presentQueueIndex != UINT32_MAX && presentQueueSupport)
				m_presentQueueIndex = i;
		}
		if (UINT32_MAX != m_graphicsQueueIndex && UINT32_MAX != m_presentQueueIndex)
			return ACCEPTABLE_SCORE;

		return 0;
	}

	uint32_t PhysicalDevice::ScoreSurfaceFormat(VkSurfaceKHR surface)
	{
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_device, surface, &formatCount, nullptr);
		if (!formatCount)
			return 0;
		std::vector<VkSurfaceFormatKHR> formats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_device, surface, &formatCount, formats.data());

		for (const VkSurfaceFormatKHR& f : formats)
		{
			if (VK_FORMAT_B8G8R8A8_SRGB == f.format && VK_COLOR_SPACE_SRGB_NONLINEAR_KHR == f.colorSpace)
			{
				m_surfaceFormat = f;
				return FORMAT_SCORE;
			}
		}

		m_surfaceFormat = formats[0];
		return ACCEPTABLE_SCORE;
	}

	uint32_t PhysicalDevice::ScoreDepthFormat()
	{
		const VkFormat formats[] = {
				VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
		};

		VkFormatProperties props;
		for (VkFormat f : formats)
		{
			vkGetPhysicalDeviceFormatProperties(m_device, f, &props);
			if ((props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				m_depthFormat = f;
				return ACCEPTABLE_SCORE;
			}
		}
		return 0;
	}

	uint32_t PhysicalDevice::ScorePresentMode(VkSurfaceKHR surface)
	{
		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_device, surface, &presentModeCount, nullptr);
		if (!presentModeCount)
			return 0;

		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_device, surface, &presentModeCount, presentModes.data());

		for (VkPresentModeKHR pm : presentModes)
		{
			if (VK_PRESENT_MODE_FIFO_KHR == pm)
			{
				m_presentMode = pm;
				return PRESENT_MODE_SCORE;
			}
		}
		m_presentMode = presentModes[0];
		return ACCEPTABLE_SCORE;
	}

	uint32_t PhysicalDevice::ScoreSwapchainSupport(VkSurfaceKHR surface)
	{
		const uint32_t formatScore = ScoreSurfaceFormat(surface);
		if (!formatScore)
			return 0;

		const uint32_t depthScore = ScoreDepthFormat();
		if (!depthScore)
			return 0;

		const uint32_t presentModeScore = ScorePresentMode(surface);
		if (!presentModeScore)
			return 0;

		return formatScore + presentModeScore;
	}

	uint32_t PhysicalDevice::ScoreMultiSampling()
	{
		VkSampleCountFlags counts = m_properties.limits.framebufferColorSampleCounts & m_properties.limits.framebufferDepthSampleCounts;
		if (counts & VK_SAMPLE_COUNT_8_BIT)
		{
			m_msaaSampleCount = VK_SAMPLE_COUNT_8_BIT;
			return MULTI_SAMPLING_8_BIT_SCORE;
		}
		if (counts & VK_SAMPLE_COUNT_4_BIT)
		{
			m_msaaSampleCount = VK_SAMPLE_COUNT_4_BIT;
			return MULTI_SAMPLING_4_BIT_SCORE;
		}
		if (counts & VK_SAMPLE_COUNT_2_BIT)
		{
			m_msaaSampleCount = VK_SAMPLE_COUNT_2_BIT;
			return MULTI_SAMPLING_2_BIT_SCORE;
		}

		m_msaaSampleCount = VK_SAMPLE_COUNT_1_BIT;
		return ACCEPTABLE_SCORE;
	}

	uint32_t PhysicalDevice::ScoreGpuType()
	{
		switch (m_properties.deviceType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			return DISCETE_GPU_SCORE;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			return INTEGRATED_GPU_SCORE;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			return VIRTUAL_GPU_SCORE;
		default:
			return ACCEPTABLE_SCORE;
		}
	}

	PhysicalDevice::PhysicalDevice(VkPhysicalDevice device)
		: m_device{device}
		, m_graphicsQueueIndex{}
		, m_presentQueueIndex{}
		, m_surfaceCapabilities{}
		, m_surfaceFormat{}
		, m_depthFormat{}
		, m_presentMode{}
		, m_msaaSampleCount{}
	{}

	uint32_t PhysicalDevice::ScoreDevice(VkSurfaceKHR surface)
	{
		vkGetPhysicalDeviceProperties(m_device, &m_properties);
		vkGetPhysicalDeviceFeatures(m_device, &m_features);
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device, surface, &m_surfaceCapabilities);

		if (!CheckDeviceExtensionSupport())
			return 0;

		const uint32_t queueScore = ScorePhysicalDeviceQueues(surface);
		if (!queueScore)
			return 0;

		const uint32_t swapchainScore = ScoreSwapchainSupport(surface);
		if (!swapchainScore)
			return 0;

		const uint32_t gpuTypeScore = ScoreGpuType();

		const uint32_t multiSamplingScore = ScoreMultiSampling();

		return queueScore + swapchainScore + gpuTypeScore + multiSamplingScore;
	}

	void PhysicalDevice::UpdateSurfaceDetails(VkSurfaceKHR surface)
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device, surface, &m_surfaceCapabilities);
	}
}
