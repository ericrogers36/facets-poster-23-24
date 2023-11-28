#ifndef POLY_H
#define POLY_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "frac.h"

#define MAX(a,b) (a >= b ? a : b)
#define MIN(a,b) (a <= b ? a : b)

#define POLY_IS_ZERO(p) (p.degree == 0 && p.coeffs[0].numerator == 0)

typedef struct {
	unsigned int degree;
	int *coeffs;
	unsigned int _capacity;
	int *_coeffs_ptr;
} poly_t;

typedef struct {
	unsigned int degree;
	frac_t *coeffs;
	unsigned int _capacity;
	frac_t *_coeffs_ptr;
} frac_poly_t;

void poly_new(poly_t*, unsigned int);
void poly_copy(poly_t*, poly_t);
void poly_destroy(poly_t*);
void poly_print(poly_t, FILE*);
void poly_recalc_deg(poly_t*);

void frac_poly_new(frac_poly_t*, unsigned int);
void frac_poly_copy(frac_poly_t*, frac_poly_t);
void frac_poly_destroy(frac_poly_t*);
void frac_poly_print(frac_poly_t, FILE*);
void frac_poly_recalc_deg(frac_poly_t*);

void frac_poly_from_poly(frac_poly_t*, poly_t);
void poly_from_frac_poly(poly_t*, frac_poly_t);

void poly_add(poly_t*, poly_t);
void poly_minus(poly_t*, poly_t);
void poly_scalar_mult(poly_t*, int);
void poly_poly_mult(poly_t*, poly_t);
void poly_scalar_mod(poly_t*, unsigned int);

void frac_poly_int_mult(frac_poly_t*, int);
void frac_poly_frac_mult(frac_poly_t*, frac_t);
//void frac_poly_frac_mono_mult(frac_poly_t*, frac_poly_t, frac_t, unsigned int);

void frac_poly_poly_mod(frac_poly_t*, poly_t);

#endif
