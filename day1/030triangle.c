/*
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * homework for day 3, adding interpolation on triangle rasterization.
 */


#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "000pixel.h"

// new triRender
void triRender(double a[2], double b[2], double c[2], double rgb[3], 
	double alpha[3], double beta[3], double gamma[3]) {
		// sorting
	double aa0, aa1, bb0, bb1, cc0, cc1;
	double abcArray[3][2] = {{a[0], a[1]}, {b[0], b[1]}, {c[0], c[1]}};
	double sortedArray[3][2];
	int curIdx = 0;
	int i;

	// find aa (leftmost)
	int aaPos = 0;
	for (i = 1; i < 3; i++) {
		// find leftmost
		if (abcArray[aaPos][0] > abcArray[i][0]) {
			aaPos = i;
		} else if (abcArray[aaPos][0] == abcArray[i][0]) {
			if (abcArray[aaPos][1] > abcArray[i][1]) 
				// if horizontal position conflicts, pick the one that is vertically lower
				aaPos = i;
		}
	}
	aa0 = abcArray[aaPos][0];
	aa1 = abcArray[aaPos][1];
	printf("abcArray: \n");
	int j;
	for (j = 0; j < 3; j++) {
		printf("(%f, %f)\n", abcArray[j][0],abcArray[j][1]);
	}

	double bcArray[2][2];
	switch (aaPos) {
		case (0): {
			printf("case 0\n");
			bcArray[0][0] = abcArray[1][0];
			bcArray[0][1] = abcArray[1][1];
			bcArray[1][0] = abcArray[2][0];
			bcArray[1][1] = abcArray[2][1];
			break;
		}
		case (1): {
			printf("case 1\n");
			bcArray[0][0] = abcArray[0][0];
			bcArray[0][1] = abcArray[0][1];
			bcArray[1][0] = abcArray[2][0];
			bcArray[1][1] = abcArray[2][1];
			break;
		}
		case (2): {
			printf("case 2\n");
			bcArray[0][0] = abcArray[0][0];
			bcArray[0][1] = abcArray[0][1];
			bcArray[1][0] = abcArray[1][0];
			bcArray[1][1] = abcArray[1][1];
			break;
		}
	}

	printf("bcArray: \n");
	int k;
	for (k = 0; k < 2; k++) {
		printf("(%f, %f)\n", bcArray[k][0],bcArray[k][1]);
	}

	if (bcArray[0][0] > bcArray[1][0]) { // compare horizontal positions 
		if (bcArray[0][1] <= bcArray[1][1]) { // compare vertical positions
			printf("1\n");
			bb0 = bcArray[0][0];
			bb1 = bcArray[0][1];
			cc0 = bcArray[1][0];
			cc1 = bcArray[1][1];
		} else {
			printf("2\n");
			bb0 = bcArray[1][0];
			bb1 = bcArray[1][1];
			cc0 = bcArray[0][0];
			cc1 = bcArray[0][1];
		} 
	} else {
		if (bcArray[0][1] >= bcArray[1][1]) {
			printf("3\n");
			bb0 = bcArray[1][0];
			bb1 = bcArray[1][1];
			cc0 = bcArray[0][0];
			cc1 = bcArray[0][1];
		} else {
			printf("4\n");
			bb0 = bcArray[0][0];
			bb1 = bcArray[0][1];
			cc0 = bcArray[1][0];
			cc1 = bcArray[1][1];
		}
	}
	printf("aa: (%f, %f)\n", aa0, aa1);
	printf("bb: (%f, %f)\n", bb0, bb1);
	printf("cc: (%f, %f)\n", cc0, cc1);
   

	int x0, x1;
	double x1low, x1high;
	// special case handling for vertical line
	if ((aa0 == bb0) && (bb0 == cc0)) {
		x0 = aa0;
		for (x1 = ceil(aa1); x1 <= floor(cc1); x1++) {
			pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
		}
	}
	// base of the triangle is the bottom-most edge
	else if ((bb0 > cc0) || (aa0 == cc0) || (bb0 == cc0)) {
		for (x0 = ceil(aa0); x0 <= floor(cc0); x0++) {
			x1low = aa1 + ((bb1 - aa1) / (bb0 - aa0)) * (x0 - aa0);
			x1high = aa1 + ((cc1 - aa1) / (cc0 - aa0)) * (x0 - aa0);
			for (x1 = ceil(x1low); x1 <= floor(x1high); x1++) {
				pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
			}
		}
		for (x0 = ceil(cc0); x0 <= floor(bb0); x0++) {
			x1low = aa1 + ((bb1 - aa1) / (bb0 - aa0)) * (x0 - aa0);
			x1high = cc1 + ((bb1 - cc1) / (bb0 - cc0)) * (x0 - cc0);
			for (x1 = ceil(x1low); x1<= floor(x1high); x1++) {
				pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
			} 
		}
	// base of the triangle is the top-most edge
	} else {
		printf("else\n");
		for (x0 = ceil(aa0); x0 <= floor(bb0); x0++) {
			// printf("else: first for loop\n");
			x1low = aa1 + ((bb1 - aa1) / (bb0 - aa0)) * (x0 - aa0);
			x1high = aa1 + ((cc1 - aa1) / (cc0 - aa0)) * (x0 - aa0);
			printf("x1low: %f, x1high: %f \n", x1low, x1high);
			if ((ceil(aa0) == floor(cc0))) {
				printf("breaks\n");
				break;
			} else {
				for (x1 = ceil(x1low); x1 <= floor(x1high); x1++) {
					// printf("else: inner for loop\n");
					pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
				}
			}
		}
		printf("else: exits first for loop\n");
		for (x0 = ceil(bb0); x0 <= floor(cc0); x0++) {
			// printf("else: second for loop\n")	;
			x1low = bb1 + ((cc1 - bb1) / (cc0 - bb0)) * (x0 - bb0);
			x1high = aa1 + ((cc1 - aa1) / (cc0 - aa0)) * (x0 - aa0);
			for (x1 = ceil(x1low); x1 <= floor(x1high); x1++) {
				pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
			}
		}
	}
}

// void triRenderOld(double a0, double a1, double b0, double b1, double c0, 
//         double c1, double r, double g, double b) {

// 	// sorting
// 	double aa0, aa1, bb0, bb1, cc0, cc1;
// 	double abcArray[3][2] = {{a0, a1}, {b0, b1}, {c0, c1}};
// 	double sortedArray[3][2];
// 	int curIdx = 0;
// 	int i;

// 	// find aa (leftmost)
// 	int aaPos = 0;
// 	for (i = 1; i < 3; i++) {
// 		// find leftmost
// 		if (abcArray[aaPos][0] > abcArray[i][0]) {
// 			aaPos = i;
// 		} else if (abcArray[aaPos][0] == abcArray[i][0]) {
// 			if (abcArray[aaPos][1] > abcArray[i][1]) 
// 				// if horizontal position conflicts, pick the one that is vertically lower
// 				aaPos = i;
// 		}
// 	}
// 	aa0 = abcArray[aaPos][0];
// 	aa1 = abcArray[aaPos][1];
// 	printf("abcArray: \n");
// 	int j;
// 	for (j = 0; j < 3; j++) {
// 		printf("(%f, %f)\n", abcArray[j][0],abcArray[j][1]);
// 	}

// 	double bcArray[2][2];
// 	switch (aaPos) {
// 		case (0): {
// 			printf("case 0\n");
// 			bcArray[0][0] = abcArray[1][0];
// 			bcArray[0][1] = abcArray[1][1];
// 			bcArray[1][0] = abcArray[2][0];
// 			bcArray[1][1] = abcArray[2][1];
// 			break;
// 		}
// 		case (1): {
// 			printf("case 1\n");
// 			bcArray[0][0] = abcArray[0][0];
// 			bcArray[0][1] = abcArray[0][1];
// 			bcArray[1][0] = abcArray[2][0];
// 			bcArray[1][1] = abcArray[2][1];
// 			break;
// 		}
// 		case (2): {
// 			printf("case 2\n");
// 			bcArray[0][0] = abcArray[0][0];
// 			bcArray[0][1] = abcArray[0][1];
// 			bcArray[1][0] = abcArray[1][0];
// 			bcArray[1][1] = abcArray[1][1];
// 			break;
// 		}
// 	}

// 	printf("bcArray: \n");
// 	int k;
// 	for (k = 0; k < 2; k++) {
// 		printf("(%f, %f)\n", bcArray[k][0],bcArray[k][1]);
// 	}

// 	if (bcArray[0][0] > bcArray[1][0]) { // compare horizontal positions 
// 		if (bcArray[0][1] <= bcArray[1][1]) { // compare vertical positions
// 			printf("1\n");
// 			bb0 = bcArray[0][0];
// 			bb1 = bcArray[0][1];
// 			cc0 = bcArray[1][0];
// 			cc1 = bcArray[1][1];
// 		} else {
// 			printf("2\n");
// 			bb0 = bcArray[1][0];
// 			bb1 = bcArray[1][1];
// 			cc0 = bcArray[0][0];
// 			cc1 = bcArray[0][1];
// 		} 
// 	} else {
// 		if (bcArray[0][1] >= bcArray[1][1]) {
// 			printf("3\n");
// 			bb0 = bcArray[1][0];
// 			bb1 = bcArray[1][1];
// 			cc0 = bcArray[0][0];
// 			cc1 = bcArray[0][1];
// 		} else {
// 			printf("4\n");
// 			bb0 = bcArray[0][0];
// 			bb1 = bcArray[0][1];
// 			cc0 = bcArray[1][0];
// 			cc1 = bcArray[1][1];
// 		}
// 	}
// 	printf("aa: (%f, %f)\n", aa0, aa1);
// 	printf("bb: (%f, %f)\n", bb0, bb1);
// 	printf("cc: (%f, %f)\n", cc0, cc1);
   

// 	int x0, x1, x1low, x1high;
// 	// special case handling for vertical line
// 	if ((aa0 == bb0) && (bb0 == cc0)) {
// 		x0 = aa0;
// 		for (x1 = ceil(aa1); x1 <= floor(cc1); x1++) {
// 			pixSetRGB(x0, x1, r, g, b);
// 		}
// 	}
// 	// base of the triangle is the bottom-most edge
// 	else if ((bb0 > cc0) || (aa0 == cc0) || (bb0 == cc0)) {
// 		for (x0 = ceil(aa0); x0 <= floor(cc0); x0++) {
// 			x1low = aa1 + ((bb1 - aa1) / (bb0 - aa0)) * (x0 - aa0);
// 			x1high = aa1 + ((cc1 - aa1) / (cc0 - aa0)) * (x0 - aa0);
// 			for (x1 = ceil(x1low); x1 <= floor(x1high); x1++) {
// 				pixSetRGB(x0, x1, r, g, b);
// 			}
// 		}
// 		for (x0 = ceil(cc0); x0 <= floor(bb0); x0++) {
// 			x1low = aa1 + ((bb1 - aa1) / (bb0 - aa0)) * (x0 - aa0);
// 			x1high = cc1 + ((bb1 - cc1) / (bb0 - cc0)) * (x0 - cc0);
// 			for (x1 = ceil(x1low); x1<= floor(x1high); x1++) {
// 				pixSetRGB(x0, x1, r, g, b);
// 			} 
// 		}
// 	// base of the triangle is the top-most edge
// 	} else {
// 		printf("else\n");
// 		for (x0 = ceil(aa0); x0 <= floor(bb0); x0++) {
// 			// printf("else: first for loop\n");
// 			x1low = aa1 + ((bb1 - aa1) / (bb0 - aa0)) * (x0 - aa0);
// 			x1high = aa1 + ((cc1 - aa1) / (cc0 - aa0)) * (x0 - aa0);
// 			printf("x1low: %f, x1high: %f \n", x1low, x1high);
// 			if ((ceil(aa0) == floor(cc0))) {
// 				printf("breaks\n");
// 				break;
// 			} else {
// 				for (x1 = ceil(x1low); x1 <= floor(x1high); x1++) {
// 					// printf("else: inner for loop\n");
// 					pixSetRGB(x0, x1, r, g, b);
// 				}
// 			}
// 		}
// 		printf("else: exits first for loop\n");
// 		for (x0 = ceil(bb0); x0 <= floor(cc0); x0++) {
// 			// printf("else: second for loop\n")	;
// 			x1low = bb1 + ((cc1 - bb1) / (cc0 - bb0)) * (x0 - bb0);
// 			x1high = aa1 + ((cc1 - aa1) / (cc0 - aa0)) * (x0 - aa0);
// 			for (x1 = ceil(x1low); x1 <= floor(x1high); x1++) {
// 				pixSetRGB(x0, x1, r, g, b);
// 			}
// 		}
// 	}
// }