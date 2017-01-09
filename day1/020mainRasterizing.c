#include <stdio.h>
#include <stdbool.h>
#include <math.h>
// #include "020triangle.c"
#include "000pixel.h"


int main(void) {
	if (pixInitialize(512, 512, "Pixel Graphics") != 0)
		return 1;
	else { 
		// tests
		pixClearRGB(0.0, 0.0, 0.0);
		
		// typical cases
		// triRender(10.0, 10.0, 400.0, 100.0, 200.0, 250.0, 20.0, 20.0, 20.0);
		// triRender(200.0, 250.0, 10.0, 10.0, 400.0, 100.0, 20.0, 20.0, 20.0);
		// triRender(400.0, 100.0, 200.0, 250.0, 10.0, 10.0, 20.0, 20.0, 20.0);
		// triRender(10.0, 10.0, 200.0, 20.0, 400.0, 300.0, 20.0, 20.0, 20.0);
		// triRender(200.0, 20.0, 400.0, 300.0, 10.0, 10.0, 20.0, 20.0, 20.0);
		// triRender(400.0, 300.0, 10.0, 10.0, 200.0, 20.0, 20.0, 20.0, 20.0);

		// lines
		// vertical alignment
		// triRender(10.0, 10.0, 10.0, 100.0, 10.0, 200.0, 20.0, 20.0, 20.0); //doesn't work
		// horizontal alignment
		// triRender(10.0, 10.0, 100.0, 10.0, 200.0, 10.0, 20.0, 20.0, 20.0); // works?
		// diagonal alignment - 45 degrees
		// triRender(10.0, 10.0, 100.0, 100.0, 200.0, 200.0, 20.0, 20.0, 20.0); // works
		// diagonal alignment - some other angle
		// triRender(0.0, 0.0, 200.0, 100.0, 400.0, 200.0, 20.0, 20.0, 20.0 ); // works
		// triRender(200.0, 100.0, 0.0, 0.0, 400.0, 200.0, 20.0, 20.0, 20.0); // works
		// triRender(400.0, 200.0, 200.0, 100.0, 0.0, 0.0, 20.0, 20.0, 20.0); // works

		// right triangles
		// triRender(10.0, 10.0, 10.0, 100.0, 100.0, 100.0, 20.0, 20.0, 20.0); // infinite loop
		



		// TODO: implement features for:
		// - various configurations of two vertices having the same horizontal position
		// - various configurations of two vertices having the same verticla position
		// - various configurations of three verticies lining up 


		pixRun();	
		return 0;	
	}
}