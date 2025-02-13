#include <unordered_map>

#include "./common.hpp"

static void process_exit()
{
    exit(EXIT_FAILURE);
}

static int get_memory_address(lua_State* L)
{
    const void* ptr = lua_topointer(L, 1);
    const uintptr_t value = reinterpret_cast<uintptr_t>(ptr);
    lua_pushinteger(L, value);
    
    return 1;
}

static std::unordered_map<std::string, int> type_sizes = {
    {"number", 8},
    {"boolean", 1},
    {"string", 4},
    {"userdata", 4},
    {"table", 4},
    {"function", 4},
    {"thread", 4}
};

static int size_of(lua_State* L)
{
    const char* type_name = lua_tostring(L, -1);
    for (const auto& [key, value] : type_sizes) {
        if (std::strcmp(key.c_str(), type_name) != 0) continue;
        
        lua_pushinteger(L, value); // cause all numbers are 64-bit in lua
        return 1;
    }
    
    return luaL_error(L, "sizeOf expects a type string, got value %s", type_name);
}

class LowLua
{
    lua_State* L;
    int argc;
    char** argv;

public:
    LowLua(const int argc, char** argv)
    {
        this->L = luaL_newstate();
        this->argc = argc;
        this->argv = argv;
    }

    void do_file(const std::string& file_path)
    {
        if (luaL_dofile(this->L, file_path.c_str()) != LUA_OK)
            this->error();
    }

    void do_string(const std::string& str)
    {
        if (luaL_dostring(this->L, str.c_str()) != LUA_OK)
            this->error();
    }

    void open_custom_libs() const
    {
        lua_register(this->L, "getMemoryAddress", get_memory_address);
        lua_register(this->L, "sizeOf", size_of);
    }

    void open_default_libs() const
    {
        luaL_openlibs(this->L);
    }

    void close() const
    {
        lua_close(this->L);
    }

    void error(const std::string& msg = "")
    {
        if (msg.empty())
            puts(lua_tostring(this->L, this->top()));
        else
            puts(("[LowLua] " + msg).c_str());

        this->pop_top();
        process_exit();
    }

    int top() const
    {
        return lua_gettop(this->L);
    }

    void pop_top() const
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

    const char* file_directory = argv[1];
    lua.open_default_libs();
    lua.open_custom_libs();
    lua.do_file(file_directory);
    lua.close();
    
    return EXIT_SUCCESS;
}