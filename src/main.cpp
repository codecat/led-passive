#include <Common.h>
#include <Program.h>

#include <lua.h>
#include <lualib.h>
#include <luacode.h>

int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	const char* source = "return \"test\"";
	size_t sourceSize = strlen(source);

	size_t bytecodeSize;
	char* bytecode = luau_compile(source, sourceSize, nullptr, &bytecodeSize);
	luau_load(L, "test", bytecode, bytecodeSize, 0);
	lua_call(L, 0, 1);

	printf("type = %d\n", lua_type(L, -1));

	int r = lua_ref(L, LUA_REGISTRYINDEX);
	lua_rawgeti(L, LUA_REGISTRYINDEX, r);

	printf("type = %d\n", lua_type(L, -1));

	lua_close(L);

	/*
	srand((unsigned int)time(nullptr));

	Program::instance = new Program;
	Program::instance->run();
	delete Program::instance;
	*/

	return 0;
}
