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

	int numBlocks = block->getBlockCount();
	for (int i = 0; i < numBlocks; i++) {
		auto blockStrip = block->getBlock(i);
		auto& clearStrip = m_clearStrips.add();
		clearStrip.r = (uint8_t)blockStrip->getInt("r");
		clearStrip.g = (uint8_t)blockStrip->getInt("g");
		clearStrip.b = (uint8_t)blockStrip->getInt("b");
	}
}

Scene::~Scene()
{
	if (m_luaUpdate != LUA_REFNIL) {
		lua_unref(Program::instance->m_lua, m_luaUpdate);
	}
}

void Scene::setActive()
{
	for (size_t i = 0; i < Program::instance->m_strips.len(); i++) {
		auto strip = Program::instance->m_strips[i];

		PixelU8 clear = m_clear;
		if (i < m_clearStrips.len()) {
			clear = m_clearStrips[i];
		}

		strip->clear(clear.r, clear.g, clear.b);
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
