#ifndef FRAC_H
#define FRAC_H

typedef struct {
	int numerator;
	int denominator;
} frac_t;

frac_t frac_new(int, unsigned int);

frac_t frac_add(frac_t, frac_t);
frac_t frac_minus(frac_t, frac_t);
frac_t frac_mult(frac_t, frac_t);
frac_t frac_div(frac_t, frac_t);
frac_t frac_mod(frac_t, unsigned int);

frac_t frac_simplify(frac_t);

#endif
