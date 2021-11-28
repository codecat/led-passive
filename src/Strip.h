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
};
