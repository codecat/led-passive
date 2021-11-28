#include <Program.h>
#include <Buffer.h>
#include <ScriptFunctions.h>

#include <lualib.h>
#include <luacode.h>

#include <unistd.h>

Program* Program::instance = nullptr;

Program::Program()
{
	m_lua = luaL_newstate();
	luaL_openlibs(m_lua);
}

Program::~Program()
{
	for (auto strip : m_strips) {
		delete strip;
	}

	m_serial.close();

	delete m_config;

	if (m_luaMainUpdate != LUA_REFNIL) {
		lua_unref(m_lua, m_luaMainUpdate);
	}
	lua_close(m_lua);
}

void Program::run()
{
	if (!initialize()) {
		printf("Initialization failed\n");
		return;
	}

	while (true) {
		update();
		present();

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

	auto strips = m_config->getBlock("strips");
	if (strips != nullptr) {
		for (int i = 0; i < strips->getBlockCount(); i++) {
			auto blockStrip = strips->getBlock(i);
			auto newStrip = new Strip(blockStrip);
			m_strips.add(newStrip);
		}
	}

	printf("Serial port: \"%s\"\n", portName);
	printf("FPS: %d\n", m_fps);
	printf("Strips: %d\n", (int)m_strips.len());

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
		uint8_t r = (uint8_t)clear->getInt("r");
		uint8_t g = (uint8_t)clear->getInt("g");
		uint8_t b = (uint8_t)clear->getInt("b");

		for (auto strip : m_strips) {
			strip->clear(r, g, b);
		}

		present();
	}

	return initializeScripting();
}

bool Program::initializeScripting()
{
	// clear(r, g, b)
	// clear(strip, r, g, b)
	lua_pushcfunction(m_lua, script::clear, "clear");
	lua_setglobal(m_lua, "clear");

	// set(pixel, r, g, b)
	// set(strip, pixel, r, g, b)
	lua_pushcfunction(m_lua, script::set, "set");
	lua_setglobal(m_lua, "set");

	// Run main script if defined
	const char* mainScriptName = m_config->getString("script");
	if (mainScriptName != nullptr) {
		if (m_luaMainUpdate != LUA_REFNIL) {
			lua_unref(m_lua, m_luaMainUpdate);
			m_luaMainUpdate = LUA_REFNIL;
		}

		printf("Running main script \"%s\"\n", mainScriptName);
		m_luaMainUpdate = runLuaFile(mainScriptName);
	}

	return true;
}

void Program::update()
{
	/*
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
	*/

	if (m_luaMainUpdate != LUA_REFNIL) {
		lua_rawgeti(m_lua, LUA_REGISTRYINDEX, m_luaMainUpdate);
		if (lua_pcall(m_lua, 0, 0, 0) != 0) {
			const char* err = lua_tostring(m_lua, -1);
			printf("[LUA] %s\n", err);
			lua_pop(m_lua, 1);

			lua_unref(m_lua, m_luaMainUpdate);
			m_luaMainUpdate = LUA_REFNIL;
		}
	}

	m_frameCount++;
}

void Program::present()
{
	if (!m_serial.isOpen()) {
		return;
	}

	for (size_t i = 0; i < m_strips.len(); i++) {
		auto strip = m_strips[i];

		m_serialBuffer.Clear();

		m_serialBuffer.Write<uint8_t>(i); // Channel
		m_serialBuffer.Write<uint8_t>(0); // 8 bit pixels
		uint16_t bufferSize = (uint16_t)strip->m_bufferSize;
		m_serialBuffer.Write<uint8_t>((bufferSize & 0xFF00) >> 8); // Data size high byte
		m_serialBuffer.Write<uint8_t>(bufferSize & 0xFF); // Data size low byte
		m_serialBuffer.Write(strip->m_pixels, strip->m_bufferSize); // Data

		size_t written = m_serial.write((const uint8_t*)m_serialBuffer.GetBuffer(), m_serialBuffer.GetSize());
		m_serial.flush();

		if (written == 0) {
			printf("Wrote 0 bytes to serial!\n");
		}
	}
}

int Program::runLuaFile(const char* filename)
{
	FILE* fh = fopen(filename, "rb");
	if (fh == nullptr) {
		printf("[LUA] Unable to open file \"%s\"\n", filename);
		return LUA_REFNIL;
	}

	fseek(fh, 0, SEEK_END);
	long sourceSize = ftell(fh);
	fseek(fh, 0, SEEK_SET);
	char* source = (char*)malloc(sourceSize);
	fread(source, 1, sourceSize, fh);
	fclose(fh);

	size_t bytecodeSize;
	char* bytecode = luau_compile(source, sourceSize, nullptr, &bytecodeSize);
	free(source);

	int r = luau_load(m_lua, filename, bytecode, bytecodeSize, 0);
	free(bytecode);

	if (r != 0) {
		const char* err = lua_tostring(m_lua, -1);
		printf("[LUA] %s\n", err);
		lua_pop(m_lua, 1);
		return LUA_REFNIL;
	}

	r = lua_pcall(m_lua, 0, 1, 0);
	if (r != 0) {
		const char* err = lua_tostring(m_lua, -1);
		printf("[LUA] %s\n", err);
		lua_pop(m_lua, 1);
		return LUA_REFNIL;
	}

	printf("top a = %d\n", lua_gettop(m_lua));
	int ref = lua_ref(m_lua, LUA_REGISTRYINDEX);
	printf("top b = %d\n", lua_gettop(m_lua));
	return ref;
}
