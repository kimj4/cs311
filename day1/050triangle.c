/*
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * homework for day 5. Abstraction
 * extensive refactoring also done
 */

#include <stdio.h>
#include <math.h>

/* forward declaration for function in main*/
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], 
        double attr[], double rgb[]);

/* calculates attr which contains interpolated values */
void calculateAttr(double alpha[], double beta[], double gamma[], double pq[], double attr[], int attrDim) {
	double betaMinusAlpha[attrDim], gammaMinusAlpha[attrDim], pBetaMinusAlpha[attrDim], 
		  qGammaMinusAlpha[attrDim], subSum[attrDim], chi[attrDim];
	vecSubtract(attrDim, beta, alpha, betaMinusAlpha);
	vecSubtract(attrDim, gamma, alpha, gammaMinusAlpha);
	vecScale(attrDim, pq[0], betaMinusAlpha, pBetaMinusAlpha);
	vecScale(attrDim, pq[1], gammaMinusAlpha, qGammaMinusAlpha);
	vecAdd(attrDim, pBetaMinusAlpha, qGammaMinusAlpha, subSum);
	vecAdd(attrDim, subSum, alpha, attr);
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
	int t1, t2, i;
	int tempCount = 0;
	for (i = 0; i < 3; i++) {
		if (i != *aaPos) {
			if (tempCount == 0) {
				t1 = i;
				tempCount++;
			} else {
				t2 = i;
			}
		}
	}
	// using tangent theta identity to determine which has a smaller angle from the 
	//  horizontal axis
	if ((posArray[t1][1] / posArray[t1][0]) < (posArray[t2][1] / posArray[t2][0])) {
		*bbPos = t1;
		*ccPos = t2;
	} else {
		*bbPos = t2;
		*ccPos = t1;
	}
}

/* function to find the contant matrix in the interpolation function*/
void findInverseMatrixMultiplier(double aa[], double bb[], double cc[], double result[2][2]) {
	double bMinusA[2];
	vecSubtract(2, bb, aa, bMinusA);
	double cMinusA[2];
	vecSubtract(2, cc, aa, cMinusA);
	double leftMatrix[2][2];
	mat22Columns(bMinusA, cMinusA, leftMatrix);
	double invLeftMatrix[2][2];
	mat22Invert(leftMatrix, result); 
}

/* function to take all information necessary for interpolation. finds color and sets pixel to 
   that color. */
void interpolateAndSet(renRenderer *ren, double unif[], texTexture *tex[], double x[], 
	double aa[], double bb[], double cc[], double pq[], double invLeftMatrix[2][2]) {
	double xMinusA[2], newRGB[3], attr[ren->attrDim];
	vecSubtract(2, x, aa, xMinusA);
	calculatePQ(invLeftMatrix, xMinusA, pq);
	calculateAttr(aa, bb, cc, pq, attr, ren->attrDim);
	colorPixel(ren, unif, tex, attr, newRGB);
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

	// rearrange givens
	double *posArray[3] = {a, b, c};
	int aaPos, bbPos, ccPos;
	aaPos = findLeftmost(posArray);
	setbbPosAndccPos(posArray, &aaPos, &bbPos, &ccPos);
	double *aa = posArray[aaPos];
	double *bb = posArray[bbPos];
	double *cc = posArray[ccPos];

	// calculate constants
	double invLeftMatrix[2][2];
	findInverseMatrixMultiplier(aa, bb, cc, invLeftMatrix);

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
		for (x[0] = ceil(aa[0]); x[0] <= floor(cc[0]); x[0]++) {
			x1low = calcSlopePoint(x, bb, aa);
			x1high = calcSlopePoint(x, cc, aa);
			for (x[1] = ceil(x1low); x[1] <= floor(x1high); x[1]++) {
				interpolateAndSet(ren, unif, tex, x, aa, bb, cc, pq, invLeftMatrix);
			}
		}
		for (x[0] = ceil(cc[0]); x[0] <= floor(bb[0]); x[0]++) {
			x1low = calcSlopePoint(x, bb, aa);
			x1high = calcSlopePoint(x, bb, cc);
			// x1low = aa[1] + ((bb[1] - aa[1]) / s (bb[0] - cc[0])) * (x[0] - cc[0]);
			for (x[1] = ceil(x1low); x[1]<= floor(x1high); x[1]++) {
				interpolateAndSet(ren, unif, tex, x, aa, bb, cc, pq, invLeftMatrix);
			} 
		}
	// base of the triangle is the top-most edge
	} else {
		for (x[0] = ceil(aa[0]); x[0] <= floor(bb[0]); x[0]++) {
			x1low = calcSlopePoint(x, bb, aa);
			x1high = calcSlopePoint(x, cc, aa);
			if ((ceil(aa[0]) == floor(cc[0]))) {
				break;
			} else {
				for (x[1] = ceil(x1low); x[1] <= floor(x1high); x[1]++) {
					interpolateAndSet(ren, unif, tex, x, aa, bb, cc, pq, invLeftMatrix);
				}
			}
		}
		for (x[0] = ceil(bb[0]); x[0] <= floor(cc[0]); x[0]++) {
			x1low = calcSlopePoint(x, cc, bb);
			x1high = calcSlopePoint(x, cc, aa);
			for (x[1] = ceil(x1low); x[1] <= floor(x1high); x[1]++) {
				interpolateAndSet(ren, unif, tex, x, aa, bb, cc, pq, invLeftMatrix);
			}
		}
	}
}