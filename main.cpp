#include "./common.hpp"
#include "./globals.hpp"

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
        lua_register(this->L, "free", free);
        lua_register(this->L, "allocate", allocate);
        lua_register(this->L, "getPointer", get_pointer);
        lua_register(this->L, "sizeOf", size_of);
        lua_register(this->L, "exit", process_exit);
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
        exit(EXIT_FAILURE);
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