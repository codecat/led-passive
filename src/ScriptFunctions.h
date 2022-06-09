#pragma once

#include <Common.h>

#include <lua.h>

namespace script
{
	void registerFunctions(lua_State* L);

	int dofile(lua_State* L);

	int getScene(lua_State* L);
	int setScene(lua_State* L);
	int nextScene(lua_State* L);
	int prevScene(lua_State* L);

	int getFps(lua_State* L);
	int setFps(lua_State* L);
	int resetFrameCounter(lua_State* L);

	int clear(lua_State* L);
	int set(lua_State* L);
	int add(lua_State* L);

	int hsv2rgb(lua_State* L);
}
