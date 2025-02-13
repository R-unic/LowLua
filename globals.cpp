#include <unordered_map>
#include "./common.hpp"
#include "./globals.hpp"

int allocate(lua_State* L)
{
    if (!luaL_checkinteger(L, 1))
        return luaL_error(L, "expected amount of bytes to allocate");
    
    const auto size = static_cast<uintptr_t>(lua_tointeger(L, 1));
    const void* ptr = std::malloc(size);
    if (ptr == nullptr)
        return luaL_error(L, "allocation failed, out of memory");

    lua_pushinteger(L, reinterpret_cast<uintptr_t>(ptr));
    return 1;
}

int free(lua_State* L)
{
    if (!luaL_checkinteger(L, 1))
        return luaL_error(L, "expected memory address to free");

    auto block = (void*)lua_topointer(L, 1);
    std::free(block);
    block = nullptr;
    lua_pushnil(L);
    
    return 1;
}

int process_exit(lua_State* L)
{
    const long long exit_code = luaL_optinteger(L, 1, 0);
    exit(static_cast<int>(exit_code));
    return 1; // never reached, but required for lua function signature
}

int get_pointer(lua_State* L)
{
    const uintptr_t ptr = reinterpret_cast<uintptr_t>(lua_topointer(L, 1));
    lua_pushinteger(L, ptr);

    return 1;
}

static std::unordered_map<std::string, int> type_sizes = {
    {"number", 8}, // cause all numbers are 64-bit in lua
    {"boolean", 1},
    {"string", 4}, // pointers from here on out
    {"userdata", 4},
    {"table", 4},
    {"function", 4},
    {"thread", 4}
};

int size_of(lua_State* L)
{
    const char* type_name = lua_tostring(L, -1);
    for (const auto& [key, value] : type_sizes) {
        if (std::strcmp(key.c_str(), type_name) != 0) continue;
        
        lua_pushinteger(L, value);
        return 1;
    }
    
    return luaL_error(L, "sizeOf expects a type string, got value %s", type_name);
}