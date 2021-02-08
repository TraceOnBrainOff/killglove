#include <stdio.h>
#include <cassert>
#include "lua-5.4.2/src/lua.hpp"
#include "MCP3008/MCP3008.hpp"

MCP3008 adc("/dev/spidev0.0", SPI_MODE_0, 1000000, 8);

static int readChannel(lua_State *L){
    if(lua_isnumber(L, 1))
    {
        int channel = (int)lua_tonumber(L, 1);
        int result = adc.readChannel(channel);
        lua_pushnumber(L, result);
        return 1;
    }
    return 0;
}

bool lua_continue_update = true;
static int exitUpdateLoop(lua_State *L)
{
	lua_continue_update=false;
	return 0;
}

static luaL_Reg ADCLib[] = {
    {"readChannel", readChannel},
    {NULL, NULL}
};

void set_aux_libs(lua_State *L)
{
    lua_newtable(L);
    luaL_setfuncs(L, ADCLib, 0);
    lua_setglobal(L, "ADC");
    lua_getglobal(L, "os");
    lua_pushliteral(L, "leave");
    lua_pushcfunction(L, exitUpdateLoop);
    lua_settable(L, 1); 
}

int main(void)
{
    lua_State *lua = luaL_newstate(); //lua interpreter
    luaL_openlibs(lua); //loads: basic, coroutine, package, string, table, math, io, os, debug
    set_aux_libs(lua); //loads: ADC
    if(luaL_dofile(lua, "main.lua")==0)//if reading the file is successful
    {
        lua_getglobal(lua, "init"); //get the test global
        lua_call(lua, 0,0); //call it with 0 arguments and 0 return values
        while(lua_continue_update==true)
        {
            lua_getglobal(lua, "update"); //get the test global
            lua_call(lua, 0,0); //call it with 0 arguments and 0 return values
        }
	lua_getglobal(lua, "uninit");
	lua_call(lua,0,0);
    }
    printf("After dofile\n");
    lua_close(lua);
    return 0;
}
