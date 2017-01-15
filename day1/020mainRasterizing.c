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
// #include "020triangle.c"
#include "000pixel.h"


int main(void) {
	if (pixInitialize(512, 512, "Pixel Graphics") != 0)
		return 1;
	else { 
		pixClearRGB(0.0, 0.0, 0.0);
		
		// T E S T S
		// typical cases
		// triRender(10.0, 10.0, 400.0, 100.0, 200.0, 250.0, 20.0, 20.0, 20.0);
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