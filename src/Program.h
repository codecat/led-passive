#pragma once

#include <Common.h>
#include <Config.h>
#include <Buffer.h>
#include <Strip.h>

#include <serial/serial.h>

#include <lua.h>

class Program
{
public:
	static Program* instance;

public:
	cfg::block* m_config = nullptr;
	serial::Serial m_serial;

	lua_State* m_lua;
	int m_luaMainUpdate = LUA_REFNIL;

	int m_fps = 30;

	s2::list<Strip*> m_strips;

	uint64_t m_frameCount = 0;
	Buffer m_serialBuffer;

public:
	Program();
	~Program();

	void run();

private:
	bool initialize();
	bool initializeScripting();

	void update();
	void present();

	int runLuaFile(const char* filename);
};
