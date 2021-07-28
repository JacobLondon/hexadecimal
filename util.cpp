#include <stdint.h>
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
