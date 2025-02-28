/*
** $Id: lobject.c,v 2.58 2013/02/20 14:08:56 roberto Exp $
** Some generic functions over Lua objects
** See Copyright Notice in lua.h
*/

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define lobject_c
#define LUA_CORE

#include "lua.h"

#include "lctype.h"
#include "ldebug.h"
#include "ldo.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "lvm.h"



LUAI_DDEF const TValue luaO_nilobject_ = {NILCONSTANT};


/*
** converts an integer to a "floating point byte", represented as
** (eeeeexxx), where the real value is (1xxx) * 2^(eeeee - 1) if
** eeeee != 0 and (xxx) otherwise.
*/
int luaO_int2fb (unsigned int x) {
  int e = 0;  /* exponent */
  if (x < 8) return x;
  while (x >= 0x10) {
    x = (x+1) >> 1;
    e++;
  }
  return ((e+1) << 3) | (cast_int(x) - 8);
}


/* converts back */
int luaO_fb2int (int x) {
  int e = (x >> 3) & 0x1f;
  if (e == 0) return x;
  else return ((x & 7) + 8) << (e - 1);
}


int luaO_ceillog2 (unsigned int x) {
  static const lu_byte log_2[256] = {
    0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
  };
  int l = 0;
  x--;
  while (x >= 256) { l += 8; x >>= 8; }
  return l + log_2[x];
}


lua_Number luaO_arith (lua_State *L, int op, lua_Number v1, lua_Number v2) {
  switch (op) {
    case LUA_OPADD: return luai_numadd(NULL, v1, v2);
    case LUA_OPSUB: return luai_numsub(NULL, v1, v2);
    case LUA_OPMUL: return luai_nummul(NULL, v1, v2);
    case LUA_OPDIV: return luai_numdiv(NULL, v1, v2);
    case LUA_OPMOD: return luai_nummod(NULL, v1, v2);
    case LUA_OPPOW: return luai_numpow(NULL, v1, v2);
    case LUA_OPIDIV: return luai_numidiv(NULL, v1, v2);
    case LUA_OPBAND: return luai_numband(NULL, v1, v2);
    case LUA_OPBOR:  return luai_numbor(NULL, v1, v2);
    case LUA_OPBXOR: return luai_numbxor(NULL, v1, v2);
    case LUA_OPSHL:  return luai_numshl(NULL, v1, v2);
    case LUA_OPSHR:  return luai_numshr(NULL, v1, v2);
    case LUA_OPLSHR: return luai_numlshr(NULL, v1, v2);
    case LUA_OPROTL: return luai_numrotl(NULL, v1, v2);
    case LUA_OPROTR: return luai_numrotr(NULL, v1, v2);
    case LUA_OPUNM:  return luai_numunm(NULL, v1);
    case LUA_OPBNOT: return luai_numbnot(NULL, v1);
    case LUA_OPPEEK: return luai_numpeek(L, v1);
    case LUA_OPPEEK2: return luai_numpeek2(L, v1);
    case LUA_OPPEEK4: return luai_numpeek4(L, v1);
    default: lua_assert(0); return 0;
  }
}


int luaO_hexavalue (int c) {
  if (lisdigit(c)) return c - '0';
  else return ltolower(c) - 'a' + 10;
}


static int isneg (const char **s) {
  if (**s == '-') { (*s)++; return 1; }
  else if (**s == '+') (*s)++;
  return 0;
}


#if false && !defined(lua_strx2number)

#include <math.h>


static lua_Number readhexa (const char **s, lua_Number r, int *count) {
  for (; lisxdigit(cast_uchar(**s)); (*s)++) {  /* read integer part */
    r = (r * cast_num(16.0)) + cast_num(luaO_hexavalue(cast_uchar(**s)));
    (*count)++;
  }
  return r;
}


/*
** convert an hexadecimal numeric string to a number, following
** C99 specification for 'strtod'
*/
static lua_Number lua_strx2number (const char *s, char **endptr) {
  lua_Number r = 0.0;
  int e = 0, i = 0;
  int neg = 0;  /* 1 if number is negative */
  *endptr = cast(char *, s);  /* nothing is valid yet */
  while (lisspace(cast_uchar(*s))) s++;  /* skip initial spaces */
  neg = isneg(&s);  /* check signal */
  if (!(*s == '0' && (*(s + 1) == 'x' || *(s + 1) == 'X')))  /* check '0x' */
    return 0.0;  /* invalid format (no '0x') */
  s += 2;  /* skip '0x' */
  r = readhexa(&s, r, &i);  /* read integer part */
  if (*s == '.') {
    s++;  /* skip dot */
    r = readhexa(&s, r, &e);  /* read fractional part */
  }
  if (i == 0 && e == 0)
    return 0.0;  /* invalid format (no digit) */
  e *= -4;  /* each fractional digit divides value by 2^-4 */
  *endptr = cast(char *, s);  /* valid up to here */
  if (*s == 'p' || *s == 'P') {  /* exponent part? */
    int exp1 = 0;
    int neg1;
    s++;  /* skip 'p' */
    neg1 = isneg(&s);  /* signal */
    if (!lisdigit(cast_uchar(*s)))
      goto ret;  /* must have at least one digit */
    while (lisdigit(cast_uchar(*s)))  /* read exponent */
      exp1 = exp1 * 10 + *(s++) - '0';
    if (neg1) exp1 = -exp1;
    e += exp1;
  }
  *endptr = cast(char *, s);  /* valid up to here */
 ret:
  if (neg) r = -r;
  return l_mathop(ldexp)(r, e);
}

#endif


/*
** convert an hexadecimal or binary numeric string to a number
*/
static lua_Number lua_strany2number (const char *s, char **endptr, int base) {
  uint32_t fixed_value = 0;
  uint32_t fractional_value = 0;
  const char* frac_part;
  double r = 0.0;
  int neg = 0;  /* 1 if number is negative */
  *endptr = cast(char *, s);  /* nothing is valid yet */

  while (lisspace(cast_uchar(*s))) s++; /* skip initial spaces */
  neg = isneg(&s);  /* check sign */

  /* Validate prefix */
  if (*s != '0' || (base == 2 && *(s + 1) != 'b' && *(s + 1) != 'B')
                || (base == 16 && *(s + 1) != 'x' && *(s + 1) != 'X'))
    return 0;  /* invalid format */

  s += 2;  /* Skip '0x' or '0b' */

  /* Look for the fractional point */
  frac_part = strchr(s, '.');

  /* Process integer part */
  fixed_value = (uint32_t)strtoul(s, endptr, base);
  if (s == *endptr) return 0;  /* No valid integer part */

  /* Process fractional part if present */
  if (frac_part) {
    int frac_digits;
    uint32_t scale_factor;
    s = frac_part + 1;
    fractional_value = (uint32_t)strtoul(s, endptr, base);

    /* Determine the scaling factor */
    frac_digits = *endptr - s;
    scale_factor = 1;
    while (frac_digits-- > 0) scale_factor *= base;

    /* Scale fractional part to fixed-point */
    r = (double)fixed_value + ((double)fractional_value / scale_factor);
  } else {
    r = (double)fixed_value;
  }

  /* Convert to 16:16 fixed point */
  fixed_value = (uint32_t)(r * 65536.0);

  return fix32_from_double(neg ? -r : r);
}


int luaO_str2d (const char *s, size_t len, lua_Number *result) {
  char *endptr;
  if (strpbrk(s, "nN"))  /* reject 'inf' and 'nan' */
    return 0;
  else if (strpbrk(s, "xX"))  /* hexa? */
    *result = lua_strany2number(s, &endptr, 16);
  else if (strpbrk(s, "bB"))  /* binary? */
    *result = lua_strany2number(s, &endptr, 2);
  else
    *result = lua_str2number(s, &endptr);
  if (endptr == s) return 0;  /* nothing recognized */
  while (lisspace(cast_uchar(*endptr))) endptr++;
  return (endptr == s + len);  /* OK if no trailing characters */
}



static void pushstr (lua_State *L, const char *str, size_t l) {
  setsvalue2s(L, L->top++, luaS_newlstr(L, str, l));
}


/* this function handles only `%d', `%c', %f, %p, and `%s' formats */
const char *luaO_pushvfstring (lua_State *L, const char *fmt, va_list argp) {
  int n = 0;
  for (;;) {
    const char *e = strchr(fmt, '%');
    if (e == NULL) break;
    luaD_checkstack(L, 2);  /* fmt + item */
    pushstr(L, fmt, e - fmt);
    switch (*(e+1)) {
      case 's': {
        const char *s = va_arg(argp, char *);
        if (s == NULL) s = "(null)";
        pushstr(L, s, strlen(s));
        break;
      }
      case 'c': {
        char buff;
        buff = cast(char, va_arg(argp, int));
        pushstr(L, &buff, 1);
        break;
      }
      case 'd': {
        setnvalue(L->top++, cast_num(va_arg(argp, int)));
        break;
      }
      case 'f': {
        setnvalue(L->top++, cast_num(va_arg(argp, l_uacNumber)));
        break;
      }
      case 'p': {
        char buff[4*sizeof(void *) + 8]; /* should be enough space for a `0x%llx' */
        int l = sprintf(buff, "0x%llx", (long long int)(intptr_t)va_arg(argp, void *));
        pushstr(L, buff, l);
        break;
      }
      case '%': {
        pushstr(L, "%", 1);
        break;
      }
      default: {
        luaG_runerror(L,
            "invalid option " LUA_QL("%%%c") " to " LUA_QL("lua_pushfstring"),
            *(e + 1));
      }
    }
    n += 2;
    fmt = e+2;
  }
  luaD_checkstack(L, 1);
  pushstr(L, fmt, strlen(fmt));
  if (n > 0) luaV_concat(L, n + 1);
  return svalue(L->top - 1);
}


const char *luaO_pushfstring (lua_State *L, const char *fmt, ...) {
  const char *msg;
  va_list argp;
  va_start(argp, fmt);
  msg = luaO_pushvfstring(L, fmt, argp);
  va_end(argp);
  return msg;
}


/* number of chars of a literal string without the ending \0 */
#define LL(x)	(sizeof(x)/sizeof(char) - 1)

#define RETS	"..."
#define PRE	"[string \""
#define POS	"\"]"

#define addstr(a,b,l)	( memcpy(a,b,(l) * sizeof(char)), a += (l) )

void luaO_chunkid (char *out, const char *source, size_t bufflen) {
  size_t l = strlen(source);
  if (*source == '=') {  /* 'literal' source */
    if (l <= bufflen)  /* small enough? */
      memcpy(out, source + 1, l * sizeof(char));
    else {  /* truncate it */
      addstr(out, source + 1, bufflen - 1);
      *out = '\0';
    }
  }
  else if (*source == '@') {  /* file name */
    if (l <= bufflen)  /* small enough? */
      memcpy(out, source + 1, l * sizeof(char));
    else {  /* add '...' before rest of name */
      addstr(out, RETS, LL(RETS));
      bufflen -= LL(RETS);
      memcpy(out, source + 1 + l - bufflen, bufflen * sizeof(char));
    }
  }
  else {  /* string; format as [string "source"] */
    const char *nl = strchr(source, '\n');  /* find first new line (if any) */
    addstr(out, PRE, LL(PRE));  /* add prefix */
    bufflen -= LL(PRE RETS POS) + 1;  /* save space for prefix+suffix+'\0' */
    if (l < bufflen && nl == NULL) {  /* small one-line source? */
      addstr(out, source, l);  /* keep it */
    }
    else {
      if (nl != NULL) l = nl - source;  /* stop at first newline */
      if (l > bufflen) l = bufflen;
      addstr(out, source, l);
      addstr(out, RETS, LL(RETS));
    }
    memcpy(out, POS, (LL(POS) + 1) * sizeof(char));
  }
}

