#pragma once

#include <cstdint>

namespace democollection
{
	struct Color
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	class Image
	{
		Color* m_pixels;
		uint32_t m_width;
		uint32_t m_height;

	public:
		Image();
		Image(const Image& other);
		Image(Image&& other);
		Image(uint32_t width, uint32_t height, const Color* pixels = nullptr);
		~Image();

		Image& operator=(const Image& lhs);
		Image& operator=(Image&& lhs);

		void Resize(uint32_t width, uint32_t height);
		void Reset();

		inline const Color& Pixel(uint32_t x, uint32_t y) const { return m_pixels[y * m_width + x]; }
		inline Color& Pixel(uint32_t x, uint32_t y) { return m_pixels[y * m_width + x]; }
		inline uint32_t Width() const { return m_width; }
		inline uint32_t Height() const { return m_height; }
		inline const Color* Pixels() const { return m_pixels; }
	};
}
