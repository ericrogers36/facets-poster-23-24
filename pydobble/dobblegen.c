#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "poly.h"

#define STR(x) #x
#define STR2(x) STR(x)
#define TOKENPASTE(x,y) x ## y
#define TOKENPASTE2(x,y) TOKENPASTE(x,y)

#define PY_MODULE_NAME cdobble

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
	int x, y, z;
} vec3_poly;

int int_exp(int base, unsigned int exponent) {
	return exponent == 1 ? base : base * int_exp(base, exponent-1);
}

static PyObject *generate(PyObject *self, PyObject *args) {
	int p, n;
	if (!PyArg_ParseTuple(args, "ii", &p, &n)) {
		return NULL;
	}

	int q = int_exp(p, n);
	unsigned int n_points = p*p+p+1;

	unsigned int cards[n_points*(p+1)];

	if (n == 1) {
		unsigned int p2 = p*p;
		unsigned int p3 = p*p*p;

		vec3_int points[n_points];
		for (int a = 0; a < p; a++) {
			points[(p+1)*a] = vec3_int_new(0, 1, a);
			for (int b = 0; b < p; b++) {
				points[(p+1)*a+b+1] = vec3_int_new(1, a, b);
			}
		}
		points[n_points-1] = vec3_int_new(0, 0, 1);

		unsigned int mapping[p3+1];
		memset(mapping, 0, sizeof(mapping));
		int mapping_i = 1;
		int symbol_i = 0;
		for (int line = 0; line < n_points; line++) {
			symbol_i = 0;
			for (int point = 0; point < n_points; point++) {
				if ((vec3_int_dot(points[line], points[point]) % p) == 0) {
					unsigned int key = points[point].x*p2+points[point].y*p+points[point].z;
					if (mapping[key] == 0) {
						mapping[key] = mapping_i;
						mapping_i++;
					}
					cards[line*(p+1)+symbol_i] = mapping[key];
					symbol_i++;
				}
			}
		}
	} else {
		// TODO
		return NULL;
	}

	PyObject* py_cardlist = PyList_New(n_points);
	for (int i = 0; i < n_points; i++) {
		PyObject* py_card = PyList_New(q+1);
		for (int j = 0; j < q+1; j++) {
			PyObject *py_int = Py_BuildValue("i", cards[i*(q+1)+j]);
			PyList_SetItem(py_card, j, py_int);
		}
		PyList_SetItem(py_cardlist, i, py_card);
	}
	return py_cardlist;
}

static PyMethodDef methods[] = {
    {"generate", generate, METH_VARARGS, "generate a dobble deck for p, n."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef py_module = {
    PyModuleDef_HEAD_INIT,
    STR2(PY_MODULE_NAME),	/* name of module */
    NULL,					/* module documentation, may be NULL */
    -1,						/* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    methods
};

#define PY_MODULE_INIT_FUNC(name) PyMODINIT_FUNC TOKENPASTE2(PyInit_, name) (void) { return PyModule_Create(&py_module); }

/*
PyMODINIT_FUNC PY_INIT_FUNCNAME (void) {
	return PyModule_Create(&py_module);
}
*/

PY_MODULE_INIT_FUNC(PY_MODULE_NAME)
