/*
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * homework for day 4, added functionality for interpolation.
 */


#include "000pixel.h"
#include "030vector.c"
#include "030matrix.c"

#include "040texture.c"
#include "040triangle.c"
#include <stdio.h>
#include <stdbool.h>
texTexture *tex;


void handleKeyUp(int key, int shiftIsDown, int controlIsDown,
		int altOptionIsDown, int superCommandIsDown) {

	printf("key up %d, shift %d, control %d, altOpt %d, supComm %d\n",
		key, shiftIsDown, controlIsDown, altOptionIsDown, superCommandIsDown);
	if (key == 257) {
		if (tex->filtering == texQUADRATIC) {
			tex->filtering = texNEAREST;
			printf("a\n");
		} else {
			tex->filtering = texQUADRATIC;
			printf("b\n");
		}


	}

}

void handleTimeStep(double oldTime, double newTime) {
	if (floor(newTime) - floor(oldTime) >= 1.0){
		printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
		
		pixClearRGB(0, 0, 0);

		double a[2] = {0, 0};	
		double b[2] = {512, 0};
		double c[2] = {256, 512};
		double rgb[3] = {1.0, 1.0, 1.0};
		double alpha[2] = {0.0, 0.0};
		double beta[2] = {1.0, 0.0};
		double gamma[2] = {0.5, .825};
		// tex->filtering = texQUADRATIC;
			

		triRender(a, b, c, rgb, tex, alpha, beta, gamma);
		
	}
} 

int main(void) {
	if (pixInitialize(512, 512, "Pixel Graphics") != 0)
		return 1;
	else { 
		pixClearRGB(0, 0, 0);
		
		tex = malloc(sizeof(texTexture));
		char *path = "/Accounts/kimj4/cs311/day1/corgi.jpg";
		texInitializeFile(tex, path);
		texSetFiltering(tex, texNEAREST);
		texSetTopBottom(tex, texREPEAT);
		texSetLeftRight(tex, texREPEAT);

		pixSetKeyUpHandler(handleKeyUp);
		pixSetTimeStepHandler(handleTimeStep);
		pixRun();	
		texDestroy(tex);
		return 0;	
	}
}