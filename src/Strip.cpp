#include <Strip.h>

Strip::Strip(cfg::block* block)
{
	int numPixels = block->getInt("pixels", 32);

	m_bufferSize = numPixels * sizeof(PixelU8);
	m_pixels = (PixelU8*)malloc(m_bufferSize);
	m_numPixels = numPixels;

	int r = block->getInt("r");
	int g = block->getInt("g");
	int b = block->getInt("b");
	clear(r, g, b);
}

Strip::~Strip()
{
	free(m_pixels);
}

void Strip::clear(uint8_t r, uint8_t g, uint8_t b)
{
	for (size_t i = 0; i < m_numPixels; i++) {
		auto& p = m_pixels[i];
		p.r = r;
		p.g = g;
		p.b = b;
	}
}
