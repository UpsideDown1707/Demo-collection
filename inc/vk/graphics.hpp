#pragma once

#include "texture.hpp"

namespace democollection::vk
{
	class Graphics : public Vulkan
	{
		std::map<std::string, std::weak_ptr<Texture>> m_loadedTextures;

	public:
		Graphics(const char* name, GLFWwindow* window);

		std::shared_ptr<Texture> LoadTexture(const std::string& name);
		void TextureCleanup();
	};
}
