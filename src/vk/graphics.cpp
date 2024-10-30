#include "vk/graphics.hpp"

namespace democollection::vk
{
	Graphics::Graphics(const char* name, GLFWwindow* window)
		: Vulkan{name, window}
	{}

	std::shared_ptr<Texture> Graphics::LoadTexture(const std::string& name)
	{
		if (auto texIter = m_loadedTextures.find(name); texIter != m_loadedTextures.end())
			if (std::shared_ptr<Texture> ptr = texIter->second.lock())
				return ptr;

		std::shared_ptr<Texture> tex = std::make_shared<Texture>(*this, name.c_str());
		m_loadedTextures[name] = tex;
		return tex;
	}

	void Graphics::TextureCleanup()
	{
		auto iter = m_loadedTextures.cbegin();
		while (iter != m_loadedTextures.cend())
		{
			if (iter->second.expired())
				iter = m_loadedTextures.erase(iter);
			else
				++iter;
		}
	}
}
