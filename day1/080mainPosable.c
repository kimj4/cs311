#define renVARYDIMBOUND 16
#define renVERTNUMBOUND 16
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
#define renUNIFR 0
#define renUNIFG 1
#define renUNIFB 2

#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

#include "000pixel.h"
#include "070vector.c"
#include "030matrix.c"
#include "040texture.c"
#include "080renderer.c"
#include "080triangle.c"
#include "070mesh.c"
#include <stdarg.h>
#include <stdio.h>

double unif[3];
renRenderer ren;
int width = 1024;
int height = 1024;
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
    vary[renVARYX] = attr[renATTRX];
    vary[renVARYY] = attr[renATTRY];
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
}


void handleTimeStep(double oldTime, double newTime) {
  if (floor(newTime) - floor(oldTime) >= 1.0) {
    printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
  }
  pixClearRGB(0, 0, 0);
  unif[0] = 1;
  unif[1] = 1;
  unif[2] = 1;
  meshRender(&mesh, &ren, unif, tex);
}

int main() {
  if (pixInitialize(width, height, "Pixel Graphics") != 0) {
    return 1;
  } else {
    pixClearRGB(0, 0, 0);


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


  double x,y,rx,ry;
  int sideNum;
  x = 512;
  y = 512;
  rx = 500;
  ry = 500;
  sideNum = 13;
  meshInitializeEllipse(&mesh, x, y, rx, ry, sideNum);

  pixSetTimeStepHandler(handleTimeStep);
  pixRun();
  }
 }
