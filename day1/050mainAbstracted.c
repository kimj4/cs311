/* 
 * 050mainAbstracted.c
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * Homework for day 5
 * abstracted some of the things we have been doing with triangles
 */

#define renATTRDIMBOUND 16

#include "050renderer.c"

#define renATTRX 0
#define renATTRY 1
#define renATTRS 2
#define renATTRT 3
#define renATTRR 4
#define renATTRG 5
#define renATTRB 6
#define renUNIFR 0
#define renUNIFG 1
#define renUNIFB 2
#define renTEXR 0
#define renTEXG 1
#define renTEXB 2
// attr is what was formally chi

#include "000pixel.h"
#include "030vector.c"
#include "030matrix.c"
#include "040texture.c"
#include "050triangle.c"

renRenderer ren;
texTexture *tex[1];
texTexture tex1;
double unif[3];


/* Sets rgb, based on the other parameters, which are unaltered. attr is an 
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], 
        double attr[], double rgb[]) {
    texSample(tex[0], attr[renATTRS], attr[renATTRT]);
    rgb[0] = tex[0]->sample[renTEXR] * unif[renUNIFR] * attr[renATTRR];
    rgb[1] = tex[0]->sample[renTEXG] * unif[renUNIFG] * attr[renATTRG];
    rgb[2] = tex[0]->sample[renTEXB] * unif[renUNIFB] * attr[renATTRB];
}

void handleTimeStep(double oldTime, double newTime) {
	if (floor(newTime) - floor(oldTime) >= 1.0){
		printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
		
		pixClearRGB(0, 0, 0);
		double a[7] = {2,   2,   0.1, 0.1,   1, 1, 1};
		double b[7] = {512, 0,   1,   0,     1, 1, 1};
		double c[7] = {256, 512, 0.5, 0.825, 1, 1, 1};

		unif[0] = 1;
		unif[1] = 1;
		unif[2] = 1;

		triRender(&ren, unif, &tex, a, b, c);
	}
} 


int main() {
	if (pixInitialize(512, 512, "Pixel Graphics") != 0)
		return 1;
	else { 
		pixClearRGB(0, 0, 0);
		
		// set number of information stored
		ren.attrDim = 7;
		ren.texNum = 1;
		ren.unifDim = 3;

		// set textures
		char *path = "/Accounts/kimj4/cs311/day1/corgi.jpg";
		texInitializeFile(&tex1, path);
		texSetFiltering(&tex1, texNEAREST);
		texSetTopBottom(&tex1, texREPEAT);
		texSetLeftRight(&tex1, texREPEAT);
		tex[0] = &tex1;

		pixSetTimeStepHandler(handleTimeStep);
		pixRun();	
		texDestroy(tex[0]);
		return 0;	
	}
}


