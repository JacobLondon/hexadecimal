#ifndef HD_MATH_H
#define HD_MATH_H

unsigned long long int_pow(unsigned long long base, unsigned int exp);
unsigned long gcd(unsigned long a, unsigned long b);
unsigned long lcm(unsigned long a, unsigned long b);
void swap(unsigned long *a, unsigned long *b);

bool is_little_endian(void);
bool is_big_endian(void);

#endif // HD_MATH_H
