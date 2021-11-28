#pragma once

#include <Common.h>
#include <Config.h>
#include <Pixel.h>
#include <Buffer.h>

#include <serial/serial.h>

class Program
{
public:
	static Program* instance;

public:
	cfg::block* m_config = nullptr;
	serial::Serial m_serial;

	int m_fps = 30;
	int m_strips = 1;
	int m_pixels = 32;

	uint64_t m_frameCount = 0;

	Buffer m_pixelBuffer;
	Buffer m_serialBuffer;

public:
	Program();
	~Program();

	void run();

private:
	bool initialize();
	void update();
	void render();
};
