/*
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * homework for day 3, adding interpolation on triangle rasterization.
 */

// TODO: figure out import stuff
// TODO: use more vector + matrix operations
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "000pixel.h"

// TODO: combine calculateP and calculateQ to create a void function
double calculateP(double leftMatrix[2][2], double xMinusA[2]) {
	double resultCol[2];
	mat221Multiply(leftMatrix, xMinusA, resultCol);

	return resultCol[0];
}

double calculateQ(double leftMatrix[2][2], double xMinusA[2]) {
	double resultCol[2];
	mat221Multiply(leftMatrix, xMinusA, resultCol);
	return resultCol[1];
}

// TODO: combine calculateR, calculateG, calculateB to make on void function
double caluclateNewR(double alpha0, double beta0, double gamma0, double p, double q) {
	return alpha0 + (p * (beta0 - alpha0)) + (q * (gamma0 - alpha0));
}

double calculateNewG(double alpha1, double beta1, double gamma1, double p, double q) {
	return alpha1 + (p * (beta1 - alpha1)) + (q * (gamma1 - alpha1));
}

double calculateNewB(double alpha2, double beta2, double gamma2, double p, double q) {
	return alpha2 + (p * (beta2 - alpha2)) + (q * (gamma2 - alpha2));
}



void triRender(double a[2], double b[2], double c[2], double rgb[3], 
	double alpha[3], double beta[3], double gamma[3]) {

	double aa0, aa1, bb0, bb1, cc0, cc1;
	double abcArray[3][2] = {{a[0], a[1]}, {b[0], b[1]}, {c[0], c[1]}};

	// move around alpha, beta, gamma in the same way as a, b, c
	double abgArray[3][3] = {{alpha[0], alpha[1], alpha[2]}, 
							 {beta[0], beta[1], beta[2]}, 
							 {gamma[0], gamma[1], gamma[2]}};
	double newAlpha[3], newBeta[3], newGamma[3];
;

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
	// TODO: combine aa0,aa1 bb0,bb1 ... into aa, bb, cc
	double aa[2] = {abcArray[aaPos][0], abcArray[aaPos][0]}; 
	aa0 = abcArray[aaPos][0];
	aa1 = abcArray[aaPos][0];
	bb0 = abcArray[bbPos][0];
	bb1 = abcArray[bbPos][1];
	cc0 = abcArray[ccPos][0];
	cc1 = abcArray[ccPos][1];

	newAlpha[0] = abgArray[aaPos][0];
	newAlpha[1] = abgArray[aaPos][1];
	newAlpha[2] = abgArray[aaPos][2];
	newBeta[0] = abgArray[bbPos][0];
	newBeta[1] = abgArray[bbPos][1];
	newBeta[2] = abgArray[bbPos][2];
	newGamma[0] = abgArray[ccPos][0];
	newGamma[1] = abgArray[ccPos][1];
	newGamma[2] = abgArray[ccPos][2];

	// find the matrix that multiplies (x - a) this is constant
	// TODO: refactor this bit
	double bMinusA[2] = {bb0 - aa0, bb1 - aa1};
	double cMinusA[2] = {cc0 - aa0, cc1 - aa1};
	double leftMatrix[2][2];
	mat22Columns(bMinusA, cMinusA, leftMatrix);
	double invLeftMatrix[2][2];
	mat22Invert(leftMatrix, invLeftMatrix);   



	int x0, x1; // TODO: see if these should be doubles
	double x1low, x1high, p, q, newR, newG, newB;
	double xMinusA[2], x[2];
	// special case handling for vertical line
	if ((aa0 == bb0) && (bb0 == cc0)) {
		x0 = aa0;
		for (x1 = ceil(aa1); x1 <= floor(cc1); x1++) {
			// TODO: refactor this bit of repeated code
			xMinusA[0] = x0 - aa0;
			xMinusA[1] = x1 - aa1;
			p = calculateP(invLeftMatrix, xMinusA);
			q = calculateQ(invLeftMatrix, xMinusA);
			newR = caluclateNewR(alpha[0], beta[0], gamma[0], p, q);
			newG = calculateNewG(alpha[1], beta[1], gamma[1], p, q);
			newB = calculateNewB(alpha[2], beta[2], gamma[2], p, q);
		}
	}
	// base of the triangle is the bottom-most edge
	else if ((bb0 > cc0) || (aa0 == cc0) || (bb0 == cc0)) {
		printf("HERE\n");
		for (x0 = ceil(aa0); x0 <= floor(cc0); x0++) {
			x1low = aa1 + ((bb1 - aa1) / (bb0 - aa0)) * (x0 - aa0);
			x1high = aa1 + ((cc1 - aa1) / (cc0 - aa0)) * (x0 - aa0);
			for (x1 = ceil(x1low); x1 <= floor(x1high); x1++) {
				x[0] = x0;
				x[1] = x1;
				vecSubtract(2, x, aa, xMinusA);
				p = calculateP(invLeftMatrix, xMinusA);
				q = calculateQ(invLeftMatrix, xMinusA);
				newR = caluclateNewR(alpha[0], beta[0], gamma[0], p, q);
				newG = calculateNewG(alpha[1], beta[1], gamma[1], p, q);
				newB = calculateNewB(alpha[2], beta[2], gamma[2], p, q);
				pixSetRGB(x0, x1, newR, newB, newG);
			}
		}
		for (x0 = ceil(cc0); x0 <= floor(bb0); x0++) {
			x1low = aa1 + ((bb1 - aa1) / (bb0 - aa0)) * (x0 - aa0);
			x1high = cc1 + ((bb1 - cc1) / (bb0 - cc0)) * (x0 - cc0);
			for (x1 = ceil(x1low); x1<= floor(x1high); x1++) {
				x[0] = x0;
				x[1] = x1;
				vecSubtract(2, x, aa, xMinusA);
				p = calculateP(invLeftMatrix, xMinusA);
				q = calculateQ(invLeftMatrix, xMinusA);
				newR = caluclateNewR(alpha[0], beta[0], gamma[0], p, q);
				newG = calculateNewG(alpha[1], beta[1], gamma[1], p, q);
				newB = calculateNewB(alpha[2], beta[2], gamma[2], p, q);
				pixSetRGB(x0, x1, newR, newB, newG);
			} 
		}
	// base of the triangle is the top-most edge
	} else {
		for (x0 = ceil(aa0); x0 <= floor(bb0); x0++) {
			x1low = aa1 + ((bb1 - aa1) / (bb0 - aa0)) * (x0 - aa0);
			x1high = aa1 + ((cc1 - aa1) / (cc0 - aa0)) * (x0 - aa0);
			if ((ceil(aa0) == floor(cc0))) {
				break;
			} else {
				for (x1 = ceil(x1low); x1 <= floor(x1high); x1++) {
					x[0] = x0;
					x[1] = x1;
					vecSubtract(2, x, aa, xMinusA);
					p = calculateP(invLeftMatrix, xMinusA);
					q = calculateQ(invLeftMatrix, xMinusA);
					newR = caluclateNewR(alpha[0], beta[0], gamma[0], p, q);
					newG = calculateNewG(alpha[1], beta[1], gamma[1], p, q);
					newB = calculateNewB(alpha[2], beta[2], gamma[2], p, q);
					pixSetRGB(x0, x1, newR, newB, newG);
				}
			}
		}
		for (x0 = ceil(bb0); x0 <= floor(cc0); x0++) {
			// printf("else: second for loop\n")	;
			x1low = bb1 + ((cc1 - bb1) / (cc0 - bb0)) * (x0 - bb0);
			x1high = aa1 + ((cc1 - aa1) / (cc0 - aa0)) * (x0 - aa0);
			for (x1 = ceil(x1low); x1 <= floor(x1high); x1++) {
				x[0] = x0;
				x[1] = x1;
				vecSubtract(2, x, aa, xMinusA);
				p = calculateP(invLeftMatrix, xMinusA);
				q = calculateQ(invLeftMatrix, xMinusA);
				newR = caluclateNewR(alpha[0], beta[0], gamma[0], p, q);
				newG = calculateNewG(alpha[1], beta[1], gamma[1], p, q);
				newB = calculateNewB(alpha[2], beta[2], gamma[2], p, q);
				pixSetRGB(x0, x1, newR, newB, newG);
			}
		}
	}
}