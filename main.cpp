#include "./common.hpp"

void process_exit()
{
    exit(EXIT_FAILURE);
}

int get_memory_address(lua_State* L)
{
    const void* ptr = lua_topointer(L, 1);
    uintptr_t ptrValue = reinterpret_cast<uintptr_t>(ptr);
    lua_pushinteger(L, static_cast<lua_Integer>(ptrValue));
    return 1;
}

int size_of(lua_State* L)
{
    auto argType = lua_type(L, 1);

    if (argType == LUA_TNUMBER)
    {
        lua_pushinteger(L, 8); // cause all numbers are 64-bit in lua
        return 1;
    }
    else if (argType == LUA_TBOOLEAN)
    {
        lua_pushinteger(L, 1); // 1 byte for a boolean, weird
        return 1;
    }
    else if (argType == LUA_TSTRING || argType == LUA_TTABLE || argType == LUA_TFUNCTION || argType == LUA_TTABLE || argType == LUA_TLIGHTUSERDATA || argType == LUA_TTHREAD)
    {
        lua_pushinteger(L, 4); // all pointers internally
        return 1;
    }
    else if (argType == LUA_TUSERDATA)
    {
        auto length = lua_rawlen(L, 1);
        lua_pushinteger(L, length);
        return 1;
    }
    else if (argType == LUA_TNIL)
    {
        lua_pushinteger(L, 0);
        return 1;
    }
    else
    {
        return luaL_error(L, "unsupported sizeOf value");
    }
}

class LowLua
{
private:
    lua_State* L;
    int argc;
    char** argv;

public:
    LowLua(int argc, char** argv)
    {
        this->L = luaL_newstate();
        this->argc = argc;
        this->argv = argv;
    }

    void do_file(std::string filePath)
    {
        if (luaL_dofile(this->L, filePath.c_str()) != LUA_OK)
            this->error();
    }

    void do_string(std::string str)
    {
        if (luaL_dostring(this->L, str.c_str()) != LUA_OK)
            this->error();
    }

    void open_libs()
    {
        lua_register(this->L, "getMemoryAddress", get_memory_address);
        lua_register(this->L, "sizeOf", size_of);
    }

    void open_default_libs()
    {
        luaL_openlibs(this->L);
    }

    void close()
    {
        lua_close(this->L);
    }

    void error(std::string msg = "")
    {
        if (msg.empty())
            puts(lua_tostring(this->L, this->top()));
        else
            puts(("[Luay] " + msg).c_str());

        this->pop_top();
        process_exit();
    }

    int top()
    {
        return lua_gettop(this->L);
    }

    void pop_top()
    {
        lua_pop(this->L, this->top());
    }
};

int main(int argc, char** argv)
{
    LowLua lua(argc, argv);

    if (argc <= 1)
    {
        puts("Usage: lowlua <file>\n");
        return EXIT_FAILURE;
    }
    else
    {
        char* fileDir = argv[1];
        lua.open_default_libs();
        lua.open_libs();
        lua.do_file(fileDir);
    }

    lua.close();
    return EXIT_SUCCESS;
}