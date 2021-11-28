#pragma once

#include <Common.h>
#include <Config.h>

#include <Pixel.h>

class Strip
{
public:
	PixelU8* m_pixels;
	size_t m_numPixels;
	size_t m_bufferSize;

public:
	Strip(cfg::block* block);
	~Strip();

	void clear(uint8_t r, uint8_t g, uint8_t b);

	inline void set(int pixel, uint8_t r, uint8_t g, uint8_t b)
	{
		auto& p = m_pixels[pixel];
		p.r = r;
		p.g = g;
		p.b = b;
	}
};
