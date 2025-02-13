#pragma once
#include "./common.hpp"

int process_exit(lua_State* L);
int get_pointer(lua_State* L);
int size_of(lua_State* L);
int allocate(lua_State* L);
int free(lua_State* L);