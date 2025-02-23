//
//  ZEPTO-8 — Fantasy console emulator
//
//  Copyright © 2016–2024 Sam Hocevar <sam@hocevar.net>
//
//  This program is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//

#ifndef LPICO8LIB_H
#define LPICO8LIB_H

#include "lua.h"

#ifdef __cplusplus
extern "C" {
#endif

int pico8_max(lua_State *l);
int pico8_min(lua_State *l);
int pico8_mid(lua_State *l);
int pico8_ceil(lua_State *l);
int pico8_flr(lua_State *l);
int pico8_cos(lua_State *l);
int pico8_sin(lua_State *l);
int pico8_atan2(lua_State *l);
int pico8_sqrt(lua_State *l);
int pico8_abs(lua_State *l);
int pico8_sgn(lua_State *l);
int pico8_band(lua_State *l);
int pico8_bor(lua_State *l);
int pico8_bxor(lua_State *l);
int pico8_bnot(lua_State *l);
int pico8_shl(lua_State *l);
int pico8_lshr(lua_State *l);
int pico8_shr(lua_State *l);
int pico8_rotl(lua_State *l);
int pico8_rotr(lua_State *l);
int pico8_tostr(lua_State *l);
int pico8_tonum(lua_State *l);
int pico8_chr(lua_State *l);
int pico8_split(lua_State* l);
LUAMOD_API int luaopen_pico8(lua_State* L);

#ifdef __cplusplus
}
#endif

#endif // LPICO8LIB_H
