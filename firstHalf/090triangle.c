/*
 * 090triangle.c
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * 090: modified (mostly just variable names) to take vary vectors instead of
 * attr vectors.
 * Known Issue: some triangles will not render properly, and it is beacuse of
 * some logical error in sorting the three vertices
 */

#include <stdio.h>
#include <math.h>

/* calculates vary which contains interpolated values */
void calculateVary(double alpha[], double beta[], double gamma[], double pq[], double vary[], int varyDim) {
	double betaMinusAlpha[varyDim], gammaMinusAlpha[varyDim], pBetaMinusAlpha[varyDim],
		  qGammaMinusAlpha[varyDim], subSum[varyDim], chi[varyDim];
	vecSubtract(varyDim, beta, alpha, betaMinusAlpha);
	vecSubtract(varyDim, gamma, alpha, gammaMinusAlpha);
	vecScale(varyDim, pq[0], betaMinusAlpha, pBetaMinusAlpha);
	vecScale(varyDim, pq[1], gammaMinusAlpha, qGammaMinusAlpha);
	vecAdd(varyDim, pBetaMinusAlpha, qGammaMinusAlpha, subSum);
	vecAdd(varyDim, subSum, alpha, vary);
}

/* to calculate p and q, take in the inverted left matrix and the x - a column matrix.
   return result in a vector of size 2 where pq[0] = p  and pq[1] = q*/
void calculatePQ(double leftMatrix[2][2], double xMinusA[2], double pq[2]) {
	mat221Multiply(leftMatrix, xMinusA, pq);
}

/* function to find the left-most vertex. If the horizontal position conflicts with another,
   pick the one that is most towards the bottom. */
int findLeftmost(double *posArray[3]) {
	int aaPos = -1;
	int i;
	for (i = 0; i < 3; i++) {
		if (aaPos == -1) {
			aaPos = i;
		} else {
			if (posArray[aaPos][0] > posArray[i][0]) {
				aaPos = i;
			} else if (posArray[aaPos][0] == posArray[i][0]) {
				if (posArray[aaPos][1] > posArray[i][1])
					aaPos = i;
			}
		}
	}
	return aaPos;
}

/* function to appropriately arrange the remaining two vertices after aa has been determined*/
void setbbPosAndccPos(double *posArray[3], int *aaPos, int *bbPos, int *ccPos) {
	if (*aaPos == 0) {
    *aaPos = 0;
    *bbPos = 1;
    *ccPos = 2;
  } else if (*aaPos == 1) {
    *aaPos = 1;
    *bbPos = 2;
    *ccPos = 0;
  } else if (*aaPos == 2) {
    *aaPos = 2;
    *bbPos = 0;
    *ccPos = 1;
  }
}

/* function to find the contant matrix in the interpolation function
   also returns the determinant of the matrix that was inverted*/
int findInverseMatrixMultiplier(double aa[], double bb[], double cc[], double result[2][2]) {
	double bMinusA[2];
	vecSubtract(2, bb, aa, bMinusA);
	double cMinusA[2];
	vecSubtract(2, cc, aa, cMinusA);
	double leftMatrix[2][2];
	mat22Columns(bMinusA, cMinusA, leftMatrix);
	double invLeftMatrix[2][2];
	return mat22Invert(leftMatrix, result);
}

/* function to take all information necessary for interpolation. finds color and sets pixel to
   that color. */
void interpolateAndSet(renRenderer *ren, double unif[], texTexture *tex[], double x[],
	double aa[], double bb[], double cc[], double pq[], double invLeftMatrix[2][2]) {
	double xMinusA[renVARYDIMBOUND], newRGB[3], vary[renVARYDIMBOUND];
	vecSubtract(2, x, aa, xMinusA);
	calculatePQ(invLeftMatrix, xMinusA, pq);
	calculateVary(aa, bb, cc, pq, vary, ren->varyDim);
	ren->colorPixel(ren, unif, tex, vary, newRGB);
  // printf("interpolateAndSet: (x[0], x[1]): (%f, %f)\n", x[0], x[1]);
	pixSetRGB(x[0], x[1], newRGB[0], newRGB[1], newRGB[2]);
}

/* refactored version of calculating the veritcal value from the horizontal value and given
   information of a line*/
double calcSlopePoint(double x[], double final[], double initial[]) {
	return final[1] + ((initial[1] - final[1]) / (initial[0] - final[0])) * (x[0] - final[0]);
}

// new abstracted version of triRender
// first two elements of a, b, c are coordinates (in screen space)
void triRender(renRenderer *ren, double unif[], texTexture *tex[],
	double a[], double b[], double c[]) {


	double *posArray[3] = {a, b, c};
	int aaPos, bbPos, ccPos;
	aaPos = findLeftmost(posArray);
	setbbPosAndccPos(posArray, &aaPos, &bbPos, &ccPos);
	double *aa = posArray[aaPos];
	double *bb = posArray[bbPos];
	double *cc = posArray[ccPos];

	// calculate constants
	double invLeftMatrix[2][2];
	if (findInverseMatrixMultiplier(aa, bb, cc, invLeftMatrix) < 0) {
		return; // backface culling
	} else {
		// rasterize
		double x1low, x1high, x[2], pq[2];
		// special case handling for vertical line
		if ((aa[0] == bb[0]) && (bb[0] == cc[0])) {
			x[0] = aa[0];
			for (x[1] = ceil(aa[1]); x[1] <= floor(cc[1]); x[1]++) {
				interpolateAndSet(ren, unif, tex, x, aa, bb, cc, pq, invLeftMatrix);
			}
		}
		// base of the triangle is the bottom-most edge
		else if ((bb[0] > cc[0]) || (aa[0] == cc[0]) || (bb[0] == cc[0])) {
			// printf("first else\n");
			// printf("before first forloop\n");
			for (x[0] = ceil(aa[0]); x[0] <= floor(cc[0]); x[0]++) {
				x1low = calcSlopePoint(x, bb, aa);
				x1high = calcSlopePoint(x, cc, aa);
				for (x[1] = ceil(x1low); x[1] <= floor(x1high); x[1]++) {
					interpolateAndSet(ren, unif, tex, x, aa, bb, cc, pq, invLeftMatrix);
				}
			}
			// printf("before first forloop\n");
			for (x[0] = ceil(cc[0]); x[0] <= floor(bb[0]); x[0]++) {
				x1low = calcSlopePoint(x, bb, aa);
				x1high = calcSlopePoint(x, bb, cc);
				for (x[1] = ceil(x1low); x[1]<= floor(x1high); x[1]++) {
					interpolateAndSet(ren, unif, tex, x, aa, bb, cc, pq, invLeftMatrix);
				}
			}
			// printf("after second forloop\n");
		} else {
			// printf("second else\n");
		  for (x[0] = ceil(aa[0]); x[0] <= floor(bb[0]); x[0]++) {
		    x1low = calcSlopePoint(x, bb, aa);
		    x1high = calcSlopePoint(x, cc, aa);
		    if ((ceil(aa[0]) == floor(cc[0]))) {
					// todo: see if this case needs to be handled
		      // break;
		    } else {
		      for (x[1] = ceil(x1low); x[1] <= floor(x1high); x[1]++) {
		        interpolateAndSet(ren, unif, tex, x, aa, bb, cc, pq, invLeftMatrix);
		      }
		    }
		  }
			// printf("after first forloop\n");
		  for (x[0] = ceil(bb[0]); x[0] <= floor(cc[0]); x[0]++) {
		    x1low = calcSlopePoint(x, cc, bb);
		    x1high = calcSlopePoint(x, cc, aa);
		    for (x[1] = ceil(x1low); x[1] <= floor(x1high); x[1]++) {
		      interpolateAndSet(ren, unif, tex, x, aa, bb, cc, pq, invLeftMatrix);
		    }
		  }
			// printf("after second forloop\n");
		}
	}
}
