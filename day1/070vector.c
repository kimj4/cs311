/*
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * updated version of 030vector.c
 * includes vecSet
 */
#include <stdarg.h>
/*** In general dimensions ***/

/* Copies the dim-dimensional vector v to the dim-dimensional vector copy. */
void vecCopy(int dim, double v[], double copy[]) {
  int i;
  for (i = 0; i < dim; i++) {
    copy[i] = v[i];
  }
}

/* Adds the dim-dimensional vectors v and w. */
void vecAdd(int dim, double v[], double w[], double vPlusW[]) {
  int i;
  for (i = 0; i < dim; i++) {
    vPlusW[i] = v[i] + w[i];
  }
}

/* Subtracts the dim-dimensional vectors v and w. */
void vecSubtract(int dim, double v[], double w[], double vMinusW[]) {
  int i;
  for (i = 0; i < dim; i++) {
    vMinusW[i] = v[i] - w[i];
  }
}

/* Scales the dim-dimensional vector w by the number c. */
void vecScale(int dim, double c, double w[], double cTimesW[]) {
  int i;
  for (i = 0; i < dim; i++) {
    cTimesW[i] = c * w[i];
  }
}

/* Assumes that there are dim + 2 arguments, the last dim of which are doubles.
Sets the dim-dimensional vector v to those doubles. */
void vecSet(int dim, double v[], ...) {
  int i;
  va_list argumentPointer;
  va_start(argumentPointer, dim);
  for (i = 0; i < dim; i++) {
    v[i] = va_arg(argumentPointer, double);
  }
  va_end(argumentPointer);
}
