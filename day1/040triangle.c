/*
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * homework for day 4, texture mapping.
 */

#include <stdio.h>
#include <math.h>
// #include "040texture.c"

/* to calculate p and q, take in the inverted left matrix and the x - a column matrix.
   return result in a vector of size 2 where pq[0] = p  and pq[1] = q*/
void calculatePQ(double leftMatrix[2][2], double xMinusA[2], double pq[2]) {
	mat221Multiply(leftMatrix, xMinusA, pq);
}

/* calculates the interpolation, and gets the color values from the texture.*/
void getTexRGB(double alpha[2], double beta[2], double gamma[2], 
	double pq[2], double newRGB[3], texTexture *tex) {
	double betaMinusAlpha[2], gammaMinusAlpha[2], pBetaMinusAlpha[2], 
		   qGammaMinusAlpha[2], subSum[2], chi[2];
	vecSubtract(2, beta, alpha, betaMinusAlpha);
	vecSubtract(2, gamma, alpha, gammaMinusAlpha);
	vecScale(2, pq[0], betaMinusAlpha, pBetaMinusAlpha);
	vecScale(2, pq[1], gammaMinusAlpha, qGammaMinusAlpha);
	vecAdd(2, pBetaMinusAlpha, qGammaMinusAlpha, subSum);
	vecAdd(2, subSum, alpha, chi);
	texSample(tex, chi[0], chi[1]);
	vecCopy(tex->texelDim, tex->sample, newRGB);
}


void triRender(double a[2], double b[2], double c[2], double rgb[3],
	texTexture *tex, double alpha[2], double beta[2], double gamma[2]) {

	double abcArray[3][2] = {{a[0], a[1]}, {b[0], b[1]}, {c[0], c[1]}};

	// move around alpha, beta, gamma in the same way as a, b, c
	double abgArray[3][2] = {{alpha[0], alpha[1]}, 
							 {beta[0], beta[1]}, 
							 {gamma[0], gamma[1]}};

	// find aa (leftmost)
	int i;
	int aaPos, bbPos, ccPos;
	aaPos = -1;
	for (i = 0; i < 3; i++) {
		if (aaPos == -1) {
			aaPos = i;
		} else {
			// find leftmost
			if (abcArray[aaPos][0] > abcArray[i][0]) {
				aaPos = i;
			} else if (abcArray[aaPos][0] == abcArray[i][0]) {
				if (abcArray[aaPos][1] > abcArray[i][1]) 
					// if horizontal position conflicts, pick the one that is vertically lower
					aaPos = i;
			}
		}
	}

	// t1, t2 are temp vars to keep track of the two remaining unsorted verticies
	int t1, t2;
	int tempCount = 0;
	for (i = 0; i < 3; i++) {
		if (i != aaPos) {
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
	if ((abcArray[t1][1] / abcArray[t1][0]) < (abcArray[t2][1] / abcArray[t2][0])) {
		bbPos = t1;
		ccPos = t2;
	} else {
		bbPos = t2;
		ccPos = t1;
	}

	// set rearranged variables
	double aa[2] = {abcArray[aaPos][0], abcArray[aaPos][1]}; 
	double bb[2] = {abcArray[bbPos][0], abcArray[bbPos][1]};
	double cc[2] = {abcArray[ccPos][0], abcArray[ccPos][1]};

	double newAlpha[2] = {abgArray[aaPos][0], abgArray[aaPos][1]};
	double newBeta[2] = {abgArray[bbPos][0], abgArray[bbPos][1]};
	double newGamma[2] = {abgArray[ccPos][0], abgArray[ccPos][1]};

	// find the matrix that multiplies (x - a) this is constant
	// TODO: refactor this bit
	double bMinusA[2];
	vecSubtract(2, bb, aa, bMinusA);
	double cMinusA[2];
	vecSubtract(2, cc, aa, cMinusA);
	double leftMatrix[2][2];
	mat22Columns(bMinusA, cMinusA, leftMatrix);
	double invLeftMatrix[2][2];
	mat22Invert(leftMatrix, invLeftMatrix);   

	double x1low, x1high, p, q, newR, newG, newB;
	double xMinusA[2], x[2], pq[2], newRGB[3];
	// special case handling for vertical line
	if ((aa[0] == bb[0]) && (bb[0] == cc[0])) {
		x[0] = aa[0];
		for (x[1] = ceil(aa[1]); x[1] <= floor(cc[1]); x[1]++) {
			vecSubtract(2, x, aa, xMinusA);
			calculatePQ(invLeftMatrix, xMinusA, pq);
			getTexRGB(newAlpha, newBeta, newGamma, pq, newRGB, tex);
			pixSetRGB(x[0], x[1], newRGB[0], newRGB[1], newRGB[2]);
		}
	}
	// base of the triangle is the bottom-most edge
	else if ((bb[0] > cc[0]) || (aa[0] == cc[0]) || (bb[0] == cc[0])) {
		for (x[0] = ceil(aa[0]); x[0] <= floor(cc[0]); x[0]++) {
			x1low = aa[1] + ((bb[1] - aa[1]) / (bb[0] - aa[0])) * (x[0] - aa[0]);
			x1high = aa[1] + ((cc[1] - aa[1]) / (cc[0] - aa[0])) * (x[0] - aa[0]);
			for (x[1] = ceil(x1low); x[1] <= floor(x1high); x[1]++) {
				vecSubtract(2, x, aa, xMinusA);
				calculatePQ(invLeftMatrix, xMinusA, pq);
				getTexRGB(newAlpha, newBeta, newGamma, pq, newRGB, tex);
				pixSetRGB(x[0], x[1], newRGB[0], newRGB[1], newRGB[2]);
			}
		}
		for (x[0] = ceil(cc[0]); x[0] <= floor(bb[0]); x[0]++) {
			x1low = aa[1] + ((bb[1] - aa[1]) / (bb[0] - aa[0])) * (x[0] - aa[0]);
			x1high = cc[1] + ((bb[1] - cc[1]) / (bb[0] - cc[0])) * (x[0] - cc[0]);
			for (x[1] = ceil(x1low); x[1]<= floor(x1high); x[1]++) {
				vecSubtract(2, x, aa, xMinusA);
				calculatePQ(invLeftMatrix, xMinusA, pq);
				getTexRGB(newAlpha, newBeta, newGamma, pq, newRGB, tex);
				pixSetRGB(x[0], x[1], newRGB[0], newRGB[1], newRGB[2]);
			} 
		}
	// base of the triangle is the top-most edge
	} else {
		for (x[0] = ceil(aa[0]); x[0] <= floor(bb[0]); x[0]++) {
			x1low = aa[1] + ((bb[1] - aa[1]) / (bb[0] - aa[0])) * (x[0] - aa[0]);
			x1high = aa[1] + ((cc[1] - aa[1]) / (cc[0] - aa[0])) * (x[0] - aa[0]);
			if ((ceil(aa[0]) == floor(cc[0]))) {
				break;
			} else {
				for (x[1] = ceil(x1low); x[1] <= floor(x1high); x[1]++) {
					vecSubtract(2, x, aa, xMinusA);
					calculatePQ(invLeftMatrix, xMinusA, pq);
					getTexRGB(newAlpha, newBeta, newGamma, pq, newRGB, tex);
					pixSetRGB(x[0], x[1], newRGB[0], newRGB[1], newRGB[2]);
				}
			}
		}
		for (x[0] = ceil(bb[0]); x[0] <= floor(cc[0]); x[0]++) {
			x1low = bb[1] + ((cc[1] - bb[1]) / (cc[0] - bb[0])) * (x[0] - bb[0]);
			x1high = aa[1] + ((cc[1] - aa[1]) / (cc[0] - aa[0])) * (x[0] - aa[0]);
			for (x[1] = ceil(x1low); x[1] <= floor(x1high); x[1]++) {
				
				vecSubtract(2, x, aa, xMinusA);
				calculatePQ(invLeftMatrix, xMinusA, pq);
				getTexRGB(newAlpha, newBeta, newGamma, pq, newRGB, tex);
				pixSetRGB(x[0], x[1], newRGB[0], newRGB[1], newRGB[2]);
			}
		}
	}
}