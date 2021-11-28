#include <Scene.h>
#include <Program.h>

Scene::Scene(cfg::block* block)
{
	m_name = block->getName();

	auto scriptPath = block->getString("script");
	if (scriptPath != nullptr) {
		m_luaUpdate = Program::instance->runLuaFile(scriptPath);
	}

	m_clear.r = (uint8_t)block->getInt("r");
	m_clear.g = (uint8_t)block->getInt("g");
	m_clear.b = (uint8_t)block->getInt("b");
}

Scene::~Scene()
{
	if (m_luaUpdate != LUA_REFNIL) {
		lua_unref(Program::instance->m_lua, m_luaUpdate);
	}
}

void Scene::setActive()
{
	for (auto strip : Program::instance->m_strips) {
		strip->clear(m_clear.r, m_clear.g, m_clear.b);
	}
}

void Scene::setInactive()
{
}

void Scene::update(uint64_t frameCount)
{
	lua_State* L = Program::instance->m_lua;

	if (m_luaUpdate != LUA_REFNIL) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, m_luaUpdate);
		lua_pushinteger(L, (int)frameCount);
		if (lua_pcall(L, 1, 0, 0) != 0) {
			const char* err = lua_tostring(L, -1);
			printf("[LUA] %s\n", err);
			lua_pop(L, 1);

			lua_unref(L, m_luaUpdate);
			m_luaUpdate = LUA_REFNIL;
		}
	}
}
