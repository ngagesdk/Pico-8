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

#ifndef FIX32_H
#define FIX32_H

#include <stdint.h> // int32_t, int64_t, …
#include <math.h> // std::abs

struct fix32
{
    // Member variable
    int32_t m_bits;

    // Constructors
    inline fix32() : m_bits(0) {}

    // Convert from/to double
    inline fix32(double d)
      : m_bits(int32_t(int64_t(d * 65536.0)))
    {}

    inline operator double() const
    {
        return double(m_bits) * (1.0 / 65536.0);
    }

    // Conversions up to int16_t are safe.
    inline fix32(int8_t x)  : m_bits(int32_t(x << 16)) {}
    inline fix32(uint8_t x) : m_bits(int32_t(x << 16)) {}
    inline fix32(int16_t x) : m_bits(int32_t(x << 16)) {}

    // Anything above int16_t is risky because of precision loss, but Lua
    // does too many implicit conversions from int that we can’t mark this
    // one as explicit.
    inline fix32(int32_t x)  : m_bits(int32_t(x << 16)) {}

    inline fix32(uint16_t x) : m_bits(int32_t(x << 16)) {}
    inline fix32(uint32_t x) : m_bits(int32_t(x << 16)) {}
    inline fix32(int64_t x)  : m_bits(int32_t(x << 16)) {}
    inline fix32(uint64_t x) : m_bits(int32_t(x << 16)) {}
    inline fix32(long int x) : m_bits(int32_t(x << 16)) {}
    inline fix32(long unsigned int x) : m_bits(int32_t(x << 16)) {}

    // Explicit casts are all allowed
    inline operator int8_t()   const { return m_bits >> 16; }
    inline operator uint8_t()  const { return m_bits >> 16; }
    inline operator int16_t()  const { return m_bits >> 16; }
    inline operator uint16_t() const { return m_bits >> 16; }
    inline operator int32_t()  const { return m_bits >> 16; }
    inline operator uint32_t() const { return m_bits >> 16; }
    inline operator int64_t()  const { return m_bits >> 16; }
    inline operator uint64_t() const { return m_bits >> 16; }
    inline operator long int() const { return static_cast<long int>(m_bits >> 16); }
    inline operator long unsigned int() const { return static_cast<long unsigned int>(m_bits >> 16); }

    // Directly initialise bits
    static inline fix32 frombits(int32_t x)
    {
        fix32 ret; ret.m_bits = x; return ret;
    }

    inline int32_t bits() const { return m_bits; }

    // Comparisons
    inline operator bool() const { return bool(m_bits); }
    inline bool operator ==(fix32 x) const { return m_bits == x.m_bits; }
    inline bool operator !=(fix32 x) const { return m_bits != x.m_bits; }
    inline bool operator  <(fix32 x) const { return m_bits  < x.m_bits; }
    inline bool operator  >(fix32 x) const { return m_bits  > x.m_bits; }
    inline bool operator <=(fix32 x) const { return m_bits <= x.m_bits; }
    inline bool operator >=(fix32 x) const { return m_bits >= x.m_bits; }

    // Increments
    inline fix32& operator ++() { m_bits += 0x10000; return *this; }
    inline fix32& operator --() { m_bits -= 0x10000; return *this; }
    inline fix32 operator ++(int) { fix32 ret = *this; ++*this; return ret; }
    inline fix32 operator --(int) { fix32 ret = *this; --*this; return ret; }

    // Math operations
    inline fix32 const &operator +() const { return *this; }
    inline fix32 operator -() const { return frombits(-m_bits); }
    inline fix32 operator ~() const { return frombits(~m_bits); }

    inline fix32 operator +(fix32 x) const { return frombits(m_bits + x.m_bits); }
    inline fix32 operator -(fix32 x) const { return frombits(m_bits - x.m_bits); }
    inline fix32 operator &(fix32 x) const { return frombits(m_bits & x.m_bits); }
    inline fix32 operator |(fix32 x) const { return frombits(m_bits | x.m_bits); }
    inline fix32 operator ^(fix32 x) const { return frombits(m_bits ^ x.m_bits); }

    fix32 operator *(fix32 x) const
    {
        return frombits(int64_t(m_bits) * x.m_bits >> 16);
    }

    fix32 operator /(fix32 x) const
    {
        // This special case ensures 0x8000/0x1 = 0x8000, not 0x8000.0001
        if (x.m_bits == 0x10000) {
            return *this;
        }

        if (x.m_bits)
        {
            int64_t result = int64_t(m_bits) * 0x10000 / x.m_bits;
            if (int64_t(abs(result)) <= 0x7fffffff) {
                return frombits(int32_t(result));
            }
        }

        // Return 0x8000.0001 (not 0x8000.0000) for -Inf, just like PICO-8
        return frombits((m_bits ^ x.m_bits) >= 0 ? 0x7fffffff : 0x80000001);
    }

    fix32 operator %(fix32 x) const
    {
        // PICO-8 always returns positive values
        x = abs(x);
        // PICO-8 0.2.5f changelog: x % 0 gives 0 (was x)
        int32_t result = x ? m_bits % x.m_bits : 0;
        return frombits(result >= 0 ? result : result + x.m_bits);
    }

    inline fix32 operator <<(int y) const
    {
        // If y is negative, use lshr() instead.
        return y < 0 ? lshr(*this, -y) : frombits(y >= 32 ? 0 : bits() << y);
    }

    inline fix32 operator >>(int y) const
    {
        // If y is negative, use << instead.
        return y < 0 ? *this << -y : frombits(bits() >> (y >= 31 ? 31 : y));
    }

    inline fix32& operator +=(fix32 x) { return *this = *this + x; }
    inline fix32& operator -=(fix32 x) { return *this = *this - x; }
    inline fix32& operator &=(fix32 x) { return *this = *this & x; }
    inline fix32& operator |=(fix32 x) { return *this = *this | x; }
    inline fix32& operator ^=(fix32 x) { return *this = *this ^ x; }
    inline fix32& operator *=(fix32 x) { return *this = *this * x; }
    inline fix32& operator /=(fix32 x) { return *this = *this / x; }
    inline fix32& operator %=(fix32 x) { return *this = *this % x; }

    // Free functions

    // PICO-8 0.2.3 changelog: abs(0x8000) should be 0x7fff.ffff
    static inline fix32 abs(fix32 a) { return a.m_bits >= 0 ? a : a.m_bits << 1 == 0 ? ~a : -a; }

    static inline fix32 min(fix32 a, fix32 b) { return a < b ? a : b; }
    static inline fix32 max(fix32 a, fix32 b) { return a > b ? a : b; }

    static inline fix32 ceil(fix32 x) { return -floor(-x); }
    static inline fix32 floor(fix32 x) { return frombits(x.m_bits & 0xffff0000); }

    static fix32 pow(fix32 x, fix32 y) { return fix32(std::pow(double(x), double(y))); }

    static inline fix32 lshr(fix32 x, int y)
    {
        // If y is negative, use << instead.
        return y < 0 ? x << -y : frombits(y >= 32 ? 0 : uint32_t(x.bits()) >> y);
    }

    static inline fix32 rotl(fix32 x, int y)
    {
        y &= 0x1f;
        return frombits((x.bits() << y) | (uint32_t(x.bits()) >> (32 - y)));
    }

    static inline fix32 rotr(fix32 x, int y)
    {
        y &= 0x1f;
        return frombits((uint32_t(x.bits()) >> y) | (x.bits() << (32 - y)));
    }
};

#endif // FIX32_H
