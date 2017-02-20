/*
 * 090mainScene.c
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * main program to test the addition of scene.c
 */

#define renVARYDIMBOUND 16
#define renVERTNUMBOUND 1000

#define renATTRDIM 7
#define renATTRX 0
#define renATTRY 1
#define renATTRS 2
#define renATTRT 3
#define renATTRR 4
#define renATTRG 5
#define renATTRB 6

#define renVARYDIM 7
#define renVARYX 0
#define renVARYY 1
#define renVARYS 2
#define renVARYT 3
#define renVARYR 4
#define renVARYG 5
#define renVARYB 6

#define renUNIFDIM 15
#define renUNIFR 0
#define renUNIFG 1
#define renUNIFB 2
#define renUNIFTRANSX 3
#define renUNIFTRANSY 4
#define renUNIFTHETA 5
#define renUNIFMAT00 6
#define renUNIFMAT01 7
#define renUNIFMAT02 8
#define renUNIFMAT10 9
#define renUNIFMAT11 10
#define renUNIFMAT12 11
#define renUNIFMAT20 12
#define renUNIFMAT21 13
#define renUNIFMAT22 14

#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

#include "000pixel.h"
#include "070vector.c"
#include "090matrix.c"
#include "040texture.c"
#include "090renderer.c"
#include "090triangle.c"
#include "090mesh.c"
#include "090scene.c"
#include <stdarg.h>
#include <stdio.h>

double unif[renUNIFDIM];
renRenderer ren;
int width = 1024;
int height = 1024;
// meshMesh mesh;

texTexture *tex[];
sceneNode root;
sceneNode branch1;
sceneNode branch2;
sceneNode branch3;
sceneNode branch4;

/* If unifParent is NULL, then sets the uniform matrix to the
rotation-translation M described by the other uniforms. If unifParent is not
NULL, but instead contains a rotation-translation P, then sets the uniform
matrix to the matrix product P * M. */
void updateUniform(renRenderer *ren, double unif[], double unifParent[]) {
    if (unifParent == NULL) {
        /* The nine uniforms for storing the matrix start at index
        renUNIFMAT00. So &unif[renUNIFMAT00] is an array containing those
        nine numbers. We use '(double(*)[3])' to cast it to a 3x3 matrix. */
        mat33Isometry(unif[renUNIFTHETA], unif[renUNIFTRANSX],
            unif[renUNIFTRANSY], (double(*)[3])(&unif[renUNIFMAT00]));
    } else {
        double m[3][3];
        mat33Isometry(unif[renUNIFTHETA], unif[renUNIFTRANSX],
            unif[renUNIFTRANSY], m);
        mat333Multiply((double(*)[3])(&unifParent[renUNIFMAT00]), m,
            (double(*)[3])(&unif[renUNIFMAT00]));
    }
}

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
    double temp[3];
    double expandedAttr[3] = {attr[renATTRX], attr[renATTRY], 1};
    mat331Multiply((double(*)[3])(&unif[renUNIFMAT00]), expandedAttr, temp);
    vary[renATTRX] = temp[0];
    vary[renATTRY] = temp[1];
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
}


void handleTimeStep(double oldTime, double newTime) {
  if (floor(newTime) - floor(oldTime) >= 1.0) {
    printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
  }
  pixClearRGB(0, 0, 0);
  unif[renUNIFTHETA] += .05;
  sceneSetUniform(&root, &ren, unif);
  sceneRender(&root, &ren, NULL);
}

int main() {
  if (pixInitialize(width, height, "Pixel Graphics") != 0) {
    return 1;
  } else {
    pixClearRGB(0, 0, 0);

    // initialize ren
    ren.attrDim = 4;
    ren.texNum = 1;
    ren.unifDim = renUNIFDIM;
    ren.varyDim = renVARYDIM;
    ren.transformVertex = transformVertex;
    ren.colorPixel = colorPixel;
    ren.updateUniform = updateUniform;

    // initialize textures`
    texTexture tex1;
    char *path1 = "cat1.jpg";
    texInitializeFile(&tex1, path1);
    texSetFiltering(&tex1, texQUADRATIC);
    texSetTopBottom(&tex1, texREPEAT);
    texSetLeftRight(&tex1, texREPEAT);
    tex[0] = &tex1;

    // initialize top level unif
    unif[renUNIFTRANSX] = 512;
    unif[renUNIFTRANSY] = 512;
    unif[renUNIFTHETA] = 0;

    // initialize some meshes
    meshMesh mesh1;
    double x = 0;
    double y = 0;
    double rx = 100;
    double ry = 100;
    int sideNum = 100;
    meshInitializeEllipse(&mesh1, x, y, rx, ry, sideNum);

    // initialize some nodes
    sceneInitialize(&branch4, &ren, unif, &tex, &mesh1, NULL, NULL);
    branch4.unif[renUNIFTRANSY] = 200;
    branch4.unif[renUNIFTRANSX] = 100;
    sceneInitialize(&branch3, &ren, unif, &tex, &mesh1, NULL, &branch4);
    branch3.unif[renUNIFTRANSY] = 300;
    branch3.unif[renUNIFTRANSX] = 100;
    sceneInitialize(&branch2, &ren, unif, &tex, &mesh1, NULL, &branch3);
    branch2.unif[renUNIFTRANSY] = 100;
    branch2.unif[renUNIFTRANSX] = 100;
    sceneInitialize(&branch1, &ren, unif, &tex, &mesh1, &branch2, NULL);
    branch1.unif[renUNIFTRANSY] = 100;
    branch1.unif[renUNIFTRANSX] = 100;
    sceneInitialize(&root, &ren, unif, &tex, &mesh1, &branch1, NULL);

    pixSetTimeStepHandler(handleTimeStep);
    pixRun();
  }
 }
