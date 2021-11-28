#include <ScriptFunctions.h>

#include <Program.h>

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
			auto& p = s->m_pixels[pixel];
			p.r = r;
			p.g = g;
			p.b = b;
		}
		return 0;
	}

	if (strip < 0 || strip >= (int)Program::instance->m_strips.len()) {
		lua_pushstring(L, "strip index out of range");
		lua_error(L);
		return 0;
	}

	auto s = Program::instance->m_strips[strip];
	auto& p = s->m_pixels[pixel];
	p.r = r;
	p.g = g;
	p.b = b;

	return 0;
}
