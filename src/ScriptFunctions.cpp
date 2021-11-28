#include <ScriptFunctions.h>

#include <Program.h>

void script::registerFunctions(lua_State* L)
{
	// set_scene(name)
	lua_pushcfunction(L, script::setScene, "set_scene");
	lua_setglobal(L, "set_scene");

	// next_scene()
	lua_pushcfunction(L, script::nextScene, "next_scene");
	lua_setglobal(L, "next_scene");

	// prev_scene()
	lua_pushcfunction(L, script::prevScene, "prev_scene");
	lua_setglobal(L, "prev_scene");

	// reset_frame_counter()
	lua_pushcfunction(L, script::resetFrameCounter, "reset_frame_counter");
	lua_setglobal(L, "reset_frame_counter");

	// set_fps(fps)
	lua_pushcfunction(L, script::setFps, "set_fps");
	lua_setglobal(L, "set_fps");

	// clear(r, g, b)
	// clear(strip, r, g, b)
	lua_pushcfunction(L, script::clear, "clear");
	lua_setglobal(L, "clear");

	// set(pixel, r, g, b)
	// set(strip, pixel, r, g, b)
	lua_pushcfunction(L, script::set, "set");
	lua_setglobal(L, "set");
}

int script::setScene(lua_State* L)
{
	const char* name = lua_tostring(L, 1);
	if (name == nullptr) {
		lua_pushstring(L, "no scene name given");
		lua_error(L);
		return 0;
	}

	auto scene = Program::instance->getScene(name);
	if (scene == nullptr) {
		lua_pushstring(L, "scene with name not found");
		lua_error(L);
		return 0;
	}

	Program::instance->setCurrentScene(name);

	return 0;
}

int script::nextScene(lua_State* L)
{
	Program::instance->advanceCurrentScene(1);
	return 0;
}

int script::prevScene(lua_State* L)
{
	Program::instance->advanceCurrentScene(-1);
	return 0;
}

int script::setFps(lua_State* L)
{
	int fps = lua_tointeger(L, 1);
	if (fps < 1) {
		lua_pushstring(L, "fps must be at least 1 or higher");
		lua_error(L);
		return 0;
	}

	Program::instance->m_fps = fps;
	return 0;
}

int script::resetFrameCounter(lua_State* L)
{
	Program::instance->m_frameCount = 0;
	return 0;
}

int script::clear(lua_State* L)
{
	int strip = -1;
	uint8_t r, g, b;

	int n = lua_gettop(L);
	if (n == 4) {
		strip = lua_tointeger(L, 1);
		r = (uint8_t)lua_tointeger(L, 2);
		g = (uint8_t)lua_tointeger(L, 3);
		b = (uint8_t)lua_tointeger(L, 4);

	} else if (n == 3) {
		r = (uint8_t)lua_tointeger(L, 1);
		g = (uint8_t)lua_tointeger(L, 2);
		b = (uint8_t)lua_tointeger(L, 3);

	} else {
		lua_pushstring(L, "invalid number of parameters");
		lua_error(L);
		return 0;
	}

	if (strip == -1) {
		for (auto s : Program::instance->m_strips) {
			s->clear(r, g, b);
		}
		return 0;
	}

	if (strip < 0 || strip >= (int)Program::instance->m_strips.len()) {
		lua_pushstring(L, "strip index out of range");
		lua_error(L);
		return 0;
	}

	Program::instance->m_strips[strip]->clear(r, g, b);

	return 0;
}

int script::set(lua_State* L)
{
	int strip = -1;
	int pixel;
	uint8_t r, g, b;

	int n = lua_gettop(L);
	if (n == 5) {
		strip = lua_tointeger(L, 1);
		pixel = lua_tointeger(L, 2);
		r = (uint8_t)lua_tointeger(L, 3);
		g = (uint8_t)lua_tointeger(L, 4);
		b = (uint8_t)lua_tointeger(L, 5);

	} else if (n == 4) {
		pixel = lua_tointeger(L, 1);
		r = (uint8_t)lua_tointeger(L, 2);
		g = (uint8_t)lua_tointeger(L, 3);
		b = (uint8_t)lua_tointeger(L, 4);

	} else {
		lua_pushstring(L, "invalid number of parameters");
		lua_error(L);
		return 0;
	}

	if (strip == -1) {
		for (auto s : Program::instance->m_strips) {
			s->set(pixel, r, g, b);
		}
		return 0;
	}

	if (strip < 0 || strip >= (int)Program::instance->m_strips.len()) {
		lua_pushstring(L, "strip index out of range");
		lua_error(L);
		return 0;
	}

	auto s = Program::instance->m_strips[strip];
	s->set(pixel, r, g, b);

	return 0;
}
