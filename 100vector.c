/*
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * updated version of 030vector.c
 * includes vecSet
 */
#include <stdarg.h>
/*** In general dimensions ***/


void vecPrint(int dim, double v[]) {
  int i;
  printf("\n");
  for (i = 0; i < dim; i++) {
    printf("%f\n", v[i]);
  }
  printf("\n");
}

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


/* Returns the dot product of the dim-dimensional vectors v and w. */
double vecDot(int dim, double v[], double w[]) {
  double dotProd = 0;
  int i;
  for (i = 0; i < dim; i++) {
    dotProd = v[i] * w[i];
  }
  return dotProd;
}

/* Returns the length of the dim-dimensional vector v. */
double vecLength(int dim, double v[]) {
  double len;
  int i;
  for (i = 0; i < dim; i++) {
    len = len + (v[i] * v[i]);
  }
  len = sqrt(len);
  return len;
}

/* Returns the length of the dim-dimensional vector v. If the length is
non-zero, then also places a scaled version of v into the dim-dimensional
vector unit, so that unit has length 1. */
double vecUnit(int dim, double v[], double unit[]) {
  double len = vecLength(dim, v);
  if (len != 0) {
    int i;
    for (i = 0; i < dim; i++) {
      unit[i] = v[i] / len;
    }
  }
  return len;
}

/* Computes the cross product of the 3-dimensional vectors v and w, and places
it into vCrossW. */
void vec3Cross(double v[3], double w[3], double vCrossW[3]){
  vCrossW[0] = (v[1] * w[2]) - (v[2] * w[1]);
  vCrossW[1] = (v[2] * w[0]) - (v[0] * w[2]);
  vCrossW[2] = (v[0] * w[1]) - (v[1] * w[0]);
}

/* Computes the 3-dimensional vector v from its spherical coordinates.
rho >= 0.0 is the radius. 0 <= phi <= pi is the co-latitude. -pi <= theta <= pi
is the longitude or azimuth. */
void vec3Spherical(double rho, double phi, double theta, double v[3]) {
  v[0] = rho * sin(phi) * cos(theta);
  v[1] = rho * sin(phi) * sin(theta);
  v[2] = rho * cos(phi);
}
