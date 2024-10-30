#include "image.hpp"

namespace democollection
{
	Image::Image()
		: m_pixels{nullptr}
		, m_width{0}
		, m_height{0}
	{}

	Image::Image(const Image& other)
		: Image(other.Width(), other.Height(), other.Pixels())
	{}

	Image::Image(Image&& other)
		: m_pixels{other.m_pixels}
		, m_width{other.m_width}
		, m_height{other.m_height}
	{
		other.m_pixels = nullptr;
		other.m_width = 0;
		other.m_height = 0;
	}

	Image::Image(uint32_t width, uint32_t height, const Color* pixels)
		: m_pixels{new Color[width * height]}
		, m_width{width}
		, m_height{height}
	{
		const uint32_t size = m_width * m_height;
		if (pixels)
		{
			for (uint32_t i = 0; i < size; ++i)
				m_pixels[i] = pixels[i];
		}
	}

	Image::~Image()
	{
		Reset();
	}

	Image& Image::operator=(const Image& lhs)
	{
		if (m_pixels)
			delete[] m_pixels;

		m_width = lhs.m_width;
		m_height = lhs.m_height;
		if (lhs.m_pixels)
		{
			const uint32_t size = m_width * m_height;
			m_pixels = new Color[size];
			for (uint32_t i = 0; i < size; ++i)
				m_pixels[i] = lhs.m_pixels[i];
		}
		else
		{
			m_pixels = nullptr;
		}
		return *this;
	}

	Image& Image::operator=(Image&& lhs)
	{
		if (m_pixels)
			delete[] m_pixels;
		m_pixels = lhs.m_pixels;
		m_width = lhs.m_width;
		m_height = lhs.m_height;
		lhs.m_pixels = nullptr;
		lhs.m_width = 0;
		lhs.m_height = 0;
		return *this;
	}

	void Image::Resize(uint32_t width, uint32_t height)
	{
		if (m_pixels)
			delete[] m_pixels;
		const uint32_t size = width * height;
		if (size)
		{
			m_pixels = new Color[size];
			m_width = width;
			m_height = height;
		}
		else
		{
			m_pixels = nullptr;
			m_width = 0;
			m_height = 0;
		}
	}

	void Image::Reset()
	{
		if (m_pixels)
		{
			delete[] m_pixels;
			m_pixels = nullptr;
			m_width = 0;
			m_height = 0;
		}
	}
}
