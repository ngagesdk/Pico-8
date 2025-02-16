/** @file api.c
 *
 *  A Pico-8 emulator for the Nokia N-Gage.
 *
 *  Copyright (c) 2025, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include <SDL3/SDL.h>
#include "z8lua/lauxlib.h"
#include "z8lua/lua.h"
#include "z8lua/trigtables.h"

/***
 * Math functions.
 */
static int pico8_abs(lua_State* L)
{
    float value = (float)lua_tonumber(L, 1);
    if (value == 0x8000)
    {
        // PICO-8 0.2.3 changelog: abs(0x8000) should be 0x7fff.ffff
        lua_pushnumber(L, 0x7fffffff);
    }
    else
    {
        lua_pushnumber(L, SDL_fabs(value));
    }
    return 1;
}

static int pico8_atan2(lua_State* L)
{
    return 0;
}

static int pico8_cos(lua_State* L)
{
    return 0;
}

static int pico8_flr(lua_State* L)
{
    return 0;
}

static int pico8_max(lua_State* L)
{
    float a = (float)lua_tonumber(L, 1);
    float b = (float)lua_tonumber(L, 2);
    lua_pushnumber(L, a > b ? a : b);
    return 1;
}

static int pico8_mid(lua_State* L)
{
    return 0;
}

static int pico8_min(lua_State* L)
{
    float a = (float)lua_tonumber(L, 1);
    float b = (float)lua_tonumber(L, 2);
    lua_pushnumber(L, a < b ? a : b);
    return 1;
}

static int pico8_rnd(lua_State* L)
{
    return 0;
}

static int pico8_sgn(lua_State* L)
{
    return 0;
}

static int pico8_sin(lua_State* L)
{
    return 0;
}

static int pico8_sqrt(lua_State* L)
{
    return 0;
}

static int pico8_srand(lua_State* L)
{
    return 0;
}

/***
 * Debug functions.
 */

static int pico8_SDL_log(lua_State* L)
{
    int n = lua_gettop(L);
    lua_getglobal(L, "tostring");
    for (int i = 1; i <= n; i++)
    {
        lua_pushvalue(L, -1);
        lua_pushvalue(L, i);
        lua_call(L, 1, 1);
        const char* s = lua_tostring(L, -1);
        if (s == NULL)
        {
            return luaL_error(L, "'tostring' must return a string to 'print'");
        }
        SDL_Log("%s", s);
        lua_pop(L, 1);
    }
    return 0;
}

void register_api(lua_State* L)
{
    // Math.
    lua_pushcfunction(L, pico8_abs);
    lua_setglobal(L, "abs");
    lua_pushcfunction(L, pico8_atan2);
    lua_setglobal(L, "atan2");
    lua_pushcfunction(L, pico8_cos);
    lua_setglobal(L, "cos");
    lua_pushcfunction(L, pico8_flr);
    lua_setglobal(L, "flr");
    lua_pushcfunction(L, pico8_max);
    lua_setglobal(L, "max");
    lua_pushcfunction(L, pico8_mid);
    lua_setglobal(L, "mid");
    lua_pushcfunction(L, pico8_min);
    lua_setglobal(L, "min");
    lua_pushcfunction(L, pico8_rnd);
    lua_setglobal(L, "rnd");
    lua_pushcfunction(L, pico8_sgn);
    lua_setglobal(L, "sgn");
    lua_pushcfunction(L, pico8_sin);
    lua_setglobal(L, "sin");
    lua_pushcfunction(L, pico8_sqrt);
    lua_setglobal(L, "sqrt");
    lua_pushcfunction(L, pico8_srand);
    lua_setglobal(L, "srand");

    // Debug.
    lua_pushcfunction(L, pico8_SDL_log);
    lua_setglobal(L, "SDL_log");
}
