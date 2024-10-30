#pragma once

#include "physicaldevice.hpp"

namespace democollection::vk
{
	constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

	class VulkanResources
	{
		VulkanResources(const VulkanResources&) = delete;
		VulkanResources(VulkanResources&&) = delete;
		void operator=(const VulkanResources&) = delete;
		void operator=(VulkanResources&&) = delete;

	protected:
		//VkAllocationCallbacks m_allocator;
		VkInstance m_instance;
#if VALIDATION_LAYER_ENABLED
		VkDebugUtilsMessengerEXT m_debugMessenger;
#endif
		VkSurfaceKHR m_surface;
		VkDevice m_device;
		VkSwapchainKHR m_swapchain;
		std::vector<VkImage> m_swapchainImages;
		std::vector<VkImageView> m_swapchainImageViews;
		std::vector<VkFramebuffer> m_swapchainFrameBuffers;
		VkImage m_colorImage;
		VkDeviceMemory m_colorImageMemory;
		VkImageView m_colorImageView;
		VkImage m_depthImage;
		VkDeviceMemory m_depthImageMemory;
		VkImageView m_depthImageView;
		VkRenderPass m_renderPass;
		VkDescriptorSetLayout m_descriptorSetLayout;
		VkPipelineLayout m_pipelineLayout;
		VkPipeline m_graphicsPipeline;
		VkCommandPool m_commandPool;
		VkCommandBuffer m_commandBuffers[MAX_FRAMES_IN_FLIGHT];
		VkSemaphore m_imageAvailableSemaphore[MAX_FRAMES_IN_FLIGHT];
		VkSemaphore m_renderFinishedSemaphore[MAX_FRAMES_IN_FLIGHT];
		VkFence m_inFlightFences[MAX_FRAMES_IN_FLIGHT];

	protected:
		VulkanResources();
		~VulkanResources();
		void CleanupScreenResources();
		inline VkAllocationCallbacks* Allocator() const { return nullptr; }
	};

	class Vulkan : private VulkanResources
	{
		class ShaderModule
		{
			const Vulkan& m_gfx;
			VkShaderModule m_shaderModule;

		public:
			ShaderModule(const Vulkan& gfx);
			ShaderModule(const ShaderModule&) = delete;
			ShaderModule(ShaderModule&& rhs);
			~ShaderModule();
			inline VkShaderModule* operator&() { return &m_shaderModule; }
			inline operator VkShaderModule() const { return m_shaderModule; }
		};

	private:
		GLFWwindow* m_window;
		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;
		PhysicalDevice m_physicalDevice;
		VkExtent2D m_swapchainExtent;
		uint32_t m_currentFrame;
		uint32_t m_imageIndex;
		bool m_resizeRequested;

	private:
		void CreateInstance(const char* name);
		PhysicalDevice SelectPhysicalDevice() const;
		void CreateLogicalDevice();
		uint32_t ChooseImageCount() const;
		VkExtent2D ChooseExtent() const;
		void CreateSwapchain();
		void CreateSwapchainImageResources();
		void CreateColorImageResources();
		void CreateDepthResources();
		void CreateRenderPass();
		void CreateDescriptorSetLayout();
		ShaderModule CreateShaderModule(const std::vector<char>& code) const;
		void CreateGraphicsPipeline();
		void CreateFrameBuffers();
		void CreateCommandPool();
		void CreateCommandBuffers();
		void CreateSyncObjects();

		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) const;

	public:
		Vulkan(const char* name, GLFWwindow* window);
		void RecreateSwapchain();
		bool BeginRender();
		void EndRender();

		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
		VkDeviceMemory AllocateMemory(VkImage image) const;

		inline void Flush() const { vkDeviceWaitIdle(m_device); }
		inline void RequestResize() { m_resizeRequested = true; }

		inline const PhysicalDevice& Gpu() const { return m_physicalDevice; }
		inline VkDevice Device() const { return m_device; }
		inline VkCommandPool CommandPool() const { return m_commandPool; }
		inline VkDescriptorSetLayout DescriptorSetLayout() const { return m_descriptorSetLayout; }
		inline VkPipelineLayout PipelineLayout() const { return m_pipelineLayout; }
		inline VkQueue Queue() const { return m_graphicsQueue; }
		inline VkCommandBuffer CommandBuffer() const { return m_commandBuffers[m_currentFrame]; }
		inline uint32_t CurrentFrame() const { return m_currentFrame; }
		inline VkAllocationCallbacks* Allocator() const { return VulkanResources::Allocator(); }
	};
}
