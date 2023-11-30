#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef PY_MOD

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#define PY_MODULE_NAME cdobble

#endif

#include "poly.h"

#define STR(x) #x
#define STR2(x) STR(x)
#define TOKENPASTE(x,y) x ## y
#define TOKENPASTE2(x,y) TOKENPASTE(x,y)

typedef struct {
	int x, y, z;
} vec3_int;

static inline vec3_int vec3_int_new(int x, int y, int z) {
	vec3_int v3 = { .x = x, .y = y, .z = z };
	return v3;
}

static inline int vec3_int_dot(vec3_int v1, vec3_int v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

typedef struct {
	frac_poly_t x, y, z;
} vec3_fp;

void vec3_fp_new(vec3_fp *v3, frac_poly_t x, frac_poly_t y, frac_poly_t z) {
	frac_poly_copy(&(v3->x), x);
	frac_poly_copy(&(v3->y), y);
	frac_poly_copy(&(v3->z), z);
	return;
}

frac_poly_t vec3_fp_dot(vec3_fp v1, vec3_fp v2) {
	frac_poly_t x_mult, y_mult, z_mult;
	frac_poly_copy(&x_mult, v1.x);
	frac_poly_frac_poly_mult(&x_mult, v2.x);
	frac_poly_copy(&y_mult, v1.y);
	frac_poly_frac_poly_mult(&y_mult, v2.y);
	frac_poly_copy(&z_mult, v1.z);
	frac_poly_frac_poly_mult(&z_mult, v2.z);
	frac_poly_add(&x_mult, y_mult);
	frac_poly_add(&x_mult, z_mult);
	frac_poly_destroy(&y_mult);
	frac_poly_destroy(&z_mult);
	return x_mult;
}

void vec3_fp_destroy(vec3_fp *v3) {
	frac_poly_destroy(&(v3->x));
	frac_poly_destroy(&(v3->y));
	frac_poly_destroy(&(v3->z));
	return;
}

void vec3_fp_print(vec3_fp v3, FILE* file) {
	fprintf(file, "(");
	frac_poly_print(v3.x, file);
	fprintf(file, ", ");
	frac_poly_print(v3.y, file);
	fprintf(file, ", ");
	frac_poly_print(v3.z, file);
	fprintf(file, ")");
	return;
}

#ifdef PY_MOD
static PyObject *generate(PyObject *self, PyObject *args) {
	int p, n;
	if (!PyArg_ParseTuple(args, "ii", &p, &n)) {
		return NULL;
	}
#else
int main(int argc, char **argv) {
	assert(argc == 3);
	int p = atoi(argv[1]);
	int n = atoi(argv[2]);
	assert((p > 1) && (n > 0));
#endif

	unsigned int q = int_exp(p, n);
	unsigned int q2 = q*q;
	unsigned int n_points = q2+q+1;

	unsigned int *cards = malloc(sizeof(unsigned int)*(n_points*(q+1)));

	if (n == 1) {
		vec3_int points[n_points];
		for (int a = 0; a < p; a++) {
			points[(p+1)*a] = vec3_int_new(0, 1, a);
			for (int b = 0; b < p; b++) {
				points[(p+1)*a+b+1] = vec3_int_new(1, a, b);
			}
		}
		points[n_points-1] = vec3_int_new(0, 0, 1);

		int symbol_i = 0;
		for (int line = 0; line < n_points; line++) {
			symbol_i = 0;
			for (int point = 0; point < n_points; point++) {
				if ((vec3_int_dot(points[line], points[point]) % p) == 0) {
					cards[line*(p+1)+symbol_i] = point;
					symbol_i++;
					if (symbol_i == p+1) break;
				}
			}
		}
	} else {
		int powers[n+1];
		for (int i = 0; i <= n; i++) powers[i] = int_exp(p, n-i);

		frac_poly_t field[q];
		for (int i = 0; i < q; i++) {
			frac_poly_t *poly = field+i;
			frac_poly_new(poly, (unsigned int)(n-1));
			for (int j = 0; j < n; j++) poly->coeffs[n-j-1] = frac_new((i % powers[j]) / powers[j+1], 1);
			frac_poly_recalc_deg(poly);
		}

		frac_poly_t poly_mod;
		frac_poly_new(&poly_mod, n);
		poly_mod.coeffs[n].numerator = 1;
		poly_mod.degree = n;

		frac_t coeffs[n+1];
		for (int i = 0; i < n; i++) coeffs[i] = frac_new(0, 1);
		coeffs[n] = frac_new(1, 1);

		int irreducible;
		while (1) {
			irreducible = 1;

			int current_inc = 0;
			while (1) {
				coeffs[current_inc].numerator++;
				coeffs[current_inc].numerator %= p;
				if (coeffs[current_inc].numerator == 0) current_inc++;
				else break;
			}

			for (int m = p; m < q; m++) {
				memcpy(poly_mod.coeffs, coeffs, sizeof(frac_t)*(n+1));
				poly_mod.degree = n;
				frac_poly_frac_poly_mod(&poly_mod, field[m]);
				frac_poly_int_mod(&poly_mod, p);
				int is_zero = FRAC_POLY_IS_ZERO(poly_mod);
				if (is_zero) {
					irreducible = 0;
					break;
				}
			}
			if (irreducible) break;
		}

		if (!irreducible) {
			fprintf(stderr, "no irreducible polynomial found - I'm assuming there's something wrong with my code rather than the laws of mathematics breaking???\n");
			exit(EXIT_FAILURE);
		}

		memcpy(poly_mod.coeffs, coeffs, sizeof(frac_t)*(n+1));
		poly_mod.degree = n;

		vec3_fp points[n_points];

		for (int a = 0; a < q; a++) {
			vec3_fp_new(points+((q+1)*a), field[0], field[1], field[a]);
			for (int b = 0; b < q; b++) {
				vec3_fp_new(points+((q+1)*a+b+1), field[1], field[a], field[b]);
			}
		}
		vec3_fp_new(points+(n_points-1), field[0], field[0], field[1]);

		for (int i = 0; i < q; i++) frac_poly_destroy(field+i);

		unsigned int symbol_i;
		for (int line = 0; line < n_points; line++) {
			symbol_i = 0;
			for (int point = 0; point < n_points; point++) {
				frac_poly_t dot = vec3_fp_dot(points[line], points[point]);
				frac_poly_frac_poly_mod(&dot, poly_mod);
				frac_poly_int_mod(&dot, p);
				if (FRAC_POLY_IS_ZERO(dot)) {
					cards[line*(q+1)+symbol_i] = point;
					symbol_i++;
					if (symbol_i == q+1) break;
				}
				frac_poly_destroy(&dot);
			}
		}

		frac_poly_destroy(&poly_mod);
		for (int i = 0; i < n_points; i++) vec3_fp_destroy(points+i);
	}

#ifdef PY_MOD
	PyObject* py_cardlist = PyList_New(n_points);
	for (int i = 0; i < n_points; i++) {
		PyObject* py_card = PyList_New(q+1);
		for (int j = 0; j < q+1; j++) {
			PyObject *py_int = Py_BuildValue("i", cards[i*(q+1)+j]);
			PyList_SetItem(py_card, j, py_int);
		}
		PyList_SetItem(py_cardlist, i, py_card);
	}

	free(cards);

	return py_cardlist;
}
#else
	for (int i = 0; i < n_points; i++) {
		for (int j = 0; j < q; j++) printf("%i, ", cards[i*(q+1)+j]);
		printf("%i\n", cards[i*(q+1)+q]);
	}

	free(cards);

	return EXIT_SUCCESS;
}
#endif

#ifdef PY_MOD
static PyMethodDef methods[] = {
    {"generate", generate, METH_VARARGS, "generate a dobble deck for (p, n)."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef py_module = {
    PyModuleDef_HEAD_INIT,
    STR2(PY_MODULE_NAME),
    NULL,
    -1,
    methods
};

#define PY_MODULE_INIT_FUNC(name) PyMODINIT_FUNC TOKENPASTE2(PyInit_, name) (void) { return PyModule_Create(&py_module); }

PY_MODULE_INIT_FUNC(PY_MODULE_NAME)
#endif
