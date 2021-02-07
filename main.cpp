#include <stdio.h>
#include <cassert>
#include "lua-5.4.2/src/lua.hpp"
#include "MCP3008/MCP3008.h"

#define CS_PIN 24
#define CLOCK_PIN 23
#define MOSI_PIN 19
#define MISO_PIN 21

MCP3008 adc("/dev/spidev0.0", SPI_MODE_0, 1000000, 8);

static int readADC(lua_State *L){
    if(lua_isnumber(L, 1))
    {
        int channel = (int)lua_tonumber(L, 1);
        int result = adc.readADC(channel);
        lua_pushnumber(L, result);
        return 1;
    }
    return 0;
}

static luaL_Reg ADCLib[] = {
    {"readADC", readADC},
    {NULL, NULL}
};

void set_aux_libs(lua_State *L)
{
    lua_newtable(L);
    luaL_setfuncs(L, ADCLib, 0);
    lua_setglobal(L, "ADC");
}

int main(void){
    lua_State *lua = luaL_newstate(); //lua interpreter
    luaL_openlibs(lua); //loads: basic, coroutine, package, string, table, math, io, os, debug
    set_aux_libs(lua);
    if(luaL_dofile(lua, "main.lua")==0)//if reading the file is successful
    {
        lua_getglobal(lua, "test"); //get the test global
        lua_call(lua, 0,0); //call it with 0 arguments and 0 return values
    }
    lua_close(lua);
    return 0;
}
