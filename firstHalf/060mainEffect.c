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
#include "030matrix.c"
#include "030vector.c"
#include "040texture.c"
#include "050triangle.c"

renRenderer ren;
texTexture *tex[2];
double unif[3];
int width = 512;
int height = 512;


/* Sets rgb, based on the other parameters, which are unaltered. attr is an
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[],
                double attr[], double rgb[]) {
  texSample(tex[0], attr[renATTRS], attr[renATTRT]);
  texSample(tex[1], attr[renATTRS], attr[renATTRT]);
  rgb[0] = tex[0]->sample[renTEXR] - tex[1]->sample[renTEXR];
  rgb[1] = tex[0]->sample[renTEXG] - tex[1]->sample[renTEXG];
  rgb[2] = tex[0]->sample[renTEXB] - tex[1]->sample[renTEXB];
}

void handleTimeStep(double oldTime, double newTime) {
  if (floor(newTime) - floor(oldTime) >= 1.0) {
    printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
  }
  pixClearRGB(0, 0, 0);
  // {x, y, s, t, r, g, b}
  // double b[7] = {2, 2, 0.1, 0.1, 1, 1, 1};
  // double c[7] = {100, 100, 1, 0, 1, 1, 1};
  // double a[7] = {2, 100, 0.5, 0.825, 1, 1, 1};

  double b[7] = {2, 100, 0.1, 0.1, 1, 1, 1};
  double c[7] = {20, 2, 1, 0, 1, 1, 1};
  double a[7] = {100, 100, 0.5, 0.825, 1, 1, 1};

  unif[0] = 1;
  unif[1] = 1;
  unif[2] = 1;

  triRender(&ren, unif, &tex, a, b, c);
}

int main() {
  if (pixInitialize(width, height, "Pixel Graphics") != 0)
    return 1;
  else {
    pixClearRGB(0, 0, 0);

    // set number of information stored
    ren.attrDim = 7;
    ren.texNum = 2;
    ren.unifDim = 3;

    // set textures
    texTexture tex1, tex2;
    char *path1 = "cat1.jpg";
    texInitializeFile(&tex1, path1);
    texSetFiltering(&tex1, texNEAREST);
    texSetTopBottom(&tex1, texREPEAT);
    texSetLeftRight(&tex1, texREPEAT);
    tex[0] = &tex1;
    char *path2 = "cat2.jpg";
    texInitializeFile(&tex2, path2);
    texSetFiltering(&tex2, texNEAREST);
    texSetTopBottom(&tex2, texREPEAT);
    texSetLeftRight(&tex2, texREPEAT);
    tex[1] = &tex2;

    pixSetTimeStepHandler(handleTimeStep);
    pixRun();
    int i;
    for (i = 0; i < ren.texNum; i++) {
      texDestroy(tex[i]);
    }
    return 0;
  }
}
