/*
 * 090mainScene.c
 * Ju Yun Kim
 * Carleton College
 * CS 311
 * main program to test the transition from 2D to 3D
 */

#define renVARYDIMBOUND 16
#define renVERTNUMBOUND 1000

#define renATTRDIM 8
#define renATTRX 0
#define renATTRY 1
#define renATTRZ 2
#define renATTRS 3
#define renATTRT 4
#define renATTRR 5
#define renATTRG 6
#define renATTRB 7

#define renVARYDIM 7
#define renVARYX 0
#define renVARYY 1
#define renVARYS 2
#define renVARYT 3
#define renVARYR 4
#define renVARYG 5
#define renVARYB 6

#define renUNIFDIM 25
#define renUNIFR 0
#define renUNIFG 1
#define renUNIFB 2
#define renUNIFTRANSX 3
#define renUNIFTRANSY 4
#define renUNIFTRANSZ 5
#define renUNIFALPHA 6
#define renUNIFPHI 7
#define renUNIFTHETA 8
#define renUNIFMAT00 9

#define texNUM 1
#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include "000pixel.h"
#include "100vector.c"
#include "100matrix.c"
#include "040texture.c"
#include "090renderer.c"
#include "090triangle.c"
#include "100mesh.c"
#include "090scene.c"


double unif[renUNIFDIM];
renRenderer ren;
int width = 512;
int height = 512;

texTexture *tex[texNUM];
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
        double I3[3][3];
        mat33SetIdentity(I3);
        double rotation[3][3];
        // double axis[3] = {1.0, 1.0, 1.0};
        double axis[3] = {0.0, 0.0, 1.0};

        mat33AngleAxisRotation(unif[renUNIFALPHA], axis, rotation);
        double trans[3] = {unif[renUNIFTRANSX], unif[renUNIFTRANSY], unif[renUNIFTRANSZ]};
        mat44Isometry(rotation, trans, (double(*)[4])(&unif[renUNIFMAT00]));
        // printf("mat44Isometry:\n");
        // mat44Print((double(*)[4])(&unif[renUNIFMAT00]));
        // mat44Isometry(double rot[3][3], double trans[3], double isom[4][4])
    } else {
        mat44SetIdentity((double(*)[4])(&unif[renUNIFMAT00]));
        // double m[3][3];
        // mat33Isometry(unif[renUNIFTHETA], unif[renUNIFTRANSX],
        //     unif[renUNIFTRANSY], m);
        // mat333Multiply((double(*)[3])(&unifParent[renUNIFMAT00]), m,
        //     (double(*)[3])(&unif[renUNIFMAT00]));
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
  // printf("colorPixel\n");
}

/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[],
        double vary[]) {
    // printf("transformVertex\n");
    double temp[4];
    double expandedAttr[4] = {attr[renATTRX], attr[renATTRY], attr[renATTRZ], 1};
    mat441Multiply((double(*)[4])(&unif[renUNIFMAT00]), expandedAttr, temp);
    vary[renATTRX] = temp[0];
    vary[renATTRY] = temp[1];
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
}


void handleTimeStep(double oldTime, double newTime) {
  if (floor(newTime) - floor(oldTime) >= 1.0) {
    printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
  }
  // printf("handleTimeStep\n");
  pixClearRGB(0, 0, 0);
  unif[renUNIFTHETA] += .05;
  unif[renUNIFALPHA] += .05;
  unif[renUNIFPHI] += .05;
  sceneSetUniform(&root, &ren, unif);
  // printf("sceneSetUniform finished\n");
  sceneRender(&root, &ren, NULL);
  // printf("sceneRender finshed\n");
}

int main() {
  if (pixInitialize(width, height, "Pixel Graphics") != 0) {
    return 1;
  } else {
    pixClearRGB(0, 0, 0);

    // initialize ren
    ren.attrDim = 8;
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
    unif[renUNIFTRANSX] = 200;
    unif[renUNIFTRANSY] = 200;
    // unif[renUNIFTRANSZ] = 200;
    unif[renUNIFALPHA] = 1;
    unif[renUNIFTHETA] = 0;

    // initialize some meshes
    meshMesh mesh1;
    // double x = 300;
    // double y = 300;
    // double rx = 100;
    // double ry = 100;
    // int sideNum = 100;
    meshInitializeSphere(&mesh1, 50, 10, 20);
    // meshInitializeEllipse(&mesh1, x, y, rx, ry, sideNum);

    // initialize some nodes
    // sceneInitialize(&branch4, &ren, unif, tex, &mesh1, NULL, NULL);
    // branch4.unif[renUNIFTRANSY] = 200;
    // branch4.unif[renUNIFTRANSX] = 100;
    // sceneInitialize(&branch3, &ren, unif, tex, &mesh1, NULL, &branch4);
    // branch3.unif[renUNIFTRANSY] = 300;
    // branch3.unif[renUNIFTRANSX] = 100;
    // sceneInitialize(&branch2, &ren, unif, tex, &mesh1, NULL, &branch3);
    // branch2.unif[renUNIFTRANSY] = 100;
    // branch2.unif[renUNIFTRANSX] = 100;
    // sceneInitialize(&branch1, &ren, unif, tex, &mesh1, &branch2, NULL);
    // branch1.unif[renUNIFTRANSY] = 100;
    // branch1.unif[renUNIFTRANSX] = 100;
    sceneInitialize(&root, &ren, unif, tex, &mesh1, NULL, NULL);
    pixSetTimeStepHandler(handleTimeStep);
    pixRun();
  }
 }
