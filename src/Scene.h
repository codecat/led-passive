#pragma once

#include <Common.h>
#include <Config.h>
#include <Pixel.h>

#include <lua.h>

class Scene
{
public:
	s2::string m_name;

	PixelU8 m_clear;

	int m_luaUpdate = LUA_REFNIL;

public:
	Scene(cfg::block* block);
	~Scene();

	void setActive();
	void setInactive();

	void update(uint64_t frameCount);
};
