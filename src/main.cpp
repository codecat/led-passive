#include <Common.h>
#include <Buffer.h>

#include <serial/serial.h>

#include <unistd.h>

struct PixelU8
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

serial::Serial _serial;

static void display_buffer(uint8_t channel, const Buffer& bufData)
{
	if (!_serial.isOpen()) {
		return;
	}

	Buffer buf;
	buf.Write<uint8_t>(channel); // Channel
	buf.Write<uint8_t>(0); // 8 bit pixels
	uint16_t bufferSize = (uint16_t)bufData.GetSize();
	buf.Write<uint8_t>((bufferSize & 0xFF00) >> 8); // Data size high byte
	buf.Write<uint8_t>(bufferSize & 0xFF); // Data size low byte
	buf.Write(bufData.GetBuffer(), bufData.GetSize()); // Data

	size_t written = _serial.write((const uint8_t*)buf.GetBuffer(), buf.GetSize());
	_serial.flush();

	if (written == 0) {
		printf("Wrote 0 bytes to serial!\n");
	}
}

int main()
{
	_serial.setPort("/dev/ttyACM0");

	_serial.open();
	if (!_serial.isOpen()) {
		printf("Unable to open serial port\n");
		return 1;
	}

	printf("Serial port opened\n");

	int frameCount = 0;

	while (true) {
		Buffer bufData;
		for (int i = 0; i < 300; i++) {
			int n = i + frameCount;

			PixelU8 p;
			if (n % 300 < 150) {
				p.r = (n % 6 < 5 ? 255 : 0);
				p.g = (n % 6 < 3 ? 255 : 0);
				p.b = (n % 6 < 1 ? 255 : 0);
			} else {
				p.r = (n % 6 < 1 ? 255 : 0);
				p.g = (n % 6 < 3 ? 255 : 0);
				p.b = (n % 6 < 5 ? 255 : 0);
			}
			bufData.Write(p);
		}

		display_buffer(0, bufData);
		display_buffer(1, bufData);

		// 30 fps
		usleep(30 * 1000);
		frameCount = (frameCount + 1) % 300;
	}

	_serial.close();
	return 0;
}
