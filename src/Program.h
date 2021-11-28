#pragma once

#include <Common.h>
#include <Config.h>
#include <Buffer.h>
#include <Strip.h>
#include <Scene.h>

#include <serial/serial.h>

#include <lua.h>

class Program
{
public:
	static Program* instance;

public:
	bool m_running = true;

	cfg::block* m_config = nullptr;
	serial::Serial m_serial;

	lua_State* m_lua;
	int m_luaMainUpdate = LUA_REFNIL;

	int m_fps = 30;

	s2::list<Strip*> m_strips;

	s2::list<Scene*> m_scenes;
	Scene* m_currentScene = nullptr;

	uint64_t m_frameCount = 0;
	Buffer m_serialBuffer;

public:
	Program();
	~Program();

	void run();

	Scene* getScene(const char* name);
	void setCurrentScene(const char* name);
	void setCurrentScene(Scene* scene);
	void advanceCurrentScene(int dir = 1);

	int runLuaFile(const char* filename);

private:
	bool initialize();
	bool initializeScripting();

	void update();
	void present();
};
