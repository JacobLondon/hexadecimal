#include <stdint.h>
#include <stdio.h>
#include "util.hpp"

/**
 * From Linux kernel int_pow.c
 * 
 * int_pow - computes the exponentiation of the given base and exponent
 * @base: base which will be raised to the given power
 * @exp: power to be raised to
 *
 * Computes: pow(base, exp), i.e. @base raised to the @exp power
 */
unsigned long long int_pow(unsigned long long base, unsigned int exp)
{
	unsigned long long result = 1;

	while (exp) {
		if (exp & 1)
			result *= base;
		exp >>= 1;
		base *= base;
	}

	return result;
}

/* If normalization is done by loops, the even/odd algorithm is a win. */
unsigned long gcd(unsigned long a, unsigned long b)
{
	unsigned long r = a | b;

	if (!a || !b)
		return r;

	/* Isolate lsbit of r */
	r &= -r;

	while (!(b & r))
		b >>= 1;
	if (b == r)
		return r;

	for (;;) {
		while (!(a & r))
			a >>= 1;
		if (a == r)
			return r;
		if (a == b)
			return a;

		if (a < b)
			swap(&a, &b);
		a -= b;
		a >>= 1;
		if (a & r)
			a += b;
		a >>= 1;
	}
}

/* Lowest common multiple */
unsigned long lcm(unsigned long a, unsigned long b)
{
	if (a && b)
		return (a / gcd(a, b)) * b;
	else
		return 0;
}

void swap(unsigned long *a, unsigned long *b)
{
	unsigned long tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

bool is_little_endian(void)
{
	const uint16_t value = 0x01;
	const void *const address = (const void *const)(void *)&value;
	const unsigned char *const least = (const unsigned char *const)(void *)(address);
	return (*least == 0x01);
}

bool is_big_endian(void)
{
	return !is_little_endian();
}

const char *ascii_lookup(int chr) noexcept {
    static bool firstTime = true;
    static char buf[4] = {0};
    static const char *table[128];

    if (firstTime) {
        firstTime = false;
        table[0] = "NUL"; // null
        table[1] = "SOH"; // start of heading
        table[2] = "STX"; // start of text
        table[3] = "ETX"; // end of text
        table[4] = "EOT"; // end of transmission
        table[5] = "ENQ"; // enquiry
        table[6] = "ACK"; // acknowledge
        table[7] = "\\a"; // BEL, ALERT, bell
        table[8] = "\\b"; // BS, backspace
        table[9] = "\\t"; // TAB, horizontal tab
        table[10] = "\\n"; // LF, line feed
        table[11] = "\\v"; // VT, vertical tab
        table[12] = "FF"; // NP, form feed, new page
        table[13] = "\\r"; // CR, carriage return
        table[14] = "SO"; // shift out
        table[15] = "SI"; // shift in
        table[16] = "DLE"; // data link escape
        table[17] = "DC1"; // device control 1
        table[18] = "DC2"; // device control 2
        table[19] = "DC3"; // device control 3
        table[20] = "DC4"; // device control 4
        table[21] = "NAK"; // negative acknowledge
        table[22] = "SYN"; // synchronous idle
        table[23] = "ETB"; // end of transmission block
        table[24] = "CAN"; // cancel
        table[25] = "EM"; // end of medium
        table[26] = "SUB"; // substitute
        table[27] = "\\e"; // ESC, escape
        table[28] = "FS"; // file seperator
        table[29] = "GS"; // group seperator
        table[30] = "RS"; // record seperator
        table[31] = "US"; // unit seperator
        table[32] = "SPACE"; // space
        table[127] = "DEL"; // delete
    }

    if (chr < 0 || chr > 255) {
        return "";
    }

    // less printable
    if ((0 <= chr && chr <= 32) || chr == 127) {
        return table[chr];
    }

    // printable
    snprintf(buf, sizeof(buf), "%c", chr);
    return buf;
}
