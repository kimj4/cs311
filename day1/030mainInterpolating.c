/*
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * homework for day 2, rasterizing a triangle.
 * The following are some of the tests that were run for the rasterization code.
 * To my knowledge, they all work. 
 */

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
// #include "030triangle.c"
#include "000pixel.h"


int main(void) {
	if (pixInitialize(512, 512, "Pixel Graphics") != 0)
		return 1;
	else { 
		pixClearRGB(0.0, 0.0, 0.0);
		
		// T E S T S
		// typical cases
		double a[2] = {10.0, 10.0};
		double b[2] = {400.0, 100.0};
		double c[2] = {200.0, 250.0};
		double rgb[3] = {1.0, 0.0, 0.0};
		double alpha[3] = {1.0, 0.0, 0.0};
		double beta[3] = {0.0, 1.0, 0.0};
		double gamma[3] = {0.0, 0.0, 1.0};
		triRender(a, b, c, rgb);
		// triRender(200.0, 250.0, 10.0, 10.0, 400.0, 100.0, 20.0, 20.0, 20.0);
		// triRender(400.0, 100.0, 200.0, 250.0, 10.0, 10.0, 20.0, 20.0, 20.0);
		// triRender(10.0, 10.0, 200.0, 20.0, 400.0, 300.0, 20.0, 20.0, 20.0);
		// triRender(200.0, 20.0, 400.0, 300.0, 10.0, 10.0, 20.0, 20.0, 20.0);
		// triRender(400.0, 300.0, 10.0, 10.0, 200.0, 20.0, 20.0, 20.0, 20.0);

		// vertical line
		// triRender(10.0, 10.0, 10.0, 100.0, 10.0, 200.0, 20.0, 20.0, 20.0); // works

		// horizontal line
		// triRender(10.0, 10.0, 100.0, 10.0, 500.0, 10.0, 20.0, 20.0, 20.0); // works

		// diagonal line - 45 degrees
		// triRender(10.0, 10.0, 100.0, 100.0, 200.0, 200.0, 20.0, 20.0, 20.0); // works

		// diagonal alignment - some other angle
		// triRender(0.0, 0.0, 200.0, 100.0, 400.0, 200.0, 20.0, 20.0, 20.0 ); // works
		// triRender(200.0, 100.0, 0.0, 0.0, 400.0, 200.0, 20.0, 20.0, 20.0); // works
		// triRender(400.0, 200.0, 200.0, 100.0, 0.0, 0.0, 20.0, 20.0, 20.0); // works

		// right triangles (100,100) (200,100) (100, 200) - - - works
		// triRender(100.0, 100.0, 200.0, 100.0, 100.0, 200.0, 20.0, 20.0, 20.0);
		// triRender(200.0, 100.0, 100.0, 200.0, 100.0, 100.0, 20.0, 20.0, 20.0); 
		// triRender(100.0, 200.0, 100.0, 100.0, 200.0, 100.0, 20.0, 20.0, 20.0); 
		
		// right triangles (100,100) (200,100) (200, 200) - - - works
		// triRender(100.0, 100.0, 200.0, 100.0, 200.0, 200.0, 20.0, 20.0, 20.0);
		// triRender(200.0, 100.0, 200.0, 200.0, 100.0, 100.0, 20.0, 20.0, 20.0);
		// triRender(200.0, 200.0, 100.0, 100.0, 200.0, 100.0, 20.0, 20.0, 20.0);

		// right triangles (100,100) (200,200) (100, 200)
		// triRender(100.0, 100.0, 200.0, 200.0, 100.0, 200.0, 20.0, 20.0, 20.0);
		// triRender(200.0, 200.0, 100.0, 200.0, 100.0, 100.0, 20.0, 20.0, 20.0);
		// triRender(100.0, 200.0, 100.0, 100.0, 200.0, 200.0, 20.0, 20.0, 20.0);

		// right triangles (100,200) (200,100) (200,200)
		// triRender(100.0, 200.0, 200.0, 100.0, 200.0, 200.0, 20.0, 20.0, 20.0);		
		// triRender(200.0, 100.0, 200.0, 200.0, 100.0, 200.0, 20.0, 20.0, 20.0);		
		// triRender(200.0, 200.0, 100.0, 200.0, 200.0, 100.0, 20.0, 20.0, 20.0);		

		pixRun();	
		return 0;	
	}
}