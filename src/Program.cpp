#include <Program.h>
#include <Buffer.h>

#include <unistd.h>

Program* Program::instance = nullptr;

Program::Program()
{
}

Program::~Program()
{
	m_serial.close();
}

void Program::run()
{
	if (!initialize()) {
		printf("Initialization failed\n");
		return;
	}

	while (true) {
		update();
		render();

		usleep((1000 / m_fps) * 1000);
	}
}

bool Program::initialize()
{
	m_config = cfg::fromFile("config");
	if (m_config == nullptr) {
		printf("Unable to open config file\n");
		return false;
	}

	const char* portName = m_config->getString("port");
	if (portName == nullptr) {
		portName = "/dev/ttyACM0";
		printf("Missing port name, using default (\"%s\")\n", portName);
	}

	m_fps = m_config->getInt("fps", m_fps);
	m_strips = m_config->getInt("strips", m_strips);
	m_pixels = m_config->getInt("pixels", m_pixels);

	printf("Serial port: \"%s\"\n", portName);
	printf("FPS: %d\n", m_fps);
	printf("Strips: %d\n", m_strips);
	printf("Pixels: %d\n", m_pixels);

	m_serial.setPort(portName);
	m_serial.open();
	if (!m_serial.isOpen()) {
		printf("Unable to open serial port\n");
		return false;
	}

	printf("Serial port opened\n");

	// Set init state
	auto clear = m_config->getBlock("init");
	if (clear != nullptr) {
		int r = clear->getInt("r");
		int g = clear->getInt("g");
		int b = clear->getInt("b");

		for (int i = 0; i < m_pixels; i++) {
			PixelU8 p;
			p.r = r;
			p.g = g;
			p.b = b;
			m_pixelBuffer.Write(p);
		}
		render();
	}

	return true;
}

void Program::update()
{
	m_pixelBuffer.Clear();

	// Set some random pixels
	for (int i = 0; i < m_pixels; i++) {
		uint64_t n = i + m_frameCount;

		PixelU8 p;
		if (n % (uint64_t)m_pixels < m_pixels / 2) {
			p.r = 255;
			p.g = 0;
			p.b = 0;
		} else {
			p.r = 0;
			p.g = 0;
			p.b = 0;
		}
		m_pixelBuffer.Write(p);
	}

	m_frameCount++;
}

void Program::render()
{
	if (!m_serial.isOpen()) {
		return;
	}

	for (int i = 0; i < m_strips; i++) {
		m_serialBuffer.Clear();

		m_serialBuffer.Write<uint8_t>(i); // Channel
		m_serialBuffer.Write<uint8_t>(0); // 8 bit pixels
		uint16_t bufferSize = (uint16_t)m_pixelBuffer.GetSize();
		m_serialBuffer.Write<uint8_t>((bufferSize & 0xFF00) >> 8); // Data size high byte
		m_serialBuffer.Write<uint8_t>(bufferSize & 0xFF); // Data size low byte
		m_serialBuffer.Write(m_pixelBuffer.GetBuffer(), m_pixelBuffer.GetSize()); // Data

		size_t written = m_serial.write((const uint8_t*)m_serialBuffer.GetBuffer(), m_serialBuffer.GetSize());
		m_serial.flush();

		if (written == 0) {
			printf("Wrote 0 bytes to serial!\n");
		}
	}
}
