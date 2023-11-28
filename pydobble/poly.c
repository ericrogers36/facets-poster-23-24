#include "poly.h"

// poly stuff

void poly_new(poly_t *p, unsigned int capacity) {
	p->degree = 0;
	p->_capacity = capacity;
	p->_coeffs_ptr = malloc(sizeof(int)*(capacity+1));
	assert(p->_coeffs_ptr != NULL);
	memset(p->_coeffs_ptr, 0, sizeof(int)*(capacity+1));
	p->coeffs = p->_coeffs_ptr;
	return;
}

void poly_copy(poly_t *new, poly_t old) {
	poly_new(new, old._capacity);
	memcpy(new->coeffs, old.coeffs, sizeof(int)*(old._capacity+1));
	return;
}

void poly_destroy(poly_t *p) {
	free(p->_coeffs_ptr);
	p->_coeffs_ptr = NULL;
	p->coeffs = NULL;
	return;
}

void poly_print(poly_t p, FILE *fp) {
	fprintf(fp, "%i", p.coeffs[0]);
	for (int i = 1; i < p.degree+1; i++) {
		fprintf(fp, " + %ix^%u", p.coeffs[i], i);
	}
	return;
}

void poly_recalc_deg(poly_t *p) {
	int i = p->degree;
	while (i >= 0) {
		if (p->coeffs[i] != 0) break;
		i--;
	}
	p->degree = i;
	return;
}

void poly_add(poly_t *a, poly_t b) {
	if (b.degree <= a->_capacity) {
		for (int i = 0; i <= b.degree; i++) {
			a->coeffs[i] += b.coeffs[i];
		}
	} else {
		poly_t c;
		poly_copy(&c, b);
		poly_add(&c, *a);
		poly_destroy(a);
		a->_capacity = b._capacity;
		a->coeffs = c.coeffs;
		a->_coeffs_ptr = c.coeffs;
	}
	poly_recalc_deg(a);
	return;
}

void poly_minus(poly_t *a, poly_t b) {
	if (b.degree <= a->_capacity) {
		for (int i = 0; i <= b.degree; i++) {
			a->coeffs[i] -= b.coeffs[i];
		}
	} else {
		poly_t c;
		poly_copy(&c, b);
		poly_scalar_mult(&c, -1);
		poly_add(&c, *a);
		poly_destroy(a);
		a->_capacity = b._capacity;
		a->coeffs = c.coeffs;
		a->_coeffs_ptr = c.coeffs;
	}
	poly_recalc_deg(a);
	return;
}

void poly_scalar_mult(poly_t *a, int b) {
	for (int i = 0; i <= a->degree; i++) {
		a->coeffs[i] *= b;
	}
	a->degree = b ? a->degree : 0;
	return;
}

void poly_poly_mult(poly_t *a, poly_t b) {
	poly_t mult;
	poly_new(&mult, a->degree+b.degree);
	mult.degree = a->degree+b.degree;
	for (int i = 0; i <= a->degree; i++) {
		for (int j = 0; j <= b.degree; j++) {
			mult.coeffs[i+j] += a->coeffs[i] * b.coeffs[j];
		}
	}
	poly_destroy(a);
	a->_capacity = mult._capacity;
	a->coeffs = mult.coeffs;
	a->_coeffs_ptr = mult.coeffs;
	a->degree = mult.degree;
	return;
}

void poly_scalar_mod(poly_t *a, unsigned int b) {
	for (int i = 0; i <= a->degree; i++) {
		a->coeffs[i] %= b;
	}
	poly_recalc_deg(a);
	return;
}

// frac_poly stuff

void frac_poly_new(frac_poly_t *p, unsigned int capacity) {
	p->degree = 0;
	p->_coeffs_ptr = malloc(sizeof(frac_t)*(capacity+1));
	assert(p->_coeffs_ptr != NULL);
	p->coeffs = p->_coeffs_ptr;
	p->_capacity = capacity;
	frac_t zero = frac_new(0, 1);
	for (int i = 0; i <= capacity; i++) {
		p->coeffs[i] = zero;
	}
	return;
}

void frac_poly_copy(frac_poly_t *copy, frac_poly_t orig) {
	frac_poly_new(copy, orig._capacity);
	memcpy(copy->coeffs, orig.coeffs, sizeof(frac_t)*(orig._capacity+1));
	return;
}

void frac_poly_destroy(frac_poly_t *p) {
	free(p->_coeffs_ptr);
	p->_coeffs_ptr = NULL;
	p->coeffs = NULL;
	return;
}

void frac_poly_print(frac_poly_t p, FILE *fp) {
	fprintf(fp, "%i/%i", p.coeffs[0].numerator, p.coeffs[0].denominator);
	for (int i = 1; i < p.degree+1; i++) {
		fprintf(fp, " + %i/%ix^%u", p.coeffs[i].numerator, p.coeffs[i].denominator, i);
	}
	return;
}

void frac_poly_recalc_deg(frac_poly_t *p) {
	int i = p->_capacity;
	while (i > 0) {
		if (p->coeffs[i].numerator != 0) break;
		i--;
	}
	p->degree = i;
	return;
}

void frac_poly_add(frac_poly_t *a, frac_poly_t b) {
	if (b.degree <= a->_capacity) {
		for (int i = 0; i <= b.degree; i++) {
			a->coeffs[i] = frac_add(a->coeffs[i], b.coeffs[i]);
		}
	} else {
		frac_poly_t c;
		frac_poly_copy(&c, b);
		frac_poly_add(&c, *a);
		frac_poly_destroy(a);
		a->_capacity = b._capacity;
		a->coeffs = c.coeffs;
		a->_coeffs_ptr = c.coeffs;
	}
	frac_poly_recalc_deg(a);
	return;
}

void frac_poly_minus(frac_poly_t *a, frac_poly_t b) {
	if (b.degree <= a->_capacity) {
		for (int i = 0; i <= b.degree; i++) {
			a->coeffs[i] = frac_minus(a->coeffs[i], b.coeffs[i]);
		}
	} else {
		frac_poly_t c;
		frac_poly_copy(&c, b);
		frac_poly_int_mult(&c, -1);
		frac_poly_add(&c, *a);
		frac_poly_destroy(a);
		a->_capacity = b._capacity;
		a->coeffs = c.coeffs;
		a->_coeffs_ptr = c.coeffs;
	}
	frac_poly_recalc_deg(a);
	return;
}

void frac_poly_int_mult(frac_poly_t *a, int b) {
	for (int i = 0; i <= a->degree; i++) {
		a->coeffs[i].numerator *= b;
	}
	a->degree = b ? a->degree : 0;
	return;
}

void frac_poly_frac_mult(frac_poly_t *a, frac_t b) {
	for (int i = 0; i <= a->degree; i++) {
		a->coeffs[i] = frac_mult(a->coeffs[i], b);
	}
	a->degree = b.numerator ? a->degree : 0;
	return;
}

/* TODO: fix - this seems to introduce some undefined behaviour (ooooh)
void frac_poly_frac_mono_mult(frac_poly_t *polyans, frac_poly_t poly, frac_t mono_coeff, unsigned int mono_deg) {
	frac_poly_new(polyans, poly.degree+mono_deg);
	memcpy(polyans->coeffs+mono_deg, poly.coeffs, sizeof(frac_poly_t)*(poly.degree+1));
	polyans->degree = poly.degree+mono_deg;
	frac_poly_frac_mult(polyans, mono_coeff);
	return;
}
*/

// conversions

void frac_poly_from_poly(frac_poly_t *fp, poly_t p) {
	frac_poly_new(fp, p._capacity);
	fp->degree = p.degree;
	for (int i = 0; i <= p.degree; i++) {
		fp->coeffs[i] = frac_new(p.coeffs[i], 1);
	}
	return;
}

void poly_from_frac_poly(poly_t *p, frac_poly_t fp) {
	poly_new(p, fp._capacity);
	p->degree = fp.degree;
	for (int i = 0; i <= fp.degree; i++) {
		p->coeffs[i] = fp.coeffs[i].numerator;
	}
	return;
}

// polynomial modulus

void frac_poly_poly_mod(frac_poly_t *ddend, poly_t b) {
	if (ddend->degree >= b.degree) {
		frac_poly_t dsor, quot, sub;
		frac_poly_from_poly(&dsor, b); // divisor
		frac_poly_new(&quot, ddend->degree-b.degree); // quotient
		frac_t b_coeff = frac_new(b.coeffs[b.degree], 1);

		quot.degree = ddend->degree - b.degree;

		while (ddend->degree >= dsor.degree) {
			frac_t term_coeff = frac_div(ddend->coeffs[ddend->degree], b_coeff);
			unsigned int term_power = ddend->degree-dsor.degree;
			quot.coeffs[term_power] = term_coeff;

			//frac_poly_frac_mono_mult(&sub, dsor, term_coeff, ddend.degree-dsor.degree);
			frac_poly_new(&sub, ddend->degree);
			for (int i = 0; i <= dsor.degree; i++) {
				sub.coeffs[i+term_power] = frac_mult(dsor.coeffs[i], term_coeff);
			}
			sub.degree = ddend->degree;
			
			frac_poly_minus(ddend, sub);
			frac_poly_destroy(&sub);
		}

		/*
		poly_print(*a, stdout);
		printf(" = (");
		poly_print(b, stdout);
		printf(")(");
		frac_poly_print(quot, stdout);
		printf(") + (");
		frac_poly_print(ddend, stdout);
		printf(")\n");
		*/
	}
	return;
}
