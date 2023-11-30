#include "frac.h"

// new frac_t
frac_t frac_new(int numerator, unsigned int denominator) {
	frac_t frac = { .numerator = numerator, .denominator = denominator };
	return frac;
}

// add frac1 to frac2
frac_t frac_add(frac_t frac1, frac_t frac2) {
	frac_t fracans = { .numerator = frac1.numerator * frac2.denominator + frac2.numerator * frac1.denominator, .denominator = frac1.denominator * frac2.denominator };
	return fracans;
}

// subtract frac2 from frac1
frac_t frac_minus(frac_t frac1, frac_t frac2) {
	frac_t fracans = { .numerator = frac1.numerator * frac2.denominator - frac2.numerator * frac1.denominator, .denominator = frac1.denominator * frac2.denominator };
	return fracans;
}

// multiply frac1 and frac2
frac_t frac_mult(frac_t frac1, frac_t frac2) {
	frac_t fracans = { .numerator = frac1.numerator * frac2.numerator, .denominator = frac1.denominator * frac2.denominator };
	return fracans;
}

// divide frac1 by frac2
frac_t frac_div(frac_t frac1, frac_t frac2) {
	frac_t fracans = { .numerator = frac1.numerator * frac2.denominator, .denominator = frac1.denominator * frac2.numerator };
	return fracans;
}

// take modulus m of frac
frac_t frac_mod(frac_t frac, unsigned int mod) {
	frac_t fracans = frac_simplify(frac);
	fracans.numerator %= ((int)mod)*fracans.denominator;
	return fracans;
}

// simplify frac
frac_t frac_simplify(frac_t frac) {
	frac_t fracans;

	int multiplier = 1;
	if (frac.numerator < 0) {
		multiplier = -1;
		frac.numerator *= -1;
	}

	int big, small, temp;

	if (frac.numerator == frac.denominator) {
		fracans.numerator = 1;
		fracans.denominator = 1;
	} else {
		if (frac.numerator > frac.denominator) {
			big = frac.numerator;
			small = frac.denominator;
		} else {
			big = frac.numerator;
			small = frac.denominator;
		}

		while (small != 0) {
			temp = small;
			small = big % temp;
			big = temp;
		}

		fracans.numerator = frac.numerator / big;
		fracans.denominator = frac.denominator / big;
	}

	fracans.numerator *= multiplier;

	return fracans;
}
