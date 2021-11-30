#pragma once

#include <Common.h>
#include <Config.h>

#include <Pixel.h>
#include <Easing.h>

class Strip
{
public:
	PixelU8* m_pixels;
	size_t m_numPixels;
	size_t m_bufferSize;

	easing::type m_pixelCurve = easing::type::linear;

public:
	Strip(cfg::block* block);
	~Strip();

	void clear(uint8_t r, uint8_t g, uint8_t b);

	inline void set(int pixel, uint8_t r, uint8_t g, uint8_t b)
	{
		if (pixel < 0 || (size_t)pixel >= m_numPixels) {
			return;
		}
		auto& p = m_pixels[pixel];
		p.r = r;
		p.g = g;
		p.b = b;
		transform(p);
	}

	inline void add(int pixel, uint8_t r, uint8_t g, uint8_t b)
	{
		if (pixel < 0 || (size_t)pixel >= m_numPixels) {
			return;
		}
		transform(r, g, b);
		auto& p = m_pixels[pixel];
		p.r = add_u8(p.r, r);
		p.g = add_u8(p.g, g);
		p.b = add_u8(p.b, b);
	}

	inline void transform(PixelU8& p)
	{
		transform(p.r, p.g, p.b);
	}

	inline void transform(uint8_t& r, uint8_t& g, uint8_t& b)
	{
		if (m_pixelCurve == easing::type::linear) {
			return;
		}
		r = (uint8_t)(easing::transform(m_pixelCurve, r / 255.0f) * 255.0f);
		g = (uint8_t)(easing::transform(m_pixelCurve, g / 255.0f) * 255.0f);
		b = (uint8_t)(easing::transform(m_pixelCurve, b / 255.0f) * 255.0f);
	}

private:
	static inline uint8_t add_u8(uint8_t x, uint8_t y)
	{
		uint16_t r = (uint16_t)x + y;
		if (r > 255) {
			return 255;
		}
		return (uint8_t)r;
	}
};
