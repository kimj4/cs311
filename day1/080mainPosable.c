#define renVARYDIMBOUND 16
#define renVERTNUMBOUND 1000
#define renATTRX 0
#define renATTRY 1
#define renATTRS 2
#define renATTRT 3
#define renATTRR 4
#define renATTRG 5
#define renATTRB 6

#define renVARYX 0
#define renVARYY 1
#define renVARYS 2
#define renVARYT 3
#define renVARYR 4
#define renVARYG 5
#define renVARYB 6

#define renUNIFDIM 6
#define renUNIFR 0
#define renUNIFG 1
#define renUNIFB 2
#define renUNIFXTRANS 3
#define renUNIFYTRANS 4
#define renUNIFROT 5


#define renTEXR 0
#define renTEXG 1
#define renTEXB 2



#include "000pixel.h"
#include "070vector.c"
#include "030matrix.c"
#include "040texture.c"
#include "080renderer.c"
#include "080triangle.c"
#include "080mesh.c"
#include <stdarg.h>
#include <stdio.h>

double unif[renUNIFDIM];
renRenderer ren;
int width = 512;
int height = 512;
meshMesh mesh;

texTexture *tex[];

/* Sets rgb, based on the other parameters, which are unaltered. attr is an
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[],
                double vary[], double rgb[]) {
  texSample(tex[0], vary[renVARYS], vary[renVARYT]);
  rgb[0] = tex[0]->sample[renTEXR];
  rgb[1] = tex[0]->sample[renTEXG];
  rgb[2] = tex[0]->sample[renTEXB];
}

/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[],
        double vary[]) {
    /* For now, just copy attr to varying. Baby steps. */
    vary[renATTRX] = cos(unif[renUNIFROT]) * attr[renATTRX] - sin(unif[renUNIFROT]) * attr[renATTRY] + unif[renUNIFXTRANS];
    vary[renATTRY] = sin(unif[renUNIFROT]) * attr[renATTRX] + cos(unif[renUNIFROT]) * attr[renATTRY] + unif[renUNIFYTRANS];
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
}


void handleTimeStep(double oldTime, double newTime) {
  if (floor(newTime) - floor(oldTime) >= 1.0) {
    printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
  }
  pixClearRGB(0, 0, 0);
  unif[renUNIFROT] += .05;
  meshRender(&mesh, &ren, unif, tex);
}

int main() {
  if (pixInitialize(width, height, "Pixel Graphics") != 0) {
    return 1;
  } else {
    pixClearRGB(0, 0, 0);
    // set number of information stored
    ren.attrDim = 4;
    ren.texNum = 1;
    ren.unifDim = 3;
    ren.varyDim = 4;

    // set textures
    texTexture tex1;
    char *path1 = "cat1.jpg";
    texInitializeFile(&tex1, path1);
    texSetFiltering(&tex1, texQUADRATIC);
    texSetTopBottom(&tex1, texREPEAT);
    texSetLeftRight(&tex1, texREPEAT);
    tex[0] = &tex1;

    unif[renUNIFXTRANS] = 256;
    unif[renUNIFYTRANS] = 256;
    unif[renUNIFROT] = 0;

    double x = 0;
    double y = 0;
    double rx = 100;
    double ry = 200;
    int sideNum = 100;
    meshInitializeEllipse(&mesh, x, y, rx, ry, sideNum);
    pixSetTimeStepHandler(handleTimeStep);
    pixRun();
  }
 }
