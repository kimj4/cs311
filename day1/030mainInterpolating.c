/*
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * homework for day 3, added functionality for interpolation.
 */



#include "000pixel.h"
#ifndef 040vector_c
#include "030vector.c"
// #include "030matrix.c"
// #include "030triangle.c"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

int main(void) {
	if (pixInitialize(512, 512, "Pixel Graphics") != 0)
		return 1;
	else { 
		pixClearRGB(0.0, 0.0, 0.0);
		
		double a[2] = {10, 10};
		double b[2] = {500, 500};
		double c[2] = {230, 100};
		double rgb[3] = {1.0, 1.0, 1.0};
		double alpha[3] = {1.0, 0.0, 0.0};
		double beta[3] = {0.0, 1.0, 0.0};
		double gamma[3] = {0.0, 0.0, 1.0};
		triRender(a, b, c, rgb, alpha, beta, gamma);

		pixRun();	
		return 0;	
	}
}