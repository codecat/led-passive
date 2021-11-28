#pragma once

#include <Common.h>

#include <lua.h>

namespace script
{
	void registerFunctions(lua_State* L);

	int setScene(lua_State* L);
	int nextScene(lua_State* L);
	int prevScene(lua_State* L);

	int setFps(lua_State* L);
	int resetFrameCounter(lua_State* L);

	int clear(lua_State* L);
	int set(lua_State* L);
}