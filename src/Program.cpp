#include <Program.h>
#include <Buffer.h>
#include <ScriptFunctions.h>

#include <lualib.h>
#include <luacode.h>

#if defined(PLATFORM_WINDOWS)
#include <Windows.h>
#else
#include <unistd.h>
#include <signal.h>
#endif

#include <chrono>

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

	while (m_running) {
		auto tmStart = std::chrono::high_resolution_clock::now();

		update();
		present();

		auto tmEnd = std::chrono::high_resolution_clock::now();
		auto frameDuration = std::chrono::duration_cast<std::chrono::microseconds>(tmEnd - tmStart);
		float frameTime = frameDuration.count() / 1000.0f;

		float fpsTargetSleep = 1000.0f / m_fps;

		if (frameTime > fpsTargetSleep) {
			printf("LONG FRAME TIME: %f ms\n", frameTime);
		}

		float sleepMilliseconds = fpsTargetSleep - frameTime;
#if defined(PLATFORM_WINDOWS)
		Sleep((DWORD)sleepMilliseconds);
#else
		usleep((useconds_t)(sleepMilliseconds * 1000));
#endif
	}

	printf("Clean shutdown\n");
}

Scene* Program::getScene(const char* name)
{
	for (auto scene : m_scenes) {
		if (scene->m_name == name) {
			return scene;
		}
	}
	return nullptr;
}

void Program::setCurrentScene(const char* name)
{
	auto scene = getScene(name);
	if (scene == nullptr) {
		return;
	}
	setCurrentScene(scene);
}

void Program::setCurrentScene(Scene* scene)
{
	if (m_currentScene != nullptr) {
		m_currentScene->setInactive();
	}
	m_currentScene = scene;
	if (m_currentScene != nullptr) {
		printf("New active scene: \"%s\"\n", m_currentScene->m_name.c_str());
		m_currentScene->setActive();
	}
}

void Program::advanceCurrentScene(int dir)
{
	if (m_scenes.len() == 0) {
		return;
	}

	int index = m_scenes.indexof(m_currentScene);
	if (index == -1) {
		index = 0;
	}

	index = (index + dir) % (int)m_scenes.len();
	setCurrentScene(m_scenes[index]);
}

bool Program::loadLuaFile(const char* filename)
{
	FILE* fh = fopen(filename, "rb");
	if (fh == nullptr) {
		printf("[LUA] Unable to open file \"%s\"\n", filename);
		return false;
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
		return false;
	}

	return true;
}

int Program::runLuaFile(const char* filename)
{
	if (!loadLuaFile(filename)) {
		return LUA_REFNIL;
	}

	int r = lua_pcall(m_lua, 0, 1, 0);
	if (r != 0) {
		const char* err = lua_tostring(m_lua, -1);
		printf("[LUA] %s\n", err);
		lua_pop(m_lua, 1);
		return LUA_REFNIL;
	}

	return lua_ref(m_lua, -1);
}

bool Program::initialize()
{
#if defined(PLATFORM_WINDOWS)
	SetConsoleCtrlHandler([](DWORD dwCtrlType) -> BOOL {
		printf("Interrupt signal received (%d), stopping..\n", (int)dwCtrlType);
		Program::instance->m_running = false;
		return TRUE;
	}, TRUE);
#else
	signal(SIGINT, [](int) {
		printf("Interrupt signal received, stopping..\n");
		Program::instance->m_running = false;
	});
	signal(SIGTERM, [](int) {
		printf("Termination signal received, stopping..\n");
		Program::instance->m_running = false;
	});
#endif

	m_config = cfg::fromFile("config");
	if (m_config == nullptr) {
		printf("Unable to open config file\n");
		return false;
	}

	const char* portName = m_config->getString("port");
	if (portName == nullptr) {
#if defined(PLATFORM_WINDOWS)
		const char* defaultPortName = "COM1";
#else
		const char* defaultPortName = "/dev/ttyACM0";
#endif
		printf("Missing \"port\" name, try something like \"%s\"\n", defaultPortName);
		return false;
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

	if (!initializeScripting()) {
		return false;
	}

	// Load scenes
	auto blockScenes = m_config->getBlock("scenes");
	if (blockScenes != nullptr) {
		int numScenes = blockScenes->getBlockCount();
		for (int i = 0; i < numScenes; i++) {
			auto blockScene = blockScenes->getBlock(i);
			auto newScene = new Scene(blockScene);
			m_scenes.add(newScene);
			printf("Added scene: \"%s\"\n", newScene->m_name.c_str());
		}
	}

	// Set default scene
	setCurrentScene("default");

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

bool Program::initializeScripting()
{
	// Register all script functions
	script::registerFunctions(m_lua);

	return true;
}

void Program::update()
{
	if (m_luaMainUpdate != LUA_REFNIL) {
		lua_rawgeti(m_lua, LUA_REGISTRYINDEX, m_luaMainUpdate);
		lua_pushinteger(m_lua, (int)m_frameCount);
		if (lua_pcall(m_lua, 1, 0, 0) != 0) {
			const char* err = lua_tostring(m_lua, -1);
			printf("[LUA] %s\n", err);
			lua_pop(m_lua, 1);

			lua_unref(m_lua, m_luaMainUpdate);
			m_luaMainUpdate = LUA_REFNIL;
		}
	}

	if (m_currentScene != nullptr) {
		m_currentScene->update(m_frameCount);
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
